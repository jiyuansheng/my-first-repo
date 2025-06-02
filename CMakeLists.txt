cmake_minimum_required(VERSION 3.14)
project(cpp_algorithm)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "/usr/local/opt/sfml@2/lib/cmake/SFML")
find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

add_executable(cpp_algorithm main.cpp)

target_link_libraries(cpp_algorithm sfml-graphics sfml-window sfml-system)