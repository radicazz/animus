cmake_minimum_required(VERSION 3.21)

set(EXAMPLE_NAME space_war)

add_executable(${EXAMPLE_NAME} main.cxx)

target_compile_features(${EXAMPLE_NAME} PRIVATE cxx_std_20)
target_link_libraries(${EXAMPLE_NAME} PRIVATE ${ENGINE_NAME})
target_include_directories(
  ${EXAMPLE_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}"
)

# Place binaries for examples in a dedicated folder.
set_target_properties(
  ${EXAMPLE_NAME} PROPERTIES
  FOLDER "examples"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
)

# Copy required runtime DLLs on Windows and ensure output dir exists.
if(WIN32)
  add_custom_command(
    TARGET ${EXAMPLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${EXAMPLE_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_RUNTIME_DLLS:${EXAMPLE_NAME}>
    $<TARGET_FILE_DIR:${EXAMPLE_NAME}>
    COMMAND_EXPAND_LISTS
  )
endif()

# Copy shared assets into the example output directory after build.
set(EXAMPLE_ASSETS_DESTINATION_DIR $<TARGET_FILE_DIR:${EXAMPLE_NAME}>/assets)
add_custom_command(
  TARGET ${EXAMPLE_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/assets"
  "${EXAMPLE_ASSETS_DESTINATION_DIR}"
  COMMENT "Copying assets to ${EXAMPLE_ASSETS_DESTINATION_DIR}"
  VERBATIM
)
