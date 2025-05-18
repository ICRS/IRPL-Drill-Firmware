#include <task.h>


void setup(){

    #if ENABLE_SAMS_CEREAL == true
    xTaskCreate(
        samsCerealTask,    // Function that should be called
        "Serial Handler",   // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &samsCerealTaskHandle             // Task handle
      );
    #endif

    #if ENABLE_DRILL_TASK == true
    xTaskCreate(
        drillTask,    // Function that should be called
        "drill control loop",   // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &drillTaskHandle             // Task handle
      );
    #endif
    
}

void loop(){
    vTaskDelay(pdMS_TO_TICKS(1000));
}