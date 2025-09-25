/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/plus.hpp>

#include <boost/fusion/include/vector.hpp>

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

TEST_CASE("plus")
{
    using x4::char_;
    using x4::alpha;
    using x4::upper;
    using x4::space;
    using x4::digit;
    using x4::no_case;
    using x4::int_;
    using x4::omit;
    using x4::lit;
    using x4::lexeme;
    using x4::_attr;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(+char_);

    CHECK(parse("aaaaaaaa", +char_));
    CHECK(parse("a", +char_));
    CHECK(!parse("", +char_));
    CHECK(parse("aaaaaaaa", +alpha));
    CHECK(!parse("aaaaaaaa", +upper));

    CHECK(parse(" a a aaa aa", +char_, space));
    CHECK(parse("12345 678 9 ", +digit, space));

    CHECK(parse("aBcdeFGH", no_case[+char_]));
    CHECK(parse("a B cde FGH  ", no_case[+char_], space));

    {
        std::vector<int> v;
        REQUIRE(parse("123 456 789", +int_, space, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 123);
        CHECK(v[1] == 456);
        CHECK(v[2] == 789);
    }

    {
        std::vector<std::string> v;
        REQUIRE(parse("a b c", +lexeme[+alpha], space, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == "a");
        CHECK(v[1] == "b");
        CHECK(v[2] == "c");
    }

    CHECK(parse("Kim Kim Kim", +lit("Kim"), space));

    {
        std::string s;
        REQUIRE(parse("bbbb", omit[+char_('b')], s));
        CHECK(s == "");
    }
    {
        std::string s;
        REQUIRE(parse("b b b b ", omit[+char_('b')], space, s));
        CHECK(s == "");
    }

    {
        // actions
        std::string v;
        auto f = [&](auto& ctx){ v = _attr(ctx); };

        REQUIRE(parse("bbb", (+char_)[f]));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 'b');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'b');
    }

    {
        // more actions
        std::vector<int> v;
        auto f = [&](auto& ctx){ v = _attr(ctx); };

        REQUIRE(parse("1 2 3", (+int_)[f], space));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
    }

    // attribute customization
    {
        x_attr x;
        (void)parse("abcde", +char_, x);
    }

    // single-element fusion vector tests
    {
        boost::fusion::vector<std::string> fs;
        REQUIRE(parse("12345", +char_, fs));
        CHECK(boost::fusion::at_c<0>(fs) == "12345");
    }

    {
        // test move only types
        std::vector<spirit_test::move_only> v;
        REQUIRE(parse("sss", +spirit_test::synth_move_only, v));
        CHECK(v.size() == 3);
    }
}
