#pragma once
#include <cstdint>
#include <driver/i2c.h>

namespace bk {
class I2C {
 public:
    I2C(uint8_t dev_addr, i2c_port_t port_num, int sda_pin, int scl_pin);
    ~I2C();

    /**
     * @brief Write i2c slave device with registered interface
     *        Master device write data to slave,
     *        the data will be stored in slave buffer.
     *        We can read them out from slave buffer.
     * ____________________________________________________________________________________
     * | start | slave_addr + wr_bit + ack | register + ack | write n bytes + ack  | stop |
     * --------|---------------------------|----------------|----------------------|------|
     *
     */
    esp_err_t write_slave_reg(uint8_t reg_addr, const uint8_t *data, size_t size = 1);

    // clang-format off
    /**
     * @brief Read i2c slave device with registered interface
     *
     * _______________________________________________________________________________________________________
     * | start | slave_addr + rd_bit +ack | register + ack | read n-1 bytes + ack | read 1 byte + nack | stop |
     * --------|--------------------------|----------------|----------------------|--------------------|------|
     *
     */
    // clang-format on
    esp_err_t read_slave_reg(uint8_t reg_addr, uint8_t *out_buff, size_t size = 1);

    protected:
    uint8_t dev_addr_;
    i2c_port_t port_num_;
};

}  // namespace bk