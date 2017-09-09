#include "constants.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
/* bools take a a byte each and there are more memory efficient ways to accomplish 
   bool-like values, but I'm not strapped for space and will take a memory hit in favor of readability */
#include <stdbool.h>
#include "typedefs.h"
#include "util/avr_util.h"
#include "util/general_util.h"
#include "protocols/nec_module.h"
#include "remotes/sparkfun_com_11759.h"

Protocol_Type protocol_from_header(struct Segment header_segments[], int size);
void process_new_header_segment(struct Segment new_segment);
void process_new_segment(struct Segment new_segment);
void hard_reset(); //used when an error occurs, and we need to recover.  resets state and ignores IR pulses for a certain amount of time
void reset(); //an expected reset - reinitialize application state to prepare for next IR pulse

volatile int32_t mark_start = -1, mark_end = -1, space_start = -1, space_end = -1;
volatile uint8_t selected_protocol = UNKNOWN;
volatile uint8_t free_header_index = 0;
volatile uint8_t data_bit_counter = 0;
volatile uint32_t decoded_data = 0;
struct Segment header_segments[HEADER_SEGMENTS_SIZE];
struct Pair data_pair;

int main(void) {
    /* Set PINB1 as an output */
    DDRB = (1 << PINB1);
    
    DDRC = (1 << BLUE_LED) | (1 << RED_LED);
	
    /*
     ICNC1 high to turn on input capture noise canceling
    */
    TCCR1B = (1 << ICNC1);
    
    usart_init();
	
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
            uint16_t mark_ticks = calculate_segment_ticks(mark_start, mark_end);
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
            uint16_t space_ticks = calculate_segment_ticks(space_start, space_end);
            space_start = -1;
            space_end = -1;
            struct Segment new_segment;
            new_segment.is_mark = false;
            new_segment.is_space = true;
            new_segment.microseconds = ticks_to_microseconds(space_ticks);
            process_new_segment(new_segment);
        }
        
        if(selected_protocol != UNKNOWN && data_pair.has_mark == true && data_pair.has_space == true) {
            data_pair.has_mark = false;
            data_pair.has_space = false;
            int8_t bit = nec_data_bit_from_pair(data_pair);
            if(bit != INVALID_PAIR_TIMINGS) {
                if(bit == 1) {
                    int pos = get_bit_position(NEC, data_bit_counter);
                    if(pos == NO_BIT_POS_FOR_UNKNOWN || pos == BIT_POS_CALC_ERROR || pos == INVALID_PROTOCOL) {
                        hard_reset();
                    }
                    BIT_SET(decoded_data, pos);
                }
                data_bit_counter++;
            } else {
                hard_reset();
            }
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
        space_start = ICR1;
        if(mark_start != -1) {
            //we don't want to set mark_end if we haven't started timing a mark yet
            //e.g. this is the first pulse of a full burst, and it is rising
            mark_end = ICR1;
        }
        icp_listen_for_falling();
    }        
}

ISR(TIMER1_OVF_vect) {
    PORTC = 0x0; //clear any debug LEDs set on PORTC
}

Protocol_Type protocol_from_header(struct Segment header_segments[], int num_segments) {
    if(num_segments == 1) return UNKNOWN;
    if(num_segments == 2) {
        if(is_nec_header(header_segments)) {
            return NEC;
        }
    }
    return UNKNOWN;
}

void process_new_segment(struct Segment new_segment) {
    if(selected_protocol == UNKNOWN) {
        //this segment must be part of the header since we haven't matched the header to a protocol yet
        process_new_header_segment(new_segment);
    } else if(selected_protocol == NEC) {
        //this is a logical data segment for the NEC protocol
        if(new_segment.is_mark) {
            data_pair.mark = new_segment;
            data_pair.has_mark = true;
        } else if(new_segment.is_space) {
            data_pair.space = new_segment;
            data_pair.has_space = true;
        } else {
            hard_reset();
        }
    } else {
        //this probably shouldn't happen 
        hard_reset();
    }
}

void process_new_header_segment(struct Segment new_segment) {
    if(free_header_index >= HEADER_SEGMENTS_SIZE) {
        //something went wrong, we haven't matched a header to a protocol yet
        //we don't want to add this mark to the header array because we'll be out of bounds.  let's hard reset
        hard_reset();
    } else {
        header_segments[free_header_index++] = new_segment;
        selected_protocol = protocol_from_header(header_segments, free_header_index);
    }
}

void hard_reset() {
    usart_transmit_string("reset\n");
    //ignore all IR pulses for set period (e.g. 300ms) to wait out the current borked pulse and reset all variables to get a clean slate
}

void reset() {
    
}
