/* 由 texas_generate7 生成, 勿手改 */
#ifndef CMG_TEXAS_ARRAY7_H
#define CMG_TEXAS_ARRAY7_H

#define TEXAS_EVAL7_SALT 0xd3d42ceu
#define TEXAS_EVAL7_SHIFT_B 3u
#define TEXAS_EVAL7_MASK_B 0x3fffu
#define TEXAS_EVAL7_SHIFT_A1 15u
#define TEXAS_EVAL7_SHIFT_A2 16u

/* 完美哈希的位移表, scramble 已折叠进来 */
extern const unsigned short texas_eval7_hash_tab[];

/* 非同花 7 张的牌力值, 下标由 texas_eval7_mph 算出 */
extern const unsigned short texas_eval7_values[];

/* 下标 (card >> 8) & 0xFF; 低 32 位是 5^rank, 高 32 位是花色计数器 */
extern const unsigned long long texas_eval7_card[];

#endif /* CMG_TEXAS_ARRAY7_H */
