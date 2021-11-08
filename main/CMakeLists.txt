idf_component_register(SRCS
    "bike_computer_esp32.c"
    "time_service.cpp"
    "activity_service.cpp"
    "event_dispatcher.cpp"
    "root_window.cpp"

    "widgets/v_list_widget.cpp"

    "view/status_view.cpp"
    "view/welcome_view.cpp"
    "view/test_view.cpp"
    "view/stats_view.cpp"

    "view/activity_splash_view.cpp"
    "view/select_activity_view.cpp"
    "view/activity_workouts_view.cpp"
    "view/activity_selected_view.cpp"
    "view/activity_do_it_view.cpp"

    "view/stats_splash_view.cpp"
    "view/stats_view.cpp"
    "view/stats_selected_view.cpp"

    "view/running_1_view.cpp"
    "view/running_2_view.cpp"


    "view/TEMPLATE_view.cpp"

    "presenter/status_presenter.cpp"
    "presenter/welcome_presenter.cpp"
    "presenter/test_presenter.cpp"

    "presenter/activity_splash_presenter.cpp"
    "presenter/select_activity_presenter.cpp"
    "presenter/activity_workouts_presenter.cpp"
    "presenter/activity_selected_presenter.cpp"
    "presenter/activity_do_it_presenter.cpp"

    "presenter/stats_splash_presenter.cpp"
    "presenter/stats_presenter.cpp"
    "presenter/stats_selected_presenter.cpp"

    "presenter/running_1_presenter.cpp"
    "presenter/running_2_presenter.cpp"
    "presenter/workout_steps_presenter.cpp"
    "presenter/workout_steps_splash.cpp"
    "presenter/activity_paused.cpp"


    "presenter/TEMPLATE_presenter.cpp"

    "layout_factory.cpp"

    "fit_file.cpp"
    "fit_activity.cpp"

    "application.cpp"

    INCLUDE_DIRS ".")

# use gnu, not cpp  because freertos depends on some gnu extensions (fix to be shipped in esp-idf in master)
component_compile_options(-std=gnu++17)