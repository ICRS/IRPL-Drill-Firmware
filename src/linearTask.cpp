#include "task.h"
#include "ESP32Encoder.h"

TaskHandle_t linearTaskHandle = nullptr;

ESP32Encoder linear_encoder;
volatile float linear_target_velocity;


void linearTask(void *parameter){

    pinMode(M1IA, OUTPUT);
    pinMode(M1IB, OUTPUT);
    pinMode(LS2, INPUT_PULLUP);

    ledcAttachPin(M1P, 1); // Attach PWM to channel 0
    ledcSetup(1, 20000, 8);  // 20kHz PWM, 8-bit resolution

    linear_encoder.attachFullQuad(ENC1A, ENC1B);
    linear_encoder.setCount(0);
    
    float last_count = 0.0;
    
    float current_velocity = 0.0;

    float kp = 20.0;
    float ki = 0.01;
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
        float new_count = linear_encoder.getCount() / 380.0 * 5.0;
        float delta = new_count - last_count;
        printf("Enc: %lu\n", new_count);
        

        /* Calculate current velocity */
        current_velocity = delta / ((float) (t_delta) / 1000.0);
        last_count = new_count;
        
        /* PID controller */
        e   = linear_target_velocity - current_velocity;
        ei += (e - last_e) * ((float) (t_delta) / 1000.0);
        ed  = (e - last_e) / ((float) (t_delta) / 1000.0);
        last_e = e;

        /* Constrain integral term */
        ei = constrain(ei, -255.0, 255.0);

        /* Get the controller output */
        output = (kp * e) + (ki * ei) + (kd * ed);
        pwm = (int16_t) output;
        pwm = constrain(pwm, -100, 100);

        printf("Error: %.2f, output: %.2f, pwm: %i\n", e, output, pwm);

        /* Stop if limit switch is pressed */
        if (digitalRead(LS2) && (linear_target_velocity < 0)){

            /* Reset PID variables */
            linear_target_velocity = 0.0;
            pwm = 0;
            ei = 0.0;

            /* Reset encoder count */
            linear_encoder.setCount(0);
        }

        if (linear_target_velocity == 0.0) {
            digitalWrite(M1IB, LOW);
            digitalWrite(M1IA, LOW);
            analogWrite(M1P, 0);
        }
        else if (pwm >= 0) {
            digitalWrite(M1IB, HIGH);
            digitalWrite(M1IA, LOW);
            analogWrite(M1P, (uint8_t) pwm);
        }
        else if (pwm < 0) {
            digitalWrite(M1IB, LOW);
            digitalWrite(M1IA, HIGH);
            analogWrite(M1P, (uint8_t) (-pwm));
        }
    }
}