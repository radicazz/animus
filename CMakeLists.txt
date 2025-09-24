cmake_minimum_required(VERSION 3.21)

project(helipad VERSION 0.1.0 DESCRIPTION "A top-down 2D game engine." LANGUAGES CXX)

# Disable in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Make sure this is top-level CMakeLists.txt
if(NOT "${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  message(FATAL_ERROR "This project does not support being included as a subdirectory yet.")
endif()

# To avoid potential name clashes with subdirectories.
set(ENGINE_NAME ${PROJECT_NAME})

# Default to Debug build if none is specified.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
  set_property(
    CACHE
      CMAKE_BUILD_TYPE
    PROPERTY
      STRINGS
        "Debug"
        "Release"
        "MinSizeRel"
        "RelWithDebInfo"
  )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include("options")
include("dependencies")

# Create the main library target.
add_library(${ENGINE_NAME} STATIC)

# Set C++ standard & require that other targets using this library also use C++20.
target_compile_features(${ENGINE_NAME} PRIVATE cxx_std_20)

option(ENGINE_BUILD_EXAMPLES "Build example applications" ON)

if(ENGINE_BUILD_EXAMPLES)
  add_subdirectory("examples")
endif()

# Generate config header from template.
engine_generate_header("config")

file(GLOB_RECURSE ENGINE_SOURCE_FILES CONFIGURE_DEPENDS "src/*.cxx")

target_sources(${ENGINE_NAME} PRIVATE ${ENGINE_SOURCE_FILES})

# Enforce some debug and release specific definitions.
target_compile_definitions(
  ${ENGINE_NAME}
  PUBLIC
    $<$<CONFIG:Debug>:
    INC_DEBUG_BUILD=1
    DEBUG
    _DEBUG
    >
    $<$<CONFIG:Release>:
    NDEBUG
    INC_RELEASE_BUILD=1
    >
)

engine_link_external_libraries(${ENGINE_NAME})

message(STATUS "=== ${ENGINE_NAME} Build Configuration ===")
message(STATUS "Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Documentation: ${ENGINE_BUILD_DOCS}")
message(STATUS "Examples: ${ENGINE_BUILD_EXAMPLES}")
message(STATUS "===== Safety Settings =====")
message(STATUS "Paranoid Build: ${ENGINE_PARANOID}")
message(STATUS "Info Logging: ${ENGINE_LOG_INFO}")
message(STATUS "Warning Logging: ${ENGINE_LOG_WARNING}")
message(STATUS "Error Logging: ${ENGINE_LOG_ERROR}")
message(STATUS "======================================")
