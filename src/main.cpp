#include <task.h>
SemaphoreHandle_t loadCellMutex;
MotorTask linearMotor;
MotorTask drillMotor;


void setup(){
  loadCellMutex = xSemaphoreCreateMutex();
    if (loadCellMutex == NULL) {
        Serial.println("❌ Failed to create mutex!");
        while (true); // Stop execution
    }

    #if ENABLE_DRILL_TASK == true
    linearMotor.begin(M1IA,M1IB,M1P,ENC1A,ENC1B,20,380,1);
    linearMotor.setPID(1.0,0.2,0.05);
    drillMotor.begin(M2IA,M2IB,M2P,ENC2A,ENC2B,20,-3605,0);
    drillMotor.setPID(1,0.1,0.05);
    #endif

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



    #if ENABLE_LOADCELL_TASK == true
    xTaskCreate(
        loadCellTask,    // Function that should be called
        "Loadcell sampling task",   // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &loadCellTaskHandle             // Task handle
      );
    #endif

    #if ENABLE_SERVO_TASK == true
    xTaskCreate(
        servoTask,    // Function that should be called
        "Servo control task",   // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &servoTaskHandle             // Task handle
      );
    #endif

    #if ENABLE_LIMIT_TASK == true
    xTaskCreate(
        limitTask,    // Function that should be called
        "Limit control task",   // Name of the task (for debugging)
        4096,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        &limitTaskHandle             // Task handle
      );
    #endif
}

void loop(){
    vTaskDelay(pdMS_TO_TICKS(1000));
}