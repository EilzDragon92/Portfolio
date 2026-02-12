cmake_minimum_required(VERSION 3.16)
project(PasswordManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# OpenSSL
find_package(OpenSSL 3.0 REQUIRED)

# GoogleTest
find_package(GTest CONFIG REQUIRED)

# Argon2
find_library(ARGON2_LIBRARY NAMES argon2)
find_path(ARGON2_INCLUDE_DIR argon2.h)

if(NOT ARGON2_LIBRARY OR NOT ARGON2_INCLUDE_DIR)
    message(FATAL_ERROR "Argon2 library not found")
endif()

set(LIB_SOURCES
    src/Core/AES_GCM.cpp
    src/Core/AES_GCM_dec.cpp
    src/Core/AES_GCM_enc.cpp
    src/Core/Entry.cpp
    src/Core/Vault.cpp
    src/Utils/library.cpp
    src/Utils/Password.cpp
)

set(HEADERS
    src/Common/header.h
    src/Core/AES_GCM.h
    src/Core/Entry.h
    src/Core/Vault.h
    src/Utils/Password.h
)

add_library(PasswordManager-lib STATIC ${LIB_SOURCES} ${HEADERS})

target_include_directories(PasswordManager-lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${ARGON2_INCLUDE_DIR}
)

target_link_libraries(PasswordManager-lib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    OpenSSL::Crypto
    ${ARGON2_LIBRARY}
)

if(WIN32)
    target_link_libraries(PasswordManager-lib PUBLIC bcrypt)
endif()

add_executable(${PROJECT_NAME} src/Common/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE PasswordManager-lib)