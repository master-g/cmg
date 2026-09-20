# Texas Hold'em 牌力评估

德州扑克手牌评估器，复刻自 Cactus Kev (Kevin Suffecool) 的 5 张算法与 Paul Senzee
的完美哈希改进，7 张部分是本仓库自己加的。

两个入口：

| 函数 | 用途 | 速度 |
| --- | --- | --- |
| `texas_eval_5hand(c1..c5)` | 5 张求值 | 2.6 ns/手 |
| `texas_eval_7hand_fast(cards)` | 7 张求值 | 2.5 ns/手 |
| `texas_eval_7hand(c1..c7)` | 7 张参考实现，枚举 21 个 5 张子集取最小 | 29.4 ns/手 |

返回值 1 (皇家同花顺) 到 7462 (最差高牌)，**越小越强**。`texas_eval_hand_rank()`
把它归到 9 个牌型之一。7 张牌只能取到其中 4824 个值——比如 7 张里不可能"最好只有 7 高"。

`texas_eval_7hand` 保留着，它是 `texas_test` 的真值来源，删掉就没有独立基准了。

## 目录分两层

这里住着**两套代码**，看代码时先分清在哪一层，否则很容易误判：

```
生成器 (离线跑一次, 产出查找表)        运行时 (实际被调用)
  bits.c        位操作辅助              texas_eval.c   5 张 + 7 张参考实现
  generate.c    造 5 张的表             texas_eval7.c  7 张快速实现
  main.c        5 张生成器入口          texas_array.c  5 张的表   (生成物)
  texas_gen7.c  7 张生成器入口          texas_array7.c 7 张的表   (生成物)
  answer.c      5 张表的参考副本        texas_utils.c  洗牌/排序/显示
```

`answer.c` 是从原作者那里抄来的参考数组（`eval_*` 命名），只给测试对拍用，不参与运行时。
`texas_array.c` / `texas_array7.c` 才是真正被查的表（`texas_eval_*` 命名）。
**两者曾经只有前者有测试覆盖**，现在 `test_runtime_arrays()` 补上了。

## 牌的编码

一张牌是 32 位，四个字段各回答一个不同的问题：

```
+--------+--------+--------+--------+
|xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
+--------+--------+--------+--------+

b = 点数位图 (16-28)  5 张一 OR, 得到 13 位集合 -> 一次查表判掉顺子和高牌
c/d/h/s = 花色 (12-15) 5 张一 AND, 非零即同花
r = 点数 (8-11)       deuce=0, trey=1, ..., ace=12
p = 点数对应的素数 (0-7)  deuce=2, trey=3, ..., ace=41
```

素数那一段是关键：素数分解唯一，所以 5 张的乘积唯一标识一个**点数多重集**（不含顺序、
不含花色）。对子、三条、葫芦这些"重复结构"就被压成了一个整数。

## 5 张怎么算

三路分派，表合计约 49 KB：

1. 五张花色位一 AND 非零 → 同花，查 `texas_eval_flushes[q]`，`q` 是点数位图的 OR
2. 否则查 `texas_eval_unique5[q]`，非零说明五个点数互不相同（顺子或高牌）
3. 否则算五个素数的乘积，过 `texas_eval_mph_search()` 完美哈希，查 `texas_eval_hash_values[]`

第 3 步的哈希覆盖 4888 个键（四条 156 + 葫芦 156 + 三条 858 + 两对 858 + 一对 2860）。

## 7 张怎么算

关键观察：**不含同花的 7 张牌，牌力只取决于点数多重集**，跟具体是哪几张无关。
这样的多重集只有 **49,205** 个。于是不必枚举 21 个子集：

```c
/* 七次载入同时拿到哈希键和花色直方图 */
for (i = 0; i < 7; i++) acc += texas_eval7_card[(cards[i] >> 8u) & 0xFFu];
/*  低 32 位: 每个点数的出现次数编成五进制码 (5^13 刚好塞进 32 位)
    高 32 位: 每个花色一个字节的张数计数器 */

hit = ((unsigned)(acc >> 32) + 0x03030303u) & 0x08080808u;
/*  某字节 +3 后进位到 bit3, 当且仅当该花色 >= 5 张 */
```

