/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.hpp>

#include <fost/file>
#include <fost/http>
#include <fost/main>
#include <fost/unicode>


namespace {
    const fostlib::setting<bool> c_verbose(__FILE__,
        "json-schema-testsuite", "Verbose", false, true);

    const fostlib::setting<fostlib::string> c_base(__FILE__,
        "json-schema-testsuite", "Base URL",
        "https://raw.githubusercontent.com/"
            "json-schema-org/JSON-Schema-Test-Suite/"
            "master/tests/draft7/", true);
}


FSL_MAIN(
    "json-schema-testsuite",
    "JSON Schema Test Suite Runner"
) (fostlib::ostream &out, fostlib::arguments &args) {
    args.commandSwitch("v", c_verbose);

    fostlib::stringstream buffer;
    fostlib::ostream &ss = c_verbose.value() ? out : buffer;
    int failed{};
    const auto base{c_base.value()};
    fostlib::http::user_agent ua;

    for ( const auto &arg : args ) {
        const auto response = ua.get(fostlib::url{base + arg});
        const auto tests = fostlib::json::parse(response->body()->data());
        for ( const auto test : tests ) {
            const auto description = fostlib::coerce<f5::u8view>(test["description"]);
            const f5::json::schema s{test["schema"]};
            for ( const auto example : test["tests"] ) {
                ss << description << ':'
                    << fostlib::coerce<f5::u8view>(example["description"]) << ':';
                const auto result = s.validate(example["data"]);
                const bool valid{result};
                if ( example["valid"] == fostlib::json(valid) ) {
                    ss << " Passed\n";
                } else {
                    ++failed;
                    ss << " FAILED\n";
                    if ( not result ) {
                        ss << "  " << result.outcome.value().assertion
                            << "\n";
                    }
                }
            }
        }
    }

    if ( failed && not c_verbose.value() )
        out << buffer.str();

    return std::min(failed, 255);
}

