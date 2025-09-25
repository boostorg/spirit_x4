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

TEST_CASE("confix")
{
    using namespace spirit_test;
    using x4::_attr;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::confix('(', ')'));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::confix("[", "]"));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::confix("/*", "*/"));

    {
        constexpr auto comment = x4::confix("/*", "*/");

        CHECK(!parse("/abcdef*/", comment["abcdef"]));
        CHECK(!parse("/* abcdef*/", comment["abcdef"]));
        CHECK(!parse("/*abcdef */", comment["abcdef"]));
        CHECK(parse("/*abcdef*/", comment["abcdef"]));

        {
            unsigned value = 0;
            REQUIRE(parse(" /* 123 */ ", comment[x4::uint_], x4::standard::space, value));
            CHECK(value == 123);
        }
        {
            unsigned value = 0;
            auto const lambda = [&value](auto& ctx) { value = _attr(ctx) + 1; };
            REQUIRE(parse("/*123*/", comment[x4::uint_][lambda], value));
            CHECK(value == 124);
        }
    }
    {
        constexpr auto array = x4::confix('[', ']');

        {
            std::vector<unsigned> values;

            CHECK(parse("[0,2,4,6,8]", array[x4::uint_ % ',']));
            REQUIRE(parse("[0,2,4,6,8]", array[x4::uint_ % ','], values));
            REQUIRE(values.size() == 5);
            CHECK(values[0] == 0);
            CHECK(values[1] == 2);
            CHECK(values[2] == 4);
            CHECK(values[3] == 6);
            CHECK(values[4] == 8);
        }
        {
            std::vector<std::vector<unsigned>> values;
            CHECK(parse("[[1,3,5],[0,2,4]]", array[array[x4::uint_ % ','] % ',']));
            REQUIRE(parse("[[1,3,5],[0,2,4]]", array[array[x4::uint_ % ','] % ','], values));
            REQUIRE(values.size() == 2);
            REQUIRE(values[0].size() == 3);
            CHECK(values[0][0] == 1);
            CHECK(values[0][1] == 3);
            CHECK(values[0][2] == 5);
            REQUIRE(values[1].size() == 3);
            CHECK(values[1][0] == 0);
            CHECK(values[1][1] == 2);
            CHECK(values[1][2] == 4);
        }
    }
}
