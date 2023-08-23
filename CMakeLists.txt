cmake_minimum_required(VERSION 3.13)

project(chess LANGUAGES CXX)
set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.82.0 REQUIRED COMPONENTS system serialization) 
find_package(spdlog REQUIRED)

add_definitions(-DSPDLOG_ACTIVE_LEVEL=0)
add_definitions(-DBOOST_MPL_LIMIT_VECTOR_SIZE=30)
add_definitions(-DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS)

set(COMPILE_WARNINGS
  -Werror
  -Wpedantic
  -Wformat
  -Wuninitialized
  -Wnull-dereference
  -Wmissing-field-initializers
  -Wpointer-arith
  -Wcast-align
  -Wfloat-equal
)

add_subdirectory(messages)
add_subdirectory(server)
add_subdirectory(user)
