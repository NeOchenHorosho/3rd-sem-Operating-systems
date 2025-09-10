cmake_minimum_required(VERSION 3.10.0)
project(lab1)

add_executable(Main main.cpp employee.h)

add_executable(Creator creator.cpp employee.h)

add_executable(Reporter reporter.cpp employee.h)