#ifndef GENERAL_UTIL_H_
#define GENERAL_UTIL_H_

#define BIT_SET(a,b) ((a) |= (1UL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1UL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1UL<<(b)))
#define BIT_CHECK(a,b) ((a) & (1UL<<(b)))

#include <avr/io.h>
#include "../typedefs.h"

bool within_range(int16_t range, int16_t expected, int16_t actual);
int ticks_to_microseconds(uint32_t ticks);
int calculate_segment_ticks(uint32_t segment_start, uint32_t segment_end);
int get_bit_position(Protocol_Type protocol, uint8_t bit_counter);
bool transmits_msb_first(Protocol_Type protocol);

#endif /* GENERAL_UTIL_H_ */