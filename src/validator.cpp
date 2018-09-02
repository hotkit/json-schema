/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/assertions.hpp>
#include <f5/json/schema.cache.hpp>
#include <fost/push_back>
#include <fost/unicode>


namespace {


    const auto g_assertions =
        []() {
            std::map<f5::u8view, f5::json::assertion::checker> a;
            a["additionalProperties"] = f5::json::assertion::additional_properties_checker;
            a["allOf"] = f5::json::assertion::all_of_checker;
            a["anyOf"] = f5::json::assertion::any_of_checker;
            a["const"] = f5::json::assertion::const_checker;
            a["contains"] = f5::json::assertion::contains_checker;
            a["dependencies"] = f5::json::assertion::dependencies_checker;
            a["enum"] = f5::json::assertion::enum_checker;
            a["exclusiveMaximum"] = f5::json::assertion::exclusive_maximum_checker;
            a["exclusiveMinimum"] = f5::json::assertion::exclusive_minimum_checker;
            a["if"] = f5::json::assertion::if_checker;
            a["items"] = f5::json::assertion::items_checker;
            a["maximum"] = f5::json::assertion::maximum_checker;
            a["maxItems"] = f5::json::assertion::max_items_checker;
            a["maxLength"] = f5::json::assertion::max_length_checker;
            a["maxProperties"] = f5::json::assertion::max_properties_checker;
            a["minimum"] = f5::json::assertion::minimum_checker;
            a["minItems"] = f5::json::assertion::min_items_checker;
            a["minLength"] = f5::json::assertion::min_length_checker;
            a["minProperties"] = f5::json::assertion::min_properties_checker;
            a["multipleOf"] = f5::json::assertion::multiple_of_checker;
            a["not"] = f5::json::assertion::not_checker;
            a["oneOf"] = f5::json::assertion::one_of_checker;
            a["pattern"] = f5::json::assertion::pattern_checker;
            a["patternProperties"] = f5::json::assertion::pattern_properties_checker;
            a["properties"] = f5::json::assertion::properties_checker;
            a["propertyNames"] = f5::json::assertion::property_names_checker;
            a["required"] = f5::json::assertion::required_checker;
            a["type"] = f5::json::assertion::type_checker;
            a["uniqueItems"] = f5::json::assertion::unique_items_checker;
            return a;
        }();


}


/**
 * ## `f5::json::validation::annotations`
 */


namespace {
    void id_handling(
        f5::json::validation::annotations *anp,
        std::shared_ptr<f5::json::schema_cache> schemas
    ) {
        if ( anp->sroot[anp->spos].has_key("$id") ) {
            if ( not schemas ) {
                schemas = std::make_shared<f5::json::schema_cache>(anp->schemas);
                anp->schemas = schemas;
            }
            anp->base = &schemas->insert(f5::json::schema{
                anp->base->self(), anp->sroot[anp->spos]});
        }
    }
    void definitions(
        f5::json::validation::annotations *anp,
        const fostlib::url &base,
        f5::json::pointer sub
    ) {
        if ( anp->sroot[anp->spos][sub].has_key("definitions") ) {
            for ( const auto &def : anp->sroot[anp->spos][sub]["definitions"].object() ) {
                fostlib::url r{anp->base->self(), anp->spos / sub};
                const auto &subschema = anp->schemas->insert(
                    fostlib::string(r.as_string()), f5::json::schema{base, def.second});
                definitions(anp, subschema.self(), sub / "definitions" / def.first);
            }
        }
    }
}


f5::json::validation::annotations::annotations(
    const json::schema &s, pointer sp, value d, pointer dp
) : base(&s),
    sroot(s.assertions()), spos(std::move(sp)),
    data(std::move(d)), dpos(std::move(dp)),
    schemas{std::make_shared<schema_cache>()}
{
    id_handling(this, schemas);
    definitions(this, base->self(), pointer{});
}


f5::json::validation::annotations::annotations(
    annotations &an, const json::schema &s, pointer sp, value d, pointer dp
) : base(&s),
    sroot(s.assertions()), spos(std::move(sp)),
    data(std::move(d)), dpos(std::move(dp)),
    schemas{std::make_shared<schema_cache>(an.schemas)}
{
    id_handling(this, schemas);
    definitions(this, base->self(), pointer{});
}


f5::json::validation::annotations::annotations(annotations &an, pointer sp, pointer dp)
: base{an.base},
    sroot(an.sroot), spos(std::move(sp)),
    data(an.data), dpos(std::move(dp)),
    schemas(an.schemas)
{
    id_handling(this, nullptr);
}


