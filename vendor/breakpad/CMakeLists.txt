cmake_minimum_required(VERSION 3.8...3.20)
project(breakpad 
    VERSION 2022.07.12 
    LANGUAGES CXX)

if (MSVC)
    add_definitions(
        -DNOMINMAX
        -DUNICODE
        -DWIN32_LEAN_AND_MEAN
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
        -D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    )
endif()

set(CMAKE_DEBUG_POSTFIX d)

option(BUILD_CLIENT_ONLY "only build libbreakpad_client" ON)
string(COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "Release" DEFAULT_INSTALL_HEADERS)
option(INSTALL_HEADERS "Install header files" ${DEFAULT_INSTALL_HEADERS})
option(INSTALL_TOOLS "Install tools" OFF)

# libbreakpad target
if(NOT BUILD_CLIENT_ONLY OR INSTALL_TOOLS)
    if(NOT ANDROID AND NOT IOS)
        file(GLOB_RECURSE LIBBREAKPAD_SOURCES 
            src/processor/*.h
            src/processor/*.cc)
        if(WIN32)
            list(FILTER LIBBREAKPAD_SOURCES EXCLUDE REGEX
                "_unittest|_selftest|synth_minidump|/tests|/testdata|/linux|/mac|/android|/solaris|microdump_stackwalk|minidump_dump|minidump_stackwalk|disassembler_objdump.cc")
        elseif(APPLE)
            list(FILTER LIBBREAKPAD_SOURCES EXCLUDE REGEX
                "_unittest|_selftest|synth_minidump|/tests|/testdata|/linux|/windows|/android|/solaris|microdump_stackwalk|minidump_dump|minidump_stackwalk|disassembler_objdump.cc")
        else()
            list(FILTER LIBBREAKPAD_SOURCES EXCLUDE REGEX
                "_unittest|_selftest|synth_minidump|/tests|/testdata|/mac|/windows|/android|/solaris|microdump_stackwalk|minidump_dump|minidump_stackwalk")
        endif()

        add_subdirectory("src/third_party/libdisasm")

        add_library(libbreakpad ${LIBBREAKPAD_SOURCES})
        target_link_libraries(libbreakpad PRIVATE libdisasm)

        target_include_directories(libbreakpad
            PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
                $<INSTALL_INTERFACE:include>
        )

        set(TARGETS libbreakpad libdisasm)
        target_compile_features(libbreakpad PRIVATE cxx_std_11)
    endif()
endif()

# breakpad_client target
if(CMAKE_SYSTEM_NAME STREQUAL Android OR OHOS)
    file(READ "android/google_breakpad/Android.mk" android_mk)
    string(REGEX MATCHALL "src/[^\n]*\\.cc" LIBBREAKPAD_CLIENT_SOURCES "${android_mk}")
else()
    if(WIN32)
        file(GLOB_RECURSE LIBBREAKPAD_CLIENT_SOURCES 
            src/client/windows/*.h 
            src/client/windows/*.cc 
            src/common/windows/*.h 
            src/common/windows/*.cc)
        if (MSVC)
            include_directories("${VS_INSTALLATION_PATH}/DIA SDK/include")
        endif()
    elseif(APPLE AND NOT IOS)
        add_definitions(-DHAVE_MACH_O_NLIST_H)
        file(GLOB_RECURSE LIBBREAKPAD_CLIENT_SOURCES 
            src/client/mac/*.h
            src/client/mac/*.cc
            src/common/mac/*.h
            src/common/mac/*.cc)
        list(APPEND LIBBREAKPAD_CLIENT_SOURCES src/common/mac/MachIPC.mm)
    elseif(LINUX)
        add_definitions(-DHAVE_A_OUT_H)
        file(GLOB_RECURSE LIBBREAKPAD_CLIENT_SOURCES
            src/client/linux/*.h
            src/client/linux/*.cc
            src/common/linux/*.h
            src/common/linux/*.cc)
    elseif(IOS)
        file(GLOB_RECURSE LIBBREAKPAD_CLIENT_SOURCES 
            src/client/apple/Framework/BreakpadDefines.h
            # src/client/ios/handler/*.mm
            # src/client/ios/handler/*.cc
            # src/client/ios/handler/*.h
            src/client/ios/exception_handler_no_mach.h
            src/client/ios/*.cc
            src/client/mac/handler/*.h
            src/client/mac/handler/*.cc
            # src/client/ios/*.mm
            src/common/mac/encoding_util.h
            src/common/mac/encoding_util.m
            # src/common/mac/GTMLogger.h
            # src/common/mac/GTMLogger.m
            # src/common/mac/HttpRequest.h
            # src/common/mac/HttpRequest.m
            # src/common/mac/HttpMultipartUpload.h
            # src/common/mac/HttpMultipartUpload.m
            src/common/mac/file_id.h
            src/common/mac/file_id.cc
            src/common/mac/macho_id.h
            src/common/mac/macho_id.cc
            src/common/mac/macho_utilities.h
            src/common/mac/macho_utilities.cc
            src/common/mac/macho_walker.h
            src/common/mac/macho_walker.cc
            src/common/mac/string_utilities.h
            src/common/mac/string_utilities.cc
            src/common/long_string_dictionary.h
            src/common/long_string_dictionary.cc
            src/common/simple_string_dictionary.h
            src/common/simple_string_dictionary.cc
            src/common/simple_string_dictionary.h
            src/common/convert_UTF.h
            src/common/convert_UTF.cc
            src/common/md5.h
            src/common/md5.cc
            src/common/string_conversion.h
            src/common/string_conversion.cc)
    endif()
    if (NOT IOS)
        file(GLOB LIBBREAKPAD_COMMON_SOURCES  
            src/common/*.h
            src/common/*.cc 
            src/common/*.c)
    endif()
    file(GLOB CLIENT_SOURCES 
        src/client/*.h 
        src/client/*.cc)
    list(APPEND LIBBREAKPAD_CLIENT_SOURCES ${LIBBREAKPAD_COMMON_SOURCES} ${CLIENT_SOURCES})
endif()
list(FILTER LIBBREAKPAD_CLIENT_SOURCES EXCLUDE REGEX "/sender|/tests|/unittests|/testcases|_unittest|_test")
if(WIN32)
    list(FILTER LIBBREAKPAD_CLIENT_SOURCES EXCLUDE REGEX "language.cc|path_helper.cc|stabs_to_module.cc|stabs_reader.cc|minidump_file_writer.cc")
    if(NOT MSVC)
        # ATL headers (atlbase.h, atlcomcli.h) are MSVC-only; exclude PDB/DIA files for MinGW
        list(FILTER LIBBREAKPAD_CLIENT_SOURCES EXCLUDE REGEX "dia_util|omap\\.cc|pe_util|pdb_source_line_writer")
    endif()
elseif(NOT APPLE)
    try_compile(HAVE_GETCONTEXT ${CMAKE_BINARY_DIR}/check_getcontext ${CMAKE_CURRENT_LIST_DIR}/check_getcontext.cc OUTPUT_VARIABLE BUILD_OUT)
    if (NOT HAVE_GETCONTEXT)
        enable_language(ASM)
        list(APPEND LIBBREAKPAD_CLIENT_SOURCES src/common/linux/breakpad_getcontext.S)
    endif()
endif()

add_library(breakpad_client ${LIBBREAKPAD_CLIENT_SOURCES})
if(WIN32)
    target_link_libraries(breakpad_client PRIVATE wininet.lib)
elseif(APPLE)
    find_library(CoreFoundation_FRAMEWORK CoreFoundation)
    target_link_libraries(breakpad_client PRIVATE ${CoreFoundation_FRAMEWORK})
else()
    find_library(PTHREAD_LIBRARIES pthread)
    if(PTHREAD_LIBRARIES)
        target_link_libraries(breakpad_client PRIVATE ${PTHREAD_LIBRARIES})
    endif()
    if (HAVE_GETCONTEXT)
        target_compile_definitions(breakpad_client PRIVATE HAVE_GETCONTEXT=1)
    endif()
endif()
if (ANDROID)
    target_include_directories(breakpad_client
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/common/android/include>
    )
endif()
target_compile_features(breakpad_client PRIVATE cxx_std_11)


set(USED_ZLIB OFF)
if(LINUX AND NOT CMAKE_SYSTEM_NAME STREQUAL Android)
    # src/common/linux/dump_symbols.cc wants zlib.h
    find_package(ZLIB REQUIRED)
    target_link_libraries(breakpad_client PRIVATE ZLIB::ZLIB)
    set(USED_ZLIB ON)
endif()

target_include_directories(breakpad_client
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)
list(APPEND TARGETS breakpad_client)

if(INSTALL_TOOLS)
   if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        add_executable(microdump_stackwalk
                src/processor/microdump_stackwalk.cc)
        target_link_libraries(microdump_stackwalk PRIVATE libbreakpad breakpad_client)
        install(TARGETS microdump_stackwalk DESTINATION bin)

        add_executable(minidump_dump
                src/processor/minidump_dump.cc)
        target_link_libraries(minidump_dump PRIVATE libbreakpad breakpad_client)
        install(TARGETS minidump_dump DESTINATION bin)

        add_executable(minidump_stackwalk
                src/processor/minidump_stackwalk.cc)
        target_link_libraries(minidump_stackwalk PRIVATE libbreakpad breakpad_client)
        install(TARGETS minidump_stackwalk DESTINATION bin)

        add_executable(core2md
            src/tools/linux/core2md/core2md.cc)
        target_link_libraries(core2md PRIVATE breakpad_client)
        install(TARGETS core2md DESTINATION bin)

        add_executable(pid2md
            src/tools/linux/pid2md/pid2md.cc)
        target_link_libraries(pid2md PRIVATE breakpad_client)
        install(TARGETS pid2md DESTINATION bin)

        add_executable(dump_syms
            src/common/dwarf_cfi_to_module.cc
            src/common/dwarf_cu_to_module.cc
            src/common/dwarf_line_to_module.cc
            src/common/dwarf_range_list_handler.cc
            src/common/language.cc
            src/common/module.cc
            src/common/path_helper.cc
            src/common/stabs_reader.cc
            src/common/stabs_to_module.cc
            src/common/dwarf/bytereader.cc
            src/common/dwarf/dwarf2diehandler.cc
            src/common/dwarf/dwarf2reader.cc
            src/common/dwarf/elf_reader.cc
            src/tools/linux/dump_syms/dump_syms.cc)
        target_link_libraries(dump_syms PRIVATE breakpad_client)
        install(TARGETS dump_syms DESTINATION bin)

        add_executable(minidump-2-core
            src/common/linux/memory_mapped_file.cc
            src/tools/linux/md2core/minidump-2-core.cc)
        target_link_libraries(minidump-2-core PRIVATE breakpad_client)
        install(TARGETS minidump-2-core DESTINATION bin)

        add_executable(minidump_upload
            src/common/linux/http_upload.cc
            src/tools/linux/symupload/minidump_upload.cc)
        target_link_libraries(minidump_upload PRIVATE breakpad_client ${CMAKE_DL_LIBS})
        install(TARGETS minidump_upload DESTINATION bin)

        add_executable(sym_upload
            src/common/linux/http_upload.cc
            src/common/linux/libcurl_wrapper.cc
            src/common/linux/symbol_collector_client.cc
            src/common/linux/symbol_upload.cc
            src/tools/linux/symupload/sym_upload.cc)
        target_link_libraries(sym_upload PRIVATE breakpad_client ${CMAKE_DL_LIBS})
        install(TARGETS sym_upload DESTINATION bin)

        add_executable(core_handler
            src/tools/linux/core_handler/core_handler.cc)
        target_link_libraries(core_handler PRIVATE breakpad_client)
        install(TARGETS core_handler DESTINATION bin)
    endif()
endif()

if(INSTALL_HEADERS)
    if(WIN32)
        set(HEADER_EXCLUDE_REGEX "/apple|/ios|/linux|/mac|/solaris|/android|/dwarf|/tests|/testdata|/unittests")
    elseif(APPLE)
        set(HEADER_EXCLUDE_REGEX "/apple|/ios|/linux|/windows|/solaris|/android|/dwarf|/tests|/testdata|/unittests|/sender|/testapp|\.xcodeproj|/gcov")
    else()
        set(HEADER_EXCLUDE_REGEX "/apple|/ios|/windows|/mac|/solaris|/android|/dwarf|/tests|/testdata|/unittests")
        install(
            DIRECTORY src/third_party/lss
            DESTINATION include/third_party
            FILES_MATCHING PATTERN "*.h"
            REGEX "${HEADER_EXCLUDE_REGEX}" EXCLUDE
        )
    endif()
    install(
        DIRECTORY src/client src/common src/google_breakpad
        DESTINATION include/
        FILES_MATCHING
        PATTERN "*.h"
        REGEX ${HEADER_EXCLUDE_REGEX} EXCLUDE
    )
endif()
