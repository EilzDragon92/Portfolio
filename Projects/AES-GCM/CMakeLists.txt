cmake_minimum_required(VERSION 3.16)
project(AES-GCM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Converage
option(COVERAGE "Enable coverage reporting" OFF)

if(COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

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
    Core/AES_GCM.cpp
    Core/AES_GCM_dec.cpp
    Core/AES_GCM_enc.cpp
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
    Utils/Password.h
)

add_library(AES-GCM-lib STATIC ${LIB_SOURCES} ${HEADERS})

target_include_directories(AES-GCM-lib PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${ARGON2_INCLUDE_DIR}
)

target_link_libraries(AES-GCM-lib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    OpenSSL::Crypto
    ${ARGON2_LIBRARY}
)

if(WIN32)
    target_link_libraries(AES-GCM-lib PUBLIC bcrypt)
endif()

add_executable(${PROJECT_NAME} Common/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE AES-GCM-lib)

enable_testing()

set(TEST_SOURCES
    Tests/AES_GCM_Test.cpp
    Tests/PasswordTest.cpp
    Tests/UtilsTest.cpp
)

add_executable(AES-GCM-tests ${TEST_SOURCES})

target_link_libraries(AES-GCM-tests PRIVATE
    AES-GCM-lib
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(AES-GCM-tests)