
#include "xc.h"
#include "sim800.h"
#include <string.h>
#include <stdio.h>
#include "comtask.h"

bool sBearer = false;
bool sHTTP = false;

#define BEARER_PROFILE_GPRS "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"
#define HTTP_PARA "AT+HTTPPARA=\"URL\",\"%s\"\r\n"
#define HTTP_CONTENT "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n"
#define HTTP_INIT "AT+HTTPINIT\r\n"


#define OK "OK\r\n"
#define DOWNLOAD "DOWNLOAD"
#define HTTP_2XX ",2XX,"
#define HTTPS_PREFIX "https://"
#define CONNECTED "+CREG: 0,1"
#define BEARER_OPEN "+SAPBR: 1,1"


bool contains(char* res, char* s) {
    if (strstr (res, s)) {
        return true;
    }
    return false;
}
bool configGprs(const char* apn) {
   
    if (!executeCommand(BEARER_PROFILE_GPRS, OK, 2000)) {
        return false;
    }
    if (!executeCommand("AT+SAPBR=3,1,\"APN\",\"v-internet\"", OK, 2000)) {
        return false;
    }
    if (!executeCommand("AT+CGATT=1", OK, 2000)) {
        return false;
    }
    return true;
}

bool openBearer() {
    if (sBearer == true) {
        return false;
    }
    const char* comm = "AT+SAPBR=1,1";
    sim_out(comm);
    const char* res = getResponse();

    if (contains(res, "OK")) {
        sBearer = true;
        return true;
    }

    delayms(1500);
    return false;
}

bool closeBearer() {
    if (sBearer == false) {
        return false;
    }
    const char* comm = "AT+SAPBR=0,1";
    sim_out(comm);
    const char* res = getResponse();

    if (contains(res, "OK")) {
        sBearer = false;
        return true;
    }

    return false;
}

bool httpInit() {
    if (executeCommand(HTTP_INIT, OK, 2000)) {
        return true;
    }
    return false;
}

bool httpTerminate() {
    sim_out("AT+HTTPTERM");

    const char* res = getResponse();

    if (contains(res, "OK")) {
        sHTTP = false;
        return true;
    }

    return false;
}

bool setURL(const char* url) {
    char httpPara[128];
    sprintf(httpPara, HTTP_PARA, url);

    if (!executeCommand(httpPara, OK, 2000)) {
        return false;
    }
    return true;
}

bool setContent(const char* body, const char* contentType) {

    char httpPara[128];
    sprintf(httpPara, HTTP_CONTENT, contentType);


    if (!executeCommand(httpPara, OK, 2000)) {
        return false;
    }
    char httpData[160];
    unsigned int delayToDownload = 10000;
    sprintf(httpData, "AT+HTTPDATA=%d,%d", strlen(body), 10000);


    if (!executeCommand(httpData, OK, 2000)) {
        return false;
    }
    sim_out(body);
    const char* res = getResponse();
    return true;
}

bool httpPost() {
    sim_out("AT+HTTPACTION=1");
    const char* res = getResponse();
    if (contains(res, "OK")) {
        res = getResponse();
    }
    return true;
}

const char* httpGet() {
    sim_out("AT+HTTPACTION=0");
    const char* res = getResponse();
    sim_out("AT+HTTPREAD");
    res = getResponse();
    return res;
}


bool executeCommand(const char* comm, const char* status, int response_time) {
    sim_out(comm);
    com_out(comm);
    int _timeout = 0;
    while  (  1 ) 
    {
        delayms(1);
        if(gprs_status(status)) return true;
        _timeout++;
        if(_timeout>response_time) return false;
    }
    return false;
//    com_out(status);
//    const char* res = getResponse();
//    if (contains(res, "OK")) {
//        return true;
//    }
//    if (contains(res, "DOWNLOAD")) {
//        return true;
//    }
//    if (contains(res, "ERROR")) {
//        return false;
//    }
}

const char* getResponse() {
    char res[160];
    read_gprs_buf(res);
    return res;
    //    while (true) {
    //        if (Serial.available()) {
    //            res = Serial.readconst char*();
    //            res.trim();
    //            return res;
    //        }
    //    }
}