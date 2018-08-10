/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#pragma once

#include <fost/json>


namespace f5 {


    inline namespace js {


        using json = fostlib::json;
        using jpointer = fostlib::jcursor;


        namespace json_schema {


            struct validation_error {
                fostlib::string assertion;
                jpointer spos, dpos;
            };


            /**
                ## JSON Schema

                Stores a JSON schema and allows it to be used to validate other JSON
                instances.
             */
            class schema {
                json validation;

            public:
                schema(json v)
                : validation(std::move(v)) {
                }

                json assertions() const {
                    return validation;
                }

                /// If the schema doesn't validate return the first position
                /// in the schema that fails.
                std::optional<validation_error> validate(json) const;
            };


        }


    }


}

