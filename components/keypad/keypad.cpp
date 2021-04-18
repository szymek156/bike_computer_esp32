#include "keypad.h"

#include <esp_log.h>

namespace bk {
Keypad::Keypad() : AbstractTask(sizeof(KeypadData), QUEUE_SIZE), data_{} {
}

void Keypad::start() {
    ESP_LOGI(TAG, "Start");

    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_SEL_21 | GPIO_SEL_22 | GPIO_SEL_23 | GPIO_SEL_34;

    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);

    gpio_.registerInterruptHandler(LU, buttonPressed, this);
    gpio_.registerInterruptHandler(RU, buttonPressed, this);
    gpio_.registerInterruptHandler(LD, buttonPressed, this);
    gpio_.registerInterruptHandler(RD, buttonPressed, this);

    start_execution(TAG);
}

void Keypad::run() {
    ESP_LOGI(TAG, "Run");

    while (true) {
        ESP_LOGD(TAG, "Waiting for interrupt...");
        ulTaskNotifyTake(true, portMAX_DELAY);

        data_.lu_pressed = gpio_.getLevel(LU);
        data_.ru_pressed = gpio_.getLevel(RU);
        data_.ld_pressed = gpio_.getLevel(LD);
        data_.rd_pressed = gpio_.getLevel(RD);

        ESP_LOGD(TAG,
                 "Got notification about buttons: %d %d %d %d",
                 data_.lu_pressed,
                 data_.ru_pressed,
                 data_.ld_pressed,
                 data_.rd_pressed);

        if (xQueueSendToBack(queue_, &data_, 0) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send data");
        }
    }
}

void Keypad::buttonPressed(void *arg) {
    auto *that = (Keypad *)arg;
    BaseType_t higher_prio_was_woken = false;
    // TODO: filter debounce
    // Set timer for like 20ms, in callback call this:

    // TaskNotifyGive and Take are better alternatives to binary semaphore
    vTaskNotifyGiveFromISR(that->getTask(), &higher_prio_was_woken);

    // TODO: Why it's needed at all?
    portYIELD_FROM_ISR(higher_prio_was_woken);
}

}  // namespace bk