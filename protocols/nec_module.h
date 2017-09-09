#ifndef NEC_MODULE_H_
#define NEC_MODULE_H_

#define NUM_NEC_DATA_BITS 32
#define NEC_HEADER_MARK_US 9000
#define NEC_HEADER_SPACE_US 4500

#define NEC_MARK_US 562
#define NEC_SPACE_LOGICAL_ZERO_US 562
#define NEC_SPACE_LOGICAL_ONE_US 1687

bool is_nec_header(struct Segment header_segments[]);
int8_t nec_data_bit_from_pair(struct Pair mark_and_space);

#endif /* NEC_MODULE_H_ */