cmake_minimum_required(VERSION 3.19)

project(messages LANGUAGES CXX)
set(TARGET_NAME ${PROJECT_NAME})

add_library(${TARGET_NAME} INTERFACE 
  include/messages/messages.hpp 
  include/messages/messages_io.hpp
  include/messages/messages_eq.hpp  
)

target_compile_features(${TARGET_NAME} INTERFACE cxx_std_17)
target_include_directories(${TARGET_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(${TARGET_NAME} INTERFACE Boost::serialization)

