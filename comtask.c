
#include "comtask.h"


bool DataReady = false;
char ComBuf[160];
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
        else if(strcmp(ComBuf,"post")==0) {test_http(); }
        else if(strcmp(ComBuf,"sms")==0)send_sms("0908586859","sms finish");
        else{
            switch (com_event){
                case COM_NORMAL: 
                    if(strcmp(ComBuf,"what")==0) com_out("yeah");
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
void print_error(int res)
{
    char error[80];
    sprintf(error, "Result: %d", res);
    com_out(error);
}
void test_http()
{
 /*   com_out("begin post http \r\n");
    
    char response[64];
    char body[160];
    
    Result result;
  
    configureBearer("navyone.xyz");
 
    result = connect();
   
    
  //  if(result==SUCCESS)
    {
        com_out("HTTP post:\r\n");
        sprintf(body, "{\"action\":\"embedded\", \"handle\": \"add\", \"data\":{ \"name\": \"Temp\", \"val\": 32, \"created_at\": \"20\"}}");
        com_out(body);
        com_out("{\"action\":\"embedded\", \"handle\": \"add\", \"data\":{ \"name\": \"Temperature\", \"val\": 32, \"created_at\": \"2018-09-19 10:00:00\"}}");
        result = post("http://navyone.xyz/api/v1", body, response);
        com_out("HTTP POST: ");
        if (result == SUCCESS) {
            com_out(response);
        }
    }   
    */
//    com_out("begin post http \r\n");
//    
//    
//    if(configGprs("navyone.xyz")==false) com_out("Error configGprs");
//    openBearer();
//    closeBearer();
httpInit();
//    setURL("http://navyone.xyz/api/v1");
//    setContent("{\"action\":\"embedded\", \"handle\": \"add\", \"data\":{ \"name\": \"Temperature\", \"val\": 32, \"created_at\": \"2018-09-19 10:00:00\"}}", 
//            "application/json");
//    httpPost();
    executeCommand("AT+CIPSTART=\"TCP\",\"navyone.xyz\",80", "OK", 2000);
}