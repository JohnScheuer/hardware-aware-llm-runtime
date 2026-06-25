cmake_minimum_required(VERSION 3.16)
project(HardwareAwareLLMRuntime CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(runtime
    core/main.cpp
    core/hardware_profiler.cpp
    core/roofline.cpp
    core/RooflineAnalyzer.cpp
)

target_include_directories(runtime PRIVATE include)