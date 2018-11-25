if(TARGET check)
    add_custom_command(OUTPUT test-all
            COMMAND json-schema-validator -b false
                --schema ${CMAKE_CURRENT_SOURCE_DIR}/any.schema.json
                ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.json
                ${CMAKE_CURRENT_SOURCE_DIR}/null.json
            MAIN_DEPENDENCY any.schema.json
            DEPENDS
                alltypes.json
                null.json
        )
    add_custom_command(OUTPUT test-all-invalid
            COMMAND json-schema-validator -b false -i true
                --schema ${CMAKE_CURRENT_SOURCE_DIR}/invalid.schema.json
                ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.json
                ${CMAKE_CURRENT_SOURCE_DIR}/null.json
            MAIN_DEPENDENCY invalid.schema.json
            DEPENDS
                alltypes.json
                null.json
        )

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


    ## Check all of the schemas against the JSON schema itself. Any failure
    ## here should be able to act as a "todo" list against the validator.
    add_custom_command(OUTPUT test-z-json-schema
            COMMAND json-schema-validator -b false
                --schema ${CMAKE_CURRENT_SOURCE_DIR}/json-schema.schema.json
                ${CMAKE_CURRENT_SOURCE_DIR}/alltypes.schema.json
                ${CMAKE_CURRENT_SOURCE_DIR}/any.schema.json
                ${CMAKE_CURRENT_SOURCE_DIR}/invalid.schema.json
                ${CMAKE_CURRENT_SOURCE_DIR}/json-schema.schema.json
                ${CMAKE_CURRENT_SOURCE_DIR}/null.schema.json
            MAIN_DEPENDENCY json-schema.schema.json
            DEPENDS
                alltypes.schema.json
                any.schema.json
                invalid.schema.json
                json-schema.schema.json
                null.schema.json
        )


    add_custom_target(json-schema-tests DEPENDS
            test-all
            test-all-invalid
            test-alltypes
            test-alltypes-invalid
            test-null
            test-null-invalid
            test-z-json-schema
        )
    add_dependencies(check json-schema-tests)
endif()
