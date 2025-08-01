#include <task.h>

/* 
want PI controller
add D component if get too much oscillation/overshoot
*/

TaskHandle_t linearTaskHandle = nullptr;

#define MAX_PWM 182
const int Kp = 1;
const int Ki = 1;

float targetPositionLinear = 0.0f; //mm
float currentPositionLinear = 0.0f; //mm

static float delta = 0.0f;
static float integralDelta = 0.0f;
static int PWMSpeed;

void linearTask(void *parameter){
  pinMode(M2IA, OUTPUT);  
  pinMode(M2IB, OUTPUT);
  pinMode(M2P, OUTPUT);

    for (;;){
      currentPositionLinear = currentPositionLinear + speedLinear*5.0f/LINEAR_TASK_FREQ/60 * delta/abs(delta);
      delta = targetPositionLinear - currentPositionLinear;
      if (delta > 0.0f) {
        digitalWrite(M2IA, HIGH);
        digitalWrite(M2IB, LOW);
      }
      else if (delta < 0.0f) {
        digitalWrite(M2IA, LOW);
        digitalWrite(M2IB, HIGH);
      }
      else {
        digitalWrite(M2IA, LOW);
        digitalWrite(M2IB, LOW);
      }
      integralDelta += delta * 1/LINEAR_TASK_FREQ;


      if (delta != 0.0f) {
        PWMSpeed += + delta * PWMSpeed * Kp + integralDelta * Ki;
      }
      else {
        PWMSpeed = 0;
      }

      analogWrite(M2P, PWMSpeed);


      vTaskDelay((1000 / LINEAR_TASK_FREQ) / portTICK_PERIOD_MS);
    }
}