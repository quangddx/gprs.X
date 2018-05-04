#include "hardware.h"


int time = 0;
void delayms(int t){
    //TMR1_SoftwareCounterClear();
    int nt = TMR1_SoftwareCounterGet();
    while(TMR1_SoftwareCounterGet()<(nt+t));   
}

void hardware_init(void){
    
}
void blinkled(void){
    if ((time+999)<TMR1_SoftwareCounterGet()||(time>TMR1_SoftwareCounterGet())){
           time = TMR1_SoftwareCounterGet();
           LED^=1;
        //t = TMR1_SoftwareCounterGet();
        //printf("test \r\n");
            //sprintf(st,"\r\n tui la%d",n++);
        //UART1_WriteBuffer("test1 test2 test3",15);
           // com_out(st);
    }
}
