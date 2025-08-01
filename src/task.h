#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
#include "helpers/motorTask.h"

/*<-----    Task functions  ----->*/

void samsCerealTask(void * parameter);
void loadCellTask(void* parameter);
void servoTask(void* parameter);

/*<-----    Task handles    ----->*/

extern TaskHandle_t samsCerealTaskHandle;
extern TaskHandle_t loadCellTaskHandle;
extern TaskHandle_t servoTaskHandle;



/*<-----    Task frequencies    ----->*/

#define SAMS_CEREAL_FREQ 10
#define LOADCELL_TASK_FREQ 10
#define SERVO_TASK_FREQ 10



/*<-----    Task enables    ----->*/

#define ENABLE_SAMS_CEREAL true
#define ENABLE_LOADCELL_TASK true
#define ENABLE_SERVO_TASK true
#define ENABLE_DRILL_TASK true

/*<-----    Shared variables    ----->*/
extern volatile float loadCellValues[3];
extern SemaphoreHandle_t loadCellMutex;
void getLoadCellValues(float *target_array);
extern MotorTask drillMotor;
extern MotorTask linearMotor; 



/*<-----    Shared structs  ----->*/

typedef enum MessageType {
    PING_IN, 
    PING_OUT,
    ERROR,
    MOTOR,
};
 

struct Message {
    MessageType type;
    union {
        bool pingValue;     // PING_IN, PING_OUT
        int errorCode;      // ERROR
        int motorValue;     // MOTOR
    };
};

#endif