cmake_minimum_required(VERSION 3.11...3.20)
project(Demos VERSION 1.0.0)

configure_file(
    ${PROJECT_SOURCE_DIR}/Version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Version.h
    @ONLY
)

file(GLOB_RECURSE src_file 
    "${PROJECT_SOURCE_DIR}/*.h"
    "${PROJECT_SOURCE_DIR}/*.hpp"
    "${PROJECT_SOURCE_DIR}/*.c"
    "${PROJECT_SOURCE_DIR}/*.cpp"
)

add_executable(${PROJECT_NAME} 
    ${src_file}
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ADV::Obfuscator
        Boost::coroutine
        Boost::filesystem
        Boost::system
        Boost::thread
        breakpad_client
        spdlog::spdlog
)

# setting up compile flags
set_property(TARGET ${PROJECT_NAME} APPEND_STRING
    PROPERTY COMPILE_FLAGS ${PROJECT_COMPILE_FLAG}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}/static_lib"
        ARCHIVE_OUTPUT_DIRECTORY "${BIN_DIR}/"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/"
        FOLDER  "Demos"
)

include(${PROJECT_SOURCE_DIR}/../cmake/UtilityTargets.cmake)
install(TARGETS ${PROJECT_NAME}
    DESTINATION ${INSTALL_DIR_NO_PREFIX}
)