#include <stdbool.h>
#include "../constants.h"

int ticks_to_microseconds(int ticks) {
    return ticks * MICROSECOND_MULT_RATIO;
}

int calculate_segment_ticks(int segment_start, int segment_end) {
    if(segment_start > segment_end) {
        //the timer must've overflown between the time the segment started and when it ended,
        //so let's take that in to account when calculating the segment length
        return (MAX_16_BIT_INT - segment_start) + segment_end;
    }
    return segment_end - segment_start;
}

bool within_range(int range, int expected, int actual) {
    int min = expected - range;
    int max = range + expected;
    if(actual < min || actual > max) return false;
    return true;
}
