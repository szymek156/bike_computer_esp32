idf_component_register(SRCS
    "bike_computer_esp32.c"
    "time_service.cpp"
    "event_dispatcher.cpp"
    "root_window.cpp"

    "widgets/v_list_widget.cpp"

    "view/status_view.cpp"
    "view/welcome_view.cpp"
    "view/stats_view.cpp"
    "view/select_activity_view.cpp"

    "presenter/status_presenter.cpp"
    "presenter/welcome_presenter.cpp"
    "presenter/stats_presenter.cpp"
    "presenter/select_activity_presenter.cpp"

    "layout_factory.cpp"

    "application.cpp"

    INCLUDE_DIRS ".")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)