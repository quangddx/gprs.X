#include "gprs.h"
//#include <xc.h>
//#include "hardware.h"
//#include "comtask.h"

//
bool DataGprs = false;
char GprsBuf[99];
char GprsLen = 0;
int gTime = 0;
int gTick = 0;
GPRS_STATES GprsEvent=1;
static bool gOK = false;
static bool gERROR = false;
char *gCommand[20];
static int gI = 0;
bool debug = true;
void gprs_init(){
    gCommand[0]="AT\r\n";
    gCommand[1]="ATE0\r\n";
    gCommand[2]="AT+CPIN?\r\n";
    gCommand[3]="AT\r\n";
    gCommand[4]="AT+CMGF=1\r\n";
    gCommand[5]="AT+CIPHEAD=1\r\n";
    gCommand[6]="AT+CIPSRIP=1\r\n";
    gCommand[7]="AT+CSTT=\"m_wap\",\"mms\",\"mms\"\r\n";
    gCommand[10]="AT+CIICR\r\n";
    gCommand[11]="AT+CIFSR\r\n";
    gCommand[12]="AT+CIPSTART=\r\n";
    gCommand[13]="AT+CIPSHUT\r\n";
    gCommand[14]="AT+CIPPING=\"www.google.com\"\r\n";
}
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
    switch(GprsEvent){
        case GPRS_OFF:
            
            if (!LDO) {LDO = 1; gTime = TMR1_SoftwareCounterGet();}
            if ((LDO)&&(((gTime+9999)<TMR1_SoftwareCounterGet())||(gTime>TMR1_SoftwareCounterGet()))){
                gTime = TMR1_SoftwareCounterGet();
                if(debug) com_out("gprs on\r\n");
                GprsEvent = GPRS_ON;              
            }         
            break;
        case GPRS_ON:
            LDO = 0;          
            if (((gTime+9999)<TMR1_SoftwareCounterGet())||(gTime>TMR1_SoftwareCounterGet())){
                gTime = TMR1_SoftwareCounterGet();
                gI = 0; gOK = 1;
                GprsEvent = GPRS_INIT;
                if(debug) com_out("gprs init\r\n");
            }
            break;
        case GPRS_INIT:
            if (gOK) {
                gOK=false;
                //if(debug) com_out(gCommand[gI]);
                sim_out(gCommand[gI++]);                
                gTime = TMR1_SoftwareCounterGet();
            }

            if(gI>7) {               
                if(debug) com_out("ready to online\r\n");
                gOK=0; sim_out("AT+CIICR\r\n");                
                gTime = TMR1_SoftwareCounterGet();
                GprsEvent = GPRS_3G;break;
            }
            if (((gTime+999)<TMR1_SoftwareCounterGet())||(gTime>TMR1_SoftwareCounterGet())){
                gTime = TMR1_SoftwareCounterGet();
                GprsEvent = GPRS_OFF;
                //com_out("wait\r\n");
            }           
            break;
        case GPRS_3G:
            if(gOK){               
                if(debug) com_out("get ip\r\n");
                gOK=0; sim_out("AT+CIFSR\r\n");
                gTime = TMR1_SoftwareCounterGet();
                gTick=0;GprsEvent = GPRS_GET_IP;break;
            }
            if (((gTime+1999)<TMR1_SoftwareCounterGet())||(gTime>TMR1_SoftwareCounterGet())){
                gTime = TMR1_SoftwareCounterGet();
                gTick++;
                sim_out("AT+CIICR\r\n");
            }
            if (gTick>60){
                if(debug) com_out("can not connect\r\n");
                GprsEvent = GPRS_OFF;
                gTick=0;
            }
            break;
        case GPRS_GET_IP:
            if(DataGprs){
                //if(debug) com_out("online success\r\n");
                //my ip in GprsBuf
                if ((GprsBuf[0]<0x3A)&&((GprsBuf[0]>0x2F))) {
                    if(debug) com_out("online success\r\n");
                    GprsBufClean();
                    gTime = TMR1_SoftwareCounterGet();
                    GprsEvent = GPRS_SERVICE;break;
                }
            }
            if (((gTime+1999)<TMR1_SoftwareCounterGet())||(gTime>TMR1_SoftwareCounterGet())){
                gTime = TMR1_SoftwareCounterGet();
                gTick++;
                if(debug) com_out("get ip again\r\n");
                sim_out("AT+CIFSR\r\n");
            }
            if(gTick>20){
                if(debug) com_out("cannot get ip\r\n");
                GprsEvent = GPRS_OFF;
            }
            break;
        case GPRS_SERVICE:
            
            break;
        
    }
    if(DataGprs){  
        if(strcmp(GprsBuf,"OK")==0) gOK = true;  
        else if(strcmp(GprsBuf,"ERROR")==0) gERROR = true;  
        GprsBufClean(); 
    }   
}
void sim_out(char *st){
    UART2_WriteBuffer(st,strlen(st));
}
void send_sms(char *number, char *data){
    //char st[200];
    //sprintf(st,"AT+CMGS="%s, \r\n"); 
    sim_out("AT+CMGS=\"");
    //UART2_Write(0x22);
    //sim_out("0903165302");
    sim_out(number);
    //UART2_Write(0x22);
    sim_out("\"\r\n");
    delayms(100);
    sim_out(data);
    UART2_Write(26);
    sim_out("\r\n");  
}
