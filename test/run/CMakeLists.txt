add_executable(json-schema-validator valid.cpp)
target_link_libraries(json-schema-validator fost-cli f5-json-schema)

add_custom_command(OUTPUT test-valid-all
        COMMAND json-schema-validator -b false
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/any.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/null.json
        MAIN_DEPENDENCY any.schema.json
        DEPENDS
            null.json
    )

add_custom_command(OUTPUT test-invalid-all
        COMMAND json-schema-validator -b false -i true
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/invalid.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/null.json
        MAIN_DEPENDENCY invalid.schema.json
        DEPENDS
            null.json
    )

add_custom_target(json-schema-tests DEPENDS
        test-valid-all
        test-invalid-all
    )
add_dependencies(check json-schema-tests)
