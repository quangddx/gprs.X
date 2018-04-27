#include "gprs.h"
//#include <xc.h>
//#include "hardware.h"
//#include "comtask.h"

//
bool DataGprs = false;
char GprsBuf[99];
char GprsLen = 0;
int GprsTime = 0;
uint8_t GprsEvent=0;
void GprsBufClean(void){
    DataGprs = false;
    int i;
    for(i=0;i<GprsLen;i++) GprsBuf[i]=0;
    GprsLen = 0;
}
void gprs_task(void){
    char c;
    if(!UART2_ReceiveBufferIsEmpty()){
        c=UART2_Read();
        cout(c);
        if(((c==0x0A)||(c==0x0D))&&GprsLen) DataGprs = true;   
        else if((c>31)&&(c<127)){GprsBuf[GprsLen++]=c;}
    }
    if(DataGprs){        
        //if(strcmp(GprsBuf,"sms")==0) 
        //else 
           // com_out(GprsBuf);         
    } 
    switch(GprsEvent){
        case 0:
            break;
        
    }
      
}
void sim_out(char *st){
    UART2_WriteBuffer(st,strlen(st));
}
void send_sms(char *number, char *data){
    //char st[200];
    //sprintf(st,"AT+CMGS="%s, \r\n"); 
    sim_out("AT\r\n");delayms(100);
    sim_out("AT+CPIN?\r\n");delayms(100);
    sim_out("AT+CMGF=1\r\n");delayms(100);
    sim_out("AT+CMGS=\"");
    //UART2_Write(0x22);
    sim_out("0903165302");
    //UART2_Write(0x22);
    sim_out("\"\r\n");
    delayms(100);
    sim_out("test1");
    UART2_Write(26);
    sim_out("\r\n");  
}
