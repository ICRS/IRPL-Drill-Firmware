#include <task.h>
TaskHandle_t drillTaskHandle = nullptr;
#include <motor.h>
Motor drill_motor;
Motor linear_motor;
volatile float drill_speed = 0;
volatile float linear_speed = 0;


void drillTask(void * parameter){
    linear_motor = Motor(M1IA,M1IB,M1P);
    drill_motor = Motor(M2IA,M2IB,M2P);

    for(;;){
        linear_motor.drive(linear_speed);
        drill_motor.drive(drill_speed);
        vTaskDelay((1000/DRILL_TASK_FREQ) / portTICK_PERIOD_MS);
    }
}

