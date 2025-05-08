cmake_minimum_required(VERSION 3.28)
project(fft_test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")

if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()
find_package(Boost 1.70 REQUIRED)
find_package(nxsim REQUIRED)

add_executable(fft_test main.cpp)
target_link_libraries(fft_test PRIVATE nxsim)
