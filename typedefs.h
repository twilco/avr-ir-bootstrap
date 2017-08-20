#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

typedef enum Protocol_Type {
    UNKNOWN,
    NEC
} Protocol_Type;

typedef struct Segment {
    bool is_mark;
    bool is_space;
    int microseconds;
} Segment;

#endif /* TYPEDEFS_H_ */