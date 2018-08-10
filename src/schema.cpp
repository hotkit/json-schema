/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/validator.hpp>


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
            return false;
        }
    };


    bool validate(
        f5::json schema, f5::jpointer spos, f5::json data, f5::jpointer dpos
    ) {
        bool validates{true};
        for ( auto rule = schema[spos].begin(), e = schema[spos].end(); rule != e && validates; ++rule ) {
            if ( rule.key() == "not" ) {
                validates = not validate(*rule, spos, data, dpos);
            } else if ( rule.key() == "type" ) {
                const auto str = fostlib::coerce<fostlib::nullable<f5::u8view>>(*rule);
                if ( str ) {
                    return data.apply_visitor(typecheck{str.value()});
                }
            } else {
                /// The correct behaviour is to ignore unknown assertions, but for now
                /// we throw until we believe the implementation is complete enough
                throw fostlib::exceptions::not_implemented(__func__, "Assertion", rule.key());
            }
        }
        return validates;
    }


}


bool f5::js::json_schema::schema::validate(json j) const {
    return ::validate(validation, jpointer{}, j, jpointer{});
}

