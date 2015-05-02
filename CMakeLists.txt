cmake_minimum_required(VERSION 3.2)
project(gait)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package( OpenCV REQUIRED )

set(SOURCE_FILES gait.cpp)
add_executable(gait ${SOURCE_FILES} gait.h gait.cpp)
target_link_libraries( gait ${OpenCV_LIBS})