/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.hpp>
#include <f5/json/schema.cache.hpp>
#include <fost/unicode>


namespace {
    /**
     * We walk down through all of the keys (excepting some particular values)
     * and we register all of the `$id`s that we find.
     *
     * The naive approach to this is going to be a bit slow, but good enough to
     * start with because it will scan each schema we do find twice (once in that
     * schema and once in its parent). Multiply nesting schemas increases this
     * scan count.
     */
    void preload_ids(f5::json::value schema, f5::json::schema_cache&cache) {
        if(schema.isobject()) {
            for(auto const [key, value] : schema.object()) {
                if(value.has_key("$id")) {
                }
            }
        }
    }
}


f5::json::schema::schema(const fostlib::url &b, value v)
: id{b,
     [v]() {
         if (v.has_key("$id")) {
             return fostlib::coerce<fostlib::string>(v["$id"]);
         } else {
             return fostlib::guid();
         }
     }()},
  validation{v},
  schemas{std::make_shared<schema_cache>()} {
      preload_ids(validation, *schemas);
}


auto f5::json::schema::validate(value j) const -> validation::result {
    return validation::first_error(
            validation::annotations{*this, pointer{}, j, pointer{}});
}
