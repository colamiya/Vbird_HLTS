cmake_minimum_required(VERSION 3.16)

project(HotelTrainingApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Automoc to handle Q_OBJECT macros
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    logger.h
    logger.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    test1.cpp
    test1.h
    test2.cpp
    test2.h
    test3.cpp
    test3.h
    utils.cpp
    utils.h
    tutorial_overlay.h
    tutorial_overlay.cpp
    config.h
    config_global.h
    config_test1.h
    config_test2.h
    config_test3.h
    config_text.h
    resources.qrc
)

add_executable(HotelTrainingApp ${PROJECT_SOURCES})

target_link_libraries(HotelTrainingApp PRIVATE Qt6::Widgets)

# Handle Window application on Windows
if(WIN32)
    set_property(TARGET HotelTrainingApp PROPERTY WIN32_EXECUTABLE ON)
endif()
