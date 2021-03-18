#pragma once
#include <cstdint>
namespace bk {
static const int DEVICE_I2C_ADDRESS = 0x60;

struct Addr {
    static const int STATUS = 0x00;
    static const int OUT_P_MSB = 0x01;
    static const int OUT_P_CSB = 0x02;
    static const int OUT_P_LSB = 0x03;
    static const int OUT_T_MSB = 0x04;
    static const int OUT_T_LSB = 0x05;
    static const int DR_STATUS = 0x06;
    static const int OUT_P_DELTA_MSB = 0x07;
    static const int OUT_P_DELTA_CSB = 0x08;
    static const int OUT_P_DELTA_LSB = 0x09;
    static const int OUT_T_DELTA_MSB = 0x0A;
    static const int OUT_T_DELTA_LSB = 0x0B;
    static const int WHO_AM_I = 0x0C;
    static const int F_STATUS = 0x0D;
    static const int F_DATA = 0x0E;
    static const int F_SETUP = 0x0F;
    static const int TIME_DLY = 0x10;
    static const int SYSMOD = 0x11;
    static const int INT_SOURCE = 0x12;
    static const int PT_DATA_CFG = 0x13;
    static const int BAR_IN_MSB = 0x14;
    static const int BAR_IN_LSB = 0x15;
    static const int P_TGT_MSB = 0x16;
    static const int P_TGT_LSB = 0x17;
    static const int T_TGT = 0x18;
    static const int P_WND_MSB = 0x19;
    static const int P_WND_LSB = 0x1A;
    static const int T_WND = 0x1B;
    static const int P_MIN_MSB = 0x1C;
    static const int P_MIN_CSB = 0x1D;
    static const int P_MIN_LSB = 0x1E;
    static const int T_MIN_MSB = 0x1F;
    static const int T_MIN_LSB = 0x20;
    static const int P_MAX_MSB = 0x21;
    static const int P_MAX_CSB = 0x22;
    static const int P_MAX_LSB = 0x23;
    static const int T_MAX_MSB = 0x24;
    static const int T_MAX_LSB = 0x25;
    static const int CTRL_REG1 = 0x26;
    static const int CTRL_REG2 = 0x27;
    static const int CTRL_REG3 = 0x28;
    static const int CTRL_REG4 = 0x29;
    static const int CTRL_REG5 = 0x2A;
    static const int OFF_P = 0x2B;
    static const int OFF_T = 0x2C;
    static const int OFF_H = 0x2D;
};

template <class T>
const uint8_t *toBits(const T &data) {
    return ((const uint8_t *)&data);
}

struct PT_DATA_CFG {
    unsigned TDEFE : 1;
    unsigned PDEFE : 1;
    unsigned DREM : 1;
};

struct CTRL_REG1 {
    unsigned SBYB : 1;
    unsigned OST : 1;
    unsigned RST : 1;
    unsigned OS : 3;
    unsigned RAW : 1;
    unsigned ALT : 1;
};

struct STATUS_MASK {
    static const uint8_t PTDR = 1 << 3;
};

}  // namespace bk
