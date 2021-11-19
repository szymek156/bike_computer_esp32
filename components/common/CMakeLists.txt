idf_component_register(SRCS "abstract_task.cpp" "paint.cpp" "font8.c" "font12.c" "font16.c" "font20.c" "font24.c"
                    INCLUDE_DIRS ".")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)