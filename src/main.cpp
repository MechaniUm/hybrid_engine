#include <Arduino.h>
#include <AccelStepper.h>
#include <Encoder.h>
#include <GyverTimers.h>
#include "button.h"
#include "config.h"
#include "light.h"
#include "stepper.h"
#include "indicator.h"

enum Stage {
    SLEEP,
    WORK,
    WAIT
};

Stage stage = SLEEP;

double charge = 0.0;
int32_t record = 0;
long long last_action_time = 0;
long long start_time = 0;
long long charge_timer = 0;
boolean sleepMod = true;
boolean can_be_boosted = false;
boolean boosted = false;
boolean sleep_started = true;
boolean electric_lamp_enable = false;

Encoder encoder(2, 3);
int32_t last_value = 0;
int32_t start_value = 0;
int32_t new_value = 0;
boolean electric_started = false;
boolean slowing = false;
boolean magnet_disabled = true;
boolean ICE_started = false;
boolean ICE_working = false;
boolean normalized = false;
boolean zeroed = false;
boolean can_uncharge = true;
long long booster_timer = 0;
long long uncharge_timer = 0;
long long result_timer = 0;
long long electric_light_time = 0;
long long display_timer = 0;
long long sleep_timer = 0;
void setup() {
    buttonInit();
    stepperInit();
    lightInit();
    indicatorInit();
    record = indicatorNormalize();
    displayTwoNumbers(0, record);
    Timer5.setPeriod(STEPPER_TICK);
    Timer5.enableISR();
}

// run stepper engine every STEPPER_TICK micros
ISR(TIMER5_A) {
    stepperRun();
}

