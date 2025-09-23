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

int main()
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
        BOOST_TEST(parse("1", alnum));
        BOOST_TEST(!parse(" ", alnum));
        BOOST_TEST(!parse("1", alpha));
        BOOST_TEST(parse("x", alpha));
        BOOST_TEST(parse(" ", blank));
        BOOST_TEST(!parse("x", blank));
        BOOST_TEST(parse("1", digit));
        BOOST_TEST(!parse("x", digit));
        BOOST_TEST(parse("a", lower));
        BOOST_TEST(!parse("A", lower));
        BOOST_TEST(parse("!", punct));
        BOOST_TEST(!parse("x", punct));
        BOOST_TEST(parse(" ", space));
        BOOST_TEST(parse("\n", space));
        BOOST_TEST(parse("\r", space));
        BOOST_TEST(parse("\t", space));
        BOOST_TEST(parse("A", upper));
        BOOST_TEST(!parse("a", upper));
        BOOST_TEST(parse("A", xdigit));
        BOOST_TEST(parse("0", xdigit));
        BOOST_TEST(parse("f", xdigit));
        BOOST_TEST(!parse("g", xdigit));
        BOOST_TEST(!parse("\xF1", print));
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
        BOOST_TEST(parse(L"1", alnum));
        BOOST_TEST(!parse(L" ", alnum));
        BOOST_TEST(!parse(L"1", alpha));
        BOOST_TEST(parse(L"x", alpha));
        BOOST_TEST(parse(L" ", blank));
        BOOST_TEST(!parse(L"x", blank));
        BOOST_TEST(parse(L"1", digit));
        BOOST_TEST(!parse(L"x", digit));
        BOOST_TEST(parse(L"a", lower));
        BOOST_TEST(!parse(L"A", lower));
        BOOST_TEST(parse(L"!", punct));
        BOOST_TEST(!parse(L"x", punct));
        BOOST_TEST(parse(L" ", space));
        BOOST_TEST(parse(L"\n", space));
        BOOST_TEST(parse(L"\r", space));
        BOOST_TEST(parse(L"\t", space));
        BOOST_TEST(parse(L"A", upper));
        BOOST_TEST(!parse(L"a", upper));
        BOOST_TEST(parse(L"A", xdigit));
        BOOST_TEST(parse(L"0", xdigit));
        BOOST_TEST(parse(L"f", xdigit));
        BOOST_TEST(!parse(L"g", xdigit));
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
        BOOST_TEST(parse(U"1", alnum));
        BOOST_TEST(!parse(U" ", alnum));
        BOOST_TEST(!parse(U"1", alpha));
        BOOST_TEST(parse(U"x", alpha));
        BOOST_TEST(parse(U" ", blank));
        BOOST_TEST(!parse(U"x", blank));
        BOOST_TEST(parse(U"1", digit));
        BOOST_TEST(!parse(U"x", digit));
        BOOST_TEST(parse(U"a", lower));
        BOOST_TEST(!parse(U"A", lower));
        BOOST_TEST(parse(U"!", punct));
        BOOST_TEST(!parse(U"x", punct));
        BOOST_TEST(parse(U" ", space));
        BOOST_TEST(parse(U"\n", space));
        BOOST_TEST(parse(U"\r", space));
        BOOST_TEST(parse(U"\t", space));
        BOOST_TEST(parse(U"A", upper));
        BOOST_TEST(!parse(U"a", upper));
        BOOST_TEST(parse(U"A", xdigit));
        BOOST_TEST(parse(U"0", xdigit));
        BOOST_TEST(parse(U"f", xdigit));
        BOOST_TEST(!parse(U"g", xdigit));

        BOOST_TEST(parse(U"A", alphabetic));
        BOOST_TEST(parse(U"9", decimal_number));
        BOOST_TEST(parse(U"\u2800", braille));
        BOOST_TEST(!parse(U" ", braille));
        BOOST_TEST(parse(U" ", ~braille));
        // TODO: Add more unicode tests
    }

    {   // test invalid unicode literals
        using namespace x4::unicode;

        constexpr auto invalid_unicode = char32_t{0x7FFFFFFF};
        auto const input = std::u32string_view(&invalid_unicode, 1);

        BOOST_TEST(!parse(input, char_));

        // force unicode category lookup
        // related issue: https://github.com/boostorg/spirit/issues/524
        BOOST_TEST(!parse(input, alpha));
        BOOST_TEST(!parse(input, upper));
        BOOST_TEST(!parse(input, lower));
    }

    {   // test attribute extraction
        using x4::traits::attribute_of_t;
        using x4::standard::alpha;
        using x4::standard::alpha_type;

        static_assert(std::same_as<attribute_of_t<alpha_type, unused_type>, char>);

        int attr = 0;
        BOOST_TEST(parse("a", alpha, attr));
        BOOST_TEST(attr == 'a');
    }

    {   // test attribute extraction
        using x4::standard::alpha;
        using x4::standard::space;
        char attr = 0;
        BOOST_TEST(parse("     a", alpha, space, attr));
        BOOST_TEST(attr == 'a');
    }

    {   // test action
        using namespace x4::standard;
        using x4::_attr;
        char ch = '\0';
        auto f = [&](auto& ctx){ ch = _attr(ctx); };

        BOOST_TEST(parse("x", alnum[f]));
        BOOST_TEST(ch == 'x');
        BOOST_TEST(parse("   A", alnum[f], space));
        BOOST_TEST(ch == 'A');
    }

    return boost::report_errors();
}
