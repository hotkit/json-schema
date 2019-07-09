/**
    Copyright 2018-2019, Proteus Technologies Co Ltd.
   <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.loaders.hpp>
#include <f5/threading/map.hpp>
#include <fost/atexit>
#include <fost/log>
#include <fost/http>
#include <fost/insert>


const fostlib::setting<f5::json::value> f5::json::c_schema_loaders(
        __FILE__,
        "JSON schema validation",
        "Schema loaders",
        value::array_t{},
        true);


namespace {
    const fostlib::module c_fost_json_schema{fostlib::c_fost, "json-schema"};
    const fostlib::module c_fost_json_schema_loader{c_fost_json_schema,
                                                    "loader"};
}


/**
 * ## Schema loader infrastructure
 */


namespace {
    f5::tsmap<f5::lstring, f5::json::schema_loader_fn> g_loaders;
    const struct s {
        s() {
            fostlib::atexit([]() { g_loaders.clear(); });
        }
    } c_loaders_unloader;
}


f5::json::schema_loader::schema_loader(lstring n, schema_loader_fn f)
: lambda(f) {
    g_loaders.insert_or_assign(n, f);
}


std::unique_ptr<f5::json::schema> f5::json::load_schema(u8view url) {
    url = f5::u8view{url.begin(), std::find(url.begin(), url.end(), '#')};
    for (const auto loader : c_schema_loaders.value()) {
        auto fn = g_loaders.find(fostlib::coerce<u8view>(loader["loader"]));
        if (fn) {
            auto s = fn(url, loader);
            if (s) return s;
        } else {
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__, "Loader not found", loader);
        }
    }
    return {};
}


/**
 * ## URL based loader
 *
 * This will fetch schemas from a whitelisted URL over HTTP/HTTPS.
 */


namespace {


    auto http(const fostlib::url &b, const fostlib::url &u) {
        fostlib::http::user_agent ua(b);
        auto response = ua.get(u);
        auto j{fostlib::json::parse(response->body()->data())};
        return std::make_unique<f5::json::schema>(u, j);
    }

    const f5::json::schema_loader c_http{
            "http",
            [](f5::u8view url,
               f5::json::value config) -> std::unique_ptr<f5::json::schema> {
                auto logger{fostlib::log::debug(c_fost_json_schema_loader)};
                logger("requested-url", url);
                if (config.has_key("prefix")) {
                    const auto prefix =
                            fostlib::coerce<f5::u8view>(config["prefix"]);
                    logger("prefix", prefix);
                    if (url.starts_with(prefix)) {
                        if (config.has_key("base")) {
                            fostlib::url base{fostlib::coerce<f5::u8view>(
                                    config["base"])};
                            fostlib::url fetch{
                                    base, url.substr(prefix.code_points())};
                            logger("base",
                                   base)("fetching", fetch)("found", true);
                            try {
                                return http(base, fetch);
                            } catch (fostlib::exceptions::exception &e) {
                                fostlib::insert(e.data(), "schema", "url", url);
                                fostlib::insert(
                                        e.data(), "schema", "base", base);
                                fostlib::insert(
                                        e.data(), "schema", "fetching", fetch);
                                throw;
                            }
                        } else {
                            fostlib::url base{fostlib::coerce<f5::u8view>(
                                    config["prefix"])};
                            fostlib::url u{url};
                            logger("base", base)("fetching", u)("found", true);
                            return http(base, u);
                        }
                    } else {
                        logger("found", false);
                        logger("reason",
                               "URL doesn't start with supplied prefix");
                    }
                } else {
                    logger("found", false);
                    logger("prefix", fostlib::json{});
                    logger("reason", "No prefix given in configuration");
                }
                return {};
            }};


}
