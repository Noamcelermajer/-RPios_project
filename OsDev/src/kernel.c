#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "memory.h"
#include "strings.h"
#include "mBox.h"
#include "delays.h"
#include "sd.h"
#include "fat.h"
#include "lfb.h"
#include "power.h"
#include "shell.h"
#include <stdint.h>
extern uint8_t bss_start;
extern uint8_t bss_end;

#define COUNTER_SECTOR 1

//extern uint8_t vbar_el1;

void kernel_main(void)
{
    uart_init();
    init_printf(0, putc);

    lfb_init();

    int el = get_el();
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
    if (mbox_call(MBOX_CH_PROP))
    {
        printf("My serial number is: ");
        printf("%x", mbox[6]);
        printf("%x", mbox[5]);
        printf("\r\n");
    }
    else
    {
        printf("Unable to query serial!\r\n");
    }

    

    printf("Waiting 1000000 CPU cycles (ARM CPU): ");
    wait_cycles(1000000);
    printf("OK\r\n");

    printf("Waiting 1000000 microsec (ARM CPU): ");
    wait_msec(1000000);
    printf("OK\r\n");

    printf("Waiting 1000000 microsec (BCM System Timer): ");
    if(get_system_timer()==0)
    {
        printf("Not available\r\n");
    }
    else
    {
        wait_msec_st(1000000);
        printf("OK\r\n");
    }

    // initialize EMMC and detect SD card type
    if(sd_init() == SD_OK && fat_getpartition())
    {
        Run_Shell();
    }
    
    /* unsigned int r;
    // generate a Data Abort with a bad address access
    r=*((volatile unsigned int*)0xFFFFFFFFFF000000); */






    
/*
        dump();
  str = (char*)malloc(100*sizeof(char));
        dump();
  memset(str, 0, 99);
  int r = 0;
  while (1) 
  {
    r = uart_rcv_string(str,99, r);
	  printf("\r\n%s\r\n",str);
    if(!strcmp(str, "exit"))
    {
      dump();
      free(str);
      dump();
    }
  }
*/
}