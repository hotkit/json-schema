if(NOT TARGET stress)
    add_custom_target(stress)
    set_property(TARGET stress PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()

add_executable(json-schema-testsuite EXCLUDE_FROM_ALL testsuite.cpp)
target_link_libraries(json-schema-testsuite f5-json-schema fost-cli fost-inet)
add_dependencies(stress json-schema-testsuite)

## The to do list:
##
## * additionalItems.json
## * allOf.json
## * anyOf.json
## * contains.json
## * definitions.json
## * dependencies.json
## * if-then-else.json
## * items.json
## * oneOf.json
## * pattern.json
## * propertyNames.json
## * ref.json
## * refRemote.json
## * uniqueItems.json

function(draftv7 name)
    add_custom_command(TARGET json-schema-testsuite
            COMMAND json-schema-testsuite -b false
                ${name}.json)
endfunction()

draftv7(additionalProperties)
draftv7(boolean_schema)
draftv7(const)
draftv7(default)
draftv7(enum)
draftv7(exclusiveMaximum)
draftv7(exclusiveMinimum)
# draftv7(items)
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
draftv7(patternProperties)
draftv7(properties)
# draftv7(ref)
draftv7(required)
draftv7(type)

