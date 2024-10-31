cmake_minimum_required(VERSION 3.15)

project(rsdoom)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable(${PROJECT_NAME} "${sources}")
target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
