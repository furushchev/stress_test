cmake_minimum_required(VERSION 2.8.3)

add_definitions("-std=c++11")
find_package(Threads)
add_executable(stress_test stress_test.cpp)
target_link_libraries(stress_test ${CMAKE_THREAD_LIBS_INIT})
