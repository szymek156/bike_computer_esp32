#pragma once
#include <string>
#include <vector>

namespace bk {
struct FileInfo {
    std::string filename;
    long int size;
};
/** @brief Abstraction over filesystem on the flash */
class FSWrapper {
 public:
    static constexpr const char *TAG = "FSWrapper";

    /** @brief mounts SPIFFS partition under /storage endpoint
     * it is possible to write and read from it using
     * POSIX and C/C++ calls if file path starts with /storage
     * Filesystem is flat - there is no support for directories
     */
    static void mountStorage();

    static std::vector<FileInfo> listFiles(const std::string &partition);
};
}  // namespace bk