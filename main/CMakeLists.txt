idf_component_register(SRCS "bike_computer_esp32.c" "hello.cpp"
                    INCLUDE_DIRS ".")

component_compile_options(-std=c++17)