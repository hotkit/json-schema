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
                enum_checker,
                exclusive_maximum_checker,
                exclusive_minimum_checker,
                maximum_checker,
                max_items_checker,
                max_length_checker,
                max_properties_checker,
                minimum_checker,
                min_items_checker,
                min_length_checker,
                min_properties_checker,
                not_checker,
                multiple_of_checker,
                pattern_properties_checker,
                properties_checker,
                required_checker,
                type_checker;


        }


    }


}

