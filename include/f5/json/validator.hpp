/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#pragma once

#include <fost/json>
#include <fost/url>


namespace f5 {


    namespace json {


        using value = fostlib::json;
        using pointer = fostlib::jcursor;


        class schema;
        class schema_cache;


        namespace validation {


            class result;


            /**
             * ## Annotations
             *
             * This structure stores the current schema and data that are
             * being validated, as well as the current position in the schema
             * and data.
             *
             * Later on it will also be used to store the annotations
             * themselves.
             */
            struct annotations {
                const schema *base;
                value sroot;
                pointer spos;
                value data;
                pointer dpos;

                std::shared_ptr<schema_cache> schemas;

              private:
                friend class json::schema;
                /// Construct the initial location
                annotations(
                        const json::schema &s, pointer sp, value d, pointer dp);

              public:
                /// Construct a later annotation, but allow more replacements
                annotations(
                        annotations &a,
                        const json::schema &s,
                        pointer sp,
                        value d,
                        pointer dp);
                /// Construct an annotations for another part of the validation
                annotations(annotations &, pointer sp, pointer dp);

                /// Construct by merging
                annotations(annotations &&base, result &&with);

                /// Merge a result with this annotation
                annotations &merge(result &&);

                /// Return the base URL for this part of the schema based
                /// on the local $id found in parent lexical scopes of the
                /// JSON
                fostlib::url spos_url() const;
            };


            /// The outcome of validation looking for a single error
            class result {
              public:
                /// In the case of a validation error then this describes where
                /// it happened
                struct error {
                    f5::u8view assertion;
                    pointer spos, dpos;
                };

              private:
                friend annotations;
                std::variant<error, annotations> outcome;

              public:
                /// Describe a result that has an error
                result(u8view assertion, pointer spos, pointer dpos)
                : outcome{error{assertion, std::move(spos), std::move(dpos)}} {}
                /// Return an annotation for merging into the base one
                result(annotations an) : outcome{std::move(an)} {}

                /// Return `true` if the result is that validation *passed*.
                /// When a value of `false` is returned there will be an
                /// error stored in the `outcome` field, otherwise the
                /// annotations can be retrieved.
                explicit operator bool() const;
                /// Return the error, or throw if there was no error
                explicit operator error() &&;
                /// Release the local annotations so they can be merged
                explicit operator annotations() && {
                    throw fostlib::exceptions::not_implemented(
                            __PRETTY_FUNCTION__);
                }
            };


            /// Perform the check
            result first_error(annotations);
            /// Recurse down into another level of the validation
            inline result
                    first_error(annotations &an, pointer spos, pointer dpos) {
                return first_error(
                        annotations{an, std::move(spos), std::move(dpos)});
            }


        }


    }


}
