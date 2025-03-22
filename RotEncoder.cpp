#include "hardware/gpio.h"
#include "pico/util/queue.h"
#include "RotEncoder.h"

RotaryEncoder::RotaryEncoder() : a_pin(ROT_A_PIN), b_pin(ROT_B_PIN), step_count(0) {
    gpio_init(a_pin);
    gpio_set_dir(a_pin, GPIO_IN);

    gpio_init(b_pin);
    gpio_set_dir(b_pin, GPIO_IN);

    queue_init(&events, sizeof(int), 10);
    gpio_set_irq_enabled(a_pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void RotaryEncoder::rot_interrupt() {
    if (gpio_get(ROT_B_PIN) == 0) {
        int clockwise = 1;
        queue_try_add(&events, &clockwise);
        step_count++;
    } else {
        int anticlockwise = -1;
        queue_try_add(&events, &anticlockwise);
        step_count++;
    }
}

int RotaryEncoder::GetStepCount(){
    return step_count;
}

void RotaryEncoder::ResetStepCount() {
    step_count = 0;
}
int  RotaryEncoder::GetPIn(){
    return a_pin;
}