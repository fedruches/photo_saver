cmake_minimum_required(VERSION 3.17)
project(photo_saver)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} curl pthread ${Boost_LIBRARIES})