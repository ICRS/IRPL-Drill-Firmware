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

    #if ENABLE_LINEAR_TASK == true
    xTaskCreate(
        linearTask,    // Function that should be called
        "linear control loop",   // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &linearTaskHandle             // Task handle
      );
    #endif

    #if ENABLE_ENCODER_TASK == true
    xTaskCreate(
        encoderTask,     // Function that should be called
        "encoder speed", // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &encoderTaskHandle             // Task handle
      );
    #endif

    #if ENABLE_PAYLOAD_TASK == true
    xTaskCreate(
        payloadTask,     // Function that should be called
        "payload control loop", // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &payloadTaskHandle             // Task handle
      );
    #endif

    #if ENABLE_SERVO_TASK == true
    xTaskCreate(
        servoTask,     // Function that should be called
        "servo control loop", // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &servoTaskHandle             // Task handle
      );
    #endif
    
}

void loop(){
    vTaskDelay(pdMS_TO_TICKS(1000));
}