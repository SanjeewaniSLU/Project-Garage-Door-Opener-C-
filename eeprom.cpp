#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "eeprom.h"

#define I2C_PORT i2c1
#define EEPROM_ADDR 0x50
#define SDA_PIN 14
#define SCL_PIN 15
#define BAUD_RATE 100000

#define DOOR_STATE 0x7FFE
#define CALIB_STATUS 0x7FFC

EEPROM::EEPROM() {
    i2c_init(I2C_PORT, BAUD_RATE);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

void EEPROM::eeprom_write_system_state(uint16_t addr, uint8_t data) {

    uint8_t buffer[4];
    buffer[0] = (uint8_t)((addr >> 8) & 0xFF);
    buffer[1] = (uint8_t)(addr & 0xFF);
    buffer[2] = data;
    buffer[3] = ~data;

    i2c_write_blocking(I2C_PORT, EEPROM_ADDR, buffer, 4, false);
    sleep_ms(30);
}

values_check EEPROM::eeprom_read_system_state(uint16_t addr) {
    uint8_t addr_buf[2] = {(uint8_t)((addr >> 8) & 0xFF), (uint8_t)(addr & 0xFF)};
    uint8_t data[2];

    i2c_write_blocking(I2C_PORT, EEPROM_ADDR, addr_buf, 2, true);
    i2c_read_blocking(I2C_PORT, EEPROM_ADDR, data, 2, false);
    values_check valuesCheck = {data[0], data[1]};
    return valuesCheck;
}

bool EEPROM::value_is_valid(values_check *valuesCheck) {
    return valuesCheck->value == (uint8_t)~valuesCheck->inv_value;
}