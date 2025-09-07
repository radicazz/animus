cmake_minimum_required(VERSION 3.21)

# Enable modern CMake policies
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW) # Use timestamps from archives
endif()

project(
    incarnate
    VERSION 0.1.0
    DESCRIPTION "An epic 2D game engine."
    LANGUAGES CXX
)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# In-source builds are ugly.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Add configuration-specific output directories
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Generate version header from template
configure_file(
    "${CMAKE_SOURCE_DIR}/src/engine/version.hxx.in"
    "${CMAKE_BINARY_DIR}/generated/engine/version.hxx"
    @ONLY
)

# Add submodules.
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)

# Create incarnate executable
add_executable(incarnate)

# Set target properties
set_target_properties(incarnate PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "incarnate"
)

# On Windows, set the executable to be a GUI application.
if(WIN32)
    set_target_properties(incarnate PROPERTIES
        WIN32_EXECUTABLE TRUE
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
endif()

# Glob game & engine source files.
# TODO: Better way to do this?
file(GLOB_RECURSE INCARNATE_SOURCES
    CONFIGURE_DEPENDS
    "src/*.cxx" "src/*.hxx"
)

# Add the sources to the executable.
target_sources(incarnate PRIVATE ${INCARNATE_SOURCES})

# Organize source files in IDE
source_group(TREE "${CMAKE_SOURCE_DIR}/src" FILES ${INCARNATE_SOURCES})

# Modern compile features.
target_compile_features(incarnate PRIVATE cxx_std_20)

# Enhanced compiler-specific warnings and optimizations
target_compile_options(incarnate PRIVATE

    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
    /W4 # High warning level
    /permissive- # Disable non-conforming code
    /utf-8 # Source and execution character sets are UTF-8
    /Zc:__cplusplus # Enable updated __cplusplus macro
    $<$<CONFIG:Debug>:/RTC1> # Runtime checks in debug
    $<$<CONFIG:Release>:/O2> # Optimize for speed in release
    >

    # GCC
    $<$<CXX_COMPILER_ID:GNU>:
    -Wall -Wextra -Wpedantic
    -Wconversion -Wsign-conversion
    -Wcast-qual -Wformat=2
    -Wundef -Werror=return-type
    $<$<CONFIG:Debug>:-Og -g>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    >

    # Clang
    $<$<CXX_COMPILER_ID:Clang>:
    -Wall -Wextra -Wpedantic
    -Wconversion -Wsign-conversion
    -Wcast-qual -Wformat=2
    -Wundef -Werror=return-type
    $<$<CONFIG:Debug>:-Og -g>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    >
)

# Debug/Release specific settings
target_compile_definitions(incarnate PRIVATE
    $<$<CONFIG:Debug>:
    INC_DEBUG_BUILD=1
    DEBUG
    _DEBUG
    >
    $<$<CONFIG:Release>:
    NDEBUG
    INC_RELEASE_BUILD=1
    >

    # Platform-specific definitions
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN NOMINMAX>
    $<$<PLATFORM_ID:Linux>:_GNU_SOURCE>
)

# Link libraries with proper visibility
target_link_libraries(
    incarnate
    PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

# Include directories - prefer target-based approach when possible
target_include_directories(
    incarnate
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/external/glm"
    "${CMAKE_SOURCE_DIR}/external/entt/single_include"
    "${CMAKE_BINARY_DIR}/generated" # For generated headers like version.hxx
)

# DLL copying with better platform handling
if(WIN32)
    set(INCARNATE_DEPENDENCY_DLLS
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE:SDL3_image::SDL3_image-shared>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>
    )

    add_custom_command(TARGET incarnate POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${INCARNATE_DEPENDENCY_DLLS}
        $<TARGET_FILE_DIR:incarnate>
        COMMENT "Copying DLLs to output directory"
    )
endif()

# Runtime library paths for Unix systems
if(UNIX)
    set_target_properties(incarnate PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()

# Cross-platform asset copying with better handling
set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DEST_DIR "$<TARGET_FILE_DIR:incarnate>/assets")

add_custom_command(TARGET incarnate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    "${ASSETS_SOURCE_DIR}"
    "${ASSETS_DEST_DIR}"
    COMMENT "Copying game assets to ${ASSETS_DEST_DIR}"
    VERBATIM
)

# Ensure assets directory exists
add_custom_command(TARGET incarnate PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DEST_DIR}"
    COMMENT "Creating assets directory"
    VERBATIM
)

# Enhanced install rules with proper component support
install(TARGETS incarnate
    RUNTIME
    DESTINATION bin
    COMPONENT Runtime
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets"
    DESTINATION bin
    COMPONENT Assets
    PATTERN "*.tmp" EXCLUDE
    PATTERN ".git*" EXCLUDE
)

# Install runtime dependencies on Windows
if(WIN32)
    install(FILES ${INCARNATE_DEPENDENCY_DLLS}
        DESTINATION bin
        COMPONENT Runtime
    )
endif()

# Enhanced packaging with more metadata
include(CPack)
set(CPACK_PACKAGE_NAME "Incarnate")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "radicazz")
set(CPACK_PACKAGE_CONTACT "https://github.com/radicazz/incarnate")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_EXECUTABLES "incarnate" "Incarnate Game Engine")

# Platform-specific packaging
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "Incarnate Game Engine")
    set(CPACK_NSIS_PACKAGE_NAME "Incarnate")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "radicazz")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
endif()

# Component-based packaging
set(CPACK_COMPONENTS_ALL Runtime Assets)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Incarnate Engine")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Core game engine executable")
set(CPACK_COMPONENT_ASSETS_DISPLAY_NAME "Game Assets")
set(CPACK_COMPONENT_ASSETS_DESCRIPTION "Sprites, fonts, and other game assets")

# Optional: Add development tools and options
option(INCARNATE_ENABLE_TESTING "Enable unit testing" OFF)
option(INCARNATE_ENABLE_DOCS "Enable documentation generation" OFF)
option(INCARNATE_STATIC_ANALYSIS "Enable static analysis tools" OFF)

# Testing support
if(INCARNATE_ENABLE_TESTING)
    enable_testing()

    # Could add test subdirectory here if tests are implemented
endif()

# Documentation support
if(INCARNATE_ENABLE_DOCS)
    find_package(Doxygen REQUIRED)

    if(DOXYGEN_FOUND)
        # Configure Doxyfile with project variables
        configure_file(
            "${CMAKE_SOURCE_DIR}/Doxyfile"
            "${CMAKE_BINARY_DIR}/Doxyfile"
            @ONLY
        )

        # Create docs directory
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

        # Copy CSS and layout files to build directory for Doxygen
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

        # Add custom target for generating documentation automatically
        add_custom_target(docs ALL
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

# Print configuration summary
message(STATUS "=== Incarnate Build Configuration ===")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Testing: ${INCARNATE_ENABLE_TESTING}")
message(STATUS "Documentation: ${INCARNATE_ENABLE_DOCS}")
message(STATUS "Static Analysis: ${INCARNATE_STATIC_ANALYSIS}")
message(STATUS "======================================")
