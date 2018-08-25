if(NOT TARGET stress)
    add_custom_target(stress)
    set_property(TARGET stress PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()

add_executable(json-schema-testsuite EXCLUDE_FROM_ALL testsuite.cpp)
target_link_libraries(json-schema-testsuite f5-json-schema fost-cli fost-inet)
add_dependencies(stress json-schema-testsuite)

## The to do list:
##
## * definitions.json
## * ref.json
## * refRemote.json

function(draftv7 name)
    add_custom_command(TARGET json-schema-testsuite
            COMMAND json-schema-testsuite -b false
                "-p" ${CMAKE_CURRENT_SOURCE_DIR}/../checks/json-schema.schema.json
                ${name}.json)
endfunction()

draftv7(additionalItems)
draftv7(additionalProperties)
draftv7(allOf)
draftv7(anyOf)
draftv7(boolean_schema)
draftv7(const)
draftv7(contains)
draftv7(default)
draftv7(definitions)
draftv7(dependencies)
draftv7(enum)
draftv7(exclusiveMaximum)
draftv7(exclusiveMinimum)
draftv7(if-then-else)
draftv7(items)
draftv7(maximum)
draftv7(maxItems)
draftv7(maxLength)
draftv7(maxProperties)
draftv7(minimum)
draftv7(minItems)
draftv7(minLength)
draftv7(minProperties)
draftv7(multipleOf)
draftv7(not)
draftv7(oneOf)
draftv7(pattern)
draftv7(patternProperties)
draftv7(properties)
draftv7(propertyNames)
draftv7(ref)
# draftv7(refRemote)
draftv7(required)
draftv7(type)
draftv7(uniqueItems)

