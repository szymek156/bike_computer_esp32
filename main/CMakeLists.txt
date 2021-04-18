idf_component_register(SRCS "bike_computer_esp32.c" "time_service.cpp" "status_view.cpp" "status_presenter.cpp"  "application.cpp"

                    INCLUDE_DIRS ".")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)