/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#pragma once

#include <f5/json/validator.hpp>


namespace f5 {


    namespace json {


        namespace assertion {


            using checker = std::function<
                validation::result(
                    u8view rule, value part,
                    value schema, pointer spos,
                    value data, pointer dpos)>;


            extern const checker
                additional_properties_checker,
                always,
                not_checker,
                max_items_checker,
                min_items_checker,
                pattern_properties_checker,
                properties_checker,
                type_checker;


        }


    }


}

