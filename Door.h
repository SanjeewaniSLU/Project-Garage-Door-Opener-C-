#include <iostream>
#ifndef PICO_MODBUS_DOOR_H
#define PICO_MODBUS_DOOR_H



enum class DoorState{
    UNKNOWN,
    CLOSED,
    OPENING,
    OPENED,
    CLOSING,
};

class Door{
private:
    DoorState state;

public:
    Door();
    DoorState GetState() const;
    void SetState(DoorState new_state);
};

std::ostream& operator<<(std::ostream& os, const DoorState& state);


#endif //PICO_MODBUS_DOOR_H