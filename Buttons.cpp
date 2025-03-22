#include <iostream>
#include <utility>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "Buttons.h"
#include "RotEncoder.h"
#include "MqttManager.h"


#define DEBOUNCE_DELAY_MS 400
#define CALIB_STATUS 0x7FFC
#define DOOR_STATE_AD 0x7FFE

Buttons* instance = nullptr;

Buttons::Buttons(int pin1, int pin2,int pin3, Motor& garage_motor, LimitSwitches& garage_switches, Door &garage_door, LEDs& led_1, LEDs& led_2, LEDs& led_3, EEPROM& eeprom, MqttManager &remote, std::shared_ptr<RotaryEncoder>encoder)
        : button1_pin(pin1), button2_pin(pin2), button3_pin(pin3), garage_motor(garage_motor), garage_switches(garage_switches), garage_door(garage_door), led_1(led_1), led_2(led_2), led_3(led_3), eeprom(eeprom),  remote(remote), encoder(std::move(encoder)),
          button2_pressed(false), sw0_sw2_pressed(false) {
    instance = this;
}

void Buttons::SetupButtons() {

    gpio_init(button1_pin);
    gpio_set_dir(button1_pin, GPIO_IN);
    gpio_pull_up(button1_pin);

    gpio_init(button2_pin);
    gpio_set_dir(button2_pin, GPIO_IN);
    gpio_pull_up(button2_pin);

    gpio_init(button3_pin);
    gpio_set_dir(button3_pin, GPIO_IN);
    gpio_pull_up(button3_pin);

    gpio_set_irq_enabled_with_callback(button1_pin, GPIO_IRQ_EDGE_FALL, true, &ButtonISR);
    gpio_set_irq_enabled_with_callback(button2_pin, GPIO_IRQ_EDGE_FALL, true, &ButtonISR);
    gpio_set_irq_enabled_with_callback(button3_pin, GPIO_IRQ_EDGE_FALL, true, &ButtonISR);

}

volatile uint32_t last_button2_press_time = 0;

void Buttons::ButtonISR(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (instance) {

        if(!gpio_get(instance->button2_pin) && (current_time - last_button2_press_time > DEBOUNCE_DELAY_MS)){
            instance->button2_pressed = true;
            instance->garage_motor.SetStopFlag();
            last_button2_press_time = current_time;
        }

        if (!gpio_get(instance->button1_pin) && !gpio_get(instance->button3_pin)) {
            instance->sw0_sw2_pressed = true;
        }
        if (gpio == instance->encoder->GetPIn()) {
            instance->encoder->rot_interrupt();

        }
    }
}

