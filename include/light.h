#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

Adafruit_NeoPixel chargeLight(10, 12, NEO_GRB + NEO_KHZ800);

void ICELightOn() {
    analogWrite(LAMP_0, 255);
}

void ICELightOff() {
    analogWrite(LAMP_0, 0);
}

void electricEngineLightOn() {
    digitalWrite(LAMP_1, HIGH);
}

void electricEngineLightOff() {
    digitalWrite(LAMP_1, LOW);
}

void buttonLightOn() {
    analogWrite(BUTTON_LIGHT, 255);
}

void buttonLightOff() {
    analogWrite(BUTTON_LIGHT, 0);
}

// display gradient colors of charge
void chargeLightSet(int value) {
    chargeLight.clear();
    if (value / 10 >= 1) {
        chargeLight.setPixelColor(0, chargeLight.Color(70, 0, 0));
    }
    if (value / 10 >= 2) {
        chargeLight.setPixelColor(1, chargeLight.Color(70, 20, 0));
    }
    if (value / 10 >= 3) {
        chargeLight.setPixelColor(2, chargeLight.Color(70, 40, 0));
    }
    if (value / 10 >= 4) {
        chargeLight.setPixelColor(3, chargeLight.Color(70, 60, 0));
    }
    if (value / 10 >= 5) {
        chargeLight.setPixelColor(4, chargeLight.Color(70, 70, 0));
    }
    if (value / 10 >= 6) {
        chargeLight.setPixelColor(5, chargeLight.Color(70, 70, 0));
    }
    if (value / 10 >= 7) {
        chargeLight.setPixelColor(6, chargeLight.Color(60, 70, 0));
    }
    if (value / 10 >= 8) {
        chargeLight.setPixelColor(7, chargeLight.Color(40, 70, 0));
    }
    if (value / 10 >= 9) {
        chargeLight.setPixelColor(8, chargeLight.Color(20, 70, 0));
    }
    if (value / 10 >= 10) {
        chargeLight.setPixelColor(9, chargeLight.Color(0, 70, 0));
    }
    chargeLight.show();
}

void lightInit() {
    chargeLight.begin();
    chargeLight.clear();
    for (int i = 0; i < 10; i++) {
        chargeLight.setPixelColor(i, chargeLight.Color(0, 0, 0));
    }
    chargeLight.show();
    pinMode(LAMP_0, OUTPUT);
    pinMode(LAMP_1, OUTPUT);
    pinMode(BUTTON_LIGHT, OUTPUT);
}

void lightOff() {
    buttonLightOff();
    electricEngineLightOff();
    ICELightOff();
    for (int i = 0; i < 10; i++) {
        chargeLight.setPixelColor(i, chargeLight.Color(0, 0, 0));
    }
    chargeLight.show();
}

#endif
