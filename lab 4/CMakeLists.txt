cmake_minimum_required(VERSION 3.10.0)
project(lab2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Receiver receiver.cpp)
add_executable(Sender sender.cpp)

find_package(Threads REQUIRED)
#target_link_libraries(MainPosix PRIVATE Threads::Threads)
