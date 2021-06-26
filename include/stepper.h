#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include <AccelStepper.h>
#include "config.h"

AccelStepper stepper(1, STEP, DIR);

void stepperInit() {
    pinMode(MAGNET, OUTPUT);
    pinMode(ENABLE, OUTPUT);
    digitalWrite(MAGNET, HIGH);
    digitalWrite(ENABLE, HIGH);
    stepper.setMaxSpeed(STEPPER_SPEED);
    stepper.setAcceleration(STEPPER_ACCELERATION);
}

void stepperResume() {
    stepper.setSpeed(-STEPPER_SPEED);
    stepper.setMaxSpeed(STEPPER_SPEED);
    stepper.setAcceleration(STEPPER_ACCELERATION);
    stepper.moveTo(-2147483647);
}

void stepperStop() {
    stepper.stop();
}

void stepperRun() {
    stepper.run();
}

void stepperStart() {

    stepper.setCurrentPosition(0);
    stepper.moveTo(-2147483647);
}

void stepperBoostOn() {
    stepper.setAcceleration(STEPPER_ACCELERATION_BOOSTED);
    stepper.setMaxSpeed(STEPPER_SPEED_BOOSTED);
}

void stepperBoostOff() {
    // stepper.move(10001);
    stepper.stop();
}

#endif
