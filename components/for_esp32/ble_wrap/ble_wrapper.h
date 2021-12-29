#pragma once
#include "file_transfer_gatts.h"
namespace bk {
/** @brief Abstraction over bluetooth low energy */
class BLEWrapper {
 public:
    void enable();
    void disable();

    void test_indicate();

};
}  // namespace bk