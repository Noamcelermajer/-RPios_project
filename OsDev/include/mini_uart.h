#ifndef _MINI_UART_H
#define _MINI_UART_H

void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void putc(void *p, char c);
void uart_hex(unsigned int d);
int uart_rcv_string(char* str,int size, int len_of_str);
void uart_dump(void *ptr);
#endif /*_MINI_UART_H */
