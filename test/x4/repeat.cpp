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

int main()
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
        BOOST_TEST(parse("aaaaaaaa", *char_));
        BOOST_TEST(parse("aaaaaaaa", repeat(8)[char_]));
        BOOST_TEST(!parse("aa", repeat(3)[char_]));
        BOOST_TEST(parse("aaa", repeat(3, 5)[char_]));
        BOOST_TEST(parse("aaaaa", repeat(3, 5)[char_]));
        BOOST_TEST(!parse("aaaaaa", repeat(3, 5)[char_]));
        BOOST_TEST(!parse("aa", repeat(3, 5)[char_]));

        BOOST_TEST(parse("aaa", repeat(3, repeat_inf)[char_]));
        BOOST_TEST(parse("aaaaa", repeat(3, repeat_inf)[char_]));
        BOOST_TEST(parse("aaaaaa", repeat(3, repeat_inf)[char_]));
        BOOST_TEST(!parse("aa", repeat(3, repeat_inf)[char_]));
    }
    {
        std::string s;
        BOOST_TEST(parse("aaaaaaaa", *(char_ >> char_), s));
        BOOST_TEST(s == "aaaaaaaa");

        s.clear();
        BOOST_TEST(parse("aaaaaaaa", repeat(4)[char_ >> char_], s));
        BOOST_TEST(s == "aaaaaaaa");

        BOOST_TEST(!parse("aa", repeat(3)[char_ >> char_]));
        BOOST_TEST(!parse("a", repeat(1)[char_ >> char_]));

        s.clear();
        BOOST_TEST(parse("aa", repeat(1, 3)[char_ >> char_], s));
        BOOST_TEST(s == "aa");

        s.clear();
        BOOST_TEST(parse("aaaaaa", repeat(1, 3)[char_ >> char_], s));
        BOOST_TEST(s == "aaaaaa");

        BOOST_TEST(!parse("aaaaaaa", repeat(1, 3)[char_ >> char_]));
        BOOST_TEST(!parse("a", repeat(1, 3)[char_ >> char_]));

        s.clear();
        BOOST_TEST(parse("aaaa", repeat(2, repeat_inf)[char_ >> char_], s));
        BOOST_TEST(s == "aaaa");

        s.clear();
        BOOST_TEST(parse("aaaaaa", repeat(2, repeat_inf)[char_ >> char_], s));
        BOOST_TEST(s == "aaaaaa");

        BOOST_TEST(!parse("aa", repeat(2, repeat_inf)[char_ >> char_]));
    }

    // from classic spirit tests
    {
        BOOST_TEST(parse("", repeat(0, repeat_inf)['x']));

        // repeat exact 8
        {
            constexpr auto rep8 = repeat(8)[alpha] >> 'X';
            BOOST_TEST(!parse("abcdefgX", rep8).is_partial_match());
            BOOST_TEST(parse("abcdefghX", rep8));
            BOOST_TEST(!parse("abcdefghiX", rep8).is_partial_match());
            BOOST_TEST(!parse("abcdefgX", rep8).is_partial_match());
            BOOST_TEST(!parse("aX", rep8).is_partial_match());
        }

        // repeat 2 to 8
        {
            constexpr auto rep28 = repeat(2, 8)[alpha] >> '*';
            BOOST_TEST(parse("abcdefg*", rep28));
            BOOST_TEST(parse("abcdefgh*", rep28));
            BOOST_TEST(!parse("abcdefghi*", rep28).is_partial_match());
            BOOST_TEST(!parse("a*", rep28).is_partial_match());
        }

        // repeat 2 or more
        {
            constexpr auto rep2_ = repeat(2, repeat_inf)[alpha] >> '+';
            BOOST_TEST(parse("abcdefg+", rep2_));
            BOOST_TEST(parse("abcdefgh+", rep2_));
            BOOST_TEST(parse("abcdefghi+", rep2_));
            BOOST_TEST(parse("abcdefg+", rep2_));
            BOOST_TEST(!parse("a+", rep2_));
        }

        // repeat 0
        {
            constexpr auto rep0 = repeat(0)[alpha] >> '/';
            BOOST_TEST(parse("/", rep0));
            BOOST_TEST(!parse("a/", rep0));
        }

        // repeat 0 or 1
        {
            constexpr auto rep01 = repeat(0, 1)[alpha >> digit] >> '?';
            BOOST_TEST(!parse("abcdefg?", rep01));
            BOOST_TEST(!parse("a?", rep01));
            BOOST_TEST(!parse("1?", rep01));
            BOOST_TEST(!parse("11?", rep01));
            BOOST_TEST(!parse("aa?", rep01));
            BOOST_TEST(parse("?", rep01));
            BOOST_TEST(parse("a1?", rep01));
        }
    }

    {
        BOOST_TEST(parse(" a a aaa aa", repeat(7)[char_], space));
        BOOST_TEST(parse("12345 678 9", repeat(9)[digit], space));
    }

    {
        std::vector<std::string> v;
        BOOST_TEST(parse("a b c d", repeat(4)[lexeme[+alpha]], v, space) && 4 == v.size() &&
            v[0] == "a" && v[1] == "b" && v[2] == "c" &&  v[3] == "d");
    }
    {
        BOOST_TEST(parse("1 2 3", int_ >> repeat(2)[int_], space));
        BOOST_TEST(!parse("1 2", int_ >> repeat(2)[int_], space));
    }

    {
        std::vector<int> v;
        BOOST_TEST(parse("1 2 3", int_ >> repeat(2)[int_], v, space));
        BOOST_TEST(v.size() == 3 && v[0] == 1 && v[1] == 2 && v[2] == 3);

        BOOST_TEST(!parse("1 2", int_ >> repeat(2)[int_], space));
    }

    {
         // test move only types
        std::vector<spirit_test::move_only> v;
        BOOST_TEST(parse("sss", repeat(3)[spirit_test::synth_move_only], v));
        BOOST_TEST_EQ(v.size(), 3);
    }

    return boost::report_errors();
}
