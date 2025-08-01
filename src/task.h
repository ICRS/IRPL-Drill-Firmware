#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

/*<-----    Task functions  ----->*/

void samsCerealTask(void * parameter);
void drillTask(void * parameter);
void linearTask(void * parameter);
void encoderTask(void * parameter);
void payloadTask(void * parameter);

/*<-----    Task handles    ----->*/

extern TaskHandle_t samsCerealTaskHandle;
extern TaskHandle_t drillTaskHandle;
extern TaskHandle_t linearTaskHandle;
extern TaskHandle_t encoderTaskHandle;
extern TaskHandle_t payloadTaskHandle;



/*<-----    Task frequencies    ----->*/

#define SAMS_CEREAL_FREQ 1
#define DRILL_TASK_FREQ 100
#define LINEAR_TASK_FREQ 100
#define ENCODER_TASK_FREQ 60
#define PAYLOAD_TASK_FREQ 1



/*<-----    Task enables    ----->*/

#define ENABLE_SAMS_CEREAL true
#define ENABLE_DRILL_TASK false
#define ENABLE_LINEAR_TASK false
#define ENABLE_ENCODER_TASK false
#define ENABLE_PAYLOAD_TASK true

/*<-----    Shared variables    ----->*/

extern float speedDrill;
extern float targetSpeedDrill;

extern float speedLinear;
extern float newPosLinear;
extern float targetPositionLinear;

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