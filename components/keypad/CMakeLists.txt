idf_component_register(SRCS "keypad.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES "gpio_wrap" "common")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)