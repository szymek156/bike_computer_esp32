#pragma once

namespace bk {
/** @brief Abstraction over filesystem on the flash */
class FSWrapper {
 public:
   /** @brief mounts SPIFFS partition under /storage endpoint
    * it is possible to write and read from it using
    * POSIX and C/C++ calls if file path starts with /storage
    * Filesystem is flat - there is no support for directories
    */
    static void mountStorage();
};
}  // namespace bk