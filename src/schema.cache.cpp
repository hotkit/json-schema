/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.cache.hpp>
#include <f5/json/schema.loaders.hpp>
#include <f5/threading/map.hpp>
#include <fost/insert>
#include <fost/push_back>


const fostlib::setting<f5::json::value> f5::json::c_schema_path(__FILE__,
    "JSON schema validation", "Schema load path", value::array_t{}, true);

/**
 * ## Schema loading
 *
 * Schemas are looked at and loaded in multiple phases.
 *
 * 1. The first is locally within the part of a schema that is currently
 *      being processed. This is handled by the schema cache present in
 *      the [`annotations` structure](../include/f5/json/validator.hpp).
 * 2. There is a `root_cache` that is populated by the pre-loaded schemas
 *      handled by the file schema loading. This is done when the
 *      `root_schema` is first requested in the code below.
 * 3. Schemas loaded by the dynamic schema loaders (immediatly below).
 *
 * This ordering ensures that a local schema can only affect validation
 * where it is used.
 */


namespace {


    f5::tsmap<fostlib::string, std::unique_ptr<f5::json::schema>> g_loader_cache;


}


/**
 * ## `schema_cache`
 */


f5::json::schema_cache::schema_cache()
: base(root_cache()) {
}
f5::json::schema_cache::schema_cache(std::shared_ptr<schema_cache> b)
: base(b) {
}


auto f5::json::schema_cache::root_cache() -> std::shared_ptr<schema_cache> {
    static std::shared_ptr<schema_cache> cache = []() {
        auto cache = std::make_shared<schema_cache>(nullptr);
        if ( const auto p = fostlib::coerce<std::optional<f5::u8view>>(c_schema_path.value()); p )
        {
            const auto fn = fostlib::coerce<boost::filesystem::path>(fostlib::string(*p));
            fostlib::json s{f5::json::value::parse(fostlib::utf::load_file(fn))};
            cache->insert(schema{fostlib::url{fostlib::url{}, fn}, std::move(s)});
        } else if ( c_schema_path.value().isarray() ) {
            for ( const auto filepath : c_schema_path.value() ) {
                throw fostlib::exceptions::not_implemented("f5::json::schema_cache::root_cache",
                    "This type of schema load path not yet supported", c_schema_path.value());
            }
        }
        return cache;
    }();
    return cache;
}


auto f5::json::schema_cache::operator [] (f5::u8view u) const -> const schema & {
    try {
        const auto pos = cache.find(u);
        if ( pos == cache.end() ) {
            if ( base ) {
                return (*base)[u];
            } else if ( const schema *s = g_loader_cache.find(u); s ) {
                return *s;
            } else {
                auto l = load_schema(u);
                if ( l ) {
                    return g_loader_cache.insert_or_assign(u, std::move(l));
                } else {
                    throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__, "Schema not found", u);
                }
            }
        } else {
            return pos->second;
        }
    } catch ( fostlib::exceptions::exception &e ) {
        if ( not e.data().has_key("schema-cache") ) {
            g_loader_cache.for_each([&e](auto k, const auto &) {
                fostlib::push_back(e.data(), "schema-cache", "", std::move(k));
            });
        }
        const fostlib::string cp{std::to_string((int64_t)this)};
        fostlib::insert(e.data(), "schema-cache", cp, value::array_t{});
        for ( const auto &c : cache ) {
            fostlib::push_back(e.data(), "schema-cache", cp, c.first);
        }
        throw;
    }
}


auto f5::json::schema_cache::insert(schema s) -> const schema & {
    if ( s.assertions().has_key("$id") ) {
        auto parts = fostlib::partition(
            fostlib::coerce<fostlib::string>(s.assertions()["$id"]), "#");
        cache.insert(std::make_pair(std::move(parts.first), s));
    }
    auto pos = cache.insert(std::make_pair(
        fostlib::coerce<fostlib::string>(s.self()),
        std::move(s)));
    return pos.first->second;
}


auto f5::json::schema_cache::insert(fostlib::string n, schema s) -> const schema & {
    cache.insert(std::make_pair(std::move(n), s));
    return insert(std::move(s));
}

