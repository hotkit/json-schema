add_executable(json-schema-validator valid.cpp)
target_link_libraries(json-schema-validator fost-cli f5-json-schema)

add_custom_command(OUTPUT test-alltypes
        COMMAND json-schema-validator -b false
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.json
        MAIN_DEPENDENCY alltypes.schema.json
        DEPENDS
            alltypes.json
    )

add_custom_command(OUTPUT test-alltypes-invalid
        COMMAND json-schema-validator -b false -i true
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/null.json
        MAIN_DEPENDENCY alltypes.schema.json
        DEPENDS
            null.json
    )

add_custom_command(OUTPUT test-invalid-all
        COMMAND json-schema-validator -b false -i true
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/invalid.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.json
            ${CMAKE_CURRENT_SOURCE_DIR}/null.json
        MAIN_DEPENDENCY invalid.schema.json
        DEPENDS
            alltypes.json
            null.json
    )

add_custom_command(OUTPUT test-null
        COMMAND json-schema-validator -b false
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/null.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/null.json
        MAIN_DEPENDENCY null.schema.json
        DEPENDS
            null.json
    )
add_custom_command(OUTPUT test-null-invalid
        COMMAND json-schema-validator -b false -i true
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/null.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.json
        MAIN_DEPENDENCY null.schema.json
        DEPENDS
            alltypes.json
    )

add_custom_command(OUTPUT test-valid-all
        COMMAND json-schema-validator -b false
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/any.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.json
            ${CMAKE_CURRENT_SOURCE_DIR}/null.json
        MAIN_DEPENDENCY any.schema.json
        DEPENDS
            alltypes.json
            null.json
    )


add_custom_target(json-schema-tests DEPENDS
        test-alltypes
        test-alltypes-invalid
        test-invalid-all
        test-null
        test-null-invalid
        test-valid-all
    )
add_dependencies(check json-schema-tests)
