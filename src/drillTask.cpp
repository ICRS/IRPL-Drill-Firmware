#include "task.h"
#include "ESP32Encoder.h"

TaskHandle_t drillTaskHandle = nullptr;

volatile float drill_target_velocity;


void drillTask(void *parameter){
    
    pinMode(M2IA, OUTPUT);
    pinMode(M2IB, OUTPUT);
    
    ledcAttachPin(M2P, 0); // Attach PWM to channel 0
    ledcSetup(0, 20000, 8);  // 20kHz PWM, 8-bit resolution
    
    ESP32Encoder encoder;
    encoder.attachFullQuad(ENC2A, ENC2B);
    encoder.setCount(0);
    
    float last_count = 0.0;
    
    float current_velocity = 0.0;

    float kp = 50.0;
    float ki = 0.02;
    float kd = 0.0;

    float e      = 0.0; /* PID error */
    float last_e = 0.0; /* Last PID error */
    float ei     = 0.0; /* PID integral error */
    float ed     = 0.0; /* PID derivative error */

    float   output;
    int16_t pwm = 0;

    /* Timing variables (all in ms) */
    uint32_t current_time;
    uint32_t last_time = millis();
    uint32_t t_delta;

    TickType_t last_wake = xTaskGetTickCount();

    while (true) {

        xTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000 / DRILL_TASK_FREQ));
        current_time = millis();
        t_delta = current_time - last_time;
        last_time = current_time;

        /* Get encoder count */
        float new_count = encoder.getCount() / -3605.0 * 5.0;
        float delta = new_count - last_count;
        
        /* Calculate current velocity */
        current_velocity = delta / ((float) (t_delta) / 1000.0);
        last_count = new_count;
        
        /* PID controller */
        e   = drill_target_velocity - current_velocity;
        ei += (e - last_e) * ((float) (t_delta) / 1000.0);
        ed  = (e - last_e) / ((float) (t_delta) / 1000.0);
        last_e = e;

        /* Constrain integral term */
        ei = constrain(ei, -255.0, 255.0);

        /* Get the controller output */
        output = (kp * e) + (ki * ei) + (kd * ed);
        pwm = (int16_t) output;
        pwm = constrain(-pwm, -255, 255);

        printf("Error: %.2f, output: %.2f, pwm: %i\n", e, output, pwm);

        if (drill_target_velocity == 0.0) {
            digitalWrite(M2IB, LOW);
            digitalWrite(M2IA, LOW);
            analogWrite(M2P, 0);
        }
        else if (pwm > 0) {
            digitalWrite(M2IB, HIGH);
            digitalWrite(M2IA, LOW);
            analogWrite(M2P, (uint8_t) pwm);
        }
        else if (pwm < 0) {
            digitalWrite(M2IB, LOW);
            digitalWrite(M2IA, HIGH);
            analogWrite(M2P, (uint8_t) (-pwm));
        }
    }
}