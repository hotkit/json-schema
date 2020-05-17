cmake_minimum_required(VERSION 3.10)
project(json-schema-circleci)

# Opt in to check tests
add_custom_target(check)

add_subdirectory(cord)
add_subdirectory(boost)
add_subdirectory(crypto)
add_subdirectory(fost-base)
add_subdirectory(fost-internet)
add_subdirectory(.. json-schema)
add_subdirectory(threading)
