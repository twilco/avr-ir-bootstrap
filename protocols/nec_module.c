#include <stdbool.h>
#include "../constants.h"
#include "../typedefs.h"
#include "nec_module.h"
#include "../util/general_util.h"

bool is_nec_header(Segment header_segments[]) {
    if(header_segments[0].is_mark && within_range(WIGGLE_ROOM, NEC_HEADER_MARK_US, header_segments[0].microseconds)
    && header_segments[1].is_space && within_range(WIGGLE_ROOM, NEC_HEADER_SPACE_US, header_segments[1].microseconds)) {
        return true;
    }
    return false;
}

int8_t nec_data_bit_from_pair(Pair mark_and_space) {
    if(within_range(WIGGLE_ROOM, NEC_MARK_US, mark_and_space.mark.microseconds) && within_range(WIGGLE_ROOM, NEC_SPACE_LOGICAL_ZERO_US, mark_and_space.space.microseconds)) {
        return 0;
    } else if(within_range(WIGGLE_ROOM, NEC_MARK_US, mark_and_space.mark.microseconds) && within_range(WIGGLE_ROOM, NEC_SPACE_LOGICAL_ONE_US, mark_and_space.space.microseconds)) {
        return 1;
    }
    return INVALID_PAIR_TIMINGS;
}
