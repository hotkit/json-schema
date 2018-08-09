cmake_minimum_required(VERSION 3.6)
project(f5-json-schema)


add_library(f5-json-schema INTERFACE)
target_include_directories(f5-json-schema INTERFACE include)
if(CMAKE_VERSION VERSION_LESS "3.8")
    message(WARNING "This verison of cmake is too old to request C++17 for f5-json-schema.")
    target_compile_options(f5-json-schema INTERFACE -std=c++17)
else()
    target_compile_features(f5-json-schema INTERFACE cxx_std_17)
endif()


enable_testing()
if(NOT TARGET check)
    add_custom_target(check)
    set_property(TARGET check PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()
add_subdirectory(test)
