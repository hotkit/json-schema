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
        bool operator () (fostlib::json::object_p) {
            return type == "object";
        }

        template<typename T>
        bool operator () (const T &) {
            throw fostlib::exceptions::not_implemented(
                __func__, "Type check", typeid(T).name());
        }
    };


    std::optional<f5::json_schema::validation_error> validate(
        f5::json schema, f5::jpointer spos, f5::json data, f5::jpointer dpos
    ) {
        for ( const auto &rule : schema[spos].object() ) {
            if ( rule.first == "not" ) {
                if ( not validate(schema, spos / rule.first, data, dpos) ) {
                    return f5::json_schema::validation_error{"not", spos, dpos};
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
                        return f5::json_schema::validation_error{"type", spos, dpos};
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
        return std::optional<f5::json_schema::validation_error>{};
    }


}


auto f5::js::json_schema::schema::validate(json j) const -> std::optional<validation_error> {
    return ::validate(validation, jpointer{}, j, jpointer{});
}

