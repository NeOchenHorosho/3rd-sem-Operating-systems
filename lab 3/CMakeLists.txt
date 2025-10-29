cmake_minimum_required(VERSION 3.10.0)
project(lab2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(MainPthread main_pthread.cpp)
add_executable(MainStdThread main_std_thread.cpp)

find_package(Threads REQUIRED)
target_link_libraries(MainPthread PRIVATE Threads::Threads)
