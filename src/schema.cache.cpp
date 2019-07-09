/**
    Copyright 2018-2019, Proteus Technologies Co Ltd.
   <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.cache.hpp>
#include <f5/json/schema.loaders.hpp>
#include <f5/threading/map.hpp>
#include <fost/insert>
#include <fost/push_back>


const fostlib::setting<f5::json::value> f5::json::c_schema_path(
        __FILE__,
        "JSON schema validation",
        "Schema load path",
        value::array_t{},
        true);

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


    auto &g_loader_cache_mutex() {
        static std::mutex m;
        return m;
    }
    auto &g_loader_cache() {
        static std::map<fostlib::url, std::unique_ptr<f5::json::schema>> c;
        return c;
    }
    auto &g_pre_load() {
        static auto cache = []() {
            auto cache = std::make_shared<f5::json::schema_cache>(nullptr);
            if (const auto p = fostlib::coerce<std::optional<f5::u8view>>(
                        f5::json::c_schema_path.value());
                p) {
                const auto fn =
                        fostlib::coerce<fostlib::fs::path>(fostlib::string(*p));
                fostlib::json s{
                        f5::json::value::parse(fostlib::utf::load_file(fn))};
                cache->insert(f5::json::schema{fostlib::url{fostlib::url{}, fn},
                                               std::move(s)});
            } else if (f5::json::c_schema_path.value().isarray()) {
                for (const auto filepath : f5::json::c_schema_path.value()) {
                    throw fostlib::exceptions::not_implemented(
                            __PRETTY_FUNCTION__,
                            "This type of schema load path not yet supported",
                            f5::json::c_schema_path.value());
                }
            }
            return cache;
        }();
        return cache;
    }


}


/**
 * ## `schema_cache`
 */


f5::json::schema_cache::schema_cache() : base(root_cache()) {}
f5::json::schema_cache::schema_cache(std::shared_ptr<schema_cache> b)
: base(b) {}


auto f5::json::schema_cache::root_cache() -> std::shared_ptr<schema_cache> {
    /**
     * This "special value" for the root cache is pretty bad, but we can't put
     * a cache in a schema and load schemas during the root cache creation
     * because we end up in an infinite loop.
     *
     * The returned pointer cannot be `nullptr` or we will go into an infinite
     * loop chasing down the pre-load list before we bottom out and try to
     * load in more schemas using the schema loading mechanism.
     */
    static auto cache{
            std::make_shared<schema_cache>(std::shared_ptr<schema_cache>{})};
    return cache;
}


auto f5::json::schema_cache::operator[](f5::u8view u) const -> schema const & {
    return (*this)[fostlib::url{u}];
}


    try {
        if(not u.fragment()) u.fragment(fostlib::string{});
        const auto pos = cache.find(u);
        if (pos == cache.end()) {
            if (base == root_cache()) {
                return (*g_pre_load())[std::move(u)];
            } else if (base) {
                return (*base)[std::move(u)];
            } else {
                std::unique_lock<std::mutex> lock{g_loader_cache_mutex()};
                if (auto pos = g_loader_cache().find(u);
                    pos != g_loader_cache().end()) {
                    return *pos->second;
                } else {
                    auto l = load_schema(u.as_string());
                    if (l) {
                        return *(
                                g_loader_cache()[std::move(u)] =
                                        std::move(l));
                    } else {
                        throw fostlib::exceptions::not_implemented(
                                __PRETTY_FUNCTION__, "Schema not found", u);
                    }
                }
            }
        } else {
            return pos->second;
        }
    } catch (fostlib::exceptions::exception &e) {
        if (not e.data().has_key("schema-cache")) {
            std::unique_lock<std::mutex> lock{g_loader_cache_mutex()};
            for (const auto &p : g_loader_cache()) {
                fostlib::push_back(e.data(), "schema-cache", "", p.first.as_string());
            }
        }
        const fostlib::string cp{std::to_string((int64_t)this)};
        fostlib::insert(e.data(), "schema-cache", cp, value::array_t{});
        for (const auto &c : cache) {
            fostlib::push_back(
                    e.data(), "schema-cache", cp,
                    fostlib::coerce<fostlib::string>(c.first));
        }
        throw;
    }
}


auto f5::json::schema_cache::insert(schema s) -> const schema & {
    auto pos = cache.insert(std::make_pair(s.self(), s));
    return pos.first->second;
}


auto f5::json::schema_cache::insert(fostlib::url n, schema s)
        -> const schema & {
    if(not n.fragment()) n.fragment(fostlib::string{});
    cache.insert(std::make_pair(std::move(n), s));
    return insert(s);
}
