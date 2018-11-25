if(TARGET check)
    add_library(json-schema-headers-tests STATIC EXCLUDE_FROM_ALL
            assertions.cpp
            schema.cpp
            schema.cache.cpp
            schema.loaders.cpp
            validator.cpp
        )
    target_link_libraries(json-schema-headers-tests f5-json-schema)
    add_dependencies(check json-schema-headers-tests)
endif()
