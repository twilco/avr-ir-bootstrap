#ifndef GENERAL_UTIL_H_
#define GENERAL_UTIL_H_

bool within_range(int range, int expected, int actual);
int ticks_to_microseconds(int ticks);
int calculate_segment_ticks(int segment_start, int segment_end);

#endif /* GENERAL_UTIL_H_ */