#pragma once

#include "configurations.h"

#define STEP_PIN 32
#define DIR_PIN 33
#define EN_PIN 34

uint8_t steps_per_mm = 100;
unsigned long current = 0;

class StepperHandler {
public:
    void setup() {
        pinMode(STEP_PIN, OUTPUT);
        pinMode(DIR_PIN, OUTPUT);
        pinMode(EN_PIN, OUTPUT);
    }
    void step() {
        // 
    }
    void setDirection(bool direction) {
        digitalWrite(DIR_PIN, direction);
    }
    void enable() {
        digitalWrite(EN_PIN, LOW);
    }
    void disable() {
        digitalWrite(EN_PIN, HIGH);
    }
};  