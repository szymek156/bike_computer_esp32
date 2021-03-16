#include "application.h"
#include "weather.h"
// extern "C" {
#include "i2c_scan.h"
void i2c_scan();
// }

#include <iostream>

void StartApplication() {
    std::cout << "weather" << std::endl;

    Weather w;
    w.run();
    std::cout << "finished weather" << std::endl;
}