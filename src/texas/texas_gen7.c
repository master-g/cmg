/**
 * Created by mg on 2026/9/20.
 *
 * 7 张牌评估表生成器, 两阶段:
 *   1. texas_generate7            -> 写出 temp_7card_keys.txt
 *      mph -dpf < temp_7card_keys.txt   (mph 见 src/mph)
 *   2. texas_generate7 perf_hash.c -> 写出 texas_array7.c / texas_array7.h
 *
 * 原理: 不含同花的 7 张牌, 牌力只取决于点数多重集。这样的多重集共 49205 个,
 * 把每个点数的出现次数(0..4)编成五进制码即得唯一键, 再用最小完美哈希落表。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "texas_array.h"
#include "texas_eval.h"

#define TAB_LEN 16384
#define RANGE 65536

static unsigned int pow5[13];
static unsigned int counts[13];

/* ---- 阶段 1: 枚举键 ---- */

static FILE *g_keyfp;
static int g_nkey;

static void emit_key(void) {
  unsigned int q = 0;
  for (int i = 0; i < 13; i++)
    q += counts[i] * pow5[i];
  fprintf(g_keyfp, "%u\n", q);
  g_nkey++;
}

/* ---- 阶段 2: 求值并落表 ---- */

static unsigned short g_scramble[256];
static unsigned char g_tab[TAB_LEN];
static unsigned short g_folded[TAB_LEN];
static unsigned short g_values[RANGE];
static char g_used[RANGE];
static unsigned int g_salt, g_shift_b, g_mask_b, g_shift_a1, g_shift_a2;
static int g_collide;

static unsigned int mph7(unsigned int v) {
  v += g_salt;
  v ^= v >> 16u;
  v += v << 8u;
  v ^= v >> 4u;
  return ((v + (v << g_shift_a1)) >> g_shift_a2) ^
         g_folded[(v >> g_shift_b) & g_mask_b];
}

/* 按 counts[] 造一副具体的牌, 任何花色都不到 5 张, 保证参考求值不会走同花分支
 */
static void build_hand(unsigned int *c) {
  int sc[4] = {0, 0, 0, 0};
  int n = 0;
  for (int r = 0; r < 13; r++) {
    for (unsigned int k = 0; k < counts[r]; k++) {
      int best = -1;
      for (int s = 0; s < 4; s++) {
        int used = 0;
        for (int t = 0; t < (int)k; t++)
          if (((c[n - 1 - t] >> 12u) & 0xFu) == (1u << s))
            used = 1;
        if (!used && (best < 0 || sc[s] < sc[best]))
          best = s;
      }
      c[n++] = texas_eval_primes[r] | ((unsigned)r << 8u) | (0x1000u << best) |
               (1u << (16 + r));
      sc[best]++;
    }
  }
}

static void emit_value(void) {
  unsigned int c[7], q = 0, h;
  build_hand(c);
  for (int i = 0; i < 13; i++)
    q += counts[i] * pow5[i];
  h = mph7(q);
  if (h >= RANGE || g_used[h]) {
    g_collide++;
    return;
  }
  g_used[h] = 1;
  g_values[h] = texas_eval_7hand(c[0], c[1], c[2], c[3], c[4], c[5], c[6]);
  g_nkey++;
}

static void walk(int rank, int left, void (*fn)(void)) {
  if (rank == 13) {
    if (left == 0)
      fn();
    return;
  }
  for (unsigned int k = 0; k <= 4 && (int)k <= left; k++) {
    counts[rank] = k;
    walk(rank + 1, left - (int)k, fn);
  }
  counts[rank] = 0;
}

/* ---- 解析 mph 生成的 perf_hash.c ---- */

static char *slurp(const char *path, long *len) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;
  fseek(f, 0, SEEK_END);
  *len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buf = malloc((size_t)*len + 1);
  if (fread(buf, 1, (size_t)*len, f) != (size_t)*len) {
    free(buf);
    fclose(f);
    return NULL;
  }
  buf[*len] = '\0';
  fclose(f);
  return f ? buf : buf;
}

/* 读出 name[] = { ... } 里的前 n 个整数 */
static int
read_array(const char *src, const char *name, unsigned int *out, int n) {
  char pat[64];
  snprintf(pat, sizeof(pat), "%s[] = {", name);
  const char *p = strstr(src, pat);
  if (!p)
    return -1;
  p += strlen(pat);
  for (int i = 0; i < n; i++) {
    while (*p && *p != '-' && (*p < '0' || *p > '9')) {
      if (*p == '}')
        return -1;
      p++;
    }
    out[i] = (unsigned int)strtoul(p, (char **)&p, 0);
  }
  return 0;
}

/* 读出紧跟 "prefix" 的整数; base 10 或 16 (prefix 须含 "0x") */
static int
read_const(const char *src, const char *prefix, int base, unsigned int *out) {
  const char *p = strstr(src, prefix);
  if (!p)
    return -1;
  *out = (unsigned int)strtoul(p + strlen(prefix), NULL, base);
  return 0;
}

static void
dump_u16(FILE *f, const char *name, const unsigned short *a, int n) {
  /* 每行 10 个定宽 7 字符的字段; 表体另有 clang-format off 保护 */
  fprintf(f, "const unsigned short %s[] = {\n", name);
  for (int i = 0; i < n; i += 10) {
    fprintf(f, "    ");
    for (int j = 0; j < 10 && i + j < n; j++) {
      char cell[8];
      snprintf(cell, sizeof(cell), "%u,", a[i + j]);
      int last = (j == 9) || (i + j == n - 1); /* 行末不补空格, 否则 fmt 会删 */
      fprintf(f, last ? "%s" : "%-7s", cell);
    }
    fprintf(f, "\n");
  }
  fprintf(f, "};\n\n");
}

