

#ifndef PICO_MODBUS_ROTENCODER_H
#define PICO_MODBUS_ROTENCODER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/util/queue.h"


#define ROT_A_PIN 4
#define ROT_B_PIN 5


class RotaryEncoder{
private:
    int a_pin;
    int b_pin;
    queue_t events;
    int step_count;

public:
    RotaryEncoder();
    int GetStepCount();
    void ResetStepCount();
    void rot_interrupt();
    int  GetPIn();
};



#endif //PICO_MODBUS_ROTENCODER_H