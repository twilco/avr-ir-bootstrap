#define F_CPU 16000000L
#define PRESCALER 8
#define MAX_16_BIT_INT 65535
#define HEADER_ARR_SIZE 15
#define MICROSECONDS_IN_SECOND_F 1000000.0
#define MICROSECOND_MULT_RATIO (PRESCALER * MICROSECONDS_IN_SECOND_F) / F_CPU

#define BLUE_LED PINC0
#define RED_LED PINC3

#include <avr/io.h>
#include <avr/interrupt.h>
/* bools take a a byte each and there are more memory efficient ways to accomplish 
   bool-like values, but I'm not strapped for space and will take a memory hit in favor of readability */
#include <stdbool.h>

typedef enum protocol_type {
    UNKNOWN,
    NEC
} protocol_type;

typedef struct Segment {
    bool is_mark;
    bool is_space;
    int microseconds;
} Segment;

bool icp_listening_for_rising();
bool icp_listening_for_falling();
bool is_nec_header(Segment header_segments[]);
bool within_range(int range, int expected, int actual);
int ticks_to_microseconds(int ticks);
protocol_type protocol_from_header(Segment header_segments[], int size);
int calculate_segment_ticks(int segment_start, int segment_end);
void icp_listen_for_rising();
void icp_listen_for_falling();
void process_new_header_segment(Segment new_segment);
void process_new_segment(Segment new_segment);
void reset();

volatile int mark_start = -1, mark_end = -1, space_start = -1, space_end = -1;
volatile int selected_protocol = UNKNOWN;
volatile int free_header_index = 0;
Segment header_segments[HEADER_ARR_SIZE];

int main(void) {
    /* Set PINB1 as an output */
    DDRB = (1 << PINB1);
    
    DDRC = (1 << BLUE_LED) | (1 << RED_LED);
	
    /*
        ICNC1 high to turn on input capture noise canceling
    */
    TCCR1B = (1 << ICNC1);
	
    /* ICIE1 high to enable the input capture interrupt */
    /* TOIE0 high to enable the Timer1 overflow interrupt */
    TIMSK1 = (1 << ICIE1) | (1 << TOIE0);
    sei();
	 
    /* Start Timer1 with a prescaler of 8 */
    TCCR1B |= (1 << CS11);
    /*
	- Prescaler of 8 will give our input capture the ability to capture events with
	lengths from .5 microseconds (1 tick) to 32767 microseconds (65535 ticks) <-- assuming 16MHz clock
		
	- Prescaler of 1 (no prescaler) can only capture a maximum event (without guaranteed overflow, which would be annoying to consistently deal with)
	of 4095 microseconds.  This is inadequate, considering the header high-segment of the NEC protocol is 9000 microseconds (9ms) <-- assuming 16MHz clock
    */
    while (1) {
        if(mark_start != -1 && mark_end != -1) {
            //a new mark has been closed (start and end have event times are not -1) - let's process this new mark
            int mark_ticks = calculate_segment_ticks(mark_start, mark_end);
            mark_start = -1;
            mark_end = -1;
            struct Segment new_segment;
            new_segment.is_mark = true;
            new_segment.is_space = false;
            new_segment.microseconds = ticks_to_microseconds(mark_ticks);
            process_new_segment(new_segment);
        }
        
        if(space_start != -1 && space_end != -1) {
            //a new space has been closed (start and end have event times are not -1) - let's process this new space
            int space_ticks = calculate_segment_ticks(space_start, space_end);
            space_start = -1;
            space_end = -1;
            struct Segment new_segment;
            new_segment.is_mark = false;
            new_segment.is_space = true;
            new_segment.microseconds = ticks_to_microseconds(space_ticks);
            process_new_segment(new_segment);
        }     
    }
}

ISR(TIMER1_CAPT_vect) {
    if(icp_listening_for_falling()) {
        mark_start = ICR1;
        if(space_start != -1) {
            //we don't want to set space_end if we haven't started timing a space yet
            //e.g. this is the first pulse of a full burst, and it is falling (the start of a mark in a TSOP382)
            space_end = ICR1;
        }
        icp_listen_for_rising(); 
    } else if(icp_listening_for_rising()) {
        if(mark_start != -1) {
            //we don't want to set mark_end if we haven't started timing a mark yet
            //e.g. this is the first pulse of a full burst, and it is rising
            mark_end = ICR1;
        }
        space_start = ICR1;
        icp_listen_for_falling();
    }        
}

ISR(TIMER1_OVF_vect) {
    PORTC = 0x0; //clear any debug LEDs set on PORTC
}

bool icp_listening_for_rising() {
    if(TCCR1B & (1 << ICES1)) {
        return true;
    }
    return false;
}

bool icp_listening_for_falling() {
    return !icp_listening_for_rising();
}

void icp_listen_for_rising() {
    TCCR1B |= (1 << ICES1);
}

void icp_listen_for_falling() {
    TCCR1B ^= (1 << ICES1);
}

protocol_type protocol_from_header(Segment header_segments[], int size) {
    if(size == 1) return UNKNOWN;
    if(size == 2) {
        if(is_nec_header(header_segments)) {
            return NEC;
        }
    }
    return UNKNOWN;
}

int ticks_to_microseconds(int ticks) {
    return ticks * MICROSECOND_MULT_RATIO;
}

int calculate_segment_ticks(int segment_start, int segment_end) {
    if(segment_start > segment_end) {
        //the timer must've overflown between the time the segment started and when it ended,
        //so let's take that in to account when calculating the segment length
        return (MAX_16_BIT_INT - segment_start) + segment_end;
    }
    return segment_end - segment_start;
}


bool is_nec_header(Segment header_segments[]) {
    if(header_segments[0].is_mark && within_range(125, 9000, header_segments[0].microseconds)
       && header_segments[1].is_space && within_range(125, 4500, header_segments[1].microseconds)) {
        return true;
    }
    return false;
}

bool within_range(int range, int expected, int actual) {
    int min = expected - range;
    int max = range + expected;
    if(actual < min || actual > max) return false;
    return true;
}

void process_new_segment(Segment new_segment) {
    if(selected_protocol == UNKNOWN) {
        //this segment must be part of the header since we haven't matched the header to a protocol yet
        process_new_header_segment(new_segment);
    } else if(selected_protocol == NEC) {
        //this is a logical data segment for the NEC protocol
    } else {
        //this probably shouldn't happen - let's turn on our red led to indicate we're in the dangerzone
        PORTC |= (1 << RED_LED);
    }
}

void process_new_header_segment(Segment new_segment) {
    if(free_header_index >= HEADER_ARR_SIZE) {
        //something went wrong, we haven't matched a header to a protocol yet
        //we don't want to add this mark to the header array because we'll be out of bounds.  let's reset
        reset();
    } else {
        header_segments[free_header_index++] = new_segment;
        selected_protocol = protocol_from_header(header_segments, sizeof(header_segments) / sizeof(header_segments[0]));
    }
}

void reset() {
    //ignore all IR pulses for set period (e.g. 300ms) to wait out the current borked pulse and reset all variables to get a clean slate
}
