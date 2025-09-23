/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2019 Christian Mazakas
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
#include <boost/spirit/x4/operator/plus.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

int main()
{
    static_assert(x4::traits::is_container_v<std::string>);
    static_assert(x4::traits::X4Container<std::string>);
    static_assert(x4::traits::CategorizedAttr<std::string, x4::traits::container_attr>);

    static_assert(!x4::traits::is_container_v<std::string_view>);
    static_assert(!x4::traits::X4Container<std::string_view>);
    static_assert(x4::traits::CategorizedAttr<std::string_view, x4::traits::plain_attr>);

    namespace standard = x4::standard;
    namespace standard_wide = x4::standard_wide;
    namespace unicode = x4::unicode;

    {
        std::string_view sv;
        auto first = sv.begin();
        auto const last = sv.end();

        constexpr auto parser = standard::char_('x');
        char ch{};
        static_assert(noexcept(parser.parse(first, last, unused, ch)));
        (void)parser.parse(first, last, unused, ch);

        // Make sure this is static
        (void)std::remove_const_t<decltype(standard::alnum)>::parse(first, last, unused, ch);
    }
    {
        std::u32string_view sv;
        auto first = sv.begin();
        auto const last = sv.end();
        constexpr auto parser = unicode::char_(U'x');
        char32_t ch{};
        static_assert(noexcept(parser.parse(first, last, unused, ch)));
        (void)parser.parse(first, last, unused, ch);

        // Make sure this is static
        (void)std::remove_const_t<decltype(unicode::alnum)>::parse(first, last, unused, ch);
    }

    {
        using namespace x4::standard;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_('x'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_('a', 'z'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~char_('x'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~char_('a', 'z'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~~char_('x'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~~char_('a', 'z'));

        BOOST_TEST(parse("x", 'x'));
        BOOST_TEST(parse(L"x", L'x'));
        BOOST_TEST(!parse("y", 'x'));
        BOOST_TEST(!parse(L"y", L'x'));

        BOOST_TEST(parse("x", char_));
        BOOST_TEST(parse("x", char_('x')));
        BOOST_TEST(!parse("x", char_('y')));
        BOOST_TEST(parse("x", char_('a', 'z')));
        BOOST_TEST(!parse("x", char_('0', '9')));

        BOOST_TEST(parse("0", char_('0', '9')));
        BOOST_TEST(parse("9", char_('0', '9')));
        BOOST_TEST(!parse("0", ~char_('0', '9')));
        BOOST_TEST(!parse("9", ~char_('0', '9')));

        BOOST_TEST(!parse("x", ~char_));
        BOOST_TEST(!parse("x", ~char_('x')));
        BOOST_TEST(parse(" ", ~char_('x')));
        BOOST_TEST(parse("X", ~char_('x')));
        BOOST_TEST(!parse("x", ~char_('b', 'y')));
        BOOST_TEST(parse("a", ~char_('b', 'y')));
        BOOST_TEST(parse("z", ~char_('b', 'y')));

        BOOST_TEST(parse("x", ~~char_));
        BOOST_TEST(parse("x", ~~char_('x')));
        BOOST_TEST(!parse(" ", ~~char_('x')));
        BOOST_TEST(!parse("X", ~~char_('x')));
        BOOST_TEST(parse("x", ~~char_('b', 'y')));
        BOOST_TEST(!parse("a", ~~char_('b', 'y')));
        BOOST_TEST(!parse("z", ~~char_('b', 'y')));
    }
    {
        using namespace x4::standard_wide;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_(L'x'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_(L'a', L'z'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~char_(L'x'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~char_(L'a', L'z'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~~char_(L'x'));
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(~~char_(L'a', L'z'));

        BOOST_TEST(parse(L"x", char_));
        BOOST_TEST(parse(L"x", char_(L'x')));
        BOOST_TEST(!parse(L"x", char_(L'y')));
        BOOST_TEST(parse(L"x", char_(L'a', L'z')));
        BOOST_TEST(!parse(L"x", char_(L'0', L'9')));

        BOOST_TEST(!parse(L"x", ~char_));
        BOOST_TEST(!parse(L"x", ~char_(L'x')));
        BOOST_TEST(parse(L" ", ~char_(L'x')));
        BOOST_TEST(parse(L"X", ~char_(L'x')));
        BOOST_TEST(!parse(L"x", ~char_(L'b', L'y')));
        BOOST_TEST(parse(L"a", ~char_(L'b', L'y')));
        BOOST_TEST(parse(L"z", ~char_(L'b', L'y')));

        BOOST_TEST(parse(L"x", ~~char_));
        BOOST_TEST(parse(L"x", ~~char_(L'x')));
        BOOST_TEST(!parse(L" ", ~~char_(L'x')));
        BOOST_TEST(!parse(L"X", ~~char_(L'x')));
        BOOST_TEST(parse(L"x", ~~char_(L'b', L'y')));
        BOOST_TEST(!parse(L"a", ~~char_(L'b', L'y')));
        BOOST_TEST(!parse(L"z", ~~char_(L'b', L'y')));
    }

    {
        using namespace x4::standard;

        BOOST_TEST(parse("   x", 'x', space));
        BOOST_TEST(parse("   x", char_, space));
        BOOST_TEST(parse("   x", char_('x'), space));
        BOOST_TEST(!parse("   x", char_('y'), space));
        BOOST_TEST(parse("   x", char_('a', 'z'), space));
        BOOST_TEST(!parse("   x", char_('0', '9'), space));
    }
    {
        using namespace x4::standard_wide;

        BOOST_TEST(parse(L"   x", L'x', space));
        BOOST_TEST(parse(L"   x", char_, space));
        BOOST_TEST(parse(L"   x", char_(L'x'), space));
        BOOST_TEST(!parse(L"   x", char_(L'y'), space));
        BOOST_TEST(parse(L"   x", char_(L'a', L'z'), space));
        BOOST_TEST(!parse(L"   x", char_(L'0', L'9'), space));
    }

    // unicode (normal ASCII)
    {
        using namespace x4::unicode;

        BOOST_TEST(parse(U"abcd", +char_(U"abcd")));
        BOOST_TEST(!parse(U"abcd", +char_(U"qwer")));

        auto const sub_delims = char_(U"!$&'()*+,;=");

        auto const delims = std::vector<std::u32string_view>{
            U"!", U"$", U"&", U"'", U"(", U")", U"*", U"+", U",", U";", U"="
        };

        auto const matched_all_sub_delims =
            std::ranges::all_of(delims, [&](auto const delim) -> bool {
                return parse(delim, sub_delims).completed();
            });

        BOOST_TEST(matched_all_sub_delims);
    }

    // unicode (escaped Unicode char literals)
    {
        using namespace x4::unicode;

        auto const chars = char_(U"\u0024\u00a2\u0939\u20ac\U00010348");

        auto const test_strings = std::vector<std::u32string_view>{
            U"\u0024", U"\u00a2", U"\u0939", U"\u20ac", U"\U00010348"
        };

        auto const bad_test_strings = std::vector<std::u32string_view>{
            U"a", U"B", U"c", U"\u0409"
        };

        auto const all_matched =
            std::ranges::all_of(test_strings, [&](auto const test_str) -> bool {
                return parse(test_str, chars).completed();
            });

        auto const none_matched =
            std::ranges::all_of(bad_test_strings, [&](auto const bad_test_str) -> bool {
                return !parse(bad_test_str, chars).completed();
            });

        BOOST_TEST(all_matched);
        BOOST_TEST(none_matched);
    }

    // single char strings
    {
        BOOST_TEST(parse("x", "x"));
        BOOST_TEST(parse(L"x", L"x"));
        BOOST_TEST(parse("x", standard::char_("x")));
        BOOST_TEST(parse(L"x", standard_wide::char_(L"x")));

        BOOST_TEST(parse("x", standard::char_('a', 'z')));
        BOOST_TEST(parse(L"x", standard_wide::char_(L'a', L'z')));
    }

    // chsets
    {
        BOOST_TEST(parse("x", standard::char_("a-z")));
        BOOST_TEST(!parse("1", standard::char_("a-z")));
        BOOST_TEST(parse("1", standard::char_("a-z0-9")));

        BOOST_TEST(parse(L"x", standard_wide::char_(L"a-z")));
        BOOST_TEST(!parse(L"1", standard_wide::char_(L"a-z")));
        BOOST_TEST(parse(L"1", standard_wide::char_(L"a-z0-9")));

        std::string set = "a-z0-9";
        BOOST_TEST(parse("x", standard::char_(set)));
    }

    return boost::report_errors();
}
