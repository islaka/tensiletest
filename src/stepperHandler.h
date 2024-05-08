#pragma once

#include "configurations.h"

#define STEP_PIN 32
#define DIR_PIN 33
#define EN_PIN 34

uint8_t steps_per_mm = 100;
unsigned long current = 0;
unsigned long last_time = 0;

class StepperHandler {
public:
    void setup() {
        pinMode(STEP_PIN, OUTPUT);
        pinMode(DIR_PIN, OUTPUT);
        pinMode(EN_PIN, OUTPUT);
    }
    void step() {
        current = millis();
        // step the motor with non-blocking delay
        if (current - last_time > 2) {
            last_time = current;
            digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
        }
    }
    void setDirection(bool direction) {
        digitalWrite(DIR_PIN, direction);
    }
};  