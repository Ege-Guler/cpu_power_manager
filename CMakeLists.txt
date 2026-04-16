cmake_minimum_required(VERSION 3.16)

project(cpu_power_manager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
else()
    message(WARNING "clang-tidy not found")
endif()

# Find cppcheck
find_program(CPPCHECK_EXE NAMES "cppcheck")
if(CPPCHECK_EXE)
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--inconclusive;--force;--inline-suppr;--suppress=missingIncludeSystem;--suppress=unusedFunction")
    message(STATUS "Found cppcheck: ${CPPCHECK_EXE}")
else()
    message(WARNING "cppcheck not found")
endif()

# Find clang-format
find_program(CLANG_FORMAT_EXE NAMES "clang-format")
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE FORMAT_SOURCES
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_SOURCE_DIR}/include/*.hpp"
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${FORMAT_SOURCES}
        COMMENT "Formatting source files..."
    )
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(cpu-power ${SOURCES})

target_include_directories(cpu-power PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(cpu-power PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cpu-power PRIVATE -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(cpu-power PRIVATE -O2)
endif()