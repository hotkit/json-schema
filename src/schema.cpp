/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.hpp>
#include <fost/unicode>


auto f5::json::schema::validate(value j) const -> validation::result {
    return validation::first_error(validation, pointer{}, j, pointer{});
}

