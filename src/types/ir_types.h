#ifndef IR_TYPES_H_
#define IR_TYPES_H_

#include <stdio.h>
#include <stdbool.h>

struct Segment {
    bool is_mark;
    bool is_space;
    uint16_t microseconds;
} Segment;

struct Pair {
    struct Segment mark;
    struct Segment space;
    bool has_mark;
    bool has_space;
} Pair;

typedef enum Protocol_Type {
    UNKNOWN,
    NEC,
    NEC_REPEAT
} Protocol_Type;

#endif /* IR_TYPES_H_ */