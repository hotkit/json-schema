/**
    Copyright 2018-2019, Proteus Technologies Co Ltd.
   <https://support.felspar.com/>

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
     * start with because it will scan each schema we do find twice (once in
     * that schema and once in its parent). Multiply nesting schemas increases
     * this scan count.
     */
    void preload_ids(
            fostlib::url const &base,
            f5::json::value schema,
            f5::json::schema_cache &cache) {
        if (schema.isobject()) {
            for (auto const [key, value] : schema.object()) {
                if (value.has_key("$id") && value["$id"].isatom()) {
                    auto &s = cache.insert(f5::json::schema{
                            fostlib::url{
                                    base,
                                    fostlib::coerce<f5::u8view>(value["$id"])},
                            value});
                }
                preload_ids(base, value, cache);
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
    if(not id.fragment()) id.fragment(fostlib::string{});
    preload_ids(id, validation, *schemas);
}


auto f5::json::schema::validate(value j) const -> validation::result {
    return validation::first_error(
            validation::annotations{*this, pointer{}, j, pointer{}});
}
