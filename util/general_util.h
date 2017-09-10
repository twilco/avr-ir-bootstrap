#ifndef GENERAL_UTIL_H_
#define GENERAL_UTIL_H_

#include "../avr_config.h"
#include "../types/errors.h"
#include "../protocols/nec_module.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define BIT_SET(a,b) ((a) |= (1UL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1UL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1UL<<(b)))
#define BIT_CHECK(a,b) ((a) & (1UL<<(b)))

#define MAX_16_BIT_INT 65535
#define MICROSECONDS_IN_SECOND_F 1000000.0
#define MICROSECOND_MULT_RATIO (TC1_PRESCALER * MICROSECONDS_IN_SECOND_F) / F_CPU

bool within_range(int16_t range, int16_t expected, int16_t actual);
uint16_t ticks_to_microseconds(uint32_t ticks);
uint16_t calculate_segment_ticks(uint32_t segment_start, uint32_t segment_end);

#endif /* GENERAL_UTIL_H_ */