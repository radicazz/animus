cmake_minimum_required(VERSION 3.20)
project(incarnate CXX)

# Don't allow in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Add the project's submodules.
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(external/glm EXCLUDE_FROM_ALL)

# Add our game's source files.
file(GLOB_RECURSE INCARNATE_SOURCES "src/*.cxx")

add_executable(incarnate WIN32 ${INCARNATE_SOURCES})

target_compile_features(incarnate PRIVATE cxx_std_23)

target_link_libraries(
    incarnate
    PRIVATE SDL3::SDL3
    PRIVATE SDL3_image::SDL3_image
    PRIVATE SDL3_ttf::SDL3_ttf
)

target_include_directories(
    incarnate
    PRIVATE external/glm
)

set(INCARNATE_DEPENDENCY_DLLS
    $<TARGET_FILE:SDL3::SDL3-shared>
    $<TARGET_FILE:SDL3_image::SDL3_image-shared>
    $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>
)

# Copy shared libraries to output path.
add_custom_command(
    TARGET incarnate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${INCARNATE_DEPENDENCY_DLLS}
            $<TARGET_FILE_DIR:incarnate>
)

# Make sure runtime loader can find libs on Linux/macOS
set_target_properties(
    incarnate PROPERTIES
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
)

# Copy assets folder over to build directory for game assets.
add_custom_command(
    TARGET incarnate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:incarnate>/assets"
)
