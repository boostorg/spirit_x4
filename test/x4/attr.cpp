/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/char/char.hpp>

#include <boost/fusion/include/std_pair.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <type_traits>
#include <concepts>

TEST_CASE("attr")
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    using x4::attr;
    using x4::int_;

    {
        [[maybe_unused]] constexpr auto attr_p = attr(1);
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<int>>);
    }
    {
        [[maybe_unused]] constexpr auto attr_p = attr(3.14);
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<double>>);
    }

    {
        [[maybe_unused]] /*constexpr*/ auto attr_p = attr("foo");
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char>>>);
    }
    {
        [[maybe_unused]] /*constexpr*/ auto attr_p = attr("foo"s);
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char>>>);
    }
    {
        [[maybe_unused]] constexpr auto attr_p = attr("foo"sv);
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string_view<char>>>);
    }

    {
        [[maybe_unused]] /*constexpr*/ auto attr_p = attr(U"foo");
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char32_t>>>);
    }
    {
        [[maybe_unused]] /*constexpr*/ auto attr_p = attr(U"foo"s);
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char32_t>>>);
    }
    {
        [[maybe_unused]] constexpr auto attr_p = attr(U"foo"sv);
        static_assert(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string_view<char32_t>>>);
    }

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(attr(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(attr("asd"));

    {
        constexpr char s[] = "asd";
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(attr(s));
    }

    {
        int d = 0;
        REQUIRE(parse("", attr(1), d));
        CHECK(d == 1);
    }
    {
        int d = 0;
        int d1 = 1;
        REQUIRE(parse("", attr(d1), d));
        CHECK(d == 1);
    }
    {
        std::pair<int, int> p;
        REQUIRE(parse("1", int_ >> attr(2), p));
        CHECK(p.first == 1);
        CHECK(p.second == 2);
    }
    {
        char c = '\0';
        REQUIRE(parse("", attr('a'), c));
        CHECK(c == 'a');
    }
    {
        std::string str;
        REQUIRE(parse("", attr("test"), str));
        CHECK(str == "test");
    }
    {
        std::string str;
        REQUIRE(parse("", attr(std::string("test")), str));
        CHECK(str == "test");
    }
    {
        std::vector<int> array = {0, 1, 2};
        std::vector<int> vec;
        REQUIRE(parse("", attr(array), vec));
        REQUIRE(vec.size() == 3);
        CHECK(vec[0] == 0);
        CHECK(vec[1] == 1);
        CHECK(vec[2] == 2);
    }

    {
        std::string s;
        REQUIRE(parse("s", "s" >> attr(std::string("123")), s));
        CHECK(s == "123");
    }
    {
        std::string s;
        REQUIRE(parse("", attr(std::string("123")) >> attr(std::string("456")), s));
        CHECK(s == "123456");
    }
}
