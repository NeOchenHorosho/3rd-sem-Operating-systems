cmake_minimum_required(VERSION 3.10.0)
project(lab2)

add_executable(Main main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(Main PRIVATE Threads::Threads)