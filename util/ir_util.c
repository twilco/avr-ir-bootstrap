#include "ir_util.h"

extern const uint8_t NUM_NEC_DATA_BITS;

int16_t get_bit_position(Protocol_Type protocol, uint8_t bit_counter) 
{
    if(protocol == UNKNOWN) {
        return NO_BIT_POS_FOR_UNKNOWN;
    } else if(protocol == NEC) {
        /* 
          NEC transmits MSB first, so the bit to set is the opposite of the counter in relation 
          to the total number of data bits for the protocol
        */
        int16_t pos = NUM_NEC_DATA_BITS - 1 - bit_counter;
        if(pos < 0) {
            return BIT_POS_CALC_ERROR;
        }
        return pos;
    } 
    return INVALID_PROTOCOL;
}

Protocol_Type protocol_from_header(struct Segment header_segments[], uint8_t num_segments)
{
    if(num_segments == 1) return UNKNOWN;
    if(num_segments == 2) {
        if(is_nec_header(header_segments)) {
            return NEC;
        }
    }
    return UNKNOWN;
}

void reinitialize_segment(struct Segment *segment) 
{
    segment->is_space = false;
    segment->is_mark = false;
    segment->microseconds = 0;
}

void reinitialize_pair(struct Pair *pair) 
{
    pair->has_mark = false;
    pair->has_space = false;
    reinitialize_segment(&pair->mark);
    reinitialize_segment(&pair->space);
}