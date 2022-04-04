cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fastforwardtask)

include_directories(3rdparty/rapidjson-1.1.0/include)

add_subdirectory(fastforwardtask)
