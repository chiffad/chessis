cmake_minimum_required(VERSION 3.13)

project(server LANGUAGES CXX)
set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME}  
  src/main.cpp
  src/logic/games_manager.hpp
  src/logic/games_manager.cpp 
  src/logic/player.cpp
  src/logic/player.hpp
  src/logic/players_holder.hpp
  src/logic/players_holder.cpp 
  src/logic/board_logic.hpp
  src/logic/board_logic.cpp 
  src/logic/coord.hpp
  src/logic/boards_holder.hpp 
  src/logic/boards_holder.cpp
  src/logic/credentials.hpp
  src/logic/credentials.cpp  
  src/logic/message_handler.cpp  
  src/logic/message_handler.hpp
  src/server/server.hpp    
  src/server/server.cpp
  src/server/base_client.cpp
  src/server/base_client.hpp
  src/server/client.hpp 
  src/server/client.cpp
  src/server/clients_holder.hpp
  src/server/clients_holder.cpp 
  src/server/authentication/client.hpp
  src/server/authentication/client.cpp  
  src/server/authentication/message_handler.cpp 
  src/server/authentication/message_handler.hpp 
  src/server/authentication/server.cpp 
  src/server/authentication/server.hpp 
  src/common/helper.hpp
  src/common/logger.hpp 
  src/common/logger.cpp 
  src/common/unique_id_generator.hpp
  src/common/unique_id_generator.cpp 
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
