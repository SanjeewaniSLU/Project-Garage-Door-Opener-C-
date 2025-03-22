
#include "Door.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const DoorState& state) {
    switch (state) {
        case DoorState::UNKNOWN:   return os << "UNKNOWN";
        case DoorState::CLOSED:    return os << "CLOSED";
        case DoorState::OPENING:   return os << "OPENING";
        case DoorState::OPENED:    return os << "OPENED";
        case DoorState::CLOSING:   return os << "CLOSING";
        default:                   return os << "INVALID STATE";
    }
}

Door::Door() : state(DoorState::UNKNOWN) {}

DoorState Door::GetState() const{
    return state;
}

void Door::SetState(DoorState new_state){
    state = new_state;

}