cmake_minimum_required(VERSION 3.10.0)
project(lab2)

add_executable(MainStdThread main_std_thread.cpp)

find_package(Threads REQUIRED)
target_link_libraries(MainStdThread PRIVATE Threads::Threads)