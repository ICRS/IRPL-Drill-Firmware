#include <task.h>
TaskHandle_t drillTaskHandle = nullptr;

void drillTask(void * parameter){
    
    for(;;){
        vTaskDelay((1000/DRILL_TASK_FREQ) / portTICK_PERIOD_MS);
    }
}