static int phase2(const char *path) {
  long len;
  char *src = slurp(path, &len);
  if (!src) {
    fprintf(stderr, "打不开 %s\n", path);
    return 1;
  }

  unsigned int tmp[TAB_LEN];
  if (read_array(src, "scramble", tmp, 256)) {
    fprintf(stderr, "找不到 scramble[]\n");
    return 1;
  }
  for (int i = 0; i < 256; i++)
    g_scramble[i] = (unsigned short)tmp[i];
  if (read_array(src, "mph_perf_tab", tmp, TAB_LEN)) {
    fprintf(stderr, "找不到 mph_perf_tab[]\n");
    return 1;
  }
  for (int i = 0; i < TAB_LEN; i++)
    g_tab[i] = (unsigned char)tmp[i];

  /* 从生成的哈希函数体里取出本次 mph 跑出来的常数 */
  if (read_const(src, "val += 0x", 16, &g_salt) ||
      read_const(src, "b = (val >> ", 10, &g_shift_b) ||
      read_const(src, "a = (val + (val << ", 10, &g_shift_a1)) {
    fprintf(stderr, "解析哈希常数失败\n");
    return 1;
  }
  {
    const char *p = strstr(src, "b = (val >> ");
    p = strstr(p, "& 0x");
    g_mask_b = (unsigned int)strtoul(p + 2, NULL, 0);
    p = strstr(src, "a = (val + (val << ");
    p = strstr(p, ")) >> ");
    g_shift_a2 = (unsigned int)strtoul(p + 6, NULL, 0);
  }
  fprintf(
      stderr, "salt=0x%x shift_b=%u mask_b=0x%x shift_a=%u/%u\n", g_salt,
      g_shift_b, g_mask_b, g_shift_a1, g_shift_a2);

  for (int i = 0; i < TAB_LEN; i++)
    g_folded[i] = g_scramble[g_tab[i]];

  g_nkey = 0;
  walk(0, 7, emit_value);
  fprintf(stderr, "键 %d, 冲突 %d\n", g_nkey, g_collide);
  if (g_collide) {
    fprintf(stderr, "完美哈希不成立, 重跑 mph\n");
    return 1;
  }

  FILE *f = fopen("texas_array7.h", "w");
  fprintf(f, "/* 由 texas_generate7 生成, 勿手改 */\n");
  fprintf(f, "#ifndef CMG_TEXAS_ARRAY7_H\n#define CMG_TEXAS_ARRAY7_H\n\n");
  fprintf(f, "#define TEXAS_EVAL7_SALT 0x%xu\n", g_salt);
  fprintf(f, "#define TEXAS_EVAL7_SHIFT_B %uu\n", g_shift_b);
  fprintf(f, "#define TEXAS_EVAL7_MASK_B 0x%xu\n", g_mask_b);
  fprintf(f, "#define TEXAS_EVAL7_SHIFT_A1 %uu\n", g_shift_a1);
  fprintf(f, "#define TEXAS_EVAL7_SHIFT_A2 %uu\n\n", g_shift_a2);
  fprintf(f, "/* 完美哈希的位移表, scramble 已折叠进来 */\n");
  fprintf(f, "extern const unsigned short texas_eval7_hash_tab[];\n\n");
  fprintf(f, "/* 非同花 7 张的牌力值, 下标由 texas_eval7_mph 算出 */\n");
  fprintf(f, "extern const unsigned short texas_eval7_values[];\n\n");
  fprintf(
      f, "/* 下标 (card >> 8) & 0xFF; 低 32 位是 5^rank, 高 32 位是花色计数器 "
         "*/\n");
  fprintf(f, "extern const unsigned long long texas_eval7_card[];\n\n");
  fprintf(f, "#endif /* CMG_TEXAS_ARRAY7_H */\n");
  fclose(f);

  f = fopen("texas_array7.c", "w");
  fprintf(
      f,
      "/* 由 texas_generate7 生成, 勿手改 */\n#include \"texas_array7.h\"\n\n"
      "/* clang-format off */\n");
  dump_u16(f, "texas_eval7_hash_tab", g_folded, TAB_LEN);
  dump_u16(f, "texas_eval7_values", g_values, RANGE);
  fprintf(f, "const unsigned long long texas_eval7_card[] = {\n");
  for (int i = 0; i < 256; i++) {
    unsigned int r = (unsigned)i & 0xFu, s = ((unsigned)i >> 4u) & 0xFu;
    int si = s == 1 ? 0 : s == 2 ? 1 : s == 4 ? 2 : s == 8 ? 3 : -1;
    unsigned long long v = 0;
    if (r < 13 && si >= 0)
      v = (unsigned long long)pow5[r] | (1ULL << (32 + 8 * si));
    fprintf(f, "%s%lluULL,", (i % 3 == 0) ? "\n    " : " ", v);
  }
  fprintf(f, "\n};\n/* clang-format on */\n");
  fclose(f);
  fprintf(stderr, "已写出 texas_array7.h / texas_array7.c\n");
  free(src);
  return 0;
}

int main(int argc, char *argv[]) {
  pow5[0] = 1;
  for (int i = 1; i < 13; i++)
    pow5[i] = pow5[i - 1] * 5;

  if (argc >= 2)
    return phase2(argv[1]);

  const char *out = "temp_7card_keys.txt";
  g_keyfp = fopen(out, "w");
  if (!g_keyfp)
    return 1;
  walk(0, 7, emit_key);
  fclose(g_keyfp);
  printf("已写出 %s (%d 个键)\n", out, g_nkey);
  printf("接着跑:\n");
  printf("  $ mph -dpf < %s\n", out);
  printf("  $ texas_generate7 perf_hash.c\n");
  return 0;
}
