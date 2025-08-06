#include <task.h>

TaskHandle_t limitTaskHandle = nullptr;

volatile bool limitReached = false;

void limitTask(void * parameter) {
    // Initialize limit switch pins
    pinMode(LS1, INPUT_PULLUP);

    const TickType_t xPeriod = pdMS_TO_TICKS(1000 / LIMIT_TASK_FREQ);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;) {
        // Check the state of the limit switch
        if (digitalRead(LS1) == 1){
            limitReached = true;
        }
        else {
            limitReached = false;
        }

        xTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}