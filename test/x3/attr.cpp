/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "test.hpp"

#include <boost/spirit/home/x3.hpp>

#include <boost/fusion/include/std_pair.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <type_traits>
#include <concepts>

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    using boost::spirit::x3::attr;
    using boost::spirit::x3::int_;

    {
        auto attr_p = attr(1);
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<int>>);
    }
    {
        auto attr_p = attr(3.14);
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<double>>);
    }

    {
        auto attr_p = attr("foo");
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<std::basic_string<char>>>);
    }
    {
        auto attr_p = attr("foo"s);
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<std::basic_string<char>>>);
    }
    {
        auto attr_p = attr("foo"sv);
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<std::basic_string_view<char>>>);
    }

    {
        auto attr_p = attr(U"foo");
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<std::basic_string<char32_t>>>);
    }
    {
        auto attr_p = attr(U"foo"s);
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<std::basic_string<char32_t>>>);
    }
    {
        auto attr_p = attr(U"foo"sv);
        static_assert(std::same_as<decltype(attr_p), x3::attr_parser<std::basic_string_view<char32_t>>>);
    }

    BOOST_SPIRIT_X3_ASSERT_CONSTEXPR_CTORS(attr(1));
    BOOST_SPIRIT_X3_ASSERT_CONSTEXPR_CTORS(attr("asd"));

    {
        constexpr char s[] = "asd";
        BOOST_SPIRIT_X3_ASSERT_CONSTEXPR_CTORS(attr(s));
    }

    {
        int d = 0;
        BOOST_TEST(parse("", attr(1), d) && d == 1);
    }
    {
        int d = 0;
        int d1 = 1;
        BOOST_TEST(parse("", attr(d1), d) && d == 1);
    }
    {
        std::pair<int, int> p;
        BOOST_TEST(parse("1", int_ >> attr(1), p) && p.first == 1 && p.second == 1);
    }
    {
        char c = '\0';
        BOOST_TEST(parse("", attr('a'), c) && c == 'a');
    }
    {
        std::string str;
        BOOST_TEST(parse("", attr("test"), str) && str == "test");
    }
    {
        std::string str;
        BOOST_TEST(parse("", attr(std::string("test")), str)) && BOOST_TEST_EQ(str, "test");
    }
    {
        std::vector<int> array = {0, 1, 2};
        std::vector<int> vec;
        BOOST_TEST(parse("", attr(array), vec) && vec.size() == 3 && vec[0] == 0 && vec[1] == 1 && vec[2] == 2);
    }

    {
        std::string s;
        BOOST_TEST(parse("s", "s" >> attr(std::string("123")), s) && s == "123");
    }

    {
        std::string s;
        BOOST_TEST(parse("", attr(std::string("123")) >> attr(std::string("456")), s))
          && BOOST_TEST_EQ(s, "123456");
    }

    return boost::report_errors();
}
