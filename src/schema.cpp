/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/validator.hpp>


namespace {


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
                    if ( str == "null" && data.isnull() ) return true;
                    else if ( str == "object" && data.isobject() ) return true;
                    else return false;
                }
            } else {
                throw fostlib::exceptions::not_implemented(__func__, "Assertion", rule.key());
            }
        }
        return validates;
    }


}


bool f5::js::json_schema::schema::validate(json j) const {
    return ::validate(validation, jpointer{}, j, jpointer{});
}

