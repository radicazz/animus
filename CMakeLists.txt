cmake_minimum_required(VERSION 3.21)

project(
  helipad
  VERSION 0.1.0
  DESCRIPTION "A top-down 2D game engine."
  LANGUAGES CXX
)

# Default to Debug build if none is specified.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Disable in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  message(STATUS "Ccache found: ${CCACHE_PROGRAM}")
else()
  message(STATUS "Ccache not found")
endif()

# Add required modules.
add_subdirectory("external/SDL" EXCLUDE_FROM_ALL)
add_subdirectory("external/SDL_image" EXCLUDE_FROM_ALL)
add_subdirectory("external/SDL_ttf" EXCLUDE_FROM_ALL)

# To avoid potential name clashes with subdirectories.
set(ENGINE_NAME ${PROJECT_NAME})

# Create the main library target.
add_library(${ENGINE_NAME} STATIC)

# Set C++ standard & require that other targets using this library also use C++20.
target_compile_features(${ENGINE_NAME} PRIVATE cxx_std_20)

string(TIMESTAMP ENGINE_BUILD_DATE "%Y-%m-%d" UTC)
string(TIMESTAMP ENGINE_BUILD_TIME "%H:%M:%S" UTC)

function(util_to_cpp_bool VARIABLE_NAME)
  if(${VARIABLE_NAME})
    set(${VARIABLE_NAME} "true" PARENT_SCOPE)
  else()
    set(${VARIABLE_NAME} "false" PARENT_SCOPE)
  endif()
endfunction()

option(ENGINE_LOG_INFO "Compile info logging" ON)
option(ENGINE_LOG_WARNING "Compile warning logging" ON)
option(ENGINE_LOG_ERROR "Compile error logging" ON)
option(ENGINE_PARANOID "Enable paranoid build checks" ON)

util_to_cpp_bool(ENGINE_LOG_INFO)
util_to_cpp_bool(ENGINE_LOG_WARNING)
util_to_cpp_bool(ENGINE_LOG_ERROR)
util_to_cpp_bool(ENGINE_PARANOID)

set(ENGINE_IS_DEBUG "false")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ENGINE_IS_DEBUG "true")
endif()

# Generate config header from template.
set(ENGINE_TEMPLATE_NAME "config")
configure_file(
  "${CMAKE_SOURCE_DIR}/src/${ENGINE_TEMPLATE_NAME}.hxx.in"
  "${CMAKE_BINARY_DIR}/generated/${ENGINE_TEMPLATE_NAME}.hxx"
  @ONLY
)

file(GLOB_RECURSE ENGINE_SOURCE_FILES CONFIGURE_DEPENDS "src/*.cxx")
target_sources(${ENGINE_NAME} PRIVATE ${ENGINE_SOURCE_FILES})

target_compile_definitions(
  ${ENGINE_NAME} PUBLIC
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

# Link the SDL libraries.
target_link_libraries(
  ${ENGINE_NAME} PUBLIC
  SDL3::SDL3
  SDL3_image::SDL3_image
  SDL3_ttf::SDL3_ttf
)

# Include directories for the engine.
target_include_directories(
  ${ENGINE_NAME}
  PUBLIC
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_BINARY_DIR}/generated"
  "${CMAKE_SOURCE_DIR}/external/glm"
  "${CMAKE_SOURCE_DIR}/external/entt/single_include"
)

option(ENGINE_BUILD_EXAMPLES "Build example projects" ON)

if(ENGINE_BUILD_EXAMPLES)
  add_subdirectory("examples")
endif()

option(ENGINE_BUILD_DOCS "Enable documentation generation" OFF)

if(ENGINE_BUILD_DOCS)
  find_package(Doxygen REQUIRED)

  if(DOXYGEN_FOUND)
    configure_file(
      "${CMAKE_SOURCE_DIR}/Doxyfile"
      "${CMAKE_BINARY_DIR}/Doxyfile"
      @ONLY
    )

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

    configure_file(
      "${CMAKE_SOURCE_DIR}/docs/style/simple-dark.css"
      "${CMAKE_BINARY_DIR}/simple-dark.css"
      COPYONLY
    )

    configure_file(
      "${CMAKE_SOURCE_DIR}/docs/layout.xml"
      "${CMAKE_BINARY_DIR}/layout.xml"
      COPYONLY
    )

    add_custom_target(
      docs ALL
      COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/Doxyfile"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating simple dark documentation with Doxygen"
      VERBATIM
      DEPENDS "${CMAKE_BINARY_DIR}/simple-dark.css" "${CMAKE_BINARY_DIR}/layout.xml"
    )

    message(STATUS "Documentation generation enabled and will build automatically.")
  else()
    message(WARNING "Doxygen not found! Documentation generation disabled.")
  endif()
endif()

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
