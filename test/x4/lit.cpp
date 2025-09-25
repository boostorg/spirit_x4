/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_SPIRIT_X4_UNICODE

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

#include <boost/fusion/include/vector.hpp>

#include <string>

TEST_CASE("lit")
{
    // standard
    {
        (void)x4::lit('f');
        (void)x4::lit("f");
        (void)x4::lit("foo");
        (void)x4::standard::lit('f');
        (void)x4::standard::lit("f");
        (void)x4::standard::lit("foo");

        (void)x4::char_('f');
        (void)x4::char_("f");
        (void)x4::char_("foo");
        (void)x4::standard::char_('f');
        (void)x4::standard::char_("f");
        (void)x4::standard::char_("foo");

        (void)x4::string('f');
        (void)x4::string("f");
        (void)x4::string("foo");
        (void)x4::standard::string('f');
        (void)x4::standard::string("f");
        (void)x4::standard::string("foo");
    }

    // standard_wide
    {
        (void)x4::lit(L'f');
        (void)x4::lit(L"f");
        (void)x4::lit(L"foo");
        (void)x4::standard_wide::lit(L'f');
        (void)x4::standard_wide::lit(L"f");
        (void)x4::standard_wide::lit(L"foo");

        (void)x4::standard_wide::char_(L'f');
        (void)x4::standard_wide::char_(L"f");
        (void)x4::standard_wide::char_(L"foo");

        (void)x4::string(L'f');
        (void)x4::string(L"f");
        (void)x4::string(L"foo");
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
        constexpr auto p = x4::standard::char_ >> x4::standard::lit("\n"); // TODO: MSVC 2022 bug, [[no_unique_address]] on binary_parser "overruns"
        REQUIRE(parse("A\n", p, attr));
        CHECK(attr == "A");
    }

    {
        std::wstring attr;
        constexpr auto p = x4::standard_wide::char_ >> x4::standard_wide::lit(L"\n");
        REQUIRE(parse(L"É\n", p, attr));
        CHECK(attr == L"É");
    }

    // -------------------------------------------------

    {
        CHECK(parse("kimpo", x4::standard::lit("kimpo")));

        std::basic_string<char> s("kimpo");
        CHECK(parse("kimpo", x4::standard::lit(s)));

        std::basic_string<wchar_t> ws(L"kimpo");
        CHECK(parse(L"kimpo", x4::standard_wide::lit(ws)));
    }

    {
        std::basic_string<char> s("kimpo");
        std::basic_string<wchar_t> ws(L"kimpo");
        CHECK(parse("kimpo", x4::standard::lit(s)));
        CHECK(parse(L"kimpo", x4::standard_wide::lit(ws)));
    }

    // -------------------------------------------------

    {
        CHECK(parse("kimpo", "kimpo"));
        CHECK(parse("kimpo", x4::standard::string("kimpo")));

        CHECK(parse("x", x4::standard::string("x")));
        CHECK(parse(L"x", x4::standard_wide::string(L"x")));

        std::basic_string<char> s("kimpo");
        std::basic_string<wchar_t> ws(L"kimpo");
        CHECK(parse("kimpo", s));
        CHECK(parse(L"kimpo", ws));
        CHECK(parse("kimpo", x4::standard::string(s)));
        CHECK(parse(L"kimpo", x4::standard_wide::string(ws)));
    }

    {
        CHECK(parse(L"kimpo", L"kimpo"));
        CHECK(parse(L"kimpo", x4::standard_wide::string(L"kimpo")));
        CHECK(parse(L"x", x4::standard_wide::string(L"x")));
    }

    {
        std::basic_string<char> s("kimpo");
        CHECK(parse("kimpo", x4::standard::string(s)));

        std::basic_string<wchar_t> ws(L"kimpo");
        CHECK(parse(L"kimpo", x4::standard_wide::string(ws)));
    }

    {
        // single-element fusion vector tests
        boost::fusion::vector<std::string> s;
        REQUIRE(parse("kimpo", x4::standard::string("kimpo"), s));
        CHECK(boost::fusion::at_c<0>(s) == "kimpo");
    }
}
