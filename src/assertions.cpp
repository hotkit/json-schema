/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>


const f5::json::assertion::checker f5::json::assertion::always = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    return validation::result{};
};


const f5::json::assertion::checker f5::json::assertion::enum_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    if ( part.isarray() ) {
        const auto value = data[dpos];
        for ( const auto &opt : part ) {
            if ( value == opt ) {
                return validation::result{};
            }
        }
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "enum_checker not array", part);
    }
    return validation::result{rule, spos, dpos};
};


const f5::json::assertion::checker f5::json::assertion::not_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    if ( validation::first_error(schema, spos / rule, data, dpos) ) {
        return validation::result{rule, spos, dpos};
    } else {
        return validation::result{};
    }
};


const f5::json::assertion::checker f5::json::assertion::type_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
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
    const auto str = fostlib::coerce<fostlib::nullable<f5::u8view>>(part);
    if ( str ) {
        if ( not data[dpos].apply_visitor(typecheck{str.value()}) ) {
            return validation::result{rule, spos, dpos};
        }
    } else if ( part.isarray() ) {
        for ( const auto t : part ) {
            const auto str = fostlib::coerce<f5::u8view>(t);
            if ( data[dpos].apply_visitor(typecheck{str}) ) {
                return validation::result{};
            }
        }
        return validation::result{rule, spos, dpos};
    } else {
        throw fostlib::exceptions::not_implemented(__func__, "type check", part);
    }
    return validation::result{};
};

