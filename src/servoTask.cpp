#include <task.h>
TaskHandle_t servoTaskHandle = nullptr;
#include <Deneyap_Servo.h>
Servo payload_servo;
Servo brush_servo;

volatile int payloadPos = 180; // Default value for payload servo
volatile int brushPos = 90;   // 25 to stop, <25 CW, >25 ACW

void inchPayload(int direction){
    payload_servo.write(90);
    payloadPos = 90;
    if (direction > 0) {
        payloadPos = 100;
        vTaskDelay(pdMS_TO_TICKS(300));
        payloadPos = 90;

    } 
    else {
        payloadPos = 70;
        vTaskDelay(pdMS_TO_TICKS(200));
        payloadPos = 90;
    }
}


void servoTask(void * parameter){
    payload_servo.attach(SERVO1,0);
    brush_servo.attach(SERVO2,1);

    

    const TickType_t xPeriod = pdMS_TO_TICKS(1000 / SERVO_TASK_FREQ);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;){
        payload_servo.write(payloadPos);
        brush_servo.write(brushPos);
    xTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

