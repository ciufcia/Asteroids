cmake_minimum_required(VERSION 3.26)

project(AsteroidsCMD)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CommandGL REQUIRED)

add_executable(Asteroids main.cpp)

target_link_libraries(Asteroids CommandGL::CommandGL)