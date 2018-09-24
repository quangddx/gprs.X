#include <xc.h>
#include"sensor.h"
typedef struct
{
	int DataRaw;
    float data;
    float A;
    float B;
    
} SENSOR_OBJECT;

static SENSOR_OBJECT sensor;

void sensor_task(void){
    sensor.DataRaw = GetConductivity();
    sensor.data = GetSal();
}

void sensor_A_update(float dat){
    sensor.A = dat;
}
void sensor_B_update(float dat){
    sensor.B = dat;
}