cmake_minimum_required (VERSION 3.6)
project(avex_cli VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB source
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(avex_cli ${source})

target_link_libraries(
    avex_cli
)