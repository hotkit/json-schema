if(NOT TARGET stress)
    add_custom_target(stress)
    set_property(TARGET stress PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()

add_executable(json-schema-testsuite EXCLUDE_FROM_ALL testsuite.cpp)
target_link_libraries(json-schema-testsuite f5-json-schema fost-cli fost-inet)

add_custom_command(OUTPUT json-schema-testsuite-v7-not
        COMMAND json-schema-testsuite -b false
            not.json
    )
add_custom_command(OUTPUT json-schema-testsuite-v7-properties
        COMMAND json-schema-testsuite -b false -v true
            properties.json
    )

add_custom_target(json-schema-testsuite-v7 DEPENDS
        json-schema-testsuite-v7-not
#         json-schema-testsuite-v7-properties
    )
add_dependencies(stress json-schema-testsuite json-schema-testsuite-v7)
