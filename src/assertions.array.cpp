/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>


const f5::json::assertion::checker f5::json::assertion::items_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    const auto array = data[dpos];
    if( array.isarray() ) {
        if ( part.isarray() ) {
            const auto psize = part.size(), dsize = data[dpos].size();
            for ( std::size_t index{}; index < std::min(psize, dsize); ++index ) {
                const auto valid = validation::first_error(
                    schema, spos / rule / index, data, dpos / index);
                if ( not valid ) return valid;
            }
        } else {
            throw fostlib::exceptions::not_implemented(__func__,
                "items checker -- not array", part);
        }
    }
    return validation::result{};
};


const f5::json::assertion::checker f5::json::assertion::max_items_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    value array = data[dpos];
    if ( array.isarray() ) {
        const auto count = fostlib::coerce<int64_t>(part);
        if ( data[dpos].size() > count ) {
            return validation::result{rule, spos, dpos};
        }
    }
    return validation::result{};
};


const f5::json::assertion::checker f5::json::assertion::min_items_checker = [](
    f5::u8view rule, f5::json::value part,
    f5::json::value schema, f5::json::pointer spos,
    f5::json::value data, f5::json::pointer dpos
) {
    value array = data[dpos];
    if ( array.isarray() ) {
        const auto count = fostlib::coerce<int64_t>(part);
        if ( data[dpos].size() < count ) {
            return validation::result{rule, spos, dpos};
        }
    }
    return validation::result{};
};


