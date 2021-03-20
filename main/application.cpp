#include "application.h"

#include "weather.h"
#include "gnss.h"

#include <iostream>

#include <esp_log.h>

void StartApplication() {
    // bk::Weather w;
    // w.run();
    bk::GNSS gnss;
    gnss.run();
}