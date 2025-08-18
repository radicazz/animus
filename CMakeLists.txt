cmake_minimum_required(VERSION 3.10)

project(incarnate CXX)

file(GLOB INCARNATE_SOURCES "src/*.cxx")

add_executable(incarnate WIN32 ${INCARNATE_SOURCES})

target_compile_features(incarnate PRIVATE cxx_std_23)
