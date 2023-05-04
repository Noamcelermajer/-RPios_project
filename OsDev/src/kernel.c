#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "memory.h"
#include "strings.h"
#include "mBox.h"
#include "delays.h"
#include "sd.h"
#include "fat.h"
#include <stdint.h>
void simpleCmd();
char cmd[256];
extern uint8_t bss_start;
extern uint8_t bss_end;

//extern uint8_t vbar_el1;

void kernel_main(void)
{
  uint32_t kernel_top = (uint32_t)&bss_end;
  uart_init();
  init_printf(0, putc);

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
        uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        printf("\n");
    }
    else
    {
        printf("Unable to query serial!\n");
    }
    init_memory(kernel_top, PBASE);

    printf("Waiting 1000000 CPU cycles (ARM CPU): ");
    wait_cycles(1000000);
    printf("OK\n");

    printf("Waiting 1000000 microsec (ARM CPU): ");
    wait_msec(1000000);
    printf("OK\n");

    printf("Waiting 1000000 microsec (BCM System Timer): ");
    if(get_system_timer()==0)
    {
        printf("Not available\n");
    }
    else
    {
        wait_msec_st(1000000);
    }

    // initialize EMMC and detect SD card type
    if(sd_init()==SD_OK)
    {
        // read the master boot record after our bss segment
        if(sd_readblock(0,&bss_end,1))
        {
            // dump it to serial console
            uart_dump(&bss_end);
        }

        if(fat_getpartition())
        {
            // list root directory entries
            fat_listdirectory();
        }
        else
        {
            printf("FAT partition not found???\n");
        }
    }
  /*
  unsigned int r;
  // generate a Data Abort with a bad address access
  r=*((volatile unsigned int*)0xFFFFFFFFFF000000);
  */

  while(1)
  {
      simpleCmd();
  }
/*
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
*/
}



void simpleCmd()
{
    int i,cmdidx=0,cmdlast=0;
    char c;
    // prompt
    printf("\r> ");
    // read until Enter pressed
    while((c=uart_recv())!='\n') {
        if(c==27) {
            c=uart_recv();
            if(c=='[') {
                c=uart_recv();
                if(c=='C') c=3; else    // left
                if(c=='D') c=2; else    // right
                if(c=='3') {
                    c=uart_recv();
                    if(c=='~') c=1;     // delete
                }
            }
        }
        // Backspace
        if(c==8 || c==127) {
            if(cmdidx>0) {
                cmdidx--;
                for(i=cmdidx;i<cmdlast;i++) cmd[i]=cmd[i+1];
                cmdlast--;
            }
        } else
        // Delete
        if(c==1) {
            if(cmdidx<cmdlast) {
                for(i=cmdidx;i<cmdlast;i++) cmd[i]=cmd[i+1];
                cmdlast--;
            }
        } else
        // cursor left
        if(c==2) {
            if(cmdidx>0) cmdidx--;
        } else
        // cursor right
        if(c==3) {
            if(cmdidx<cmdlast) cmdidx++;
        } else {
            // is there a valid character and space to store it?
            if(c<' ' || cmdlast>=sizeof(cmd)-1) {
                continue;
            }
            // if we're not appending, move bytes after cursor
            if(cmdidx<cmdlast) {
                for(i=cmdlast;i>cmdidx;i--)
                    cmd[i]=cmd[i-1];
            }
            cmdlast++;
            cmd[cmdidx++]=c;
        }
        cmd[cmdlast]=0;
        printf("\r> %s \r\e[%dC",cmd,cmdidx+2);
    }
    if(!strcmp(cmd,"exit"))
    {
        exit();
    }
    printf("\n");
}
