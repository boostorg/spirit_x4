/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

#include <boost/fusion/include/vector.hpp>

#include <string>

namespace {

using x4::rule;

constexpr rule<class direct_rule, int> direct_rule = "direct_rule";
constexpr rule<class indirect_rule, int> indirect_rule = "indirect_rule";

constexpr auto direct_rule_def = x4::int_;
constexpr auto indirect_rule_def = direct_rule;

BOOST_SPIRIT_X4_DEFINE(direct_rule)
BOOST_SPIRIT_X4_DEFINE(indirect_rule)

} // anonymous

TEST_CASE("omit")
{
    using namespace x4::standard;
    using x4::omit;
    using x4::int_;
    using x4::_attr;

    using boost::fusion::vector;
    using boost::fusion::at_c;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(omit['x']);

    CHECK(parse("a", omit['a']));

    {
        // omit[] means we don't receive the attribute
        char attr{};
        REQUIRE(parse("abc", omit[char_] >> omit['b'] >> char_, attr));
        CHECK(attr == 'c');
    }

    {
        // If all elements except 1 is omitted, the attribute is
        // a single-element sequence. For this case alone, we allow
        // naked attributes (unwrapped in a fusion sequence).
        char attr{};
        REQUIRE(parse("abc", omit[char_] >> 'b' >> char_, attr));
        CHECK(attr == 'c');
    }

    {
        // omit[] means we don't receive the attribute
        vector<> attr;
        CHECK(parse("abc", omit[char_] >> omit['b'] >> omit[char_], attr));
    }

    {
        // omit[] means we don't receive the attribute
        // this test is merely a compile test, because using a unused as the
        // explicit attribute doesn't make any sense
        unused_type attr;
        CHECK(parse("abc", omit[char_ >> 'b' >> char_], attr));
    }

    {
        // omit[] means we don't receive the attribute, if all elements of a
        // sequence have unused attributes, the whole sequence has an unused
        // attribute as well
        vector<char, char> attr;
        REQUIRE(parse("abcde", char_ >> (omit[char_] >> omit['c'] >> omit[char_]) >> char_, attr));
        CHECK(at_c<0>(attr) == 'a');
        CHECK(at_c<1>(attr) == 'e');
    }

    {
        // "hello" has an unused_type. unused attrubutes are not part of the sequence
        vector<char, char> attr;
        REQUIRE(parse("a hello c", char_ >> "hello" >> char_, space, attr));
        CHECK(at_c<0>(attr) == 'a');
        CHECK(at_c<1>(attr) == 'c');
    }

    {
        // if only one node in a sequence is left (all the others are omitted),
        // then we need "naked" attributes (not wrapped in a tuple)
        int attr = 0;
        REQUIRE(parse("a 123 c", omit['a'] >> int_ >> omit['c'], space, attr));
        CHECK(attr == 123);
    }

    // unused means we don't care about the attribute
    CHECK(parse("abc", char_ >> 'b' >> char_, unused));

    {
        // test action with omitted attribute
        char c = 0;
        auto f = [&](auto& ctx){ c = _attr(ctx); };

        REQUIRE(parse("x123\"a string\"", (char_ >> omit[int_] >> "\"a string\"")[f]));
        CHECK(c == 'x');
    }

    {
        // test action with omitted attribute
        int n = 0;
        auto f = [&](auto& ctx){ n = _attr(ctx); };

        REQUIRE(parse("x 123 \"a string\"", (omit[char_] >> int_ >> "\"a string\"")[f], space));
        CHECK(n == 123);
    }

    // test with simple rule
    CHECK(parse("123", omit[direct_rule], unused));

    // test with complex rule
    CHECK(parse("123", omit[indirect_rule], unused));
}
