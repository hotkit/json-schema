/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#pragma once

#include <fost/json>


namespace f5 {


    namespace json {


        using value = fostlib::json;
        using pointer = fostlib::jcursor;


        /**
            ## JSON Schema

            Stores a JSON schema and allows it to be used to validate other JSON
            instances.
            */
        class schema {
            value validation;

        public:
            schema(value v)
            : validation(std::move(v)) {
            }

            value assertions() const {
                return validation;
            }

            /// In the case of a validation error then this describes where it
            /// happened
            struct error {
                fostlib::string assertion;
                pointer spos, dpos;
            };

            /// If the schema doesn't validate return the first position
            /// in the schema that fails.
            std::optional<error> validate(value) const;
        };


    }


}

