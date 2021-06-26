#ifndef INDICATOR_H
#define INDICATOR_H
#include <Arduino.h>
#include "config.h"


byte numberToByte(int n) {
    switch (n) {
    case 0:
        return B11111100;
        break;
    case 1:
        return B01100000;
        break;
    case 2:
        return B11011010;
        break;
    case 3:
        return B11110010;
        break;
    case 4:
        return B01100110;
        break;
    case 5:
        return B10110110;
        break;
    case 6:
        return B10111110;
        break;
    case 7:
        return B11100000;
        break;
    case 8:
        return B11111110;
        break;
    case 9:
        return B11110110;
        break;
    case 10:
        return B11101110; // Hexidecimal A
        break;
    case 11:
        return B00111110; // Hexidecimal B
        break;
    case 12:
        return B10011100; // Hexidecimal C or use for Centigrade
        break;
    case 13:
        return B01111010; // Hexidecimal D
        break;
    case 14:
        return B10011110; // Hexidecimal E
        break;
    case 15:
        return B10001110; // Hexidecimal F or use for Fahrenheit
        break;  
    default:
        return B10010010; // Error condition, displays three vertical bars
        break;   
    }
}

void displayTwoNumbers(int n_1, int n_2)
{
    if (n_1 == -1) {
        digitalWrite(RCK, LOW);  // prepare shift register for data
        shiftOut(DATA, SRCK, LSBFIRST, B00000000);
        shiftOut(DATA, SRCK, LSBFIRST, B00000000);
        shiftOut(DATA, SRCK, LSBFIRST, B00000000);
        shiftOut(DATA, SRCK, LSBFIRST, B00000000);
        shiftOut(DATA, SRCK, LSBFIRST, B00000000);
        shiftOut(DATA, SRCK, LSBFIRST, B00000000);
        digitalWrite(RCK, HIGH); // update digit display
    } else {
        digitalWrite(RCK, LOW);  // prepare shift register for data
        byte bits = numberToByte(n_1%10) ; //first digit
        shiftOut(DATA, SRCK, LSBFIRST, bits);
        bits = numberToByte((n_1/10)%10) ; //second digit
        shiftOut(DATA, SRCK, LSBFIRST, bits);
        bits = numberToByte((n_1/100)%10) ; //third 
        shiftOut(DATA, SRCK, LSBFIRST, bits);

        bits = numberToByte(n_2%10) ; //first digit
        shiftOut(DATA, SRCK, LSBFIRST, bits);
        bits = numberToByte((n_2/10)%10) ; //second digit
        shiftOut(DATA, SRCK, LSBFIRST, bits);
        bits = numberToByte((n_2/100)%10) ; //third 
        shiftOut(DATA, SRCK, LSBFIRST, bits);
        digitalWrite(RCK, HIGH); // update digit display
    }
}

void indicatorInit() {
    pinMode(DATA, OUTPUT);
    pinMode(RCK, OUTPUT);
    pinMode(SRCK, OUTPUT);
}

void indicatorOff() {
    // displayTwoNumbers(-1, -1);
}

int indicatorNormalize() {
    int record = random(50, 150);
    displayTwoNumbers(0, record);
    return record;
}

#endif
