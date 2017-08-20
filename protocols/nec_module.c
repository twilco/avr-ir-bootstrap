#include <stdbool.h>
#include "../typedefs.h"
#include "nec_module.h"
#include "../util/general_util.h"

bool is_nec_header(Segment header_segments[]) {
    if(header_segments[0].is_mark && within_range(125, NEC_HEADER_MARK_US, header_segments[0].microseconds)
    && header_segments[1].is_space && within_range(125, NEC_HEADER_SPACE_US, header_segments[1].microseconds)) {
        return true;
    }
    return false;
}
