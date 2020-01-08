/**
    Copyright 2018-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */

#include <f5/json/assertions.hpp>


const f5::json::assertion::checker f5::json::assertion::all_of_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            if (not part.isarray() || part.size() == 0) {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "allOf -- must be a non-empty array", part);
            }
            for (std::size_t index{}; index < part.size(); ++index) {
                auto valid = validation::first_error(
                        an, an.spos / rule / index, an.dpos);
                if (not valid) return valid;
                an.merge(std::move(valid));
            }
            return validation::result{std::move(an)};
        };


const f5::json::assertion::checker f5::json::assertion::any_of_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            if (not part.isarray() || part.size() == 0) {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "anyOf -- must be a non-empty array", part);
            }
            for (std::size_t index{}; index < part.size(); ++index) {
                const auto valid = validation::first_error(
                        an, an.spos / rule / index, an.dpos);
                if (valid) return validation::result{std::move(an)};
            }
            return validation::result{rule, an.spos, an.dpos};
        };


const f5::json::assertion::checker f5::json::assertion::always =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            return validation::result{std::move(an)};
        };


const f5::json::assertion::checker f5::json::assertion::const_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            if (an.data[an.dpos] == part) {
                return validation::result{std::move(an)};
            } else {
                return validation::result{rule, an.spos / rule,
                                          std::move(an.dpos)};
            }
        };


const f5::json::assertion::checker f5::json::assertion::enum_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            if (part.isarray()) {
                const auto value = an.data[an.dpos];
                for (const auto &opt : part) {
                    if (value == opt) {
                        return validation::result{std::move(an)};
                    }
                }
            } else {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__, "enum_checker not array", part);
            }
            return validation::result{rule, an.spos / rule, an.dpos};
        };


const f5::json::assertion::checker f5::json::assertion::if_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            auto passed = validation::first_error(an, an.spos / rule, an.dpos);
            const bool pflag{passed};
            if (pflag) { an.merge(std::move(passed)); }
            if (pflag && an.sroot[an.spos].has_key("then")) {
                auto valid =
                        validation::first_error(an, an.spos / "then", an.dpos);
                if (not valid) return valid;
                an.merge(std::move(valid));
            } else if (not pflag && an.sroot[an.spos].has_key("else")) {
                auto valid =
                        validation::first_error(an, an.spos / "else", an.dpos);
                if (not valid) return valid;
                an.merge(std::move(valid));
            }
            return validation::result{std::move(an)};
        };


const f5::json::assertion::checker f5::json::assertion::not_checker = [](f5::u8view
                                                                                 rule,
                                                                         f5::json::value
                                                                                 part,
                                                                         f5::json::validation::annotations
                                                                                 an) {
    if (validation::first_error(an, an.spos / rule, an.dpos)) {
        return validation::result{rule, an.spos / rule, std::move(an.dpos)};
    } else {
        return validation::result{std::move(an)};
    }
};


const f5::json::assertion::checker f5::json::assertion::one_of_checker =
        [](f5::u8view rule,
           f5::json::value part,
           f5::json::validation::annotations an) {
            if (not part.isarray() || part.size() == 0) {
                throw fostlib::exceptions::not_implemented(
                        __PRETTY_FUNCTION__,
                        "anyOf -- must be a non-empty array", part);
            }
            std::size_t count{};
            for (std::size_t index{}; index < part.size();
                 ++index && count < 2) {
                auto valid = validation::first_error(
                        an, an.spos / rule / index, an.dpos);
                if (valid) {
                    an.merge(std::move(valid));
                    ++count;
                }
            }
            if (count == 1) {
                return validation::result{std::move(an)};
            } else {
                return validation::result{rule, an.spos / rule, an.dpos};
            }
        };


const f5::json::assertion::checker f5::json::assertion::type_checker = [](f5::u8view
                                                                                  rule,
                                                                          f5::json::value
                                                                                  part,
                                                                          f5::json::validation::annotations
                                                                                  an) {
    struct typecheck {
        f5::u8view type;

        bool operator()(std::monostate) { return type == "null"; }
        bool operator()(bool) { return type == "boolean"; }
        bool operator()(double) { return type == "number"; }
        bool operator()(int64_t) {
            return type == "integer" || type == "number";
        }
        bool operator()(std::shared_ptr<fostlib::string>) {
            return type == "string";
        }
        bool operator()(f5::u8view) { return type == "string"; }
        bool operator()(fostlib::json::array_p) { return type == "array"; }
        bool operator()(fostlib::json::object_p) { return type == "object"; }
    };
    const auto str = fostlib::coerce<fostlib::nullable<f5::u8view>>(part);
    if (str) {
        if (not an.data[an.dpos].apply_visitor(typecheck{str.value()})) {
            return validation::result{rule, std::move(an.spos),
                                      std::move(an.dpos)};
        }
    } else if (part.isarray()) {
        for (const auto t : part) {
            const auto str = fostlib::coerce<f5::u8view>(t);
            if (an.data[an.dpos].apply_visitor(typecheck{str})) {
                return validation::result{std::move(an)};
            }
        }
        return validation::result{rule, std::move(an.spos), std::move(an.dpos)};
    } else {
        throw fostlib::exceptions::not_implemented(
                __PRETTY_FUNCTION__, "type check", part);
    }
    return validation::result{std::move(an)};
};
