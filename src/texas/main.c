//
// Created by MasterG on 2020/5/18.
//

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "generate.h"

void dump_array(FILE *fp, const char *name, uint16_t *arr, int len) {
  const int max_columns = 12;
  char buf[120];
  int row_num = len / max_columns;
  int tail = len % max_columns;

  fprintf(fp, "unsigned short %s[] = {\n", name);

  for (int row = 0; row <= row_num; row++) {
    memset(buf, 0, sizeof(buf));
    int columns;
    int is_tail;
    if (row == row_num) {
      columns = tail;
      is_tail = 1;
    } else {
      columns = max_columns;
      is_tail = 0;
    }

    for (int col = 0; col < columns; col++) {
      char blob[6];
      int offset = row * columns;
      int buf_off = 6 * col; // strlen("%4d, ") = 6
      uint16_t v = arr[col + offset];
      int n;
      if (is_tail) {
        n = snprintf(blob, sizeof(blob), "%d,", v);
      } else {
        n = snprintf(blob, sizeof(blob), "%d,", v);
      }

      if (col == columns - 1) {
        memset(blob + n, '\0', 6 - n);
      } else {
        memset(blob + n, ' ', 6 - n);
      }
      snprintf(buf + buf_off, sizeof(buf), "%s", blob);
    }
    fprintf(fp, "    %s\n", buf);
  }

  fprintf(fp, "};\n");
}

void dump_array_32(FILE *fp, const char *name, uint32_t *arr, int len) {
  char buf[120];

  fprintf(fp, "unsigned int %s[] = {\n", name);

  int pos = 0;
  while (pos < len) {
    int trylen = 0;
    int str_len = 0;
    while (1) {
      trylen++;
      if (pos + trylen > len) {
        trylen = len - pos;
        break;
      }

      str_len = 0;
      for (int i = 0; i < trylen; i++) {
        str_len +=
            snprintf(buf + str_len, sizeof(buf) - str_len, "%d,", arr[pos + i]);
      }
      if (str_len > 80) {
        trylen--;
        break;
      }
    }

    memset(buf, 0, sizeof(buf));
    str_len = 0;
    for (int i = 0; i < trylen; i++) {
      str_len +=
          snprintf(buf + str_len, sizeof(buf) - str_len, "%d,", arr[pos + i]);
    }
    pos += trylen;
    fprintf(fp, "%s\n", buf);
  }

  fprintf(fp, "};\n");
}

void populate_flushes(FILE *fp) {
  uint16_t flushes[MAGIC_UNIQUE5_SIZE] = {0};
  texas_gen_flushes(flushes);

  fprintf(fp, "/**\n");
  fprintf(fp,
          " * this is a table lookup for all \"flush\" hands (e.g.  both\n");
  fprintf(fp, " * flushes and straight-flushes).  entries containing a zero\n");
  fprintf(fp, " * mean that combination is not possible with a five-card\n");
  fprintf(fp, " * flush hand.\n");
  fprintf(fp, " */\n");

  dump_array(fp, "texas_eval_flushes", flushes, MAGIC_UNIQUE5_SIZE);
}

void populate_unique5(FILE *fp) {
  uint16_t unique5[MAGIC_UNIQUE5_SIZE] = {0};
  texas_gen_unique5(unique5);

  fprintf(fp, "/**\n");
  fprintf(fp, " * this is a table lookup for all non-flush hands consisting\n");
  fprintf(fp, " * of five unique ranks (i.e.  either Straights or High Card\n");
  fprintf(fp, " * hands).  it's similar to the above \"flushes\" array.\n");
  fprintf(fp, " */\n");

  dump_array(fp, "texas_eval_unique5", unique5, MAGIC_UNIQUE5_SIZE);
}

void populate_others_key(FILE *fp) {
  texas_magic_kv_t *list = NULL;
  texas_magic_kv_t *iter = NULL;
  texas_gen_others(&list);

  iter = list;
  while (iter != NULL) {
    fprintf(fp, "%d\n", iter->product);
    iter = iter->next;
  }

  texas_free_others(list);
}

void populate_others_gen(FILE *fp) {
  uint32_t keys[4888] = {0};
  uint32_t values[4888] = {0};
  texas_magic_kv_t *list = NULL;
  texas_magic_kv_t *iter = NULL;
  texas_gen_others(&list);

  int i = 0;
  iter = list;
  while (iter != NULL) {
    keys[i] = iter->product;
    values[i] = iter->magic;
    iter = iter->next;
    i++;
  }

  texas_free_others(list);

  fprintf(fp, "#include <stdio.h>\n");
  fprintf(fp, "#include \"perf_hash.h\"");
  fprintf(fp, "\n\n");
  dump_array_32(fp, "hash_keys", keys, sizeof(keys) / sizeof(keys[0]));
  fprintf(fp, "\n");
  dump_array_32(fp, "hash_values", values, sizeof(values) / sizeof(values[0]));
  fprintf(fp, "\n");
  fprintf(fp, "int main(int argc, char* argv[]) {\n");
  fprintf(fp, "uint16_t hashes[MPH_PERF_RANGE] = {0};\n");
  fprintf(
      fp,
      "for (int i = 0; i < sizeof(hash_keys)/sizeof(hash_keys[0]); i++) {\n");
  fprintf(fp, "uint32_t pos = mph_perf_s(hash_keys[i]);\n");
  fprintf(fp, "hashes[pos] = hash_values[i];\n");
  fprintf(fp, "}\n\n");
  fprintf(fp, "printf(\"unsigned short eval_other_hashes[] = {\\n\");\n");
  fprintf(fp, "for (int i = 0; i < MPH_PERF_RANGE; i++) {\n");
  fprintf(fp, "printf(\"%%d,\", hashes[i]);\n");
  fprintf(fp, "if (i != 0 && i %% 16 == 0) {\n");
  fprintf(fp, "printf(\"\\n\");\n");
  fprintf(fp, "}\n");
  fprintf(fp, "}\n");
  fprintf(fp, "printf(\"\\n};\\n\");\n");

  fprintf(fp, "return 0;\n}\n");
}

int main(int argc, char *argv[]) {
  // flushes and unique5
  char *filename = "gen_arr.c";
  FILE *fp = fopen(filename, "w+");
  if (fp == NULL) {
    return errno;
  }

  populate_flushes(fp);
  fprintf(fp, "\n");
  populate_unique5(fp);

  fclose(fp);

  // others
  filename = "temp_other_keys.txt";
  fp = fopen(filename, "w+");
  if (fp == NULL) {
    return errno;
  }

  populate_others_key(fp);
  fclose(fp);

  printf("run following command to generate minimal-perfect-hash\n");
  printf("you can find mph's source code here "
         "https://burtleburtle.net/bob/hash/perfect.html\n");
  printf("or a more modern version "
         "https://github.com/driedfruit/jenkins-minimal-perfect-hash\n");
  printf("$ mph -dps < temp_other_keys.txt\n");

  // source
  filename = "temp_other_gen.c";
  fp = fopen(filename, "w+");
  if (fp == NULL) {
    return errno;
  }

  populate_others_gen(fp);

  return 0;
}
