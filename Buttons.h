
#ifndef PICO_MODBUS_BUTTONS_H
#define PICO_MODBUS_BUTTONS_H

#include <memory>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "Motor.h"
#include "LimitSw.h"
#include "Door.h"
#include "LEDs.h"
#include "eeprom.h"
#include "MqttManager.h"

class Buttons {
private:
    int button1_pin;
    int button2_pin;
    int button3_pin;
    Motor& garage_motor;
    LimitSwitches& garage_switches;
    Door& garage_door;
    LEDs& led_1;
    LEDs& led_2;
    LEDs& led_3;
    EEPROM& eeprom;
    MqttManager &remote;
    std::shared_ptr<RotaryEncoder>encoder;


public:
    volatile bool button2_pressed = false;
    volatile bool sw0_sw2_pressed = false;

    Buttons(int pin1, int pin2, int pin3, Motor& garage_motor, LimitSwitches& garage_switches, Door &garage_door, LEDs& led_1, LEDs& led_2, LEDs& led_3, EEPROM& eeprom,MqttManager &remote, std::shared_ptr<RotaryEncoder>encoder);
    void SetupButtons();
    static void ButtonISR(uint gpio, uint32_t events);
    void HandleButtonPress(const int run_steps, bool &calibrated);
    void HandleCalibration(bool &calibrated);
    void StuckCase();
    bool WaitingCalib();
};




#endif //PICO_MODBUS_BUTTONS_H