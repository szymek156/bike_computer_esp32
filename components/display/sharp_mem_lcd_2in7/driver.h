/** @brief porting Adafruit library for Sharp memory LCD's to the ESP32
 *  https://github.com/adafruit/Adafruit_SHARP_Memory_Display
 */
#pragma once
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include <vector>

// #include <esp_log.h>

namespace bk {

class Driver {
 public:
    Driver(uint16_t width, uint16_t height);
    void clearDisplay();
    void setFrame(uint8_t *back);
    void draw();

    void toggleVcom();

 private:
    static constexpr const char *TAG = "MEM_LCD";
    /** @brief Custom allocator for DMA region */
    template <class T>
    struct DMAAllocator {
        typedef T value_type;

        DMAAllocator() = default;

        /** @brief Allocates memory in DMA region, set by default to 0 */
        [[nodiscard]] T *allocate(std::size_t n) {
            auto p = (T *)heap_caps_calloc(n, sizeof(uint8_t), MALLOC_CAP_DMA);

            // ESP_LOGI("DMAAllocator", "Allocating %u", n);

            if (p == nullptr)
            // GCC complains here, but code is completely fine
            // TODO: uncomment when GCC will be finally fixed
            // [[unlikely]]
            {
                // ESP_LOGE("DMAAllocator", "Unable to allocate %u bytes in DMA", n);
            }

            return p;
        }

        void deallocate(T *p, std::size_t /*n*/) noexcept {
            heap_caps_free(p);
        }
    };

    uint16_t width_;
    uint16_t height_;
    uint8_t vcom_;
    std::vector<uint8_t, DMAAllocator<uint8_t> > front_;

    spi_device_handle_t spi_;

    void initSPI();
    void clearDisplayBuffer();
    /** @brief Returns alternating vcom_ value, always use this method to get the vcom_ */
    uint8_t getToggledVCom();

    void digitalWrite(gpio_num_t pin, int value);
    int digitalRead(gpio_num_t pin);
    void transfer(uint8_t *data, size_t len);
};

}  // namespace bk
