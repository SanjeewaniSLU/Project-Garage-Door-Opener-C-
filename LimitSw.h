
#ifndef PICO_MODBUS_LIMITSW_H
#define PICO_MODBUS_LIMITSW_H

#include "pico/stdlib.h"
#include <array>

#define LIMIT_SW_LEFT 16
#define LIMIT_SW_RIGHT 17


class LimitSwitches {
private:
    std::array<int, 2> switch_limits = {LIMIT_SW_LEFT,LIMIT_SW_RIGHT};

public:
    LimitSwitches(){SetSwitches();}

    void SetSwitches();
    bool LowTriggered();
    bool UpperTriggered();
};

#endif //PICO_MODBUS_LIMITSW_H