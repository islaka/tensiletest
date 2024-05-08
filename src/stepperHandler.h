#pragma once

#include "configurations.h"

uint8_t steps_per_mm = 100; // T8 leadscrew with 2mm pitch, 1.8 degree stepper motor => 100 steps per mm

class StepperHandler {
private:
    unsigned long last_time = 0;
public:
    void setup() {
        pinMode(STEP_PIN, OUTPUT);
        pinMode(DIR_PIN, OUTPUT);
        // pinMode(EN_PIN, OUTPUT);
    }
    void step(int speed = 2, int stride = 1) {
        unsigned int count = 0;
        // step the motor with non-blocking delay
        while (count < stride) {
            if (millis() - last_time > speed) {
                last_time = millis();
                digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
                count++;
            }
        }
    }
    void setDirection(bool direction) {
        digitalWrite(DIR_PIN, direction);
    }
};
StepperHandler stepper;