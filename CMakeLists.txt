cmake_minimum_required(VERSION 3.20)
project(Minima)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

include_directories("./src")
set(CMAKE_CXX_STANDARD 17)

add_executable(Minima main.cpp src/Print.cpp src/Editor.h src/Document.h src/Commands.h)
target_link_libraries(Minima ${CURSES_LIBRARY})
