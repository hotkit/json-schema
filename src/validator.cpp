/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>
#include <fost/push_back>
#include <fost/unicode>


namespace {


    const auto g_assertions =
        []() {
            std::map<f5::u8view, f5::json::assertion::checker> a;
            a["additionalProperties"] = f5::json::assertion::additional_properties_checker;
            a["allOf"] = f5::json::assertion::all_of_checker;
            a["anyOf"] = f5::json::assertion::any_of_checker;
            a["const"] = f5::json::assertion::const_checker;
            a["contains"] = f5::json::assertion::contains_checker;
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
            a["patternProperties"] = f5::json::assertion::pattern_properties_checker;
            a["properties"] = f5::json::assertion::properties_checker;
            a["propertyNames"] = f5::json::assertion::property_names_checker;
            a["required"] = f5::json::assertion::required_checker;
            a["type"] = f5::json::assertion::type_checker;
            a["uniqueItems"] = f5::json::assertion::unique_items_checker;
            return a;
        }();


}


auto f5::json::validation::first_error(
    value schema, pointer spos, value data, pointer dpos
) -> result {
    try {
        if ( schema[spos] == fostlib::json(true) ) {
            return result{};
        } else if ( schema[spos] == fostlib::json(false) ) {
            return result{"false", spos, dpos};
        } else if ( schema[spos].isobject() ) {
            if ( schema[spos].has_key("$ref") ) {
                return first_error(schema,
                    fostlib::jcursor::parse_json_pointer_fragment(
                        fostlib::coerce<f5::u8view>(schema[spos]["$ref"])),
                    data, dpos);
            } else {
                for ( const auto &rule : schema[spos].object() ) {
                    const auto apos = g_assertions.find(rule.first);
                    if ( apos != g_assertions.end() ) {
                        const auto v = apos->second(apos->first, rule.second, schema, spos, data, dpos);
                        if ( not v ) return v;
                    }
                }
            }
        } else {
            throw fostlib::exceptions::not_implemented(__func__,
                "A schema must be a boolean or an object", schema[spos]);
        }
        return result{};
    } catch ( fostlib::exceptions::exception &e ) {
        fostlib::json::array_t proc;
        fostlib::push_back(proc, fostlib::coerce<fostlib::json>(spos));
        fostlib::push_back(proc, fostlib::coerce<fostlib::json>(dpos));
        fostlib::push_back(e.data(), "first_error stack", proc);
        throw;
    }
}

