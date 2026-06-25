cmake_minimum_required(VERSION 3.16)

project(HardwareAwareLLMRuntime LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create executable
add_executable(runtime
    core/main.cpp
    core/hardware_profiler.cpp
    core/roofline.cpp
    core/RooflineAnalyzer.cpp
)

# Include headers
target_include_directories(runtime PRIVATE include)

# Optional: enable optimizations in Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(runtime PRIVATE -O3 -march=native)
endif()

