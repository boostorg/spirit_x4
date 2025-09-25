/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "rule_separate_tu_grammar.hpp"

#include <concepts>
#include <utility>
#include <type_traits>

namespace {

namespace sem_act {

constexpr auto nop = [](auto const&) {};

constexpr x4::rule<class used_attr1_r, int> used_attr1 = "used_attr1";
constexpr auto used_attr1_def = used_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(used_attr1);

constexpr x4::rule<class used_attr2_r, int> used_attr2 = "used_attr2";
constexpr auto used_attr2_def = unused_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(used_attr2);

constexpr x4::rule<class unused_attr1_r> unused_attr1 = "unused_attr1";
constexpr auto unused_attr1_def = used_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(unused_attr1);

constexpr x4::rule<class unused_attr2_r> unused_attr2 = "unused_attr2";
constexpr auto unused_attr2_def = unused_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(unused_attr2);

} // sem_act

} // anonymous

TEST_CASE("rule_separate_tu")
{
    {
        CHECK(parse("*", unused_attr::skipper));
        CHECK(parse("#", unused_attr::skipper2));
        CHECK(parse("==", unused_attr::grammar));
        CHECK(parse("*=*=", unused_attr::grammar, unused_attr::skipper));
        CHECK(parse("#=#=", unused_attr::grammar, unused_attr::skipper2));
    }

    {
        long l = 0;
        static_assert(
            !std::same_as<decltype(l), used_attr::grammar_type::attribute_type>,
            "Ensure we have instantiated the rule with a different attribute type"
        );
        REQUIRE(parse("123", used_attr::grammar, l));
        CHECK(l == 123);
        REQUIRE(parse(" 42", used_attr::grammar, used_attr::skipper, l));
        CHECK(l == 42);
    }

    {
        long i = 0;
        CHECK(parse("123", sem_act::used_attr1, i));
    }
    {
        using Context = x4::context<
            x4::contexts::expectation_failure,
            x4::expectation_failure<std::string_view::const_iterator>,
            x4::context<
                x4::contexts::rule_val,
                int
            >
        >;
        using RuleAgnosticContext = x4::context<
            x4::contexts::expectation_failure,
            x4::expectation_failure<std::string_view::const_iterator>
        >;

        static_assert(std::same_as<
            std::remove_cvref_t<decltype(
                x4::remove_first_context<x4::contexts::rule_val>(std::declval<Context const&>())
            )>,
            RuleAgnosticContext
        >);

        long i = 0;
        CHECK(parse("===", sem_act::used_attr2, i));
    }
    {
        CHECK(parse("123", sem_act::unused_attr1));
    }
    {
        CHECK(parse("===", sem_act::unused_attr2));
    }
}
