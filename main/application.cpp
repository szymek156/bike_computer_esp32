#include "application.h"

#include "event_dispatcher.h"
#include "gnss.h"
#include "keypad.h"
#include "layout_factory.h"
#include "root_window.h"
#include "time_service.hpp"
#include "weather.h"
#include "fs_wrapper.h"
#include <display.h>
#include <string.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

void StartApplication() {
    static const char* TAG = "Application";

    bk::FSWrapper::mountStorage();

    bk::Display display;

    display.start();

    bk::Weather weather;
    bk::GNSS gnss;
    bk::Keypad keypad;
    bk::TimeService time_service;

    bk::EventDispatcher dispatcher(&weather, &gnss, &keypad, &time_service);
    time_service.setEventDispatcher(&dispatcher);

    weather.start();
    gnss.start();
    keypad.start();
    time_service.start();

    bk::LayoutFactory factory(&display, &dispatcher);

    bk::RootWindow root(&dispatcher, &factory);

    dispatcher.listenForEvents();
}