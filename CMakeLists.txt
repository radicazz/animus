cmake_minimum_required(VERSION 3.21)

project(
    incarnate
    VERSION 0.1.0
    DESCRIPTION "An epic 2D game engine."
    LANGUAGES CXX
)

# In-source builds are ugly.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Add submodules.
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(external/glm EXCLUDE_FROM_ALL)

add_executable(incarnate)

# On Windows, set the executable to be a GUI application.
if(WIN32)
    set_target_properties(incarnate PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Glob game & engine source files.
# TODO: Better way to do this?
file(GLOB_RECURSE INCARNATE_SOURCES
    CONFIGURE_DEPENDS
    "src/*.cxx" "src/*.hxx"
)

# Add the sources to the executable.
target_sources(incarnate PRIVATE ${INCARNATE_SOURCES})

# Modern compile features.
target_compile_features(incarnate PRIVATE cxx_std_20)

# Compiler-specific warnings and optimizations
target_compile_options(incarnate PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Debug/Release specific settings
target_compile_definitions(incarnate PRIVATE
    $<$<CONFIG:Debug>:INC_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>
)

target_link_libraries(
    incarnate PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

target_include_directories(
    incarnate PRIVATE
    external/glm
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

# Cross-platform asset copying.
add_custom_command(TARGET incarnate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:incarnate>/assets"
    COMMENT "Copying game assets"
    VERBATIM
)

# Install rules.
install(TARGETS incarnate
    RUNTIME DESTINATION bin
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets"
    DESTINATION bin
)

# Packaging
include(CPack)
set(CPACK_PACKAGE_NAME "Incarnate")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
