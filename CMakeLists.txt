cmake_minimum_required(VERSION 3.6)
project(guiraspberry)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT5_LIBRARIES Qt5::Widgets Qt5::Sql)
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} -lpthread -lwiringPi -lwiringPiDev ")
set(SOURCE_FILES src/main.cpp src/gui/gui.cpp src/flex/func.cpp src/db/db.cpp src/db/derive.cpp src/gui/draw.cpp src/flex/flex.cpp src/flex/connect.cpp src/flex/my.cpp src/struct.h src/gui/gpio.cpp)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Threads REQUIRED)

include_directories(${Qt5Sql_INCLUDES})
include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Sql_DEFINITIONS})
add_executable(guiraspberry ${SOURCE_FILES})
target_link_libraries(guiraspberry Qt5::Widgets)
target_link_libraries(guiraspberry Qt5::Sql)
target_link_libraries(guiraspberry pthread)
target_link_libraries(guiraspberry wiringPi)


