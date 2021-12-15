#pragma once

namespace bk {
/** @brief Abstraction over bluetooth low energy */
class BLEWrapper {
 public:
    void enable();
    void disable();

    void test_indicate();
};
}  // namespace bk