# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# Here is the place to select hardware/platform specific components
# Be careful with selecting names, i2c, esp32 are so generic, cmake goes crazy,
# hence the need for_ prefix and _wrap suffix
list(APPEND EXTRA_COMPONENT_DIRS "components/for_esp32/i2c_wrap")
list(APPEND EXTRA_COMPONENT_DIRS "components/for_esp32/uart_wrap")
list(APPEND EXTRA_COMPONENT_DIRS "components/for_esp32/gpio_wrap")
list(APPEND EXTRA_COMPONENT_DIRS "components/for_esp32/fs_wrap")
list(APPEND EXTRA_COMPONENT_DIRS "components/for_esp32/ble_wrap")
list(APPEND EXTRA_COMPONENT_DIRS "components/common")
list(APPEND EXTRA_COMPONENT_DIRS "components/weather/SF-MPL3115A2")
list(APPEND EXTRA_COMPONENT_DIRS "components/gnss/NEO-7M-C")
list(APPEND EXTRA_COMPONENT_DIRS "components/display/sharp_mem_lcd_2in7")

# For now idea with e-ink is abandoned, it would be nice to have to
# merge MIP and e-ink functionalities to one class (where possible),
# having only drivers separate. But since there is no such real use case,
# It's not worth the time, so I simply disable compilation of e-ink component.
# So long and thanks for all you did for me, my dear waveshare2in9!
# xoxoxoxo.
# list(APPEND EXTRA_COMPONENT_DIRS "components/display/waveshare2in9")

list(APPEND EXTRA_COMPONENT_DIRS "components/keypad")
list(APPEND EXTRA_COMPONENT_DIRS "components/FIT")



include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(bike_computer_esp32)
