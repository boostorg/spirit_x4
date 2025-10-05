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
        STATIC_CHECK(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<int>>);
    }
    {
        [[maybe_unused]] constexpr auto attr_p = attr(3.14);
        STATIC_CHECK(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<double>>);
    }

    {
        constexpr auto attr_p = attr("foo");
        STATIC_REQUIRE(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char>, std::basic_string_view<char>>>);

        // Make sure `attr(std::string_view)` is parsable into std::string
        {
            constexpr auto result = [&](std::string_view expected_str) consteval {
                std::string str;
                std::string_view const input;
                auto it = input.begin();
                auto const se = input.end();
                bool const ok = attr_p.parse(it, se, unused, str);
                return std::make_pair(ok, str == expected_str);
            }("foo");
            STATIC_REQUIRE(result.first == true);
            STATIC_CHECK(result.second == true);
        }
        {
            std::string str;
            std::string_view const input;
            auto it = input.begin();
            auto const se = input.end();
            REQUIRE(attr_p.parse(it, se, unused, str));
            CHECK(str == "foo");
        }
    }
    {
        [[maybe_unused]] /*constexpr*/ auto attr_p = attr("foo"s);
        STATIC_CHECK(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char>>>);
    }
    {
        [[maybe_unused]] constexpr auto attr_p = attr("foo"sv);
        STATIC_CHECK(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string_view<char>>>);
    }

    {
        [[maybe_unused]] constexpr auto attr_p = attr(U"foo");
        STATIC_CHECK(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char32_t>, std::basic_string_view<char32_t>>>);
    }
    {
        [[maybe_unused]] /*constexpr*/ auto attr_p = attr(U"foo"s);
        STATIC_CHECK(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string<char32_t>>>);
    }
    {
        [[maybe_unused]] constexpr auto attr_p = attr(U"foo"sv);
        STATIC_CHECK(std::same_as<std::remove_const_t<decltype(attr_p)>, x4::attr_parser<std::basic_string_view<char32_t>>>);
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
        std::vector<std::string> strs;
        REQUIRE(parse("", attr(std::string("123")) >> attr(std::string("456")), strs));
        CHECK(strs == std::vector<std::string>{"123", "456"});
    }
    {
        std::string s;
        REQUIRE(parse("", attr(std::string("123")) >> attr(std::string("456")), s));
        CHECK(s == "123456");
    }

    {
        std::vector<int> ints;
        REQUIRE(parse("", attr(std::vector<int>{1, 2, 3}) >> attr(std::vector<int>{4, 5, 6}), ints));
        CHECK(ints == std::vector<int>{1, 2, 3, 4, 5, 6});
    }

    {
        std::vector<int> ints;
        REQUIRE(parse("",
            (attr(std::vector<int>{1, 2, 3}) >> attr(std::vector<int>{4, 5, 6})) >>
            (attr(std::vector<int>{7, 8, 9}) >> attr(std::vector<int>{0, 1, 2})),
            ints
        ));
        CHECK(ints == std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2});
    }
}
