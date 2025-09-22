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

namespace x4 = boost::spirit::x4;

constexpr auto nop = [](auto const&) {};

x4::rule<class used_attr1_r, int> used_attr1;
auto const used_attr1_def = used_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(used_attr1);

x4::rule<class used_attr2_r, int> used_attr2;
auto const used_attr2_def = unused_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(used_attr2);

x4::rule<class unused_attr1_r> unused_attr1;
auto const unused_attr1_def = used_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(unused_attr1);

x4::rule<class unused_attr2_r> unused_attr2;
auto const unused_attr2_def = unused_attr::grammar[nop];
BOOST_SPIRIT_X4_DEFINE(unused_attr2);

} // sem_act

} // anonymous

int main()
{
    {
        BOOST_TEST(parse("*", unused_attr::skipper));
        BOOST_TEST(parse("#", unused_attr::skipper2));
        BOOST_TEST(parse("==", unused_attr::grammar));
        BOOST_TEST(parse("*=*=", unused_attr::grammar, unused_attr::skipper));
        BOOST_TEST(parse("#=#=", unused_attr::grammar, unused_attr::skipper2));
    }

    {
        long i = 0;
        static_assert(
            !std::same_as<decltype(i), used_attr::grammar_type::attribute_type>,
            "ensure we have instantiated the rule with a different attribute type"
        );
        BOOST_TEST(parse("123", used_attr::grammar, i));
        BOOST_TEST_EQ(i, 123);
        BOOST_TEST(parse(" 42", used_attr::grammar, used_attr::skipper, i));
        BOOST_TEST_EQ(i, 42);
    }

    {
        long i = 0;
        BOOST_TEST(parse("123", sem_act::used_attr1, i));
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
        BOOST_TEST(parse("===", sem_act::used_attr2, i));
    }
    {
        BOOST_TEST(parse("123", sem_act::unused_attr1));
    }
    {
        BOOST_TEST(parse("===", sem_act::unused_attr2));
    }

    return boost::report_errors();
}