- `hit` 非零 → 走同花分支。7 张里某花色满 5 张时，**葫芦和四条在组合上不可能**
  （四条需要同点数 4 张不同花色，同花色内点数互异所以最多贡献 1 张，剩下 ≤2 张不够；
  葫芦同理），所以答案必是同花或同花顺。取该花色的点数位图，先比 10 个顺子位型，
  否则剥到最大 5 张，查 `texas_eval_flushes[]`。
- `hit` 为零 → 五进制码过完美哈希，一次查表出结果。

表合计约 160 KB（位移表 32 KB + 值表 128 KB + 卡片表 2 KB）。
拿 11.9× 换 160 KB，如果哪天要嵌到内存受限的地方，这个取舍需要重新算。

## 重新生成查找表

改了生成器才需要跑，产物已经提交进仓库。两套表各自独立。

### 5 张的表

```sh
texas_generate                         # -> gen_arr.c, temp_other_keys.txt, temp_other_gen.c
mph -dps < temp_other_keys.txt         # -> perf_hash.c/h   (mph 见 src/mph)
```

然后按 `temp_other_gen.c` 的提示把哈希值表填进 `texas_array.c`。哈希常数目前是手工内联在
`texas_eval_mph_search()` 里的，重跑 mph 换了参数要手动同步。

### 7 张的表

```sh
texas_generate7                        # -> temp_7card_keys.txt (49205 个键)
mph -dpf < temp_7card_keys.txt         # -> perf_hash.c/h
texas_generate7 perf_hash.c            # -> texas_array7.c / texas_array7.h
```

第二阶段会自己从 `perf_hash.c` 里解析出 salt 和位移常数写进头文件，所以重跑 mph 换了
哈希参数**不用改代码**。冲突不为零会直接报错退出，重跑 mph 即可。

注意 `mph` 生成的 `perf_hash.c` 本身编译不过（`tab[b]` 应为 `mph_perf_tab[b]`，
头文件里的 `mph_perf_s` 签名也和实现不符），所以生成器是当文本解析它，不是链接它。

生成的表体带 `/* clang-format off */`，`make fmt` 不会重排它们。

## 测试

```sh
make build TARGET=texas_test && ./bin/texas_test
```

五条，全部是"重新生成一遍再逐项对拍"，不是快照比对：

| 测试 | 覆盖 |
| --- | --- |
| `test_flushes` / `test_unique5` / `test_others` | `generate.c` 对 `answer.c` 参考副本 |
| `test_runtime_arrays` | 运行时的 `texas_array.c`，含完整的素数积→哈希→取值路径 |
| `test_7card_table` | 49,205 个多重集逐个对拍，外加 40 万手随机牌覆盖同花路径 |

全量 C(52,7) = 133,784,560 手的对拍跑一轮约 4 秒，太慢没进常规测试，改动评估器时值得单独跑一次。
上次跑的结果是零不一致。

## 命令行

```sh
$ texas_eval AS KS QS JS 10S            # 5 张
eval value: 1 (Straight Flush)

$ texas_eval 2C 2D 2H 2S 3C 3D 7H       # 7 张, 走快速路径
eval value: 162 (Four of a Kind)
```

点数写 `2`-`10` / `J` / `Q` / `K` / `A`，花色写 `C` / `D` / `H` / `S`。

## 已知的粗糙处

- `texas_utils_cards_sort()` 的 `default` 分支只判 `value == 10`（钢轮同花顺），
  普通 A5432 顺子 (value 1609) 走不到，会显示成 `A5432` 而不是 `5432A`
- `texas_utils_dump_list()` 里对全排列做 `shuffle_deck(deck, time(NULL))` 没有意义，
  只是让同值牌的输出顺序每次都变；该函数还要连续分配 260 万个节点（RSS 约 211 MB）且不检查 `malloc`
- `TEXAS_CARD_RANK(x)` 的 `x` 没加括号，`TEXAS_CARD_RANK(a|b)` 会算错
- `texas_eval_is_loyal_straight_flush` —— royal 拼成了 loyal
- `define.h` 是空文件
