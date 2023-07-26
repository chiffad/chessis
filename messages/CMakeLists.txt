cmake_minimum_required(VERSION 3.19)

project(messages LANGUAGES CXX)
set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${TARGET_NAME} INTERFACE include/messages/messages.hpp)
target_compile_features(${TARGET_NAME} INTERFACE cxx_std_17)
target_include_directories(${TARGET_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(${TARGET_NAME} INTERFACE Boost::serialization)

