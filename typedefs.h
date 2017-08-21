#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <avr/io.h>

typedef enum Protocol_Type {
    UNKNOWN,
    NEC
} Protocol_Type;

typedef struct Segment {
    bool is_mark;
    bool is_space;
    uint16_t microseconds;
} Segment;

typedef struct Pair {
    Segment mark;
    Segment space;
} Pair;

#endif /* TYPEDEFS_H_ */