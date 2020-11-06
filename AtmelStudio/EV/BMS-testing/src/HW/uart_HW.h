/*
 * uart_HW.h
 *
 * Created: 2019-12-11 19:07:30
 *  Author: Måns Lindeberg
 */ 


#ifndef UART_HW_H_
#define UART_HW_H_

struct usart_module usart_instance;
#define MAX_RX_BUFFER_LENGTH   100

volatile uint8_t rx_buffer[MAX_RX_BUFFER_LENGTH];

void usart_read_callback(struct usart_module *const usart_module);
void usart_write_callback(struct usart_module *const usart_module);

void configure_usart(void);
void configure_usart_callbacks(void);

void uart_transmit(uint8_t*, uint8_t);
void tohex(unsigned char *, size_t, char *, size_t);





#endif /* UART_HW_H_ */