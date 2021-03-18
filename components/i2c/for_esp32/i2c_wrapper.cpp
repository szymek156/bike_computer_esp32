#include "i2c_wrapper.h"

#include <driver/i2c.h>

namespace bk {
I2C::I2C(uint8_t dev_addr, i2c_port_t port_num, int sda_pin, int scl_pin)
    : dev_addr_(dev_addr),
      port_num_(port_num) {
    // Establish i2c communication on port
    // - set mode of operation as master
    // - assign GPIO pins for SDA and SCL
    // - set internal pullups resistors
    // - set clk speed

    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_pin;
    conf.scl_io_num = scl_pin;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    ESP_ERROR_CHECK(i2c_param_config(port_num_, &conf));

    // Install the driver
    ESP_ERROR_CHECK(i2c_driver_install(port_num_, I2C_MODE_MASTER, 0, 0, 0));
}

I2C::~I2C() {
    ESP_ERROR_CHECK(i2c_driver_delete(port_num_));
};

esp_err_t I2C::write_slave_reg(uint8_t reg_addr, const uint8_t *data, size_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));

    // first, send device address (indicating write) & register to be written
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (dev_addr_ << 1) | I2C_MASTER_WRITE, true));

    // send register we want
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg_addr, true));

    // write the data
    ESP_ERROR_CHECK(i2c_master_write(cmd, data, size, true));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    esp_err_t ret = i2c_master_cmd_begin(port_num_, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t I2C::read_slave_reg(uint8_t reg_addr, uint8_t *out_buff, size_t size) {
    if (size == 0) {
        return ESP_OK;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));

    // first, send device address (indicating write) & register to be read
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (dev_addr_ << 1), true));

    // send register we want
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg_addr, true));

    // Start reading, separate it with |Start|
    ESP_ERROR_CHECK(i2c_master_start(cmd));

    // now send device address (indicating read) & read data
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (dev_addr_ << 1) | I2C_MASTER_READ, true));

    if (size > 1) {
        ESP_ERROR_CHECK(i2c_master_read(cmd, out_buff, size - 1, I2C_MASTER_ACK));
    }

    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, out_buff + size - 1, I2C_MASTER_NACK));

    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    esp_err_t ret = i2c_master_cmd_begin(port_num_, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
    return ret;
}

}  // namespace bk