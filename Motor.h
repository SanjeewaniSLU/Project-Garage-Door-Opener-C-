#ifndef PICO_MODBUS_MOTOR_H
#define PICO_MODBUS_MOTOR_H

#include <array>
#include "pico/stdlib.h"
#include "LimitSw.h"
#include "RotEncoder.h"
#include "LEDs.h"
#include "eeprom.h"
#include "Door.h"


class Door;

class Motor{
private:
    static const int IN1 = 2, IN2 = 3, IN3 = 6, IN4 = 13;
    const int half_step_sequence[8][4] = { //--------------------------------------------------------------------------
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 1},
            {0, 0, 0, 1},
            {1, 0, 0, 1},
    };

    int step_index;
    int direction;
    RotaryEncoder &encoder;
    LEDs &led_2;
    EEPROM &eeprom;
    Door &door;

    int encoder_steps;
    bool stop_flag= true;
    int motor_step_count;

public:
    Motor(RotaryEncoder &enc, LEDs &led_2, EEPROM &eeprom, Door &door);

    void SetupMotor();

    void RotateMotor();

    void StopMotor();

    void SetMotorDirection(int dir);

    void MoveMotor(int steps);

    void MotorCalib(LimitSwitches &garage_switches);

    int GetDirection();

    void SetStopFlag();

    bool GetStopFlag();

    bool MoveDoor(LimitSwitches &garage_switches, int steps, bool &calibrated);

};


#endif //MOTOR_H