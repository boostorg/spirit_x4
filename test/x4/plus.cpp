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

namespace boost::spirit::x4::traits
{
    template <>
    struct container_value<x_attr>
    {
        using type = char; // value type of container
    };

    template <>
    struct push_back_container<x_attr>
    {
        static constexpr void call(x_attr& /*c*/, char /*val*/) noexcept
        {
            // push back value type into container
        }
    };

} // x4::traits

int main()
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

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(+char_);

    {
        BOOST_TEST(parse("aaaaaaaa", +char_));
        BOOST_TEST(parse("a", +char_));
        BOOST_TEST(!parse("", +char_));
        BOOST_TEST(parse("aaaaaaaa", +alpha));
        BOOST_TEST(!parse("aaaaaaaa", +upper));
    }

    {
        BOOST_TEST(parse(" a a aaa aa", +char_, space));
        BOOST_TEST(parse("12345 678 9 ", +digit, space));
    }

    {
        BOOST_TEST(parse("aBcdeFGH", no_case[+char_]));
        BOOST_TEST(parse("a B cde FGH  ", no_case[+char_], space));
    }

    {
        std::vector<int> v;
        BOOST_TEST(parse("123 456 789 10", +int_, v, space) && 4 == v.size() &&
            v[0] == 123 && v[1] == 456 && v[2] == 789 &&  v[3] == 10);
    }

    {
        std::vector<std::string> v;
        BOOST_TEST(parse("a b c d", +lexeme[+alpha], v, space) && 4 == v.size() &&
            v[0] == "a" && v[1] == "b" && v[2] == "c" &&  v[3] == "d");
    }

    {
        BOOST_TEST(parse("Kim Kim Kim", +lit("Kim"), space));
    }

    // FIXME
    /*{
        // The following 2 tests show that omit does not inhibit explicit attributes

        std::string s;
        BOOST_TEST(parse("bbbb", omit[+char_('b')], s) && s == "bbbb");

        s.clear();
        BOOST_TEST(parse("b b b b ", omit[+char_('b')], s, space) && s == "bbbb");
    }*/

    {
        // actions
        std::string v;
        auto f = [&](auto& ctx){ v = _attr(ctx); };

        BOOST_TEST(parse("bbbb", (+char_)[f]) && 4 == v.size() &&
            v[0] == 'b' && v[1] == 'b' && v[2] == 'b' &&  v[3] == 'b');
    }

    {
        // more actions
        std::vector<int> v;
        auto f = [&](auto& ctx){ v = _attr(ctx); };

        BOOST_TEST(parse("1 2 3", (+int_)[f], space) && 3 == v.size() &&
            v[0] == 1 && v[1] == 2 && v[2] == 3);
    }

    // attribute customization
    {
        x_attr x;
        (void)parse("abcde", +char_, x);
    }

    // single-element fusion vector tests
    {
        boost::fusion::vector<std::string> fs;
        BOOST_TEST(parse("12345", +char_, fs)); // ok
        BOOST_TEST(boost::fusion::at_c<0>(fs) == "12345");
    }

    {
        // test move only types
        std::vector<spirit_test::move_only> v;
        BOOST_TEST(parse("sss", +spirit_test::synth_move_only, v));
        BOOST_TEST_EQ(v.size(), 3);
    }

    return boost::report_errors();
}
