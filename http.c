/*
 * File:   http.c
 * Author: Do Hong Hoat
 *
 * Created on April 27, 2018, 1:48 PM
 */


#include "xc.h"
#include "http.h"
#include <string.h>
#include <stdio.h>
#include "comtask.h"

#define BEARER_PROFILE_GPRS "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"
#define BEARER_PROFILE_APN "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n"
#define QUERY_BEARER "AT+SAPBR=2,1\r\n"
#define OPEN_GPRS_CONTEXT "AT+SAPBR=1,1\r\n"
#define CLOSE_GPRS_CONTEXT "AT+SAPBR=0,1\r\n"
#define HTTP_INIT "AT+HTTPINIT\r\n"
#define HTTP_CID "AT+HTTPPARA=\"CID\",1\r\n"
#define HTTP_PARA "AT+HTTPPARA=\"URL\",\"%s\"\r\n"
#define HTTP_GET "AT+HTTPACTION=0\r\n"
#define HTTP_POST "AT+HTTPACTION=1\n"
#define HTTP_DATA "AT+HTTPDATA=%d,%d\r\n"
#define HTTP_READ "AT+HTTPREAD\r\n"
#define HTTP_CLOSE "AT+HTTPTERM\r\n"
#define HTTP_CONTENT "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n"
#define HTTPS_ENABLE "AT+HTTPSSL=1\r\n"
#define HTTPS_DISABLE "AT+HTTPSSL=0\r\n"
#define NORMAL_MODE "AT+CFUN=1,1\r\n"
#define REGISTRATION_STATUS "AT+CREG?\r\n"
#define SIGNAL_QUALITY "AT+CSQ\r\n"
#define READ_VOLTAGE "AT+CBC\r\n"
#define SLEEP_MODE "AT+CSCLK=1\r\n"

#define OK "OK\r\n"
#define DOWNLOAD "DOWNLOAD"
#define HTTP_2XX ",2XX,"
#define HTTPS_PREFIX "https://"
#define CONNECTED "+CREG: 0,1"
#define BEARER_OPEN "+SAPBR: 1,1"

#define SUCCESS 1
void cleanBuffer(char *buffer, int count)
{
    int i;
    for(i=0; i < count; i++) {
        buffer[i] = '\0';
    }
}

bool sendCmdAndWaitForResp(const char* cmd, const char* status, int response_time)
{
    com_out(cmd);
    sim_out(cmd);
    int _timeout = 0;
    while  (  1 ) 
    {
        delayms(1);
        
        if(gprs_status(status)) return true;
        _timeout++;
        if(_timeout>response_time) return false;
    }
    return false;
}

Result configureBearer(const char *apn){

  int result = SUCCESS;

  unsigned int attempts = 0;
  unsigned int MAX_ATTEMPTS = 10;

 

  while (sendCmdAndWaitForResp(REGISTRATION_STATUS, CONNECTED, 2000) != true && attempts < MAX_ATTEMPTS){
 //   sendCmdAndWaitForResp(READ_VOLTAGE, OK, 1000);
 //   sendCmdAndWaitForResp(SIGNAL_QUALITY, OK, 1000);
    attempts ++;
    delayms(1000);
    if (attempts == MAX_ATTEMPTS) {
      result = ERROR_REGISTRATION_STATUS;
      //preInit();
    }
  }

  if (sendCmdAndWaitForResp(BEARER_PROFILE_GPRS, OK, 2000) == false)
    result = ERROR_BEARER_PROFILE_GPRS;

  char httpApn[64];
  sprintf(httpApn, BEARER_PROFILE_APN, apn);
  if (sendCmdAndWaitForResp(httpApn, OK, 2000) == false)
    result = ERROR_BEARER_PROFILE_APN;

  return result;
}

Result connect() {

  int result = SUCCESS;
  unsigned int attempts = 0;
  unsigned int MAX_ATTEMPTS = 10;

  while (sendCmdAndWaitForResp(QUERY_BEARER, BEARER_OPEN, 2000) == false && attempts < MAX_ATTEMPTS){
    attempts ++;
    if (sendCmdAndWaitForResp(OPEN_GPRS_CONTEXT, OK, 2000) == false){
      result = ERROR_OPEN_GPRS_CONTEXT;
    }
    else {
      result = SUCCESS;
    }
  }
  

  if (sendCmdAndWaitForResp(HTTP_INIT, OK, 2000) == false)
    result = ERROR_HTTP_INIT;

  return result;
}

Result disconnect() {

  Result result = SUCCESS;

  if (sendCmdAndWaitForResp(CLOSE_GPRS_CONTEXT, OK, 2000) == false)
    result = ERROR_CLOSE_GPRS_CONTEXT;
  if (sendCmdAndWaitForResp(HTTP_CLOSE, OK, 2000) == false)
    result = ERROR_HTTP_CLOSE;

  return result;
}

Result post(const char *uri, const char *body, char *response) {
    
  Result result = setHTTPSession(uri);

  char httpData[160];
  unsigned int delayToDownload = 10000;
  sprintf(httpData, HTTP_DATA, strlen(body), 10000);
  if (sendCmdAndWaitForResp(httpData, DOWNLOAD, 2000) == false){
    result = ERROR_HTTP_DATA;
  }

//  purgeSerial();
  delayms(500);
  
  sim_out(body);

  if (sendCmdAndWaitForResp(HTTP_POST, HTTP_2XX, delayToDownload) == true) {
    sim_out(HTTP_READ);
    readResponse(response);
    result = SUCCESS;
  }
  else {
    result = ERROR_HTTP_POST;
  }

  return result;
}


Result setHTTPSession(const char *uri){

  Result result;
  if (sendCmdAndWaitForResp(HTTP_CID, OK, 2000) == false)
    result = ERROR_HTTP_CID;

  char httpPara[128];
  sprintf(httpPara, HTTP_PARA, uri);

  if (sendCmdAndWaitForResp(httpPara, OK, 2000) == false)
    result = ERROR_HTTP_PARA;

  bool https = strncmp(HTTPS_PREFIX, uri, strlen(HTTPS_PREFIX)) == 0;
  if (sendCmdAndWaitForResp(https ? HTTPS_ENABLE : HTTPS_DISABLE, OK, 2000) == false) {
    result = https ? ERROR_HTTPS_ENABLE : ERROR_HTTPS_DISABLE;
  }

  if (sendCmdAndWaitForResp(HTTP_CONTENT, OK, 2000) == false)
    result = ERROR_HTTP_CONTENT;

  return result;
}

void readResponse(char *response){

  char buffer[128];
  cleanBuffer(buffer, sizeof(buffer));
  cleanBuffer(response, sizeof(response));

  if (read_gprs_buf(buffer) == true){
    parseJSONResponse(buffer, sizeof(buffer), response);
  }
}

void parseJSONResponse(const char *buffer, unsigned int bufferSize, char *response){

  int start_index = 0;
  int i = 0;
  while (i < bufferSize - 1 && start_index == 0) {
    char c = buffer[i];
    if ('{' == c){
      start_index = i;
    }
    ++i;
  }

  int end_index = 0;
  int j = bufferSize - 1;
  while (j >= 0 && end_index == 0) {
    char c = buffer[j];
    if ('}' == c) {
      end_index = j;
    }
    --j;
  }
int k;
  for( k = 0; k < (end_index - start_index) + 2; ++k){
    response[k] = buffer[start_index + k];
    response[k + 1] = '\0';
  }
}