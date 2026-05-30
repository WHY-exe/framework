cmake_minimum_required(VERSION 3.11...3.20)
project(ADVObfuscator)

add_library(${PROJECT_NAME} INTERFACE)
add_library(ADV::Obfuscator ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)