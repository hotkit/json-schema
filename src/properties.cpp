/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>


const f5::json::assertion::checker f5::json::assertion::pattern_properties_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    if ( schema[spos].isobject() ) {
    } else {
        throw fostlib::exceptions::not_implemented(__func__, "pattern_properties_checker", part);
    }
    return validation::result{};
};


const f5::json::assertion::checker f5::json::assertion::properties_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    if ( part.isobject() ) {
        for ( const auto &p : part.object() ) {
            if ( data[dpos].has_key(p.first) ) {
                const auto v = validation::first_error(
                    schema, spos / rule / p.first, data, dpos / p.first);
                if ( not v ) return v;
            }
        }
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "properties check must be an object", part);
    }
    return validation::result{};
};


