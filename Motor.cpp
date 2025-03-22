#include "Motor.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "LEDs.h"

#define DOOR_STATE_AD 0x7FFE


Motor::Motor(RotaryEncoder &enc, LEDs &led_2, EEPROM &eeprom, Door &door) : step_index(0), direction(1), encoder(enc), led_2(led_2), eeprom(eeprom), door(door) {
    SetupMotor();
}

void Motor::SetupMotor(){

    std::array<int, 4> motor_pins = {IN1, IN2, IN3, IN4};

    for (int pin : motor_pins){
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
    }
}

void Motor::RotateMotor(){

    if (direction == -1){
        step_index = (step_index +1) % 8;
    }
    else if (direction == 1){
        step_index = (step_index -1 + 8 ) % 8;
    }
    gpio_put(IN1, half_step_sequence[step_index][0]);
    gpio_put(IN2, half_step_sequence[step_index][1]);
    gpio_put(IN3, half_step_sequence[step_index][2]);
    gpio_put(IN4, half_step_sequence[step_index][3]);
    sleep_ms(1);

}

void Motor::StopMotor(){

    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);
    step_index = 0;
    direction = 0;
}

void Motor::SetMotorDirection(int dir){
    direction = dir;
}

void Motor::MoveMotor(int steps){
    for(int i=0; i < steps; i++){
        RotateMotor();
    }
}

bool Motor::MoveDoor(LimitSwitches &garage_switches, int run_steps, bool &calibrated){
    int steps_done = 0;
    encoder.ResetStepCount();
    int current_step_encoder = encoder.GetStepCount();
    int last_step_encoder = 0;

    if(GetDirection() == 1){
        eeprom.eeprom_write_system_state(DOOR_STATE_AD, 5);
        while(garage_switches.UpperTriggered() !=0){
            led_2(true);
            if(GetStopFlag()){
                led_2(false);
                StopMotor();
                encoder_steps = encoder.GetStepCount();
                return true;
            }
            else{
                MoveMotor(1);
                steps_done++;
                if(steps_done % 300 == 0){
                    current_step_encoder = encoder.GetStepCount();
                    if(current_step_encoder > last_step_encoder){
                        last_step_encoder = current_step_encoder;
                    }
                    else{
                        SetStopFlag();
                        calibrated = false;
                    }
                }
            }
        }
        return false;
    }
    else if(GetDirection() == -1) {
        eeprom.eeprom_write_system_state(DOOR_STATE_AD, 6) ;
        while(garage_switches.LowTriggered() !=0){
            led_2(true);
            if(GetStopFlag()){
                led_2(false);
                StopMotor();
                return true;
            }
            else{
                MoveMotor(1);
                steps_done++;
                if(steps_done % 300 == 0){
                    current_step_encoder = encoder.GetStepCount();
                    if(current_step_encoder > last_step_encoder){
                        last_step_encoder = current_step_encoder;
                    }
                    else{
                        SetStopFlag();
                        calibrated = false;
                    }
                }

            }
        }
        return false;
    }
}


void Motor::MotorCalib(LimitSwitches &garage_switches){
    motor_step_count = 0;
    printf("Calibrating...\n");
    SetMotorDirection(1);
    while(garage_switches.UpperTriggered() !=0){
        MoveMotor(1);
    }
    sleep_ms(5);
    StopMotor();
    sleep_ms(5);

    SetMotorDirection(-1);
    encoder.ResetStepCount();
    while(garage_switches.LowTriggered() !=0){
        MoveMotor(1);
        motor_step_count++;
    }
    sleep_ms(5);
    StopMotor();
    encoder_steps = encoder.GetStepCount();

    printf("System is calibrated.\n");
    //printf("Total number of steps between switches: %d \n", motor_step_count);
    //printf("Encoder steps between switches: %d \n", encoder_steps);
}

int Motor::GetDirection(){
    return direction;
}

void Motor::SetStopFlag(){
    stop_flag = !stop_flag;
}

bool Motor::GetStopFlag(){
    return stop_flag;
}