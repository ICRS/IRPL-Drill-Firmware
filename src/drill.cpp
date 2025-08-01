#include <task.h>

TaskHandle_t drillTaskHandle = nullptr;

#define MAX_PWM 182

float targetSpeedDrill = 20.0f;
float absTargetSpeedDrill;

static float delta = 0.0f;
static int PWMSpeed;

void drillTask(void *parameter){
  pinMode(M1IA, OUTPUT);  
  pinMode(M1IB, OUTPUT);
  pinMode(M1P, OUTPUT);

    for (;;){
      if (targetSpeedDrill > 0) {
        digitalWrite(M1IA, HIGH);
        digitalWrite(M1IB, LOW);
        absTargetSpeedDrill = targetSpeedDrill;
      } 
      else if (targetSpeedDrill < 0) {
        digitalWrite(M1IA, LOW);
        digitalWrite(M1IB, HIGH);
        absTargetSpeedDrill = - targetSpeedDrill;
      } 
      else {
        digitalWrite(M1IA, LOW);
        digitalWrite(M1IB, LOW);;
      }

      delta = absTargetSpeedDrill - speedDrill;
      
      if (absTargetSpeedDrill != 0.0f) {
        PWMSpeed = (PWMSpeed  * delta/absTargetSpeedDrill) + PWMSpeed;
      }
      else {
        PWMSpeed = 0;
      }



      if (PWMSpeed > MAX_PWM) {
        PWMSpeed = MAX_PWM;
      }
      analogWrite(M1P, PWMSpeed);
      
      vTaskDelay((1000 / DRILL_TASK_FREQ) / portTICK_PERIOD_MS);
    }
}