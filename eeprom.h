#include <unistd.h>
#ifndef PICO_MODBUS_EEPROM_H
#define PICO_MODBUS_EEPROM_H

typedef struct {
    uint8_t value;
    uint8_t inv_value;
} values_check;

class EEPROM{
private:

public:
    EEPROM();
    void eeprom_write_system_state(uint16_t addr, uint8_t data);
    values_check eeprom_read_system_state(uint16_t addr);
    bool value_is_valid(values_check *valuesCheck);
};


#endif //PICO_MODBUS_EEPROM_H