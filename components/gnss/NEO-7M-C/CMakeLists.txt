idf_component_register(SRCS "gnss.cpp" "nmea_parser.c"
                    INCLUDE_DIRS "."
                    REQUIRES "uart_wrap" "common")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)