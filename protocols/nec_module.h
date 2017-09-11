#ifndef NEC_MODULE_H_
#define NEC_MODULE_H_

#include "../util/general_util.h"
#include "../types/ir_types.h"
#include "../types/errors.h"

#include <stdbool.h>
#include <stdint.h>

bool is_nec_header(struct Segment header_segments[]);
bool all_data_bits_received(uint8_t data_bit_counter);
int8_t nec_data_bit_from_pair(struct Pair mark_and_space);

#endif /* NEC_MODULE_H_ */