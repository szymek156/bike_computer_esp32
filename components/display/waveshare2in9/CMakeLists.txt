idf_component_register(SRCS "display.cpp" "epd2in9.cpp" "epdif.cpp" "epdpaint.cpp"
                    "font8.c" "font16.c" "font20.c" "font24.c"
                    INCLUDE_DIRS ".")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)