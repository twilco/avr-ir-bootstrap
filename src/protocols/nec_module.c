#include "nec_module.h"

extern const uint16_t IR_SIGNAL_WIGGLE_ROOM_US;

const uint8_t NUM_NEC_DATA_BITS = 32;
const uint16_t NEC_HEADER_MARK_US = 9000;
const uint16_t NEC_HEADER_SPACE_US = 4500;

const uint16_t NEC_MARK_US = 562;
const uint16_t NEC_SPACE_LOGICAL_ZERO_US = 562;
const uint16_t NEC_SPACE_LOGICAL_ONE_US = 1687;

const uint16_t NEC_REPEAT_HEADER_SPACE_US = 2250;

bool is_nec_header(struct Segment header_segments[]) 
{
    if(header_segments[0].is_mark && within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_HEADER_MARK_US, header_segments[0].microseconds)
    && header_segments[1].is_space && within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_HEADER_SPACE_US, header_segments[1].microseconds)) {
        return true;
    }
    return false;
}

bool is_nec_repeat_header(struct Segment header_segments[]) 
{
    if(header_segments[0].is_mark && within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_HEADER_MARK_US, header_segments[0].microseconds)
    && header_segments[1].is_space && within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_REPEAT_HEADER_SPACE_US, header_segments[1].microseconds)
    && header_segments[2].is_mark && within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_MARK_US, header_segments[2].microseconds)) {
        return true;
    }
    return false;
}

int8_t nec_data_bit_from_pair(struct Pair mark_and_space) 
{
    if(within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_MARK_US, mark_and_space.mark.microseconds) && within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_SPACE_LOGICAL_ZERO_US, mark_and_space.space.microseconds)) {
        return 0;
    } else if(within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_MARK_US, mark_and_space.mark.microseconds) && within_range(IR_SIGNAL_WIGGLE_ROOM_US, NEC_SPACE_LOGICAL_ONE_US, mark_and_space.space.microseconds)) {
        return 1;
    }
    return INVALID_PAIR_TIMINGS;
}
