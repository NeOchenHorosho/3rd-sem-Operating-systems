cmake_minimum_required(VERSION 3.10.0)
project(lab2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Server server.cpp)
add_executable(Client client.cpp)

find_package(Threads REQUIRED)
#target_link_libraries(MainPosix PRIVATE Threads::Threads)
