cmake_minimum_required(VERSION 3.13)

project(server LANGUAGES CXX)
set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET_NAME}  
  src/main.cpp
  src/chess.cpp
  src/handle_message.cpp
  src/desk.cpp
  src/server.cpp
  src/client.cpp
  src/chess.h
  src/handle_message.h
  src/desk.h
  src/server.h
  src/coord.h
  src/client.h
  src/helper.h
  src/logger.hpp 
  src/logger.cpp 
)

set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_include_directories(${TARGET_NAME} PRIVATE src .)
target_link_libraries(${TARGET_NAME} PRIVATE 
  messages
  spdlog::spdlog
  Boost::system 
  Boost::serialization 
)
target_compile_options(${TARGET_NAME} PRIVATE ${COMPILE_WARNINGS})