void Buttons::HandleButtonPress(const int run_steps, bool &calibrated){

    DoorState current_state = garage_door.GetState();

    switch (current_state) {
        case DoorState::CLOSED: {

            garage_door.SetState(DoorState::OPENING);
            eeprom.eeprom_write_system_state(DOOR_STATE_AD,4);
            remote.publish("status:OPENING");
            remote.checkMessages();
            current_state = garage_door.GetState();
            std::cout << "Current state: ";
            std::cout << current_state << std::endl;
            garage_motor.SetMotorDirection(1);

            bool steps_left = garage_motor.MoveDoor(garage_switches, run_steps, calibrated);

            if (steps_left) {
                if(!calibrated){
                    remote.publish("status:DOOR STUCK");
                    remote.checkMessages();
                    StuckCase();
                }
                else{
                    std::cout <<"Motor was stopped while opening."<<std::endl;
                    remote.publish("status:STOPPED");
                    remote.checkMessages();
                    garage_door.SetState(DoorState::CLOSING);
                    eeprom.eeprom_write_system_state(DOOR_STATE_AD,3);
                    button2_pressed = false;
                }

            } else {
                garage_door.SetState(DoorState::OPENED);
                eeprom.eeprom_write_system_state(DOOR_STATE_AD, 2);
                remote.publish("status:OPENED");
                remote.checkMessages();
                current_state = garage_door.GetState();
                std::cout <<"Current state: ";
                std::cout << current_state << std::endl;
                garage_motor.SetStopFlag();
                led_2(false);
            }
            break;
        }
        case DoorState::OPENED: {

            garage_door.SetState(DoorState::CLOSING);
            eeprom.eeprom_write_system_state(DOOR_STATE_AD,3);
            remote.publish("status:CLOSING");
            remote.checkMessages();
            current_state = garage_door.GetState();
            std::cout <<"Current state: ";
            std::cout << current_state << std::endl;
            garage_motor.SetMotorDirection(-1);
            bool steps_left = garage_motor.MoveDoor(garage_switches, run_steps, calibrated);

            if (steps_left) {
                if (!calibrated){
                    remote.publish("status:DOOR STUCK");
                    remote.checkMessages();
                    StuckCase();
                }
                else{
                    std::cout <<"Motor was stopped while closing."<<std::endl;
                    garage_door.SetState(DoorState::OPENING);
                    eeprom.eeprom_write_system_state(DOOR_STATE_AD,4);
                    button2_pressed = false;
                }

            } else {
                garage_door.SetState(DoorState::CLOSED);
                eeprom.eeprom_write_system_state(DOOR_STATE_AD, 1);
                remote.publish("status:CLOSED");
                remote.checkMessages();
                current_state = garage_door.GetState();
                std::cout <<"Current state: ";
                std::cout << current_state << std::endl;
                garage_motor.SetStopFlag();
                led_2(false);
            }
            break;
        }

        case DoorState::OPENING: {
            current_state = garage_door.GetState();
            std::cout <<"Current state: ";
            std::cout << current_state << std::endl;
            remote.publish("status:OPENING");
            remote.checkMessages();
            garage_motor.SetMotorDirection(1);
            bool steps_left = garage_motor.MoveDoor(garage_switches, run_steps, calibrated);

            if (steps_left) {
                if (!calibrated){
                    remote.publish("status:DOOR STUCK");
                    remote.checkMessages();
                    StuckCase();
                }
                else{
                    std::cout <<"Motor was stopped while opening" << std::endl;
                    garage_door.SetState(DoorState::CLOSING);
                    eeprom.eeprom_write_system_state(DOOR_STATE_AD,3);
                    remote.publish("status:STOPPED");
                    remote.checkMessages();
                    button2_pressed = false;
                }

            } else {
                garage_door.SetState(DoorState::OPENED);
                eeprom.eeprom_write_system_state(DOOR_STATE_AD, 2);
                current_state = garage_door.GetState();
                std::cout <<"Current state: ";
                std::cout << current_state << std::endl;
                remote.publish("status:OPENED");
                remote.checkMessages();
                button2_pressed = false;
                garage_motor.SetStopFlag();
                led_2(false);
            }
            break;
        }

        case DoorState::CLOSING: {
            current_state = garage_door.GetState();
            std::cout <<"Current state: ";
            std::cout << current_state << std::endl;
            remote.publish("status:CLOSING");
            remote.checkMessages();
            garage_motor.SetMotorDirection(-1);
            bool steps_left = garage_motor.MoveDoor(garage_switches, run_steps, calibrated );

            if (steps_left) {
                if (!calibrated){
                    remote.publish("status:DOOR STUCK");
                    remote.checkMessages();
                    StuckCase();
                }
                else{
                    std::cout <<"Motor was stopped while closing" << std::endl;
                    garage_door.SetState(DoorState::OPENING);
                    eeprom.eeprom_write_system_state(DOOR_STATE_AD,4);
                    remote.publish("status:STOPPED");
                    remote.checkMessages();
                    button2_pressed = false;
                }

            }else {
                garage_door.SetState(DoorState::CLOSED);
                eeprom.eeprom_write_system_state(DOOR_STATE_AD, 1);
                current_state = garage_door.GetState();
                std::cout <<"Current state: ";
                std::cout << current_state << std::endl;
                remote.publish("status:CLOSED");
                garage_motor.SetStopFlag();
                remote.checkMessages();
                led_2(false);
            }
            break;
        }
    }
}

void Buttons::HandleCalibration(bool &calibrated){
    std::cout<<"SW0 and SW2 are pressed!" << std::endl;
    sw0_sw2_pressed = false;
    garage_motor.MotorCalib(garage_switches);
    button2_pressed = false;
    calibrated = true;
    garage_door.SetState(DoorState::CLOSED);
    eeprom.eeprom_write_system_state(DOOR_STATE_AD, 1);
    DoorState current_state = garage_door.GetState();
    std::cout<<"Current state: ";
    std::cout << current_state<< std::endl;
    if(!garage_motor.GetStopFlag()){
        garage_motor.SetStopFlag();
    }
    button2_pressed = false;

}
 void Buttons::StuckCase(){

     if(!garage_motor.GetStopFlag()){
         garage_motor.SetStopFlag();
     }
     garage_door.SetState(DoorState::UNKNOWN);
     sw0_sw2_pressed = false;
     eeprom.eeprom_write_system_state(CALIB_STATUS, 0);
     std::cout<<"DOOR is STUCK. CALIBRATE THE SYSTEM"<<std::endl;
     while(WaitingCalib()){
             led_2(true);
             sleep_ms(200);
             led_2(false);
             sleep_ms(200);

     }
}
bool Buttons::WaitingCalib() {
    return !sw0_sw2_pressed;
}