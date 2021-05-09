#include "weather.h"

#include "registers.h"
#include "sensor_data.h"

#include <thread>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
namespace bk {
static const i2c_port_t I2C_PORT = I2C_NUM_0;
static const int SDA_PIN = 19;
static const int SCL_PIN = 18;

Weather::Weather()
    : AbstractTask(sizeof(WeatherData)),
      i2c_(I2C(DEVICE_I2C_ADDRESS, I2C_PORT, SDA_PIN, SCL_PIN)) {
}

void Weather::start() {
    ESP_LOGD(TAG, "Initializing");

    // Configure the device

    // Set to altimeter, oversampling to 128
    CTRL_REG1 ctrl1 = {};
    ctrl1.OS = 0b111;
    ctrl1.ALT = 1;

    ESP_ERROR_CHECK(i2c_.write_slave_reg(Addr::CTRL_REG1, toBits(ctrl1)));

    // Events from temp and pressure
    PT_DATA_CFG ptCfg = {};
    ptCfg.DREM = 1;
    ptCfg.PDEFE = 1;
    ptCfg.TDEFE = 1;
    ESP_ERROR_CHECK(i2c_.write_slave_reg(Addr::PT_DATA_CFG, toBits(ptCfg)));

    // Activate
    ctrl1.SBYB = 1;
    ESP_ERROR_CHECK(i2c_.write_slave_reg(Addr::CTRL_REG1, toBits(ctrl1)));

    start_execution("Weather");
}

void Weather::run() {
    ESP_LOGI(TAG, "Is running...");

    auto isDataReady = [](uint8_t status) {
        return (status & STATUS_MASK::PTDR);
    };

    while (true) {
        uint8_t status;
        ESP_ERROR_CHECK(i2c_.read_slave_reg(Addr::STATUS, &status));

        ESP_LOGV(TAG, "Status register 0x%X", status);

        if (isDataReady(status)) {
            uint8_t out_p_msb;
            ESP_ERROR_CHECK(i2c_.read_slave_reg(Addr::OUT_P_MSB, &out_p_msb));

            uint8_t out_p_csb;
            ESP_ERROR_CHECK(i2c_.read_slave_reg(Addr::OUT_P_CSB, &out_p_csb));

            uint8_t out_p_lsb;
            ESP_ERROR_CHECK(i2c_.read_slave_reg(Addr::OUT_P_LSB, &out_p_lsb));

            float temp_csb = (out_p_lsb >> 4) / 16.0f;

            uint8_t out_t_msb;
            ESP_ERROR_CHECK(i2c_.read_slave_reg(Addr::OUT_T_MSB, &out_t_msb));

            uint8_t out_t_lsb;
            ESP_ERROR_CHECK(i2c_.read_slave_reg(Addr::OUT_T_LSB, &out_t_lsb));

            WeatherData data = {.altitude_m = (float)((out_p_msb << 8) | out_p_csb) + temp_csb,
                                .temp_c = out_t_msb + ((out_t_lsb >> 4) / 16.0f)};

            ESP_LOGD(TAG, "Alt %f [m], Temp %f [c]", data.altitude_m, data.temp_c);

            if (xQueueOverwrite(queue_, &data) != pdPASS) {
                ESP_LOGE(TAG, "Failed to send data");
            }

        } else {
            ESP_LOGW(TAG, "Data NOT ready");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    ESP_LOGI(TAG, "Leaving...");
}

}  // namespace bk
