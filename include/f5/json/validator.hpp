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


        namespace validation {


            /// The outcome of validation looking for a single error
            class result {
            public:
                /// In the case of a validation error then this describes where it
                /// happened
                struct error {
                    f5::u8view assertion;
                    pointer spos, dpos;
                };
                const std::optional<error> outcome;

                /// Describe a successful result
                result() {}
                /// Describe a result that has an error
                result(u8view assertion, pointer spos, pointer dpos)
                : outcome{error{assertion, spos, dpos}} {
                }

                /// Return `true` if the result is that validation *passed*.
                /// When a value of `false` is returned there will be an
                /// error stored in the `outcome` field.
                explicit operator bool () const {
                    return not outcome.has_value();
                }
            };


            /// Validate part of an object against part of a schema. Typically
            /// you will want to call the `validate` member on a schema instance
            /// to perform validation.
            result first_error(value schema, pointer spos, value data, pointer dpos);


        }


    }


}

