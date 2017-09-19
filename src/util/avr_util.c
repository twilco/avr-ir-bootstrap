#include "avr_util.h"

bool icp_listening_for_rising() 
{
    if(TCCR1B & (1 << ICES1)) {
        return true;
    }
    return false;
}

bool icp_listening_for_falling() 
{
    return !icp_listening_for_rising();
}

void icp_listen_for_rising() 
{
    TCCR1B |= (1 << ICES1);
}

void icp_listen_for_falling() 
{
    TCCR1B ^= (1 << ICES1);
}

void usart_init() 
{
    /*
     UBRR is a 16-bit register, but only the 4 least significant bits of UBBR0H are valid.
     Let's put our calculated baud rate into these registers.
    */
    UBRR0H = (CALCULATED_BAUD >> 8);
    UBRR0L = CALCULATED_BAUD;
    
    /*
      Set the TXEN0 bit in the USART control register B to enable transmission off the TX pin.
    */
    UCSR0B = (1 << TXEN0);
    
    /*
      Set these bits to set 8-bits in the USART control register C to set a character size of 8.
    */
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

void usart_transmit(unsigned char data) 
{
    // Wait for transmit buffer to be empty
    while ( !(UCSR0A & (1 << UDRE0)) );
    
    UDR0 = data;
}

void usart_transmit_string(char* string) 
{
    for(uint8_t i = 0; i < strlen(string); i++) {
        usart_transmit(string[i]);
    }
}