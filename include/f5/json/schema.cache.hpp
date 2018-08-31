/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#pragma once

#include <f5/json/schema.hpp>


namespace f5 {


    namespace json {


        extern const fostlib::setting<value> c_schema_path;


        class schema_cache {
            friend validation::annotations;

            std::shared_ptr<schema_cache> base;
            std::map<fostlib::string, schema> cache;

        public:
            /// Create an empty cache which uses the root cache
            /// as its base
            schema_cache();
            /// Create a cache which is built on top of another cache
            schema_cache(std::shared_ptr<schema_cache>);

            /// Perform a lookup in this case and its bases
            const schema &operator [] (f5::u8view) const;

            /// The root cache. The root cache is the only cache which
            /// should have an empty base.
            static std::shared_ptr<schema_cache> root_cache();

            /// Add a schema at a given position in the cache
            const schema &insert(fostlib::string, schema);
            /// Add a schema at an unnamed position, i.e. only if it
            /// contains a `$id` describing its proper location
            const schema &insert(schema);
        };


    }


}

