/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.hpp>

#include <fost/file>
#include <fost/main>
#include <fost/unicode>


namespace {
    const fostlib::setting<bool> c_verbose(__FILE__,
        "json-schema-validator", "Verbose", false, true);
    const fostlib::setting<bool> c_check_invalid(__FILE__,
        "json-schema-validator", "Success when invalid", false, true);

    const fostlib::setting<fostlib::string> c_schema(__FILE__,
        "json-schema-validator", "Schema file name", "any.schema.json", true);

    auto load_json(fostlib::string fn) {
        return f5::json::value::parse(
            fostlib::utf::load_file(fostlib::coerce<boost::filesystem::path>(fn)));
    }

    void print(f5::json::schema s, f5::json::value d, f5::json::validation::result::error e) {
        std::cout << "Assertion: " << e.assertion <<
            "\nSchema position: " << e.spos <<
            "\nData position: " << e.dpos <<
            "\nSchema: " << s.assertions()[e.spos] <<
            "\nData: " << d[e.dpos] << std::endl;
    }
}


FSL_MAIN(
    "json-schema-validator",
    "JSON Schema Validator"
) (fostlib::ostream &out, fostlib::arguments &args) {
    args.commandSwitch("i", c_check_invalid);
    args.commandSwitch("v", c_verbose);
    args.commandSwitch("-schema", c_schema);

    const f5::json::schema s{fostlib::url{}, load_json(c_schema.value())};

    for ( const auto &arg : args ) {
        if ( c_verbose.value() ) {
            std::cout << "Loading and validating " << arg << std::endl;
        }
        const auto j = load_json(arg);
        if ( c_check_invalid.value() ) {
            if ( const auto v = s.validate(j); v ) {
                std::cout << arg << " validated when it should not have" << std::endl;
                return 2;
            }
        } else {
            if ( auto v = s.validate(j); not v ) {
                std::cout << arg << " did not validate" << std::endl;
                print(s, j, (f5::json::validation::result::error)std::move(v));
                return 1;
            }
        }
    }

    return 0;
}

