cmake_minimum_required(VERSION 3.10)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
include_directories(include)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Build for Linux")
    set(OS_SOURCES src/os_unix.cpp)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Build for Windows")
    set(OS_SOURCES src/os_win.cpp)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /Zi")
endif()

add_library(common STATIC ${OS_SOURCES})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(parent src/parent.cpp)
add_executable(child src/child.cpp)
target_link_libraries(parent PRIVATE common)
target_link_libraries(child PRIVATE common)