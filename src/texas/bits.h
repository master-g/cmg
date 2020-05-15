//
// Created by MasterG on 2020/5/15.
//

#ifndef CMD_TEXAS_BITS_H
#define CMD_TEXAS_BITS_H

#include <stdint.h>

/**
 * convert uint32_t value to binary string
 *
 * @param v
 * @param buf
 * @return bytes needed for the string
 */
int texas_bits_str_32(uint32_t v, char *buf);

/**
 * convert uint16_t value to binary string
 *
 * @param v
 * @param buf
 * @return bytes needed for the string
 */
int texas_bits_str_16(uint16_t v, char *buf);

/**
 * count the number of bit '1' in uint32_t value
 *
 * @param v
 * @return number of bit '1' in the value
 */
int texas_bits_ones_count_32(uint32_t v);

/**
 * count the number of bit '1' in uint16_t value
 *
 * @param v
 * @return the number of bit '1' in the value
 */
int texas_bits_ones_count_16(uint16_t v);

#endif // CMD_TEXAS_BITS_H
