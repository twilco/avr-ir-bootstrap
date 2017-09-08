#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <avr/io.h>

typedef enum Protocol_Type {
    UNKNOWN,
    NEC
} Protocol_Type;

typedef enum Error_Codes {
    INVALID_PAIR_TIMINGS = -120,
    NO_BIT_POS_FOR_UNKNOWN,
    BIT_POS_CALC_ERROR,
    INVALID_PROTOCOL
} Error_codes;

typedef struct Segment {
    bool is_mark;
    bool is_space;
    uint16_t microseconds;
} Segment;

typedef struct Pair {
    Segment mark;
    Segment space;
    bool has_mark;
    bool has_space;
} Pair;

#endif /* TYPEDEFS_H_ */