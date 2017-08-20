#ifndef NEC_MODULE_H_
#define NEC_MODULE_H_

#define NUM_NEC_DATA_BITS 32
#define NEC_HEADER_MARK_US 9000
#define NEC_HEADER_SPACE_US 4500

bool is_nec_header(Segment header_segments[]);

#endif /* NEC_MODULE_H_ */