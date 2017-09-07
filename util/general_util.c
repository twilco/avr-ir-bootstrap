#include <stdbool.h>
#include <avr/io.h>
#include "../constants.h"
#include "../typedefs.h"
#include "../protocols/nec_module.h"

int ticks_to_microseconds(uint32_t ticks) {
    return ticks * MICROSECOND_MULT_RATIO;
}

int calculate_segment_ticks(uint32_t segment_start, uint32_t segment_end) {
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

int get_bit_position(Protocol_Type protocol, uint8_t bit_counter) {
    if(protocol == UNKNOWN) {
        return NO_BIT_POS_FOR_UNKNOWN;
    } else if(protocol == NEC) {
        //these protocols are MSB first, so the bit to set is the opposite of the counter in relation to the total number of data bits for the protocol
        int pos = NUM_NEC_DATA_BITS - 1 - bit_counter;
        if(pos < 0) {
            return BIT_POS_CALC_ERROR;
        }
        return pos;
    } 
    return UNKNOWN_PROTOCOL;
}
