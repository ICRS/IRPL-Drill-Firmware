#ifndef MOTORTASK_H
#define MOTORTASK_H

#include <Arduino.h>
#include <QuickPID.h>
#include <ESP32Encoder.h>
#define FREQ_TO_US_PERIOD(freq) (1000000UL / (freq))


class MotorTask
{
private:

    uint8_t in1Pin;
    uint8_t in2Pin;
    uint8_t enPin;
    uint8_t encA;
    uint8_t encB;

public:
    MotorTask() {}

    bool begin(uint8_t in1, uint8_t in2, uint8_t en, uint8_t encA, uint8_t encB, int16_t freq, int16_t cpr, bool limit=0);

    bool setPID(float vel_kp, float vel_ki, float vel_kd);

    void setVelocity(float speed);

    float getPosition();
    
    float getVelocity();

    void taskFunction();

    QuickPID velocityPID;
    float pwm_in;

// private:
    TaskHandle_t task_handle_;
    int16_t taskFrequency;
    float current_velocity;
    float target_velocity;
    ESP32Encoder encoder;
    int16_t cpr;
    float pwm_change;
    bool limit;
};

#endif
