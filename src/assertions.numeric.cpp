/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>


namespace {
    /**
        Due to small errors in floating point calculations, we can't directly
        compare them. The `double` type has around 53 bits of precision
        so this error term should allow us to just disregard the last few
        bits in the calculations when comparing numbers for equality.
    */
    const double epsilon = std::pow(2, -50);


    template<typename P>
    const auto bounds_checker(f5::lstring name, const P p) {
        return [=](
            f5::u8view rule, f5::json::value part,
            f5::json::value schema, f5::json::pointer spos,
            f5::json::value data, f5::json::pointer dpos
        ) -> f5::json::validation::result {
            if ( const auto max{part.get<int64_t>()}; max ) {
                return data[dpos].apply_visitor(
                    [&](int64_t v) {
                        return p(max.value(), v) ? f5::json::validation::result{} : f5::json::validation::result{rule, spos, dpos};
                    },
                    [&](double v) {
                        return p(max.value(), v) ? f5::json::validation::result{} : f5::json::validation::result{rule, spos, dpos};
                    },
                    [](const auto &) {
                        return f5::json::validation::result{};
                    });
            } else if ( const auto max{part.get<double>()}; max ) {
                return data[dpos].apply_visitor(
                    [&](int64_t v) {
                        return p(max.value(), v) ? f5::json::validation::result{} : f5::json::validation::result{rule, spos, dpos};
                    },
                    [&](double v) {
                        bool passed;
                        if ( std::abs(max.value() - v) < epsilon ) {
                            passed = p(v, v);
                        } else {
                            passed = p(max.value(), v);
                        }
                        return passed ? f5::json::validation::result{} : f5::json::validation::result{rule, spos, dpos};
                    },
                    [](const auto &) {
                        return f5::json::validation::result{};
                    });
            } else {
                throw fostlib::exceptions::not_implemented(__func__, name, part);
            }
            return f5::json::validation::result{};
        };
    };
}
const f5::json::assertion::checker f5::json::assertion::exclusive_maximum_checker =
    bounds_checker("exclusiveMaximum", [](auto m, auto v) {
        return v < m;
    });
const f5::json::assertion::checker f5::json::assertion::exclusive_minimum_checker =
    bounds_checker("exclusiveMinimum", [](auto m, auto v) {
        return v > m;
    });
const f5::json::assertion::checker f5::json::assertion::maximum_checker =
    bounds_checker("maximum", [](auto m, auto v) {
        return v <= m;
    });
const f5::json::assertion::checker f5::json::assertion::minimum_checker =
    bounds_checker("minimum", [](auto m, auto v) {
        return v >= m;
    });
const f5::json::assertion::checker f5::json::assertion::multiple_of_checker =
    bounds_checker("multipleOf", [](auto m, auto v) {
        return std::abs(std::remainder(v, m)) < epsilon;
    });

