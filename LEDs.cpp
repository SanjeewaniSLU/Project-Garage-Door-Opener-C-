
#include "hardware/gpio.h"
#include "LEDs.h"


LEDs::LEDs(int pin, bool input, bool pullup, bool invert): pin_num(pin), input(input), pullup(pullup), invert(invert) {
    gpio_init(pin_num);

    if (input) {
        gpio_set_dir(pin_num, GPIO_IN);
        if (pullup) {
            gpio_pull_up(pin_num);
        } else {
            gpio_pull_down(pin_num);
        }
    } else {
        gpio_set_dir(pin_num, GPIO_OUT);
        gpio_put(pin_num, 0);
    }

}

bool LEDs::read() {
    return gpio_get(pin_num);
}

bool LEDs::operator()(){
    return read();
}

void LEDs::write(bool value) {
    gpio_put(pin_num, value);
}

void LEDs::operator()(bool value){
    write(value);
}
LEDs::operator uint() const{
    return pin_num;
}