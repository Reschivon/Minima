cmake_minimum_required(VERSION 3.18)
project(Minima)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

#add_subdirectory(${PROJECT_SOURCE_DIR}/clang-highlight)

include_directories("./src")
set(CMAKE_CXX_STANDARD 17)

add_executable(Minima main.cpp src/Print.cpp src/Editor.h src/Document.h src/Commands.h src/History.h src/Structure.h)
target_link_libraries(Minima ${CURSES_LIBRARY})
