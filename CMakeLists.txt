cmake_minimum_required(VERSION 3.7)
project(xrun-logger)

set(CMAKE_C_STANDARD 11)
set(SOURCE_FILES main.c)

add_executable(xrun-logger ${SOURCE_FILES})

target_link_libraries(xrun-logger jack pthread)