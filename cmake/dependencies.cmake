include_guard(GLOBAL)

# Try find and use ccache if available.
find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Add git submodules to the build.
add_subdirectory("${CMAKE_SOURCE_DIR}/external/SDL" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_SOURCE_DIR}/external/SDL_image" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_SOURCE_DIR}/external/SDL_ttf" EXCLUDE_FROM_ALL)

function(engine_link_external_libraries ENGINE_TARGET_NAME)
  if(NOT TARGET ${ENGINE_TARGET_NAME})
    message(FATAL_ERROR "Target '${ENGINE_TARGET_NAME}' does not exist.")
  endif()

  # Link the SDL libraries.
  target_link_libraries(
    ${ENGINE_TARGET_NAME}
    PUBLIC
      SDL3::SDL3
      SDL3_image::SDL3_image
      SDL3_ttf::SDL3_ttf
  )

  # Include the header only libs.
  target_include_directories(
    ${ENGINE_TARGET_NAME}
    PUBLIC
      "${CMAKE_SOURCE_DIR}/src"
      "${CMAKE_BINARY_DIR}/generated"
      "${CMAKE_SOURCE_DIR}/external/glm"
      "${CMAKE_SOURCE_DIR}/external/entt/single_include"
  )
endfunction()
