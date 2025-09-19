/*=============================================================================
    Copyright (c) 2009 Chris Hoeppler
    Copyright (c) 2014 Lee Clagett
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/directive/confix.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/numeric/uint.hpp>
#include <boost/spirit/x4/operator/list.hpp>

int main()
{
    namespace x4 = boost::spirit::x4;
    using namespace spirit_test;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::confix('(', ')'));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::confix("[", "]"));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::confix("/*", "*/"));

    {
        constexpr auto comment = x4::confix("/*", "*/");

        BOOST_TEST(!parse("/abcdef*/", comment["abcdef"]));
        BOOST_TEST(!parse("/* abcdef*/", comment["abcdef"]));
        BOOST_TEST(!parse("/*abcdef */", comment["abcdef"]));
        BOOST_TEST(parse("/*abcdef*/", comment["abcdef"]));

        {
            unsigned value = 0;
            BOOST_TEST(
                parse(" /* 123 */ ", comment[x4::uint_], value, x4::standard::space));
            BOOST_TEST(value == 123);

            using x4::_attr;
            value = 0;
            auto const lambda = [&value](auto& ctx) { value = _attr(ctx) + 1; };
            BOOST_TEST(parse("/*123*/", comment[x4::uint_][lambda], value));
            BOOST_TEST(value == 124);
        }
    }
    {
        constexpr auto array = x4::confix('[', ']');

        {
            std::vector<unsigned> values;

            BOOST_TEST(parse("[0,2,4,6,8]", array[x4::uint_ % ',']));
            BOOST_TEST(parse("[0,2,4,6,8]", array[x4::uint_ % ','], values));
            BOOST_TEST(
                values.size() == 5 &&
                values[0] == 0 &&
                values[1] == 2 &&
                values[2] == 4 &&
                values[3] == 6 &&
                values[4] == 8);
        }
        {
            std::vector<std::vector<unsigned>> values;
            BOOST_TEST(
                parse("[[1,3,5],[0,2,4]]", array[array[x4::uint_ % ','] % ',']));
            BOOST_TEST(
                parse(
                    "[[1,3,5],[0,2,4]]",
                    array[array[x4::uint_ % ','] % ','],
                    values));
            BOOST_TEST(
                values.size() == 2 &&
                values[0].size() == 3 &&
                values[0][0] == 1 &&
                values[0][1] == 3 &&
                values[0][2] == 5 &&
                values[1].size() == 3 &&
                values[1][0] == 0 &&
                values[1][1] == 2 &&
                values[1][2] == 4);
        }
    }

    return boost::report_errors();
}