f5::json::validation::annotations::annotations(annotations &&b, result &&w)
: base{b.base},
    sroot{std::move(b.sroot)}, spos{std::move(b.spos)},
    data{std::move(b.data)}, dpos{std::move(b.dpos)},
    schemas{b.schemas}
{
    id_handling(this, nullptr);
    merge(std::move(w));
}


auto f5::json::validation::annotations::merge(result &&r) -> annotations & {
    struct v {
        void operator () (result::error &&) {
            throw fostlib::exceptions::not_implemented(__PRETTY_FUNCTION__,
                "Trying to merge an error with an annotation");
        }
        void operator () (annotations &&) {
            /// For now there are no actual annotations to merge in
        }
    };
    std::visit(v{}, std::move(r.outcome));
    return *this;
}


fostlib::url f5::json::validation::annotations::spos_url() const {
    fostlib::url u{base->self(), pointer{spos.begin(), spos.end()}};
    for ( auto pos = spos.begin(), end = spos.end(); pos != end; ++pos ) {
        pointer from_base{spos.begin(), pos}, to_tip{pos, end};
        if ( sroot[from_base].has_key("$id") ) {
            u = fostlib::url{u, fostlib::coerce<u8view>(sroot[from_base]["$id"])};
            u = fostlib::url{u, pointer{pos, end}};
        }
    }
    return u;
}


/**
 * ## `f5::json::validation::result`
 */


f5::json::validation::result::operator bool () const {
    struct v {
        bool operator () (const error&) { return false; };
        bool operator () (const annotations&) { return true; };
    };
    return std::visit(v{}, outcome);
}


f5::json::validation::result::operator error () && {
    struct v {
        error operator () (error&&e) { return std::move(e); };
        error operator () (annotations&&) {
            throw fostlib::exceptions::not_implemented{__PRETTY_FUNCTION__};
        };
    };
    return std::visit(v{}, std::move(outcome));
}


/**
 * ## `f5::json::validation::first_error`
 */


auto f5::json::validation::first_error(annotations an) -> result {
    try {
        if ( an.sroot[an.spos] == fostlib::json(true) ) {
            return result{std::move(an)};
        } else if ( an.sroot[an.spos] == fostlib::json(false) ) {
            return result{"false", std::move(an.spos), std::move(an.dpos)};
        } else if ( auto part = an.sroot[an.spos]; part.isobject() ) {
            if ( part.has_key("$ref") ) {
                const auto ref = fostlib::coerce<f5::u8view>(part["$ref"]);
                if ( ref.bytes() && *ref.begin() == '#' ) {
                    auto valid = first_error(an,
                        fostlib::jcursor::parse_json_pointer_fragment(
                            fostlib::coerce<f5::u8view>(part["$ref"])),
                        an.dpos);
                    if ( not valid ) return valid;
                    else return annotations(std::move(an), std::move(valid));
                } else {
                    const auto &cache = *an.schemas;
                    if ( const auto frag = std::find(ref.begin(), ref.end(), '#'); frag == ref.end() ) {
                        const fostlib::url u{an.spos_url(), ref};
                        const auto &ref_schema = cache[u.as_string()];
                        auto valid = first_error(
                            annotations{an, ref_schema, pointer{}, an.data, an.dpos});
                        if ( not valid ) return valid;
                        return annotations{std::move(an), std::move(valid)};
                    } else {
                        const f5::u8view us{ref.begin(), frag};
                        const fostlib::url u{an.spos_url(), us};
                        std::cout << "URL: " << u << std::endl;
                        const auto &ref_schema = cache[u.as_string()];
                        auto valid = first_error(
                            annotations{an, ref_schema,
                                fostlib::jcursor::parse_json_pointer_fragment(
                                    f5::u8view{frag, ref.end()}),
                                an.data, an.dpos});
                        if ( not valid ) return valid;
                        return annotations(std::move(an), std::move(valid));
                    }
                }
            } else {
                for ( const auto &rule : part.object() ) {
                    const auto apos = g_assertions.find(rule.first);
                    if ( apos != g_assertions.end() ) {
                        auto v = apos->second(apos->first, rule.second, an);
                        if ( not v ) return v;
                        an.merge(std::move(v));
                    }
                }
            }
        } else {
            throw fostlib::exceptions::not_implemented(__func__,
                "A schema must be a boolean or an object", part);
        }
        return result{std::move(an)};
    } catch ( fostlib::exceptions::exception &e ) {
        fostlib::json::object_t proc;
        proc["base"] = fostlib::coerce<fostlib::json>(an.base->self());
        proc["spos"] = fostlib::coerce<fostlib::json>(an.spos);
        proc["dpos"] = fostlib::coerce<fostlib::json>(an.dpos);
        fostlib::push_back(e.data(), "first_error stack", proc);
        throw;
    }
}

