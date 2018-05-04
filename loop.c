#include "loop.h"
#include <string.h>
void loop(void){
    //__delay_ms(1000);
    hardware_init();
    //sal_init();
    gprs_init();
    char st[200];
    sprintf(st," test........... \r\n");  
    com_out(st);
    delayms(3000);
    
    int time = 0;
    while(1){
        if(TMR1_SoftwareCounterGet()>9999) TMR1_SoftwareCounterClear();
        if (((time+999)<TMR1_SoftwareCounterGet())||(time>TMR1_SoftwareCounterGet())){
           time = TMR1_SoftwareCounterGet();
           LED^=1;
           //sal_task();
        }
       // blinkled();
        com_task();
        gprs_task();
        
    }
  /*
  delayms(1000);UART2_WriteBuffer("AT+CSQ",6);
      delayms(1000);UART2_WriteBuffer("AT+CSCS=GSM",10);
    delayms(1000);UART2_WriteBuffer("AT+CMGS=+84903165302",20);
   delayms(1000);UART2_WriteBuffer("TEST^Z",10);
   * */  
}
