#pragma once

// TODO: that should be thread safe probably?
class HealthService {
 public:
    static void report();

private:
    static constexpr const char *TAG = "Health";

    static void reportFS();
    static void reportRAM();
    static void reportCPU();
    static void reportOS();
};