idf_component_register(SRCS "fit.c"
                            "fit_crc.c"
                            "fit_example.c"
                            "fit_product.c"
                    INCLUDE_DIRS "."
                    REQUIRES "common")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)