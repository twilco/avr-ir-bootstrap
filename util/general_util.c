#include "general_util.h"
#include "../avr_config.h"

#include <stdint.h>

#define MAX_16_BIT_INT 65535
#define MICROSECONDS_IN_SECOND_F 1000000.0
#define MICROSECOND_MULT_RATIO (TC1_PRESCALER * MICROSECONDS_IN_SECOND_F) / F_CPU

uint16_t ticks_to_microseconds(uint32_t ticks) {
    return ticks * MICROSECOND_MULT_RATIO;
}

uint16_t calculate_segment_ticks(uint32_t segment_start, uint32_t segment_end) {
    if(segment_start > segment_end) {
        //the timer must've overflown between the time the segment started and when it ended,
        //so let's take that in to account when calculating the segment length
        return (MAX_16_BIT_INT - segment_start) + segment_end;
    }
    return segment_end - segment_start;
}

bool within_range(int16_t range, int16_t expected, int16_t actual) {
    int16_t min = expected - range;
    int16_t max = range + expected;
    if(actual < min || actual > max) return false;
    return true;
}
