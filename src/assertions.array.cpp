/**
    Copyright 2018-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */

#include <f5/json/assertions.hpp>


const f5::json::assertion::checker f5::json::assertion::contains_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            const auto array = an.data[an.dpos];
            if (array.isarray()) {
                for (std::size_t index{}; index < array.size(); ++index) {
                    const auto valid = validation::first_error(
                            an, an.spos / rule, an.dpos / index);
                    if (valid) return validation::result{std::move(an)};
                }
                return validation::result{rule, an.spos / rule, an.dpos};
            }
            return validation::result{std::move(an)};
        };


const f5::json::assertion::checker f5::json::assertion::items_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            const auto array = an.data[an.dpos];
            if (array.isarray()) {
                if (part.isarray()) {
                    const auto psize = part.size(), dsize = array.size();
                    for (std::size_t index{}; index < std::min(psize, dsize);
                         ++index) {
                        auto valid = validation::first_error(
                                an, an.spos / rule / index, an.dpos / index);
                        if (not valid) return valid;
                        an.merge(std::move(valid));
                    }
                    if (an.sroot[an.spos].has_key("additionalItems")) {
                        for (std::size_t index{std::min(psize, dsize)};
                             index < dsize; ++index) {
                            auto valid = validation::first_error(
                                    an, an.spos / "additionalItems",
                                    an.dpos / index);
                            if (not valid) return valid;
                            an.merge(std::move(valid));
                        }
                    }
                } else {
                    for (std::size_t index{}; index < array.size(); ++index) {
                        auto valid = validation::first_error(
                                an, an.spos / rule, an.dpos / index);
                        if (not valid) return valid;
                        an.merge(std::move(valid));
                    }
                }
            }
            return validation::result{std::move(an)};
        };


const f5::json::assertion::checker f5::json::assertion::max_items_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            value array = an.data[an.dpos];
            if (array.isarray()) {
                const auto count = fostlib::coerce<int64_t>(part);
                if (array.size() > count) {
                    return validation::result{
                            rule, an.spos / rule, std::move(an.dpos)};
                }
            }
            return validation::result{std::move(an)};
        };


const f5::json::assertion::checker f5::json::assertion::min_items_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            value array = an.data[an.dpos];
            if (array.isarray()) {
                const auto count = fostlib::coerce<int64_t>(part);
                if (array.size() < count) {
                    return validation::result{
                            rule, an.spos / rule, std::move(an.dpos)};
                }
            }
            return validation::result{std::move(an)};
        };


const f5::json::assertion::checker f5::json::assertion::unique_items_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            value array = an.data[an.dpos];
            if (array.isarray()) {
                if (part == fostlib::json(true)) {
                    std::set<f5::json::value> found;
                    for (const auto item : array) {
                        if (found.find(item) != found.end()) {
                            return validation::result{
                                    rule, an.spos / rule, an.dpos};
                        }
                        found.insert(item);
                    }
                } else if (part == fostlib::json(false)) {
                    return validation::result{std::move(an)};
                } else {
                    throw fostlib::exceptions::not_implemented(
                            __func__, "unique items -- must be true or false",
                            part);
                }
            }
            return validation::result{std::move(an)};
        };
