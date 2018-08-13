/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>

#include <regex>


namespace {
    auto property_names(f5::json::value obj) {
        std::set<fostlib::string> names;
        for ( const auto &prop : obj.object() ) {
            names.insert(prop.first);
        }
        return names;
    }
    auto additional_properties(
        const std::set<fostlib::string> &remaining,
        f5::json::value schema, f5::json::pointer spos,
        f5::json::value data, f5::json::pointer dpos
    ) {
        for ( const auto &pname : remaining ) {
            const auto valid = f5::json::validation::first_error(
                schema, spos / "additionalProperties", data, dpos / pname);
            if ( not valid ) return valid;
        }
        return f5::json::validation::result{};
    }
    auto pattern_properties(
       std::set<fostlib::string> &remaining,
        f5::json::value schema, f5::json::pointer spos,
        f5::json::value data, f5::json::pointer dpos
    ) {
        const auto patterns = schema[spos]["patternProperties"];
        auto properties = data[dpos];
        for ( const auto &pattern : patterns.object() ) {
            std::regex re{pattern.first.std_str()};
            for ( const auto &property : properties.object() ) {
                if ( std::regex_search(property.first.std_str(), re) ) {
                    const auto v = f5::json::validation::first_error(
                        schema, spos / "patternProperties" / pattern.first, data, dpos / property.first);
                    if ( not v ) return v;
                    auto rit = remaining.find(property.first);
                    if ( rit != remaining.end() ) remaining.erase(rit);
                }
            }
        }
        return f5::json::validation::result{};
    }
}


const f5::json::assertion::checker f5::json::assertion::additional_properties_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    if ( schema[spos].has_key("properties") || schema[spos].has_key("patternProperties") ) {
        /// The schema has at least one of the above, so the processing
        /// of this assertion must happen after and as part of the
        /// processing of those.
        return validation::result{};
    } else {
        auto properties = data[dpos];
        if ( not properties.isobject() ) return validation::result{};
        for ( const auto &property : properties.object() ) {
            const auto valid = validation::first_error(
                schema, spos / rule, data, dpos / property.first);
            if ( not valid ) return valid;
        }
        return validation::result{};
    }
};


const f5::json::assertion::checker f5::json::assertion::pattern_properties_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    if ( schema[spos].has_key("properties") ) {
        /// The schema has a `properties` assertion, in which case this
        /// assertion will run after that as part of the properies checks.
        return validation::result{};
    } else if ( schema[spos].isobject() ) {
        auto properties = data[dpos];
        if ( not properties.isobject() ) return validation::result{};
        auto remaining{property_names(properties)};
        const auto valid = pattern_properties(remaining, schema, spos, data, dpos);
        if ( not valid ) return valid;

        if ( schema[spos].has_key("additionalProperties") ) {
            return additional_properties(remaining, schema, spos, data, dpos);
        }
    } else {
        throw fostlib::exceptions::not_implemented(__func__, "pattern_properties_checker -- not object", part);
    }
    return validation::result{};
};


const f5::json::assertion::checker f5::json::assertion::properties_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    if ( part.isobject() ) {
        auto properties = data[dpos];
        if ( not properties.isobject() ) return validation::result{};
        auto remaining{property_names(properties)};
        const auto rpos = spos / rule;
        for ( const auto &p : part.object() ) {
            if ( properties.has_key(p.first) ) {
                const auto v = validation::first_error(
                    schema, rpos / p.first, data, dpos / p.first);
                if ( not v ) return v;
                auto rit = remaining.find(p.first);
                if ( rit != remaining.end() ) remaining.erase(rit);
            }
        }
        if ( schema[spos].has_key("patternProperties") ) {
            const auto valid = pattern_properties(remaining, schema, spos, data, dpos);
            if ( not valid ) return valid;
        }
        if ( schema[spos].has_key("additionalProperties") ) {
            return additional_properties(remaining, schema, spos, data, dpos);
        }
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "properties check must be an object", part);
    }
    return validation::result{};
};


