#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

/*<-----    Task functions  ----->*/

void samsCerealTask(void * parameter);
void drillTask(void * parameter);
void loadCellTask(void* parameter);

/*<-----    Task handles    ----->*/

extern TaskHandle_t samsCerealTaskHandle;
extern TaskHandle_t drillTaskHandle;
extern TaskHandle_t loadCellTaskHandle;



/*<-----    Task frequencies    ----->*/

#define SAMS_CEREAL_FREQ 1
#define DRILL_TASK_FREQ 100
#define LOADCELL_TASK_FREQ 10



/*<-----    Task enables    ----->*/

#define ENABLE_SAMS_CEREAL true
#define ENABLE_DRILL_TASK true
#define ENABLE_LOADCELL_TASK false

/*<-----    Shared variables    ----->*/
extern volatile float loadCellValues[3];
extern volatile float drill_speed;


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