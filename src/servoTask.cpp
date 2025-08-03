#include <task.h>
TaskHandle_t servoTaskHandle = nullptr;
#include <ESP32Servo.h>
Servo payload_servo;
Servo brush_servo;


void servoTask(void * parameter){
    payload_servo.attach(SERVO1);
    brush_servo.attach(SERVO2);

    const TickType_t xPeriod = pdMS_TO_TICKS(1000 / SERVO_TASK_FREQ);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;){
        
    
    xTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

