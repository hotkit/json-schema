add_executable(json-schema-validator valid.cpp)
target_link_libraries(json-schema-validator fost-cli f5-json-schema)
install(TARGETS json-schema-validator
    EXPORT json-schema-validator
    RUNTIME DESTINATION bin)

