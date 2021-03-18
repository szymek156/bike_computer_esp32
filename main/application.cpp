#include "application.h"

#include "i2c_scan.h"
#include "weather.h"

#include <iostream>

#include <esp_log.h>

void StartApplication() {
    std::cout << "weather" << std::endl;

    bk::Weather w;
    w.run();
    std::cout << "finished weather" << std::endl;
}