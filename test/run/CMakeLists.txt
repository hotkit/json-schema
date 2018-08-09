add_executable(valid EXCLUDE_FROM_ALL valid.cpp)
target_link_libraries(valid f5-json-schema)

add_custom_command(OUTPUT test-valid
        COMMAND valid
    )

add_custom_target(json-schema-tests DEPENDS
        test-valid
    )
add_dependencies(check json-schema-tests)
