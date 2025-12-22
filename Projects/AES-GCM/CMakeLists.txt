cmake_minimum_required(VERSION 3.16)
project(AES-GCM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# OpenSSL
find_package(OpenSSL REQUIRED)

# Argon2
find_library(ARGON2_LIBRARY NAMES argon2)
find_path(ARGON2_INCLUDE_DIR argon2.h)

if(NOT ARGON2_LIBRARY OR NOT ARGON2_INCLUDE_DIR)
    message(FATAL_ERROR "Argon2 library not found")
endif()

set(SOURCES
    Common/main.cpp
    Core/AES_GCM.cpp
    Core/AES_GCM_decryption.cpp
    Core/AES_GCM_encryption.cpp
    Core/Worker.cpp
    GUI/MainGUI.cpp
    GUI/InputGUI.cpp
    GUI/ProgressGUI.cpp
    GUI/ModeButton.cpp
    GUI/PWLineEdit.cpp
    Utils/Password.cpp
    Utils/library.cpp
)

set(HEADERS
    Common/header.h
    Core/AES_GCM.h
    Core/Worker.h
    GUI/MainGUI.h
    GUI/InputGUI.h
    GUI/ProgressGUI.h
    GUI/ModeButton.h
    GUI/PWLineEdit.h
    Utils/File.h
    Utils/Password.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${ARGON2_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    OpenSSL::Crypto
    ${ARGON2_LIBRARY}
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE bcrypt)
endif()