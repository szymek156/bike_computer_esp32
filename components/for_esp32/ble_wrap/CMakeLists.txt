set(EXTRA_COMPONENT_DIRS, $ENV{IDF_PATH}/components/bt)

idf_component_register(SRCS "gatts.cpp" "ble_wrapper.cpp"

                    INCLUDE_DIRS
                    "."
                    $ENV{IDF_PATH}/components/bt/include/esp32/include
                    $ENV{IDF_PATH}/components/bt/host/bluedroid/api/include/api
                    $ENV{IDF_PATH}/components/nvs_flash/include
                    )



# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)