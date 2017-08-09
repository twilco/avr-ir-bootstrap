#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	/* Set PINB1 as an output */
	DDRB = (1 << PINB1);
	
	/*
		ICNC1 high to turn on input capture noise cancelling
		ICES1 high to initially trigger input capture on rising edge
	*/
	TCCR1B = (1 << ICNC1) | (1 << ICES1);
	
	/* 
		ICIE1 high to enable the input capture interrupt
		TOIE1 high to enable Timer1 overflow interrupt
	*/
	TIMSK1 = (1 << ICIE1) | (1 << TOIE1);
	sei();
	
	/* Start Timer1 with a prescaler of 8 */
	TCCR1B |= (1 << CS11);
	/*
		- Prescaler of 8 will give our input capture the ability to capture events with
		lengths from .5 microseconds (1 tick) to 32767 microseconds (65535 ticks) <-- assuming 16MHz clock
		
		- Prescaler of 1 (no prescaler) can only capture a maximum event (without guaranteed overflow, which would be annoying to consistently deal with)
		of 4095 microseconds.  This is inadequate, considering the header high-segment of the NEC protocol is 9000 microseconds (9ms) <-- assuming 16MHz clock
	*/
    while (1) 
    {
    }
}

ISR(TIMER1_CAPT_vect)
{
	//Turn on our debugging LED at PINB1 so we know we're triggering the interrupt - we will clear when Timer1 overflows
	PORTB = (1 << PINB1);
}

ISR(TIMER1_OVF_vect)
{
	PORTB = 0x0; //andddddd clear debugging LED at PINB1
}
