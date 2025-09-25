/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/plus.hpp>

#include <string>
#include <vector>

struct x_attr {};

namespace boost::spirit::x4::traits {

template<>
struct container_value<x_attr>
{
    using type = char; // value type of container
};

template<>
struct push_back_container<x_attr>
{
    static constexpr void call(x_attr& /*c*/, char /*val*/) noexcept
    {
        // push back value type into container
    }
};

} // x4::traits

TEST_CASE("kleene")
{
    using x4::char_;
    using x4::alpha;
    using x4::upper;
    using x4::space;
    using x4::digit;
    using x4::int_;
    using x4::lexeme;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(*char_);

    CHECK(parse("aaaaaaaa", *char_));
    CHECK(parse("a", *char_));
    CHECK(parse("", *char_));
    CHECK(parse("aaaaaaaa", *alpha));
    CHECK(!parse("aaaaaaaa", *upper));

    CHECK(parse(" a a aaa aa", *char_, space));
    CHECK(parse("12345 678 9", *digit, space));

    {
        std::string s;
        REQUIRE(parse("bbbb", *char_, s));
        CHECK(s == "bbbb");
    }
    {
        std::string s;
        REQUIRE(parse("b b b b ", *char_, space, s));
        CHECK(s == "bbbb");
    }

    {
        std::vector<int> v;
        REQUIRE(parse("123 456 789 10", *int_, space, v));
        REQUIRE(v.size() == 4);
        CHECK(v[0] == 123);
        CHECK(v[1] == 456);
        CHECK(v[2] == 789);
        CHECK(v[3] == 10);
    }

    {
        std::vector<std::string> v;
        REQUIRE(parse("a b c d", *lexeme[+alpha], space, v));
        REQUIRE(v.size() == 4);
        CHECK(v[0] == "a");
        CHECK(v[1] == "b");
        CHECK(v[2] == "c");
        CHECK(v[3] == "d");
    }

    {
        std::vector<int> v;
        REQUIRE(parse("123 456 789", *int_, space, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 123);
        CHECK(v[1] == 456);
        CHECK(v[2] == 789);
    }

    {
        // actions
        using x4::_attr;

        std::string v;
        auto f = [&](auto& ctx){ v = _attr(ctx); };

        REQUIRE(parse("bbbb", (*char_)[f]));
        REQUIRE(v.size() == 4);
        CHECK(v[0] == 'b');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'b');
        CHECK(v[3] == 'b');
    }

    {
        // more actions
        using x4::_attr;

        std::vector<int> v;
        auto f = [&](auto& ctx){ v = _attr(ctx); };

        REQUIRE(parse("123 456 789", (*int_)[f], space));
        CHECK(v.size() == 3);
        CHECK(v[0] == 123);
        CHECK(v[1] == 456);
        CHECK(v[2] == 789);
    }

    {
        // attribute customization

        x_attr x;
        (void)parse("abcde", *char_, x);
    }

    {
        // test move only types
        std::vector<spirit_test::move_only> v;
        REQUIRE(parse("sss", *spirit_test::synth_move_only, v));
        CHECK(v.size() == 3);
    }
}
