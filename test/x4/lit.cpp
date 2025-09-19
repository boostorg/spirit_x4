/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#undef BOOST_SPIRIT_NO_STANDARD_WIDE

#ifndef BOOST_SPIRIT_X4_UNICODE
# define BOOST_SPIRIT_X4_UNICODE
#endif

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

#include <boost/fusion/include/vector.hpp>

#include <string>

int main()
{
    namespace x4 = boost::spirit::x4;
    // standard
    {
        (void)x4::lit('f'); // deprecated
        (void)x4::lit("f"); // deprecated
        (void)x4::lit("foo"); // deprecated
        (void)x4::standard::lit('f');
        (void)x4::standard::lit("f");
        (void)x4::standard::lit("foo");

        (void)x4::char_('f'); // deprecated
        (void)x4::char_("f"); // deprecated
        (void)x4::char_("foo"); // deprecated
        (void)x4::standard::char_('f');
        (void)x4::standard::char_("f");
        (void)x4::standard::char_("foo");

        (void)x4::string('f'); // deprecated
        (void)x4::string("f"); // deprecated
        (void)x4::string("foo"); // deprecated
        (void)x4::standard::string('f');
        (void)x4::standard::string("f");
        (void)x4::standard::string("foo");
    }

    // standard_wide
    {
        (void)x4::lit(L'f'); // deprecated
        (void)x4::lit(L"f"); // deprecated
        (void)x4::lit(L"foo"); // deprecated
        (void)x4::standard_wide::lit(L'f');
        (void)x4::standard_wide::lit(L"f");
        (void)x4::standard_wide::lit(L"foo");

        (void)x4::standard_wide::char_(L'f');
        (void)x4::standard_wide::char_(L"f");
        (void)x4::standard_wide::char_(L"foo");

        (void)x4::string(L'f'); // deprecated
        (void)x4::string(L"f"); // deprecated
        (void)x4::string(L"foo"); // deprecated
        (void)x4::standard_wide::string(L'f');
        (void)x4::standard_wide::string(L"f");
        (void)x4::standard_wide::string(L"foo");
    }

    // unicode
    {
        (void)x4::unicode::lit(U'f');
        (void)x4::unicode::lit(U"f");
        (void)x4::unicode::lit(U"foo");

        (void)x4::unicode::char_(U'f');
        (void)x4::unicode::char_(U"f");
        (void)x4::unicode::char_(U"foo");

        (void)x4::unicode::string(U'f');
        (void)x4::unicode::string(U"f");
        (void)x4::unicode::string(U"foo");
    }

    {
        std::string attr;
        auto p = x4::standard::char_ >> x4::standard::lit("\n");
        BOOST_TEST(parse("A\n", p, attr));
        BOOST_TEST(attr == "A");
    }

    {
        std::wstring attr;
        auto p = x4::standard_wide::char_ >> x4::standard_wide::lit(L"\n");
        BOOST_TEST(parse(L"É\n", p, attr));
        BOOST_TEST(attr == L"É");
    }

    // -------------------------------------------------

    {
        BOOST_TEST(parse("kimpo", x4::standard::lit("kimpo")));

        std::basic_string<char> s("kimpo");
        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST(parse("kimpo", x4::standard::lit(s)));
        BOOST_TEST(parse(L"kimpo", x4::standard_wide::lit(ws)));
    }

    {
        std::basic_string<char> s("kimpo");
        BOOST_TEST(parse("kimpo", x4::standard::lit(s)));

        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST(parse(L"kimpo", x4::standard_wide::lit(ws)));
    }

    // -------------------------------------------------

    {
        BOOST_TEST(parse("kimpo", "kimpo"));
        BOOST_TEST(parse("kimpo", x4::standard::string("kimpo")));

        BOOST_TEST(parse("x", x4::standard::string("x")));
        BOOST_TEST(parse(L"x", x4::standard_wide::string(L"x")));

        std::basic_string<char> s("kimpo");
        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST(parse("kimpo", s));
        BOOST_TEST(parse(L"kimpo", ws));
        BOOST_TEST(parse("kimpo", x4::standard::string(s)));
        BOOST_TEST(parse(L"kimpo", x4::standard_wide::string(ws)));
    }

    {
        BOOST_TEST(parse(L"kimpo", L"kimpo"));
        BOOST_TEST(parse(L"kimpo", x4::standard_wide::string(L"kimpo")));
        BOOST_TEST(parse(L"x", x4::standard_wide::string(L"x")));
    }

    {
        std::basic_string<char> s("kimpo");
        BOOST_TEST(parse("kimpo", x4::standard::string(s)));

        std::basic_string<wchar_t> ws(L"kimpo");
        BOOST_TEST(parse(L"kimpo", x4::standard_wide::string(ws)));
    }

    {
        // single-element fusion vector tests
        boost::fusion::vector<std::string> s;
        BOOST_TEST(parse("kimpo", x4::standard::string("kimpo"), s));
        BOOST_TEST(boost::fusion::at_c<0>(s) == "kimpo");
    }
    return boost::report_errors();
}
