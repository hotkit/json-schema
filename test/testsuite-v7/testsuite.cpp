/**
    Copyright 2018-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */

#include <f5/json/schema.cache.hpp>
#include <f5/json/schema.loaders.hpp>

#include <fost/file>
#include <fost/http>
#include <fost/main>
#include <fost/unicode>


namespace {
    const fostlib::setting<bool> c_verbose(
            __FILE__, "json-schema-testsuite", "Verbose", false, true);

    const fostlib::setting<fostlib::string> c_base(
            __FILE__,
            "json-schema-testsuite",
            "Base URL",
            "https://raw.githubusercontent.com/json-schema-org/"
            "JSON-Schema-Test-Suite/e64ebf90a001f4e0e18984d2086ea15765cfead2/"
            "tests/draft7/",
            //"https://raw.githubusercontent.com/"
            //"json-schema-org/JSON-Schema-Test-Suite/"
            //"master/tests/draft7/",
            true);

    const fostlib::setting<fostlib::json> c_loaders{
            __FILE__, f5::json::c_schema_loaders, []() {
                f5::json::value::array_t loaders;
                loaders.push_back([]() {
                    f5::json::value::object_t github;
                    github["loader"] = "http";
                    github["prefix"] = "http://localhost:1234/";
                    github["base"] =
                            "https://raw.githubusercontent.com/"
                            "json-schema-org/JSON-Schema-Test-Suite/master/"
                            "remotes/";
                    return github;
                }());
                return loaders;
            }()};


}


FSL_MAIN("json-schema-testsuite", "JSON Schema Test Suite Runner")
(fostlib::ostream &out, fostlib::arguments &args) {
    args.commandSwitch("v", c_verbose);
    args.commandSwitch("p", f5::json::c_schema_path);

    fostlib::stringstream buffer;
    fostlib::ostream &ss = c_verbose.value() ? out : buffer;
    int failed{};
    const fostlib::url base{c_base.value()};
    fostlib::http::user_agent ua;

    try {
        for (const auto &arg : args) {
            const fostlib::url loc{base, arg};
            const auto response = ua.get(loc);
            const auto tests = fostlib::json::parse(response->body()->data());
            for (const auto test : tests) {
                const auto description =
                        fostlib::coerce<f5::u8view>(test["description"]);
                const f5::json::schema s{loc, test["schema"]};
                for (const auto example : test["tests"]) {
                    ss << description << ':'
                       << fostlib::coerce<f5::u8view>(example["description"])
                       << ':';
                    auto result = s.validate(example["data"]);
                    const bool valid{result};
                    if (example["valid"] == fostlib::json(valid)) {
                        ss << " Passed\n";
                    } else {
                        ++failed;
                        ss << " FAILED\n";
                        if (not result) {
                            auto e{(f5::json::validation::result::error)
                                           std::move(result)};
                            ss << "  " << e.assertion << "\n";
                        }
                    }
                }
            }
        }

        if (failed && not c_verbose.value()) out << buffer.str();

        return std::min(failed, 255);
    } catch (...) {
        out << buffer.str();
        throw;
    }
}
