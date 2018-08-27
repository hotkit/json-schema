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
            f5::json::validation::annotations an
        ) -> f5::json::validation::result {
            if ( const auto bound{part.get<int64_t>()}; bound ) {
                return an.data[an.dpos].apply_visitor(
                    [&](int64_t v) mutable {
                        return p(bound.value(), v) ? f5::json::validation::result{std::move(an)} : f5::json::validation::result{rule, an.spos, an.dpos};
                    },
                    [&](double v) mutable {
                        return p(bound.value(), v) ? f5::json::validation::result{std::move(an)} : f5::json::validation::result{rule, an.spos, an.dpos};
                    },
                    [&](const auto &) mutable {
                        return f5::json::validation::result{std::move(an)};
                    });
            } else if ( const auto bound{part.get<double>()}; bound ) {
                return an.data[an.dpos].apply_visitor(
                    [&](int64_t v) mutable {
                        return p(bound.value(), v) ? f5::json::validation::result{std::move(an)} : f5::json::validation::result{rule, an.spos, an.dpos};
                    },
                    [&](double v) mutable {
                        bool passed;
                        if ( std::abs(bound.value() - v) < epsilon ) {
                            passed = p(v, v);
                        } else {
                            passed = p(bound.value(), v);
                        }
                        return passed ? f5::json::validation::result{std::move(an)} : f5::json::validation::result{rule, an.spos, an.dpos};
                    },
                    [&](const auto &) mutable {
                        return f5::json::validation::result{std::move(an)};
                    });
            } else {
                throw fostlib::exceptions::not_implemented(__PRETTY_FUNCTION__,
                    name, part);
            }
            return f5::json::validation::result{std::move(an)};
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

