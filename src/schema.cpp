/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.hpp>
#include <fost/unicode>


f5::json::schema::schema(const fostlib::url &b, value v)
: id{b, [v](){
        if ( v.has_key("$id") ) {
            return fostlib::coerce<fostlib::string>(v["$id"]);
        } else {
            return fostlib::guid();
        }
    }()},
    validation{v}
{
}


auto f5::json::schema::validate(value j) const -> validation::result {
    return validation::first_error(
        validation::annotations{*this, pointer{}, j, pointer{}});
}

