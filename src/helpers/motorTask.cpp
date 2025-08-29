#include "motorTask.h"

#define PID_SAMPLE_TIME_US 5000

static void motorTaskEntry(void *pvParameters)
{
    MotorTask *motor = static_cast<MotorTask *>(pvParameters);
    motor->taskFunction(); // calls the real task logic on the right object
}

bool MotorTask::setPID(float vel_kp, float vel_ki, float vel_kd)
{
    velocityPID.SetTunings(vel_kp, vel_ki, vel_kd);
    return true;
}

bool MotorTask::begin(uint8_t in1, uint8_t in2, uint8_t en, uint8_t encA, uint8_t encB, int16_t freq, int16_t cpr, bool limit)
{
    this->cpr = cpr;
    in1Pin = in1;
    in2Pin = in2;
    enPin = en;
    this->encA = encA;
    this->encB = encB;
    this->taskFrequency = freq;
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    if (limit)
    {
        pinMode(LS2, INPUT_PULLUP);
    }
    pinMode(LS2, INPUT);
    ledcAttachPin(enPin, 0); // Attach PWM to channel 0
    ledcSetup(0, 20000, 8);  // 20kHz PWM, 8-bit resolution

    encoder.attachFullQuad(encA, encB);
    encoder.setCount(0);

    current_velocity = 0;
    target_velocity = 0;
    pwm_in = 0;
    pwm_change = 0;
    velocityPID = QuickPID(&current_velocity, &pwm_change, &target_velocity, 0, 0, 0, QuickPID::Action::direct);
    velocityPID.SetOutputLimits(-255, 255);
    velocityPID.SetMode(QuickPID::Control::automatic);
    velocityPID.SetSampleTimeUs(FREQ_TO_US_PERIOD(taskFrequency));
    velocityPID.SetAntiWindupMode(QuickPID::iAwMode::iAwClamp);

    // Start task
    xTaskCreate(
        motorTaskEntry, // Task function
        "MotorTask",    // Name
        4096,           // Stack size
        this,           // Parameter
        1,              // Priority
        &task_handle_   // Task handle
    );

    return true;
}

void MotorTask::setVelocity(float speed)
{
    target_velocity = speed;
}

float MotorTask::getPosition()
{
    return (encoder.getCount() / static_cast<float>(cpr)) * 5.0;
}

float MotorTask::getVelocity()
{
    return current_velocity;
}

void MotorTask::taskFunction()
{
    float last_count = 0;
    TickType_t last_wake = xTaskGetTickCount();

    while (true)
    {
        // Wait for the PID sample time
        xTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000 / taskFrequency));
        float new_count = (encoder.getCount() / static_cast<float>(cpr)) * 5.0;
        float delta = new_count - last_count;

        // Calculate velocity (ticks per PID_SAMPLE_TIME_MS)
        current_velocity = delta * static_cast<float>(taskFrequency);
        last_count = new_count;

        // Compute new PWM using PID
        velocityPID.Compute();
        pwm_in += pwm_change;
        pwm_in = constrain(pwm_in, -255, 255);
        // Set motor direction and power
        
        Serial.printf("pwm_in = %.2f\n", pwm_in);

        if (digitalRead(LS2))
        {
            // Stop if limit switch is pressed
            if (target_velocity < 0)
            {
                target_velocity = 0;
                pwm_in = 0;
            }
            encoder.setCount(0); // Reset encoder count
        }
        if (target_velocity == 0.0)
        {
            pwm_in = 0;
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, LOW);
            analogWrite(enPin, 0);
        }
        else if (pwm_in > 0)
        {
            digitalWrite(in1Pin, HIGH);
            digitalWrite(in2Pin, LOW);
            float scaled_pwm = map(pwm_in, 0, 255, 35, 255);
            analogWrite(enPin, static_cast<uint8_t>(scaled_pwm));
        }
        else if (pwm_in < 0)
        {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, HIGH);
            float scaled_pwm = map(pwm_in, -255, 0, -255, -35);
            analogWrite(enPin, static_cast<uint8_t>(-scaled_pwm));
        }
    }
}
