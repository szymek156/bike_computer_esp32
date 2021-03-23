# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# Here is the place to select hardware/platform specific components
# Be careful with selecting names, i2c, esp32 are so generic, cmake goes crazy,
# hence for_ prefix and _wrap suffix
list(APPEND EXTRA_COMPONENT_DIRS "components/for_esp32/i2c_wrap")
list(APPEND EXTRA_COMPONENT_DIRS "components/for_esp32/uart_wrap")
list(APPEND EXTRA_COMPONENT_DIRS "components/common")
list(APPEND EXTRA_COMPONENT_DIRS "components/weather/SF-MPL3115A2")
list(APPEND EXTRA_COMPONENT_DIRS "components/gnss/NEO-7M-C")
list(APPEND EXTRA_COMPONENT_DIRS "components/display/waveshare2in9")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(bike_computer_esp32)
