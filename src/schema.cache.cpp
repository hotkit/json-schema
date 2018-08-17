/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.cache.hpp>


const fostlib::setting<f5::json::value> f5::json::c_schema_path(__FILE__,
    "JSON schema validation", "Schema load path", value::array_t{}, true);


auto f5::json::schema_cache::root_cache() -> std::shared_ptr<schema_cache> {
    static std::shared_ptr<schema_cache> cache = []() {
        auto cache = std::make_shared<schema_cache>();
        if ( const auto p = fostlib::coerce<std::optional<f5::u8view>>(c_schema_path.value()); p )
        {
            fostlib::json s{f5::json::value::parse(
                fostlib::utf::load_file(fostlib::coerce<boost::filesystem::path>(
                    fostlib::string(*p))))};
            if ( s.has_key("$id") ) {
                const auto parts = fostlib::partition(fostlib::coerce<fostlib::string>(s["$id"]), "#");
                cache->cache.insert(std::make_pair(fostlib::url{parts.first}, schema{std::move(s)}));
            }
        } else {
            throw fostlib::exceptions::not_implemented("f5::json::schema_cache::root_cache",
                "This type of schema load path not yet supported", c_schema_path.value());
        }
        return cache;
    }();
    return cache;
}


auto f5::json::schema_cache::operator [] (const fostlib::url &u) const -> const schema & {
    const auto pos = cache.find(u);
    if ( pos == cache.end() ) {
        throw fostlib::exceptions::not_implemented(__func__, "Schema not found", u);
    } else {
        return pos->second;
    }
}

