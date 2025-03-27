cmake_minimum_required(VERSION 3.10)
project(ThreadProcessAppCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(thread_process_app_cpp main.cpp)

# For pthread (needed on some systems)
find_package(Threads REQUIRED)
target_link_libraries(thread_process_app_cpp PRIVATE Threads::Threads)