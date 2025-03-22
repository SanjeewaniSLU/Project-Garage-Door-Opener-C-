#include <iostream>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "Motor.h"
#include "LimitSw.h"
#include "RotEncoder.h"
#include "Buttons.h"
#include "Door.h"
#include "LEDs.h"
#include "eeprom.h"
#include <memory>
#include "MqttManager.h"
#include "IPStack.h"

#define LIMIT_SW_LEFT 16
#define LIMIT_SW_RIGHT 17

#define SW0 9
#define SW1 8
#define SW2 7

#define DOOR_STATE_AD 0x7FFE
#define CALIB_STATUS 0x7FFC


int main(){

    stdio_init_all();
    sleep_ms(2000);

    printf("\nBoot...\n");

    LEDs led_1(22, false, false, false);
    LEDs led_2(21, false, false, false);
    LEDs led_3(20, false, false, false);

    led_1(false);
    led_2(false);
    led_3(false);

    Door garage_door;
    LimitSwitches garage_switches;
    EEPROM eeprom;
    auto encoder = std::make_shared<RotaryEncoder>();

    const char* WIFI_SSID = "SmartIotMQTT";
    const char* WIFI_PASS = "SmartIot";
    const char* MQTT_BROKER = "192.168.1.164";
    const int MQTT_PORT = 1883;
    const char* CMD_TOPIC = "garage1/door/command";
    const char* STATUS_TOPIC = "garage1/door/status";

    IPStack ipstack(WIFI_SSID, WIFI_PASS); // example
    MqttManager remote(MQTT_BROKER, MQTT_PORT,CMD_TOPIC,STATUS_TOPIC,ipstack);
    Motor garage_motor(*encoder, led_2, eeprom, garage_door);
    Buttons garage_buttons(SW0, SW1, SW2, garage_motor, garage_switches, garage_door, led_1, led_2, led_3, eeprom, remote, encoder);

    garage_motor.SetupMotor();
    garage_buttons.SetupButtons();

    int door_state_number =0;
    bool calibrated = false;
    int run_steps = 0;

    values_check  result_door = eeprom.eeprom_read_system_state(DOOR_STATE_AD);
    if(eeprom.value_is_valid(&result_door)){
        door_state_number = (int)result_door.value;
    }
    else{
        std::cout << "Invalid state of the system. CALIBRATE THE SYSTEM." << std::endl;
        calibrated = false;
        garage_door.SetState(DoorState::UNKNOWN);
        garage_buttons.sw0_sw2_pressed = false;
        garage_buttons.button2_pressed - false;
    }

    values_check result_system = eeprom.eeprom_read_system_state(CALIB_STATUS);

    if(eeprom.value_is_valid(&result_system)){
        calibrated = result_system.value;
        std::cout << std::boolalpha << "Read from EEPROM (calibrated): " << calibrated << std::endl;

        if(calibrated){
            if(!garage_motor.GetStopFlag()){
                garage_motor.SetStopFlag();
            }
            if(door_state_number == 1){
                garage_door.SetState(DoorState::CLOSED);
            }
            else if(door_state_number == 2){
                garage_door.SetState(DoorState::OPENED);
            }
            else if(door_state_number == 3){
                garage_door.SetState(DoorState::CLOSING);
            }
            else if(door_state_number == 4){
                garage_door.SetState(DoorState::OPENING);

            }
            else if(door_state_number == 5 || door_state_number == 6 ){
                garage_buttons.button2_pressed = true;
                if(garage_motor.GetStopFlag()){
                    garage_motor.SetStopFlag();
                }
                if(door_state_number == 5){
                    garage_door.SetState(DoorState::OPENING);
                }
                else{
                    garage_door.SetState(DoorState::CLOSING);
                }
            }
        }
    }
    else if (!eeprom.value_is_valid(&result_system) || !calibrated){
        std::cout << "Invalid state of the system. CALIBRATE THE SYSTEM." << std::endl;
        calibrated = false;
        garage_door.SetState(DoorState::UNKNOWN);
        garage_buttons.sw0_sw2_pressed = false;
        garage_buttons.button2_pressed - false;
    }
    while (true) {

        if (garage_buttons.sw0_sw2_pressed  && !calibrated && garage_door.GetState() == DoorState::UNKNOWN) {
            led_2(false);
            garage_buttons.sw0_sw2_pressed = false;
            led_1(true);
            led_3(true);
            garage_buttons.HandleCalibration(calibrated);
            led_1(false);
            led_3(false);
            eeprom.eeprom_write_system_state(CALIB_STATUS, 1);
            remote.publish("Calibrated");
        }

        if(garage_buttons.button2_pressed && calibrated){
            garage_buttons.button2_pressed = false;
            garage_buttons.HandleButtonPress(run_steps, calibrated);
        }
      remote.checkMessages();
    }
}