#include "LimitSw.h"

void LimitSwitches::SetSwitches(){

    for (int limit_switch : switch_limits){
        gpio_init(limit_switch);
        gpio_set_dir(limit_switch, GPIO_IN);
        gpio_pull_up(limit_switch);
    }
}
bool LimitSwitches::LowTriggered(){
    return(gpio_get(LIMIT_SW_LEFT));
}
bool LimitSwitches::UpperTriggered(){
    return(gpio_get(LIMIT_SW_RIGHT));
}