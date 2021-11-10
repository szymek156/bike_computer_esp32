#include "application.h"

#include "event_dispatcher.h"
#include "fs_wrapper.h"
#include "gnss.h"
#include "health_service.h"
#include "keypad.h"
#include "layout_factory.h"
#include "root_window.h"
#include "time_service.hpp"
#include "weather.h"

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

    bk::EventDispatcher dispatcher(
        weather.getQueue(), gnss.getQueue(), keypad.getQueue(), time_service.getQueue());

    time_service.setEventDispatcher(&dispatcher);
    // TODO: This is a dangling pointer - in theory
    // In practice application never ends
    bk::ActivityService::instance().setEventDispatcher(&dispatcher);

    weather.start();
    gnss.start();
    keypad.start();
    time_service.start();

    bk::LayoutFactory factory(&display, &dispatcher);

    bk::RootWindow root(&dispatcher, &factory);

    HealthService::reportAll();

    dispatcher.listenForEvents();
}