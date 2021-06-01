add_library(app OBJECT)

target_sources(app PRIVATE
    clock.cpp
    gpio.cpp
    flash.cpp
    main.cpp
    power.cpp
    system.cpp
)

target_include_directories(app PRIVATE
    ${PROJECT_SOURCE_DIR}/vendor/
)
