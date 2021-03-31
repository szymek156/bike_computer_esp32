#include "keypad.h"

#include "sensor_data.h"

#include <esp_log.h>

namespace bk {
Keypad::Keypad()
    : AbstractTask(sizeof(KeypadData)),
      lu_pressed_(false),
      ru_pressed_(false),
      ld_pressed_(false),
      rd_pressed_(false),
      button_pressed_(xSemaphoreCreateBinary()) {
}

void Keypad::start() {
    ESP_LOGI(TAG, "Start");
    gpio_.registerInterruptHandler(LU, handleLU, this);
    gpio_.registerInterruptHandler(RU, handleRU, this);
    gpio_.registerInterruptHandler(LD, handleLD, this);
    gpio_.registerInterruptHandler(RD, handleRD, this);

    start_execution(TAG);
}

void Keypad::run() {
    while (true) {
        ESP_LOGI(TAG, "Waiting for interrupt...");
        ulTaskNotifyTake(true, portMAX_DELAY);

        ESP_LOGI(TAG,
                 "Got notification about buttons: %d %d %d %d",
                 lu_pressed_,
                 ru_pressed_,
                 ld_pressed_,
                 rd_pressed_);
    }
}

void Keypad::buttonPressed(int gpio_pin) {
    int pressed_so_far = lu_pressed_ + ru_pressed_ + ld_pressed_ + rd_pressed_;

    switch (gpio_pin) {
        case LU: {
            lu_pressed_ = true;
            break;
        }
        case RU: {
            ru_pressed_ = true;
            break;
        }
        case LD: {
            ld_pressed_ = true;
            break;
        }
        case RD: {
            rd_pressed_ = true;
            break;
        }
        default:
            break;
    }

    int currently_pressed_ = lu_pressed_ + ru_pressed_ + ld_pressed_ + rd_pressed_;

    if (currently_pressed_ > pressed_so_far) {
        // Another button pressed_

        BaseType_t higher_prio_was_woken = false;

        vTaskNotifyGiveFromISR(getTask(), &higher_prio_was_woken);

        // TODO: Why it's needed at all?
        portYIELD_FROM_ISR(higher_prio_was_woken);
    }
    // currently_pressed_ == pressed__so_far <-- button toggling
    // TODO: filter debounce

    // currently_pressed_ < pressed__so_far <-- button released?
}

void Keypad::handleLU(void *arg) {
    auto *that = (Keypad *)arg;

    that->buttonPressed(LU);
}

void Keypad::handleRU(void *arg) {
    auto *that = (Keypad *)arg;

    that->buttonPressed(RU);
}

void Keypad::handleLD(void *arg) {
    auto *that = (Keypad *)arg;

    that->buttonPressed(LD);
}

void Keypad::handleRD(void *arg) {
    auto *that = (Keypad *)arg;

    that->buttonPressed(RD);
}
}  // namespace bk