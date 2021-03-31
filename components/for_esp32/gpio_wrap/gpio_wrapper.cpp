#include "gpio_wrapper.h"


#include <esp_log.h>

namespace bk {

GPIOWrapper::GPIOWrapper() {
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_SEL_21 | GPIO_SEL_22 | GPIO_SEL_23 | GPIO_SEL_34;

    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);

    // install gpio isr service
    gpio_install_isr_service(0);
}

GPIOWrapper::~GPIOWrapper() {
    gpio_isr_handler_remove((gpio_num_t)LU);
    gpio_isr_handler_remove((gpio_num_t)LD);
    gpio_isr_handler_remove((gpio_num_t)RU);
    gpio_isr_handler_remove((gpio_num_t)RD);
}

int GPIOWrapper::getLevel(int gpio_pin) {
    return gpio_get_level((gpio_num_t)gpio_pin);
}

esp_err_t GPIOWrapper::setLevel(int gpio_pin, int level) {
    return gpio_set_level((gpio_num_t)gpio_pin, level);
}

esp_err_t GPIOWrapper::registerInterruptHandler(int gpio_pin, gpio_isr_t handler, void *arg) {
        // hook isr handler for specific gpio pin
    return gpio_isr_handler_add((gpio_num_t)gpio_pin, handler, arg);
}

}  // namespace bk