void loop() {
    new_value = encoder.read();
    if (last_value > new_value) {
        last_value = 0;
        new_value = 0;
        encoder.reset(0);
    }
    if (new_value > ENCODER_TRESHOLD) {
        int32_t tmp = new_value - last_value;
        last_value = 0;
        new_value = tmp;
    }
    switch (stage) {
    case SLEEP: {
         // executing one time when sleep is on
        if (sleep_started) {
            sleep_started = false;
            displayTwoNumbers(0, record);
            lightOff();
            new_value = 0;
            last_value = 0;
            encoder.reset(0);
            digitalWrite(MAGNET, LOW);
            magnet_disabled = true;
        }
        // if one full rotate then start working
        if (new_value - last_value > STEPS_PER_ROTATE) {
            stage = WORK;
            start_value = new_value;
            last_action_time = millis();
        }
        break;
    }

    case WAIT:
    {

        // display result RESULT_DISPLAY_TIME ms after end of rotating
        if (millis() - display_timer > RESULT_DISPLAY_TIME && !zeroed) {
            displayTwoNumbers(0, record);
            zeroed = true;
        }

        // if one full rotate then start working
        if (new_value - last_value > STEPS_PER_ROTATE) { 
            stage = WORK;
            start_value = new_value;
            digitalWrite(ENABLE, HIGH);
            last_action_time = millis();
        }
        // if doesn't rotating TIME_BEFORE_SLEEP ms then sleep
        if (millis() - display_timer > TIME_BEFORE_SLEEP) {
            stage = SLEEP;
            if (record > 150) {
                record = indicatorNormalize();
                sleep_timer = millis();
            }
        }
        break;
    }

    case WORK:
    {
        // executing one time when start work
        if (!sleep_started) {
            digitalWrite(ENABLE, HIGH);
            sleep_started = true;
            // start_time = millis();
        }
        // if rotating clockwise and NOT boosted add charge
        if (new_value > last_value) {
            display_timer = millis();
            if (charge < CHARGE_MAX && !slowing && !boosted)
                charge += CHARGE_PER_STEP;
            last_action_time = millis();
            last_value = new_value;
        }
        // if boosted more then BOOSTER_TIME_WITHOUT_UNCHARGE ms then can uncharge
        if (!can_uncharge && millis() - booster_timer > BOOSTER_TIME_WITHOUT_UNCHARGE && abs(stepper.speed()) >= STEPPER_SPEED_BOOSTED) {
            can_uncharge = true;
        }
        // if can uncharge then uncharge every ELECTRIC_ENGINE_TICK ms
        if (can_uncharge && stepper.isRunning() && millis() - uncharge_timer > ELECTRIC_ENGINE_TICK) {
            
            display_timer = millis();
            if (ICE_working)
                charge -= UNCHARGE_PER_STEP_WITH_ICE;
            else
                charge -= UNCHARGE_PER_STEP;

            if (boosted)
                charge -= UNCHARGE_PER_STEP_BOOST;
            uncharge_timer = millis();
        }

        // after changing charge it can be outer bounds
        if (charge < 0)
            charge = 0;
        else if (charge > CHARGE_MAX)
            charge = CHARGE_MAX;

        // if user maked one rotate when ICE off then enable ICE
        if (!boosted && new_value - start_value > STEPS_PER_ROTATE && !ICE_started && millis() - last_action_time < TIME_BEFORE_WAIT) {
            // if electric engine not running magnet should work
            if (!stepper.isRunning()) {
                digitalWrite(MAGNET, HIGH);
                magnet_disabled = false;
            }
            ICELightOn();
            ICE_working = true;
            ICE_started = true;
        }
        
        if (electric_started) {
            // calculate time after start of wheel's rotating
            int delta_time = int((millis() - start_time) / 1000);
            if (delta_time > 999) {
                delta_time = 999;
            }
            // update record
            record = max(record, delta_time);

            // display record and current time with indicators
            displayTwoNumbers(delta_time, record);
        }

        // update charge light every CHARGE_LIGHT_TICK ms
        if (millis() - charge_timer > CHARGE_LIGHT_TICK) {
            chargeLightSet(charge);
            charge_timer = millis();
        }
        // start to stop electric engine
        if (!slowing && charge <= STEPPER_STOP_CHARGE && electric_started) {
            stepper.setAcceleration(STEPPER_STOP_ACCELERATION);
            electric_started = false;
            stepperStop();
        }
        // off electric engine light
        if (stepper.speed() == 0) {
            electric_lamp_enable = false;
            electricEngineLightOff();
            digitalWrite(MAGNET, HIGH);
            magnet_disabled = false;
        }
        
        // if charge more then STEPPER_START_CHARGE then enable electric engine
        if (charge > STEPPER_START_CHARGE && !electric_started) {
            stepper.setAcceleration(STEPPER_ACCELERATION);
            start_time = millis();
            electric_started = true;
            stepperStart();
            electric_light_time = millis();
        }

        // when electric engine running magnet should be disabled
        if (electric_started && !electric_lamp_enable && millis() - electric_light_time > ELECTRIC_ENABLE_TIME) {
            electric_lamp_enable = true;
            electricEngineLightOn();
            digitalWrite(MAGNET, LOW);
            magnet_disabled = true;
        }

        // if charge > STEPPER_BOOSTE_CHARGE then can be boosted
        if (!boosted && charge >= STEPPER_BOOST_CHARGE && abs(stepper.speed()) == STEPPER_SPEED) {
            can_be_boosted = true;
            buttonLightOn();
        }

        // boost if can when button was pressed
        if (!boosted && can_be_boosted) {
            if (buttonIsPressed()) {
                can_be_boosted = false;
                booster_timer = millis();
                can_uncharge = false;
                if (charge > 100)
                    charge = 100;
                buttonLightOff();
                boosted = true;
                Timer5.pause();
                stepperBoostOn();
                Timer5.resume();
            }
        }
        // if not boosted disable button light when charge < BUTTON_LIGHT_CHARGE
        if (charge < BUTTON_LIGHT_CHARGE && can_be_boosted) {
            buttonLightOff();
            can_be_boosted = false;
        }
        // disable boost when charge < STEPPER_BOOST_STOP_CHARGE
        if (boosted && !slowing && charge < STEPPER_BOOST_STOP_CHARGE) {
            stepperBoostOff();
            slowing = true;
        }
        if (slowing && charge < STEPPER_BOOST_STOP_CHARGE && abs(stepper.speed()) <= STEPPER_SPEED + 50) {
            slowing = false;
            boosted = false;
            stepperResume();
            electric_started = true;
        }

        if (millis() - last_action_time > TIME_BEFORE_WAIT) {
            // disable ICE when user doesn't rotate
            ICELightOff();
            if (charge > 100) {
                charge = 100;
            }
            ICE_working = false;
            start_value = new_value;
            digitalWrite(MAGNET, LOW);
            magnet_disabled = true;
            ICE_started = false;
            // if nothing is working go to WAIT mode
            if (!stepper.isRunning()) {
                charge = 0;
                digitalWrite(ENABLE, LOW);
                chargeLightSet(0);
                result_timer = millis();
                zeroed = false;
                stage = WAIT;
                display_timer = millis();
            }
        }
        break;
    }
    
    default:
        break;
    }
}