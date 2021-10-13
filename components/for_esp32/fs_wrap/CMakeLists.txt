set(EXTRA_COMPONENT_DIRS, $ENV{IDF_PATH}/components/spiffs)

idf_component_register(SRCS "fs_wrapper.cpp"

                    INCLUDE_DIRS
                    "."
                    # For some reason spiffs component is not included
                    # in search path. I don't plan waste my life figuring
                    # why is that (it is visible, and many more inside
                    # main component, where Application.cpp resides)
                    # So I point here search path directly.
                    $ENV{IDF_PATH}/components/spiffs/include)



# include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)