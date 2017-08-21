#ifndef GENERAL_UTIL_H_
#define GENERAL_UTIL_H_

#include <avr/io.h>

bool within_range(int16_t range, int16_t expected, int16_t actual);
int ticks_to_microseconds(uint32_t ticks);
int calculate_segment_ticks(uint32_t segment_start, uint32_t segment_end);

#endif /* GENERAL_UTIL_H_ */