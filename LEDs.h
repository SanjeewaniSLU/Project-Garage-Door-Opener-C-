
#include <iostream>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#ifndef PICO_MODBUS_LEDS_H
#define PICO_MODBUS_LEDS_H

class LEDs {
public:
    LEDs(int pin, bool input = true, bool pullup = true, bool invert = false);
    LEDs(const LEDs &) = delete;
    bool read();
    bool operator()();
    void write(bool value);
    void operator()(bool value);
    explicit operator uint() const;

private:
    const uint pin_num;
    bool input;
    bool pullup;
    bool invert;
};




#endif //PICO_MODBUS_LEDS_H