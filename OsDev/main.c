

#include "uart.h"

void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Hello there!\n");
    uart_puts("Welcome to our new basic operating system!\n");

    // echo everything back
    while(1) {
           uart_send(uart_getc());

    }
}
