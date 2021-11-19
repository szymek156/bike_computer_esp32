idf_component_register(SRCS "driver.cpp"
                    REQUIRES "common"
                    INCLUDE_DIRS ".")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)