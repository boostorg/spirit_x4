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

using namespace spirit_test;

int main()
{
    using namespace x4::standard;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ % ',');

    {
        BOOST_TEST(parse("a,b,c,d,e,f,g,h", char_ % ','));
        BOOST_TEST(parse("a,b,c,d,e,f,g,h,", char_ % ',').is_partial_match());
    }

    {
        BOOST_TEST(parse("a, b, c, d, e, f, g, h", char_ % ',', space));
        BOOST_TEST(parse("a, b, c, d, e, f, g, h,", char_ % ',', space).is_partial_match());
    }

    {
        std::string s;
        BOOST_TEST(parse("a,b,c,d,e,f,g,h", char_ % ',', s));
        BOOST_TEST(s == "abcdefgh");

        BOOST_TEST(!parse("a,b,c,d,e,f,g,h,", char_ % ','));
    }

    {
        std::string s;
        BOOST_TEST(parse("ab,cd,ef,gh", (char_ >> char_) % ',', s));
        BOOST_TEST(s == "abcdefgh");

        BOOST_TEST(!parse("ab,cd,ef,gh,", (char_ >> char_) % ','));
        BOOST_TEST(!parse("ab,cd,ef,g", (char_ >> char_) % ','));

        s.clear();
        BOOST_TEST(parse("ab,cd,efg", (char_ >> char_) % ',' >> char_, s));
        BOOST_TEST(s == "abcdefg");
    }

    { // regression test for has_attribute
        using x4::int_;
        using x4::omit;

        int i;
        BOOST_TEST(parse("1:2,3", int_ >> ':' >> omit[int_] % ',', i)) && BOOST_TEST_EQ(i, 1);
    }

    {
        using x4::int_;

        std::vector<int> v;
        BOOST_TEST(parse("1,2", int_ % ',', v));
        BOOST_TEST(2 == v.size() && 1 == v[0] && 2 == v[1]);
    }

    {
        using x4::int_;

        std::vector<int> v;
        BOOST_TEST(parse("(1,2)", '(' >> int_ % ',' >> ')', v));
        BOOST_TEST(2 == v.size() && 1 == v[0] && 2 == v[1]);
    }

    {
        std::vector<std::string> v;
        BOOST_TEST(parse("a,b,c,d", +alpha % ',', v));
        BOOST_TEST(4 == v.size() && "a" == v[0] && "b" == v[1] && "c" == v[2] && "d" == v[3]);
    }

    {
        std::vector<std::optional<char>> v;
        BOOST_TEST(parse("#a,#", ('#' >> -alpha) % ',', v));
        BOOST_TEST(2 == v.size() && v[0] && *v[0] == 'a' && !v[1]);

        std::vector<char> v2;
        BOOST_TEST(parse("#a,#", ('#' >> -alpha) % ',', v2));
        BOOST_TEST(1 == v2.size() && 'a' == v2[0]);
    }

    { // actions
        using x4::_attr;

        std::string s;
        auto f = [&](auto& ctx){ s = std::string(_attr(ctx).begin(), _attr(ctx).end()); };

        BOOST_TEST(parse("a,b,c,d,e,f,g,h", (char_ % ',')[f]));
        BOOST_TEST(s == "abcdefgh");
    }

    { // test move only types
        std::vector<move_only> v;
        BOOST_TEST(parse("s.s.s.s", synth_move_only % '.', v));
        BOOST_TEST_EQ(v.size(), 4);
    }

    return boost::report_errors();
}
