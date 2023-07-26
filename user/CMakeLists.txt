cmake_minimum_required(VERSION 3.13)
project(user LANGUAGES CXX)
set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_PACKAGE Qt5) 

find_package(${QT_PACKAGE} REQUIRED COMPONENTS Core Widgets Quick Qml QuickCompiler OpenGL)

qt5_add_resources(QML_QRC_PATH res.qrc)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${TARGET_NAME}  
  src/client/client.h
  src/client/client.cpp
  src/client/handle_message.h
  src/controller/menu_layout.hpp 
  src/controller/menu_layout.cpp 
  src/controller/figures_model.hpp
  src/controller/figures_model.cpp 
  src/controller/board_graphic.cpp
  src/controller/board_graphic.hpp
  src/controller/login_input.hpp
  src/controller/login_input.cpp
  src/controller/message_processor.hpp 
  src/controller/message_processor.cpp 
  src/controller/app.hpp 
  src/controller/app.cpp
  src/coord.h
  src/figure.h
  src/helper.h
  src/helper.cpp
  src/main.cpp
  src/logger.hpp 
  src/logger.cpp 
  ${QML_QRC_PATH}  
)

set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_include_directories(${TARGET_NAME} PUBLIC src .)
target_link_libraries(${TARGET_NAME} 
  PRIVATE 
  spdlog::spdlog
  Boost::system 
  Boost::serialization
  messages
  ${QT_PACKAGE}::Core 
  ${QT_PACKAGE}::Widgets
  ${QT_PACKAGE}::Quick 
  ${QT_PACKAGE}::Qml 
  ${QT_PACKAGE}::OpenGL
)
target_compile_options(${TARGET_NAME} PRIVATE ${COMPILE_WARNINGS})
