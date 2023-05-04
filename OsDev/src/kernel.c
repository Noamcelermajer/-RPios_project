#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "memory.h"
#include "strings.h"
#include "mBox.h"
#include <stdint.h>

extern uint8_t bss_start;
extern uint8_t bss_end;
void kernel_main(void) {
  uint32_t kernel_top = (uint32_t)&bss_end;
  uart_init();
  init_printf(0, putc);

  int el = get_el();

 char* str;
  printf("Exception level: %d \r\n", el);
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_GETSERIAL;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        printf("My serial number is: ");
        uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        printf("\n");
    } else {
        printf("Unable to query serial!\n");
    }
  init_memory(kernel_top, PBASE);
        dump();
  str = (char*)malloc(100*sizeof(char));
        dump();
  memset(str, 0, 99);
  int r = 0;
  while (1) 
  {
    r = uart_rcv_string(str,99, r);
	  printf("\n%s\n",str);
    if(!strcmp(str, "exit"))
    {
      dump();
      free(str);
      dump();
    }
  }
}
