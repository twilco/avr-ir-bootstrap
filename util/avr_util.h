#ifndef AVR_UTIL_H_
#define AVR_UTIL_H_

bool icp_listening_for_rising();
bool icp_listening_for_falling();
void icp_listen_for_rising();
void icp_listen_for_falling();
void usart_init();
void usart_transmit(unsigned char data);
void usart_print_string(char* string);

#endif /* AVR_UTIL_H_ */