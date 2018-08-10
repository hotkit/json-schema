/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/validator.hpp>
#include <fost/unicode>


namespace {


    struct typecheck {
        f5::u8view type;

        bool operator () (std::monostate) {
            return type == "null";
        }
        bool operator () (bool) {
            return type == "boolean";
        }
        bool operator () (double) {
            return type == "number";
        }
        bool operator () (int64_t) {
            return type == "integer" || type == "number";
        }
        bool operator () (fostlib::json::string_p) {
            return type == "string";
        }
        bool operator () (f5::lstring) {
            return type == "string";
        }
        bool operator () (fostlib::json::array_p) {
            return type == "array";
        }
        bool operator () (fostlib::json::object_p) {
            return type == "object";
        }
    };


    std::optional<f5::json::schema::error> validate(
        f5::json::value schema, f5::json::pointer spos,
        f5::json::value data, f5::json::pointer dpos
    ) {
        for ( const auto &rule : schema[spos].object() ) {
            if ( rule.first == "not" ) {
                if ( not validate(schema, spos / rule.first, data, dpos) ) {
                    return f5::json::schema::error{"not", spos, dpos};
                }
            } else if ( rule.first == "properties" ) {
                if ( rule.second.isobject() ) {
                    for ( const auto &p : rule.second.object() ) {
                        if ( data[dpos].has_key(p.first) ) {
                            const auto v = validate(schema, spos / rule.first / p.first, data, dpos / p.first);
                            if ( v ) return v;
                        }
                    }
                } else {
                    throw fostlib::exceptions::not_implemented(__func__,
                        "properties check must be an object", rule.second);
                }
            } else if ( rule.first == "type" ) {
                const auto str = fostlib::coerce<fostlib::nullable<f5::u8view>>(rule.second);
                if ( str ) {
                    if ( not data[dpos].apply_visitor(typecheck{str.value()}) ) {
                        return f5::json::schema::error{"type", spos, dpos};
                    }
                } else {
                    throw fostlib::exceptions::not_implemented(__func__, "type check", rule.second);
                }
            } else {
                /// The correct behaviour is to ignore unknown assertions, but for now
                /// we throw until we believe the implementation is complete enough
                throw fostlib::exceptions::not_implemented(__func__, "Assertion", rule.first);
            }
        }
        return std::optional<f5::json::schema::error>{};
    }


}


auto f5::json::schema::validate(value j) const -> std::optional<error> {
    return ::validate(validation, pointer{}, j, pointer{});
}

