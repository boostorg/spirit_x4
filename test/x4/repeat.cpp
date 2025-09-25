/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/directive/repeat.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/plus.hpp>

#include <vector>
#include <string>

TEST_CASE("repeat")
{
    using namespace x4::standard;
    using x4::repeat;
    using x4::repeat_inf;
    using x4::omit;
    using x4::int_;
    using x4::lexeme;
    using x4::char_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(*x4::lit('x'));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(repeat(3)['x']);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(repeat(3, 5)['x']);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(repeat(3, repeat_inf)['x']);

    {
        CHECK(parse("aaaaaaaa", *char_));
        CHECK(parse("aaaaaaaa", repeat(8)[char_]));
        CHECK(!parse("aa", repeat(3)[char_]));
        CHECK(parse("aaa", repeat(3, 5)[char_]));
        CHECK(parse("aaaaa", repeat(3, 5)[char_]));
        CHECK(!parse("aaaaaa", repeat(3, 5)[char_]));
        CHECK(!parse("aa", repeat(3, 5)[char_]));

        CHECK(parse("aaa", repeat(3, repeat_inf)[char_]));
        CHECK(parse("aaaaa", repeat(3, repeat_inf)[char_]));
        CHECK(parse("aaaaaa", repeat(3, repeat_inf)[char_]));
        CHECK(!parse("aa", repeat(3, repeat_inf)[char_]));
    }
    {
        std::string s;
        REQUIRE(parse("aaaaaaaa", *(char_ >> char_), s));
        CHECK(s == "aaaaaaaa");
    }

    {
        std::string s;
        REQUIRE(parse("aaaaaaaa", repeat(4)[char_ >> char_], s));
        CHECK(s == "aaaaaaaa");
    }

    CHECK(!parse("aa", repeat(3)[char_ >> char_]));
    CHECK(!parse("a", repeat(1)[char_ >> char_]));

    {
        std::string s;
        REQUIRE(parse("aa", repeat(1, 3)[char_ >> char_], s));
        CHECK(s == "aa");
    }
    {
        std::string s;
        REQUIRE(parse("aaaaaa", repeat(1, 3)[char_ >> char_], s));
        CHECK(s == "aaaaaa");
    }

    CHECK(!parse("aaaaaaa", repeat(1, 3)[char_ >> char_]));
    CHECK(!parse("a", repeat(1, 3)[char_ >> char_]));

    {
        std::string s;
        REQUIRE(parse("aaaa", repeat(2, repeat_inf)[char_ >> char_], s));
        CHECK(s == "aaaa");
    }
    {
        std::string s;
        REQUIRE(parse("aaaaaa", repeat(2, repeat_inf)[char_ >> char_], s));
        CHECK(s == "aaaaaa");
    }

    CHECK(!parse("aa", repeat(2, repeat_inf)[char_ >> char_]));

    // from classic spirit tests
    {
        CHECK(parse("", repeat(0, repeat_inf)['x']));

        // repeat exact 8
        {
            constexpr auto rep8 = repeat(8)[alpha] >> 'X';
            CHECK(!parse("abcdefgX", rep8));
            CHECK(parse("abcdefghX", rep8));
            CHECK(!parse("abcdefghiX", rep8));
            CHECK(!parse("abcdefgX", rep8));
            CHECK(!parse("aX", rep8));
        }

        // repeat 2 to 8
        {
            constexpr auto rep28 = repeat(2, 8)[alpha] >> '*';
            CHECK(parse("abcdefg*", rep28));
            CHECK(parse("abcdefgh*", rep28));
            CHECK(!parse("abcdefghi*", rep28));
            CHECK(!parse("a*", rep28));
        }

        // repeat 2 or more
        {
            constexpr auto rep2_ = repeat(2, repeat_inf)[alpha] >> '+';
            CHECK(parse("abcdefg+", rep2_));
            CHECK(parse("abcdefgh+", rep2_));
            CHECK(parse("abcdefghi+", rep2_));
            CHECK(parse("abcdefg+", rep2_));
            CHECK(!parse("a+", rep2_));
        }

        // repeat 0
        {
            constexpr auto rep0 = repeat(0)[alpha] >> '/';
            CHECK(parse("/", rep0));
            CHECK(!parse("a/", rep0));
        }

        // repeat 0 or 1
        {
            constexpr auto rep01 = repeat(0, 1)[alpha >> digit] >> '?';
            CHECK(!parse("abcdefg?", rep01));
            CHECK(!parse("a?", rep01));
            CHECK(!parse("1?", rep01));
            CHECK(!parse("11?", rep01));
            CHECK(!parse("aa?", rep01));
            CHECK(parse("?", rep01));
            CHECK(parse("a1?", rep01));
        }
    }

    {
        CHECK(parse(" a a aaa aa", repeat(7)[char_], space));
        CHECK(parse("12345 678 9", repeat(9)[digit], space));
    }

    {
        std::vector<std::string> v;
        REQUIRE(parse("a b c", repeat(3)[lexeme[+alpha]], space, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == "a");
        CHECK(v[1] == "b");
        CHECK(v[2] == "c");
    }

    CHECK(parse("1 2 3", int_ >> repeat(2)[int_], space));
    CHECK(!parse("1 2", int_ >> repeat(2)[int_], space));

    {
        std::vector<int> v;
        REQUIRE(parse("1 2 3", int_ >> repeat(2)[int_], space, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);

        CHECK(!parse("1 2", int_ >> repeat(2)[int_], space));
    }

    {
         // test move only types
        std::vector<spirit_test::move_only> v;
        REQUIRE(parse("sss", repeat(3)[spirit_test::synth_move_only], v));
        CHECK(v.size() == 3);
    }
}
