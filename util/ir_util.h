#ifndef IR_UTIL_H_
#define IR_UTIL_H_

#include "../types/ir_types.h"
#include "../types/errors.h"
#include "../protocols/nec_module.h"
#include <stdint.h>

int16_t get_bit_position(Protocol_Type protocol, uint8_t bit_counter);
Protocol_Type protocol_from_header(struct Segment header_segments[], uint8_t size);
void reinitialize_segment(struct Segment *segment);
void reinitialize_pair(struct Pair *pair);

#endif /* IR_UTIL_H_ */