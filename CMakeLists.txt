# cmake root file
cmake_minimum_required(VERSION 3.11...3.20)

# enter your project name here
project(project_wrapper LANGUAGES C CXX)
# option(BUILD_EXAMPLE OFF)
# option(BUILD_TEST "Whether compile unit test" OFF)
# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# export symbols if specified
set(CMAKE_CXX_VISIBILITY_PRESET     hidden)
set(CMAKE_C_VISIBILITY_PRESET       hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# msvc compile option
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # set debug compile option
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF: $< AND: $<C_COMPILER_ID:MSVC>, $<CXX_COMPILER_ID:MSVC> >, $< $<CONFIG:Debug,RelWithDebInfo>:EditAndContinue >, $< $<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
    add_compile_definitions(
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    )
    foreach(config "" "_DEBUG" "_RELEASE" "_MINSIZEREL" "_RELWITHDEBINFO")
        string(REGEX REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS${config} "${CMAKE_CXX_FLAGS${config}}")
        string(REGEX REPLACE "/MD" "/MT" CMAKE_C_FLAGS${config} "${CMAKE_C_FLAGS${config}}")
    endforeach()
endif()

if (WIN32 AND NOT BUILD_FOR_XP)
    # set windows api level to win7
    add_compile_definitions(
        _WIN32_WINNT=0x0601
    )
endif()

# public include dir
set(THIRD_PARTY_INCLUDE "${PROJECT_SOURCE_DIR}/third-party")
# binary output dir
set(BIN_DIR "${CMAKE_BINARY_DIR}/bin")
# example binary output dir
# set(EXAMPLE_DIR "${CMAKE_BINARY_DIR}/example")
# test binary output dir
# set(TEST_DIR "${CMAKE_BINARY_DIR}/test")

find_package(OpenGL REQUIRED)
add_subdirectory(vendor)
# cpp source dir
add_subdirectory(src)
# unit test option
# option(DO_TEST "Whether compile unit test" on)
# if(BUILD_TEST)
#     message(STATUS "${PROJECT_NAME}: building unitest for the project")
#     enable_testing()
#     add_subdirectory(test)
#     add_test(NAME unit_test COMMAND test_catch2)
# endif()
# copy compile_commands.json to SOURCE_DIR to enable clangd magic
if (EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json" DESTINATION "${CMAKE_SOURCE_DIR}")
endif()