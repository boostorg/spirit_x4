/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>

#include <vector>
#include <set>

TEST_CASE("x3_rule_problem")
{
    enum class strong_int : int {};

    // Primitive (int)
    {
        using It = std::string_view::const_iterator;
        using Se = It;
        using Rule = x4::rule<struct my_rule, int>;

        STATIC_CHECK(x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, int>);
        STATIC_CHECK(x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, long long>);

        // Narrowing conversion
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, short>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, unsigned long long>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, double>);

        // Not permitted as of now, but can be relaxed in the future
        STATIC_CHECK(!x4::detail::RuleAttrTransformable<strong_int, int>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, strong_int>);
    }

    // Primitive (double)
    {
        using It = std::string_view::const_iterator;
        using Se = It;
        using Rule = x4::rule<struct my_rule, double>;

        STATIC_CHECK(x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, double>);
        STATIC_CHECK(x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, long double>);

        // Narrowing conversion
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, int>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, long long>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, unsigned long long>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, float>);
    }

    // "The Spirit X3 rule problem" in Boost.Parser's documentation
    // https://www.boost.org/doc/libs/1_89_0/doc/html/boost_parser/this_library_s_relationship_to_boost_spirit.html#boost_parser.this_library_s_relationship_to_boost_spirit.the_spirit_x3_rule_problem
    // https://github.com/boostorg/spirit_x4/issues/38
    {
        using It = std::string_view::const_iterator;
        using Se = It;
        using Rule = x4::rule<struct my_rule, std::vector<int>>;

        STATIC_CHECK(x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, std::vector<int>>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, std::set<int>>);
        STATIC_CHECK(!x4::is_parsable_v<Rule, It, Se, x4::parse_context_for<It, Se>, std::vector<strong_int>>);
    }
}
