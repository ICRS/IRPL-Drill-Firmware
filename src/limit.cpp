#include <task.h>

TaskHandle_t limitTaskHandle = nullptr;

volatile bool limitReached = false;

void limitTask(void * parameter) {
    // Initialize limit switch pins
    pinMode(LS2, INPUT_PULLUP);

    const TickType_t xPeriod = pdMS_TO_TICKS(1000 / LIMIT_TASK_FREQ);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;) {
        // Check the state of the limit switch
        limitReached = digitalRead(LS2);
        // Serial.printf("<LIMIT:%d>\n",limitReached);
        xTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}