#pragma once
#include <string>

namespace bk {
// TODO: that should be thread safe probably?
// TODO: for now simply assuming all calls comes from one thread
// Main - event_dispatcher
class HealthService {
 public:
    static void reportAll();

    static void reportFS();
    static void reportRAM();
    static void reportCPU();
    static void reportOS();

 private:
    static constexpr const char *TAG = "Health";
    static void reportFiles(const std::string &partition);
};

}  // namespace bk
