/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/plus.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <boost/variant.hpp>

#include <string>
#include <vector>
#include <cstring>

#ifdef _MSC_VER
// bogus https://developercommunity.visualstudio.com/t/buggy-warning-c4709/471956
# pragma warning(disable: 4709) // comma operator within array index expression
#endif

struct f
{
    template<class Context>
    void operator()(Context const& ctx) const
    {
        x4::_val(ctx) += x4::_attr(ctx);
    }
};

namespace check_stationary {

x4::rule<class a_r, spirit_test::stationary> const a;
x4::rule<class b_r, spirit_test::stationary> const b;

auto const a_def = '{' >> x4::int_ >> '}';
auto const b_def = a;

BOOST_SPIRIT_X4_DEFINE(a)
BOOST_SPIRIT_X4_DEFINE(b)

} // check_stationary

namespace check_recursive {

using node_t = boost::make_recursive_variant<
   int,
   std::vector<boost::recursive_variant_>
>::type;

x4::rule<class recursive_grammar_r, node_t> const grammar;

auto const grammar_def = '[' >> grammar % ',' >> ']' | x4::int_;

BOOST_SPIRIT_X4_DEFINE(grammar)

} // check_recursive

namespace check_recursive_scoped {

using check_recursive::node_t;

x4::rule<class intvec_r, node_t> const intvec;
auto const grammar = intvec = '[' >> intvec % ',' >> ']' | x4::int_;

} // check_recursive_scoped

struct recursive_tuple
{
    int value;
    std::vector<recursive_tuple> children;
};

BOOST_FUSION_ADAPT_STRUCT(recursive_tuple, value, children)

// regression test for #461
namespace check_recursive_tuple {

using iterator_type = std::string_view::const_iterator;

x4::rule<class recursive_tuple_grammar_r, recursive_tuple> const grammar;
auto const grammar_def = x4::int_ >> ('{' >> grammar % ',' >> '}' | x4::eps);
BOOST_SPIRIT_X4_DEFINE(grammar)

BOOST_SPIRIT_X4_INSTANTIATE(decltype(grammar), iterator_type, x4::parse_context_for<iterator_type>)

} // check_recursive_tuple

TEST_CASE("rule3")
{
    using namespace x4::standard;
    using x4::rule;
    using x4::lit;
    using x4::eps;
    using x4::_val;
    using x4::_attr;

    // synth attribute value-init
    {
        std::string s;
        using rule_type = rule<class r, std::string>;

        auto rdef = rule_type{} = alpha [f()];

        REQUIRE(parse("abcdef", +rdef, s));
        CHECK(s == "abcdef");
    }

    // synth attribute value-init
    {
        std::string s;
        using rule_type = rule<class r, std::string>;

        auto rdef = rule_type() =
            alpha[([](auto& ctx) {
                _val(ctx) += _attr(ctx);
            })];

        REQUIRE(parse("abcdef", +rdef, s));
        CHECK(s == "abcdef");
    }

    {
        auto r = rule<class r_id, int>{} = eps[([] ([[maybe_unused]] auto& ctx) {
            static_assert(
                std::is_same_v<std::decay_t<decltype(_val(ctx))>, unused_type>,
                "Attr must not be synthesized"
            );
        })];
        CHECK(parse("", r));
    }

    // ensure no unneeded synthesization, copying and moving occurred
    {
        spirit_test::stationary st { 0 };
        REQUIRE(parse("{42}", check_stationary::b, st));
        CHECK(st.val == 42);
    }

    {
        using namespace check_recursive;
        node_t v;
        REQUIRE(parse("[4,2]", grammar, v));
        CHECK((node_t{std::vector<node_t>{{4}, {2}}} == v));
    }
    {
        using namespace check_recursive_scoped;
        node_t v;
        REQUIRE(parse("[4,2]", grammar, v));
        CHECK((node_t{std::vector<node_t>{{4}, {2}}} == v));
    }
}
