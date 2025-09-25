/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_SPIRIT_X4_UNICODE

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/char/unicode_char_class.hpp>
#include <boost/spirit/x4/char/negated_char.hpp>

#include <concepts>
#include <type_traits>

TEST_CASE("char_class")
{
    {
        std::string_view sv;
        auto first = sv.begin();
        auto const last = sv.end();
        char ch{};
        static_assert(noexcept(x4::standard::alnum.parse(first, last, unused, ch)));
        (void)x4::standard::alnum.parse(first, last, unused, ch);
    }
    {
        std::u32string_view sv;
        auto first = sv.begin();
        auto const last = sv.end();
        char32_t ch{};
        static_assert(noexcept(x4::unicode::alnum.parse(first, last, unused, ch)));
        (void)x4::unicode::alnum.parse(first, last, unused, ch);
    }

    {
        using namespace x4::standard;
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(alnum);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(alpha);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(digit);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(xdigit);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(cntrl);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(graph);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(lower);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(print);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(punct);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(space);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(blank);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(upper);
        CHECK(parse("1", alnum));
        CHECK(!parse(" ", alnum));
        CHECK(!parse("1", alpha));
        CHECK(parse("x", alpha));
        CHECK(parse(" ", blank));
        CHECK(!parse("x", blank));
        CHECK(parse("1", digit));
        CHECK(!parse("x", digit));
        CHECK(parse("a", lower));
        CHECK(!parse("A", lower));
        CHECK(parse("!", punct));
        CHECK(!parse("x", punct));
        CHECK(parse(" ", space));
        CHECK(parse("\n", space));
        CHECK(parse("\r", space));
        CHECK(parse("\t", space));
        CHECK(parse("A", upper));
        CHECK(!parse("a", upper));
        CHECK(parse("A", xdigit));
        CHECK(parse("0", xdigit));
        CHECK(parse("f", xdigit));
        CHECK(!parse("g", xdigit));
        CHECK(!parse("\xF1", print));
    }

    {
        using namespace x4::standard_wide;
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(alnum);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(alpha);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(digit);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(xdigit);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(cntrl);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(graph);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(lower);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(print);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(punct);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(space);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(blank);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(upper);
        CHECK(parse(L"1", alnum));
        CHECK(!parse(L" ", alnum));
        CHECK(!parse(L"1", alpha));
        CHECK(parse(L"x", alpha));
        CHECK(parse(L" ", blank));
        CHECK(!parse(L"x", blank));
        CHECK(parse(L"1", digit));
        CHECK(!parse(L"x", digit));
        CHECK(parse(L"a", lower));
        CHECK(!parse(L"A", lower));
        CHECK(parse(L"!", punct));
        CHECK(!parse(L"x", punct));
        CHECK(parse(L" ", space));
        CHECK(parse(L"\n", space));
        CHECK(parse(L"\r", space));
        CHECK(parse(L"\t", space));
        CHECK(parse(L"A", upper));
        CHECK(!parse(L"a", upper));
        CHECK(parse(L"A", xdigit));
        CHECK(parse(L"0", xdigit));
        CHECK(parse(L"f", xdigit));
        CHECK(!parse(L"g", xdigit));
    }

    {
        using namespace x4::unicode;
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(alnum);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(alpha);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(digit);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(xdigit);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(cntrl);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(graph);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(lower);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(print);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(punct);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(space);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(blank);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(upper);
        CHECK(parse(U"1", alnum));
        CHECK(!parse(U" ", alnum));
        CHECK(!parse(U"1", alpha));
        CHECK(parse(U"x", alpha));
        CHECK(parse(U" ", blank));
        CHECK(!parse(U"x", blank));
        CHECK(parse(U"1", digit));
        CHECK(!parse(U"x", digit));
        CHECK(parse(U"a", lower));
        CHECK(!parse(U"A", lower));
        CHECK(parse(U"!", punct));
        CHECK(!parse(U"x", punct));
        CHECK(parse(U" ", space));
        CHECK(parse(U"\n", space));
        CHECK(parse(U"\r", space));
        CHECK(parse(U"\t", space));
        CHECK(parse(U"A", upper));
        CHECK(!parse(U"a", upper));
        CHECK(parse(U"A", xdigit));
        CHECK(parse(U"0", xdigit));
        CHECK(parse(U"f", xdigit));
        CHECK(!parse(U"g", xdigit));

        CHECK(parse(U"A", alphabetic));
        CHECK(parse(U"9", decimal_number));
        CHECK(parse(U"\u2800", braille));
        CHECK(!parse(U" ", braille));
        CHECK(parse(U" ", ~braille));
        // TODO: Add more unicode tests
    }

    {   // test invalid unicode literals
        using namespace x4::unicode;

        constexpr auto invalid_unicode = char32_t{0x7FFFFFFF};
        auto const input = std::u32string_view(&invalid_unicode, 1);

        CHECK(!parse(input, char_));

        // force unicode category lookup
        // related issue: https://github.com/boostorg/spirit/issues/524
        CHECK(!parse(input, alpha));
        CHECK(!parse(input, upper));
        CHECK(!parse(input, lower));
    }

    {   // test attribute extraction
        using x4::traits::attribute_of_t;
        using x4::standard::alpha;
        static_assert(std::same_as<attribute_of_t<std::remove_const_t<decltype(x4::standard::alpha)>, unused_type>, char>);

        int attr = 0;
        REQUIRE(parse("a", alpha, attr));
        CHECK(attr == 'a');
    }

    {   // test attribute extraction
        using x4::standard::alpha;
        using x4::standard::space;
        char attr = 0;
        REQUIRE(parse("     a", alpha, space, attr));
        CHECK(attr == 'a');
    }

    {   // test action
        using namespace x4::standard;
        using x4::_attr;
        char ch = '\0';
        auto f = [&](auto& ctx){ ch = _attr(ctx); };

        REQUIRE(parse("x", alnum[f]));
        CHECK(ch == 'x');
        REQUIRE(parse("   A", alnum[f], space));
        CHECK(ch == 'A');
    }
}
