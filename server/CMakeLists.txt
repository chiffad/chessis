project(chess_server LANGUAGES CXX)
cmake_minimum_required(VERSION 3.13)
set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.82.0 REQUIRED COMPONENTS system serialization) 

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
  src/messages.h
  src/server.h
  src/coord.h
  src/client.h
  src/helper.h
)

set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_include_directories(${TARGET_NAME} PRIVATE src .)
target_link_libraries(${TARGET_NAME} PRIVATE 
  Boost::system 
  Boost::serialization 
)
target_compile_options(${TARGET_NAME} PRIVATE ${COMPILE_WARNINGS})
