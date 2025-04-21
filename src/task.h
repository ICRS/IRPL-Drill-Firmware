#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

/*<-----    Task functions  ----->*/

void samsCerealTask(void * parameter);

/*<-----    Task handles    ----->*/

extern TaskHandle_t samsCerealTaskHandle;

/*<-----    Task frequencies    ----->*/

#define SAMS_CEREAL_FREQ 1

/*<-----    Task enables    ----->*/

#define ENABLE_SAMS_CEREAL true

/*<-----    Shared variables    ----->*/

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