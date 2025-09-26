include_guard(GLOBAL)

set(ENGINE_IS_DEBUG "false")

# Determine if this is a debug build.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ENGINE_IS_DEBUG "true")
endif()

string(TIMESTAMP ENGINE_BUILD_DATE "%Y-%m-%d" UTC)
string(TIMESTAMP ENGINE_BUILD_TIME "%H:%M:%S" UTC)

# Compile-time options for the build.
option(ENGINE_LOG_INFO "Compile info logging" ON)
option(ENGINE_LOG_WARNING "Compile warning logging" ON)
option(ENGINE_LOG_ERROR "Compile error logging" ON)
option(ENGINE_PARANOID "Enable paranoid build checks" ON)

function(engine_option_to_cpp_bool VARIABLE_NAME)
  if(${VARIABLE_NAME})
    set(${VARIABLE_NAME} "true" PARENT_SCOPE)
  else()
    set(${VARIABLE_NAME} "false" PARENT_SCOPE)
  endif()
endfunction()

function(engine_generate_header TEMPLATE_NAME)
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/src/${TEMPLATE_NAME}.hxx.in")
    message(FATAL_ERROR "Template file '${TEMPLATE_NAME}.hxx.in' does not exist in 'src/'.")
  endif()

  if(NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
  endif()

  # Very ugly but seemingly has to be done....
  engine_option_to_cpp_bool(ENGINE_LOG_INFO)
  engine_option_to_cpp_bool(ENGINE_LOG_WARNING)
  engine_option_to_cpp_bool(ENGINE_LOG_ERROR)
  engine_option_to_cpp_bool(ENGINE_PARANOID)

  configure_file(
    "${CMAKE_SOURCE_DIR}/src/${TEMPLATE_NAME}.hxx.in"
    "${CMAKE_BINARY_DIR}/generated/${TEMPLATE_NAME}.hxx"
    @ONLY
  )
endfunction()

option(ENGINE_BUILD_DOCS "Enable documentation generation" OFF)

if(ENGINE_BUILD_DOCS)
  find_package(Doxygen REQUIRED)

  if(DOXYGEN_FOUND)
    configure_file("${CMAKE_SOURCE_DIR}/Doxyfile" "${CMAKE_BINARY_DIR}/Doxyfile" @ONLY)

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

    configure_file(
      "${CMAKE_SOURCE_DIR}/docs/doxygen/simple-dark.css"
      "${CMAKE_BINARY_DIR}/simple-dark.css"
      COPYONLY
    )
    configure_file("${CMAKE_SOURCE_DIR}/docs/doxygen/layout.xml" "${CMAKE_BINARY_DIR}/layout.xml" COPYONLY)

    add_custom_target(
      docs
      ALL
      COMMAND
      ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/Doxyfile"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating simple dark documentation with Doxygen"
      VERBATIM
      DEPENDS
      "${CMAKE_BINARY_DIR}/simple-dark.css"
      "${CMAKE_BINARY_DIR}/layout.xml"
    )
  else()
    message(WARNING "Doxygen not found! Documentation generation disabled.")
  endif()
endif()
