cmake_minimum_required(VERSION 3.26)

project(AsteroidsCMD)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    CommandGL
    GIT_REPOSITORY https://github.com/ciufcia/CommandGL.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(CommandGL)

add_executable(Asteroids main.cpp)

target_link_libraries(Asteroids CommandGL)