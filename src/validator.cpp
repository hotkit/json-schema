/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>
#include <f5/json/schema.cache.hpp>
#include <fost/push_back>
#include <fost/unicode>


namespace {


    const auto g_assertions = []() {
        std::map<f5::u8view, f5::json::assertion::checker> a;
        a["additionalProperties"] =
                f5::json::assertion::additional_properties_checker;
        a["allOf"] = f5::json::assertion::all_of_checker;
        a["anyOf"] = f5::json::assertion::any_of_checker;
        a["const"] = f5::json::assertion::const_checker;
        a["contains"] = f5::json::assertion::contains_checker;
        a["dependencies"] = f5::json::assertion::dependencies_checker;
        a["enum"] = f5::json::assertion::enum_checker;
        a["exclusiveMaximum"] = f5::json::assertion::exclusive_maximum_checker;
        a["exclusiveMinimum"] = f5::json::assertion::exclusive_minimum_checker;
        a["if"] = f5::json::assertion::if_checker;
        a["items"] = f5::json::assertion::items_checker;
        a["maximum"] = f5::json::assertion::maximum_checker;
        a["maxItems"] = f5::json::assertion::max_items_checker;
        a["maxLength"] = f5::json::assertion::max_length_checker;
        a["maxProperties"] = f5::json::assertion::max_properties_checker;
        a["minimum"] = f5::json::assertion::minimum_checker;
        a["minItems"] = f5::json::assertion::min_items_checker;
        a["minLength"] = f5::json::assertion::min_length_checker;
        a["minProperties"] = f5::json::assertion::min_properties_checker;
        a["multipleOf"] = f5::json::assertion::multiple_of_checker;
        a["not"] = f5::json::assertion::not_checker;
        a["oneOf"] = f5::json::assertion::one_of_checker;
        a["pattern"] = f5::json::assertion::pattern_checker;
        a["patternProperties"] =
                f5::json::assertion::pattern_properties_checker;
        a["properties"] = f5::json::assertion::properties_checker;
        a["propertyNames"] = f5::json::assertion::property_names_checker;
        a["required"] = f5::json::assertion::required_checker;
        a["type"] = f5::json::assertion::type_checker;
        a["uniqueItems"] = f5::json::assertion::unique_items_checker;
        return a;
    }();


}


/**
 * ## `f5::json::validation::result`
 */


f5::json::validation::result::operator bool() const {
    struct v {
        bool operator()(const error &) { return false; };
        bool operator()(const annotations &) { return true; };
    };
    return std::visit(v{}, outcome);
}


f5::json::validation::result::operator error() && {
    struct v {
        error operator()(error &&e) { return std::move(e); };
        error operator()(annotations &&) {
            throw fostlib::exceptions::not_implemented{__PRETTY_FUNCTION__};
        };
    };
    return std::visit(v{}, std::move(outcome));
}


/**
 * ## `f5::json::validation::first_error`
 */


auto f5::json::validation::first_error(annotations an) -> result {
    try {
        if (an.sroot[an.spos] == fostlib::json(true)) {
            return result{std::move(an)};
        } else if (an.sroot[an.spos] == fostlib::json(false)) {
            return result{"false", std::move(an.spos), std::move(an.dpos)};
        } else if (auto part = an.sroot[an.spos]; part.isobject()) {
            if (part.has_key("$ref")) {
                auto const ref = fostlib::coerce<f5::u8view>(part["$ref"]);
                if (ref.starts_with("#/") || ref == "#") {
                    auto valid = first_error(
                            an,
                            fostlib::jcursor::parse_json_pointer_fragment(ref),
                            an.dpos);
                    if (not valid)
                        return valid;
                    else
                        return annotations(std::move(an), std::move(valid));
                } else {
                    fostlib::string url{ref};
                    if (ref.starts_with("#")) {
                        url = fostlib::coerce<fostlib::string>(
                                fostlib::url{an.spos_url(), ref});
                    }
                    const auto &cache = *an.schemas;
                    if (const auto frag =
                                std::find(url.begin(), url.end(), '#');
                        frag == url.end()) {
                        auto const &[ref_schema, location] =
                                cache[fostlib::url{an.spos_url(), url}];
                        auto valid = first_error(annotations{
                                an, ref_schema, pointer{}, an.data, an.dpos});
                        if (not valid) return valid;
                        return annotations{std::move(an), std::move(valid)};
                    } else {
                        const f5::u8view us{url.begin(), frag};
                        auto const &[ref_schema, location] =
                                cache[fostlib::url{an.spos_url(), us}];
                        auto valid = first_error(annotations{
                                an, ref_schema,
                                fostlib::jcursor::parse_json_pointer_fragment(
                                        f5::u8view{frag, url.end()}),
                                an.data, an.dpos});
                        if (not valid) return valid;
                        return annotations(std::move(an), std::move(valid));
                    }
                }
            } else {
                for (const auto &rule : part.object()) {
                    const auto apos = g_assertions.find(rule.first);
                    if (apos != g_assertions.end()) {
                        auto v = apos->second(apos->first, rule.second, an);
                        if (not v) return v;
                        an.merge(std::move(v));
                    }
                }
            }
        } else {
            throw fostlib::exceptions::not_implemented(
                    __func__, "A schema must be a boolean or an object", part);
        }
        return result{std::move(an)};
    } catch (fostlib::exceptions::exception &e) {
        fostlib::json::object_t proc;
        proc["base"] = fostlib::coerce<fostlib::json>(an.base->self());
        proc["spos"] = fostlib::coerce<fostlib::json>(an.spos);
        proc["dpos"] = fostlib::coerce<fostlib::json>(an.dpos);
        fostlib::push_back(e.data(), "first_error stack", proc);
        throw;
    }
}
