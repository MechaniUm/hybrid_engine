#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "stepper.h"
#include "config.h"

void buttonInit() {
    pinMode(BUTTON, INPUT);
}

boolean buttonIsPressed() {
    return digitalRead(BUTTON) == LOW;
}

void boost() {
    stepperBoostOn();
}

#endif