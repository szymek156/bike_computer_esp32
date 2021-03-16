# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# Here is the place to select hardware/platform specific components
list(APPEND EXTRA_COMPONENT_DIRS "components/weather/SF-MPL3115A2")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(bike_computer_esp32)
