cmake_minimum_required(VERSION 3.16)

project(FPDesk VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(sender)
add_subdirectory(receiver)
