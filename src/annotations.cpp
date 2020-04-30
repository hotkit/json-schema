/**
    Copyright 2018-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */

#include <f5/json/assertions.hpp>
#include <f5/json/schema.cache.hpp>
#include <fost/push_back>
#include <fost/unicode>


/**
 * ## `f5::json::validation::annotations`
 */


namespace {
    void id_handling(
            f5::json::validation::annotations *anp,
            std::shared_ptr<f5::json::schema_cache> schemas) {
        if (anp->sroot[anp->spos].has_key("$id")) {
            if (not schemas) {
                schemas =
                        std::make_shared<f5::json::schema_cache>(anp->schemas);
                anp->schemas = schemas;
            }
            anp->base = &schemas->insert(
                    f5::json::schema{anp->base->self(), anp->sroot[anp->spos]});
        }
    }
    void definitions(
            f5::json::validation::annotations *anp,
            const fostlib::url &base,
            f5::json::pointer sub) {
        if (anp->sroot[anp->spos][sub].has_key("definitions")) {
            for (const auto &def :
                 anp->sroot[anp->spos][sub]["definitions"].object()) {
                fostlib::url r{anp->base->self(), anp->spos / sub};
                const auto &subschema = anp->schemas->insert(
                        fostlib::string(r.as_string()),
                        f5::json::schema{base, def.second});
                definitions(
                        anp, subschema.self(), sub / "definitions" / def.first);
            }
        }
    }
}


f5::json::validation::annotations::annotations(
        const json::schema &s, pointer sp, value d, pointer dp)
: base(&s),
  sroot(s.assertions()),
  spos(std::move(sp)),
  data(std::move(d)),
  dpos(std::move(dp)),
  schemas{std::make_shared<schema_cache>()} {
    id_handling(this, schemas);
    definitions(this, base->self(), pointer{});
}


f5::json::validation::annotations::annotations(
        annotations &an, const json::schema &s, pointer sp, value d, pointer dp)
: base(&s),
  sroot(s.assertions()),
  spos(std::move(sp)),
  data(std::move(d)),
  dpos(std::move(dp)),
  schemas{std::make_shared<schema_cache>(an.schemas)} {
    id_handling(this, schemas);
    definitions(this, base->self(), pointer{});
}


f5::json::validation::annotations::annotations(
        annotations &an, pointer sp, pointer dp)
: base{an.base},
  sroot(an.sroot),
  spos(std::move(sp)),
  data(an.data),
  dpos(std::move(dp)),
  schemas(an.schemas) {
    id_handling(this, nullptr);
}


f5::json::validation::annotations::annotations(annotations &&b, result &&w)
: base{b.base},
  sroot{std::move(b.sroot)},
  spos{std::move(b.spos)},
  data{std::move(b.data)},
  dpos{std::move(b.dpos)},
  schemas{b.schemas} {
    id_handling(this, nullptr);
    merge(std::move(w));
}


auto f5::json::validation::annotations::merge(result &&r) -> annotations & {
    struct v {
        void operator()(result::error &&) {
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__,
                    "Trying to merge an error with an annotation");
        }
        void operator()(annotations &&) {
            /// For now there are no actual annotations to merge in
        }
    };
    std::visit(v{}, std::move(r.outcome));
    return *this;
}


fostlib::url f5::json::validation::annotations::spos_url() const {
    fostlib::url u{base->self(), pointer{spos.begin(), spos.end()}};
    for (auto pos = spos.begin(), end = spos.end(); pos != end; ++pos) {
        pointer from_base{spos.begin(), pos}, to_tip{pos, end};
        if (sroot[from_base].has_key("$id")) {
            u = fostlib::url{
                    u, fostlib::coerce<u8view>(sroot[from_base]["$id"])};
            u = fostlib::url{u, pointer{pos, end}};
        }
    }
    return u;
}
