#include "weather.h"

#include "registers.h"

Weather::Weather() {
}

Weather::~Weather() {
}

bool Weather::init() {
    // Establish i2c communication
    // - set mode of operation as master
    // - assign GPIO pins for SDA and SCL
    // - set internal pullups resistors
    // - set clk speed

    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_PIN;
    conf.scl_io_num = SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &conf);

    // Install the driver
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    // using namespace Common;

    // _fd = wiringPiI2CSetup(DEVICE_I2C_ADDRESS);

    // _log->info("fdopened: {}", _fd);

    CTRL_REG1 ctrl1;
    ctrl1.OS = 0b111;
    ctrl1.ALT = 1;

    // Set to altimeter, oversampling to 128
    // watch(wiringPiI2CWriteReg8(_fd, Addr::CTRL_REG1, toBits(ctrl1)));

    PT_DATA_CFG ptCfg;
    ptCfg.DREM = 1;
    ptCfg.PDEFE = 1;
    ptCfg.TDEFE = 1;

    // Events from temp and pressure
    // watch(wiringPiI2CWriteReg8(_fd, Addr::PT_DATA_CFG, toBits(ptCfg)));

    ctrl1.SBYB = 1;
    // Activate
    // watch(wiringPiI2CWriteReg8(_fd, Addr::CTRL_REG1, toBits(ctrl1)));

    return true;
}

void Weather::run() {
    // if (not init()) {
    //     _log->error("Failed to init, good bye");

    //     return;
    // }

    // messages::Weather data;
    // data.altitudeMeters = NAN;
    // data.tempCelsius = NAN;

    // while (true) {
    //     int dataReady = wiringPiI2CReadReg8(_fd, Addr::STATUS);

    //     if (dataReady & 0x8) {
    //         int outpmsb = wiringPiI2CReadReg8(_fd, Addr::OUT_P_MSB);
    //         int outpcsb = wiringPiI2CReadReg8(_fd, Addr::OUT_P_CSB);
    //         int outplsb = wiringPiI2CReadReg8(_fd, Addr::OUT_P_LSB);

    //         float tempcsb = (outplsb >> 4) / 16.0;

    //         int outtmsb = wiringPiI2CReadReg8(_fd, Addr::OUT_T_MSB);
    //         int outtlsb = wiringPiI2CReadReg8(_fd, Addr::OUT_T_LSB);

    //         float outA = (float)((outpmsb << 8) | outpcsb) + tempcsb;
    //         float outT = outtmsb + ((outtlsb >> 4) / 16.0);

    //         data.altitudeMeters = outA;
    //         data.tempCelsius = outT;

    //         _log->debug("weather: {}", data);

    //         messages::Message<messages::Weather> msg(data);

    //         _publisher.send(msg);

    //     } else {
    //         _log->warn("data not ready");
    //     }

    //     std::this_thread::sleep_for(std::chrono::milliseconds(800));
    // }
}
