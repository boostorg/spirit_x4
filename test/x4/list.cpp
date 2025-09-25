/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/optional.hpp>
#include <boost/spirit/x4/operator/plus.hpp>

#include <string>
#include <vector>
#include <set>
#include <map>

TEST_CASE("list")
{
    using namespace x4::standard;

    using x4::int_;
    using x4::omit;
    using x4::_attr;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ % ',');

    CHECK(parse("a,b,c,d,e,f,g,h", char_ % ','));
    CHECK(parse("a,b,c,d,e,f,g,h,", char_ % ',').is_partial_match());

    CHECK(parse("a, b, c, d, e, f, g, h", char_ % ',', space));
    CHECK(parse("a, b, c, d, e, f, g, h,", char_ % ',', space).is_partial_match());

    {
        std::string s;
        REQUIRE(parse("a,b,c,d,e,f,g,h", char_ % ',', s));
        CHECK(s == "abcdefgh");

        CHECK(!parse("a,b,c,d,e,f,g,h,", char_ % ','));
    }

    {
        std::string s;
        REQUIRE(parse("ab,cd,ef,gh", (char_ >> char_) % ',', s));
        CHECK(s == "abcdefgh");
    }

    CHECK(!parse("ab,cd,ef,gh,", (char_ >> char_) % ','));
    CHECK(!parse("ab,cd,ef,g", (char_ >> char_) % ','));

    {
        std::string s;
        REQUIRE(parse("ab,cd,efg", (char_ >> char_) % ',' >> char_, s));
        CHECK(s == "abcdefg");
    }

    {
        // regression test for has_attribute
        int i = 0;
        REQUIRE(parse("1:2,3", int_ >> ':' >> omit[int_] % ',', i));
        CHECK(i == 1);
    }

    {
        std::vector<int> v;
        REQUIRE(parse("1,2", int_ % ',', v));
        REQUIRE(v.size() == 2);
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
    }

    {
        std::vector<int> v;
        REQUIRE(parse("(1,2)", '(' >> int_ % ',' >> ')', v));
        REQUIRE(v.size() == 2);
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
    }

    {
        std::vector<std::string> v;
        REQUIRE(parse("a,b,c", +alpha % ',', v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == "a");
        CHECK(v[1] == "b");
        CHECK(v[2] == "c");
    }

    {
        std::vector<std::optional<char>> v;
        REQUIRE(parse("#a,#", ('#' >> -alpha) % ',', v));
        REQUIRE(v.size() == 2);
        REQUIRE(v[0].has_value());
        CHECK(*v[0] == 'a');
        CHECK(!v[1].has_value());
    }
    {
        std::vector<char> v2;
        REQUIRE(parse("#a,#", ('#' >> -alpha) % ',', v2));
        REQUIRE(v2.size() == 1);
        CHECK(v2[0] == 'a');
    }

    {
        // actions
        std::string s;
        auto f = [&](auto& ctx){ s = std::string(_attr(ctx).begin(), _attr(ctx).end()); };

        REQUIRE(parse("a,b,c,d,e,f,g,h", (char_ % ',')[f]));
        CHECK(s == "abcdefgh");
    }

    {
        // test move only types
        std::vector<spirit_test::move_only> v;
        REQUIRE(parse("s.s.s.s", spirit_test::synth_move_only % '.', v));
        CHECK(v.size() == 4);
    }
}
