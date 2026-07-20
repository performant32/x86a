cmake_minimum_required(VERSION 3.19)
project("x86a" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC src)
target_precompile_headers(${CMAKE_PROJECT_NAME} PUBLIC "src/pch.h")

