
#include "comtask.h"

bool DataReady = false;
char ComBuf[99];
char ComLen=0;
COM_STATES   com_event=0;
void cout(char c){
    OE = 1;
    UART1_Write(c);
    
}
void com_out(char *st){
    OE = 1;
    UART1_WriteBuffer(st,strlen(st));
}
/*
char com_in( char *st){
    unsigned int size = UART1_ReceiveBufferSizeGet();
    if (size) {
        UART1_ReadBuffer(st,size);
        return 1;
    }
    return 0;
}
 */
void com_task(void){
    char c;
    
    if(U1STAbits.TRMT) OE = 0;
    if(!UART1_ReceiveBufferIsEmpty()){
        c=UART1_Read();
        if(((c==0x0A)||(c==0x0D))&&ComLen) DataReady = true;   
        else if((c>31)&&(c<127)){ComBuf[ComLen++]=c;}
    }
    if(DataReady){
        DataReady = false;
        if(strcmp(ComBuf,"sensor")==0) {com_event = COM_SENSOR;com_out("sensor calib\r\n");}
        else if(strcmp(ComBuf,"system")==0) {com_event = COM_SENSOR;com_out("system calib\r\n");}
        else if(strcmp(ComBuf,"gprs")==0) {com_event = COM_TEST_GPRS;com_out("check gprs\r\n");}
        else if(strcmp(ComBuf,"normal")==0) {com_event = COM_NORMAL;com_out("return to normal\r\n");}
        else if(strcmp(ComBuf,"sms")==0)send_sms("+84903165302","test");
        else{
            switch (com_event){
                case COM_NORMAL: 
                    if(strcmp(ComBuf,"what")==0) com_out("yeah");
                    else if(strcmp(ComBuf,"a2 ten gi?")==0) com_out(" Vien Bach");
                    else if(strcmp(ComBuf,"em ten gi?")==0) com_out(" Que Lam");
                    else  com_out(ComBuf); 
                    break;
                case COM_SENSOR: 
                    break;

                case COM_TEST_GPRS: 
                    sim_out(ComBuf);UART2_Write(0X0D);UART2_Write(0X0A);
                    break;
                case COM_SYSTEM: 
                    break;
                    

            }
       
        }
        int i;
        for(i=0;i<ComLen;i++) ComBuf[i]=0;
        ComLen = 0;
    }
}