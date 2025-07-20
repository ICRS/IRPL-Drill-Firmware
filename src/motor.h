#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor
{
private:
    uint8_t in1Pin;
    uint8_t in2Pin;
    uint8_t enPin;

public:
    Motor() {}
    Motor(uint8_t in1, uint8_t in2, uint8_t en)
    {
        in1Pin = in1;
        in2Pin = in2;
        enPin = en;

        pinMode(in1Pin, OUTPUT);
        pinMode(in2Pin, OUTPUT);
        pinMode(enPin, OUTPUT);
    }

    void drive(int speed)
    {
        speed = constrain(speed, -255, 255); // Clamp to allowed range

        if (speed > 0)
        {
            digitalWrite(in1Pin, HIGH);
            digitalWrite(in2Pin, LOW);
            analogWrite(enPin, speed);
        }
        else if (speed < 0)
        {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, HIGH);
            analogWrite(enPin, -speed); // Use absolute value
        }
        else
        {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, LOW);
            analogWrite(enPin, 0);
        }
    }
};

#endif
