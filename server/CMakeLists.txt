cmake_minimum_required(VERSION 3.13)

project(server LANGUAGES CXX)
set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET_NAME}  
  src/main.cpp
  src/logic/board_logic.hpp
  src/logic/board_logic.cpp 
  src/logic/desk.cpp
  src/logic/desk.hpp
  src/logic/coord.hpp
  src/logic/boards_holder.hpp 
  src/logic/boards_holder.cpp 
  src/server/handle_message.cpp  
  src/server/handle_message.hpp
  src/server/server.hpp    
  src/server/server.cpp
  src/server/client.cpp
  src/server/client.hpp
  src/server/clients_holder.hpp
  src/server/clients_holder.cpp 
  src/common/helper.hpp
  src/common/logger.hpp 
  src/common/logger.cpp 
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
