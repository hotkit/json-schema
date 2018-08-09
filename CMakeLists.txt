cmake_minimum_required(VERSION 3.6)
project(f5-json-schema)

add_subdirectory(src)

enable_testing()
if(NOT TARGET check)
    add_custom_target(check)
    set_property(TARGET check PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()
add_subdirectory(test)
