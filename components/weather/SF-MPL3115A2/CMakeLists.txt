idf_component_register(SRCS "weather.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES "i2c_wrap")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)