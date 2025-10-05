/*=============================================================================
    Copyright (c) 2001-2017 Joel de Guzman
    Copyright (c) 2017 think-cell GmbH
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_SPIRIT_X4_UNICODE

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/negated_char.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/symbols.hpp>
#include <boost/spirit/x4/binary.hpp>
#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eoi.hpp>
#include <boost/spirit/x4/auxiliary/eol.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/with.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/directive/expect.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/directive/matches.hpp>
#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/directive/no_skip.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/directive/raw.hpp>
#include <boost/spirit/x4/directive/repeat.hpp>
#include <boost/spirit/x4/directive/seek.hpp>
#include <boost/spirit/x4/directive/skip.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/numeric/uint.hpp>
#include <boost/spirit/x4/numeric/real.hpp>
#include <boost/spirit/x4/numeric/bool.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/and_predicate.hpp>
#include <boost/spirit/x4/operator/difference.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>
#include <boost/spirit/x4/operator/optional.hpp>

#include <ranges>
#include <algorithm>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

// On failed parse,
//   - The `first` iterator shall point to the previous position, and
//   - The attribute shall not be modified.
//     - Best-effort; there are many exceptions as of now. In other words,
//       rollbacks may or may not occur depending on the situation.
//     - In practice, the current implementation only "holds" the attribute
//       only when it is semantically possible to preliminarily check the
//       parse success on the right hand side of the underlying parser.

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("rollback on failed parse (numeric)")
{
    using x4::int_;
    using x4::uint_;
    using x4::double_;

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -99;
        REQUIRE_FALSE(int_.parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -99);
    }
    {
        constexpr auto input = "-"sv;
        auto first = input.begin();
        int dummy_int = -99;
        REQUIRE_FALSE(int_.parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -99);
    }
    {
        constexpr auto input = "-9999999999999999999999999999999999999"sv; // overflow
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(int_.parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }

    {
        constexpr auto input = "9999999999999999999999999999999999999"sv; // overflow
        auto first = input.begin();
        unsigned dummy_uint = static_cast<unsigned>(-1);
        REQUIRE_FALSE(uint_.parse(first, input.end(), unused, dummy_uint));
        CHECK(first == input.begin());
        CHECK(dummy_uint == static_cast<unsigned>(-1));
    }

    // TODO: https://github.com/boostorg/spirit_x4/issues/63
    //{
    //    //constexpr double aaa = 9.9e999999999999999999999; // error
    //    constexpr auto input = "9.9e999999999999999999999"sv; // overflow
    //    auto first = input.begin();
    //    double dummy_double = 3.14;
    //    REQUIRE_FALSE(double_.parse(first, input.end(), unused, dummy_double));
    //    CHECK(first == input.begin());
    //    CHECK(dummy_double == static_cast<double>(3.14));
    //}
}

TEST_CASE("rollback on failed parse (char)")
{
    using x4::char_;
    using x4::lit;

    {
        constexpr auto input = U"\x00110000"sv; // not a valid Unicode code point
        auto first = input.begin();
        char32_t dummy_char = U'd';
        REQUIRE_FALSE(x4::unicode::char_.parse(first, input.end(), unused, dummy_char)); // NOLINT(readability-static-accessed-through-instance)
        CHECK(first == input.begin());
        CHECK(dummy_char == U'd');
    }
    {
        constexpr auto input = "x"sv;
        auto first = input.begin();
        char dummy_char = 'd';
        REQUIRE_FALSE(char_('a').parse(first, input.end(), unused, dummy_char));
        CHECK(first == input.begin());
        CHECK(dummy_char == 'd');
    }

    {
        constexpr auto input = "1"sv;
        auto first = input.begin();
        char dummy_char = 'd';
        REQUIRE_FALSE(char_('a', 'z').parse(first, input.end(), unused, dummy_char));
        CHECK(first == input.begin());
        CHECK(dummy_char == 'd');
    }
    {
        constexpr auto input = "1"sv;
        auto first = input.begin();
        char dummy_char = 'd';
        REQUIRE_FALSE(char_("a-z").parse(first, input.end(), unused, dummy_char));
        CHECK(first == input.begin());
        CHECK(dummy_char == 'd');
    }
    {
        constexpr auto input = "1"sv;
        auto first = input.begin();
        char dummy_char = 'd';
        REQUIRE_FALSE((~char_).parse(first, input.end(), unused, dummy_char));
        CHECK(first == input.begin());
        CHECK(dummy_char == 'd');
    }
    {
        constexpr auto input = "1"sv;
        auto first = input.begin();
        char dummy_char = 'd';
        REQUIRE_FALSE(lit('a').parse(first, input.end(), unused, dummy_char));
        CHECK(first == input.begin());
        CHECK(dummy_char == 'd');
    }
}

TEST_CASE("rollback on failed parse (string)")
{
    using x4::string;
    using x4::lit;
    using x4::unique_symbols;

    {
        constexpr auto input = "x"sv;
        auto first = input.begin();
        REQUIRE_FALSE(lit("foo").parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "x"sv;
        auto first = input.begin();
        std::string dummy_string = "dummy";
        REQUIRE_FALSE(string("foo").parse(first, input.end(), unused, dummy_string));
        CHECK(first == input.begin());
        CHECK(dummy_string == "dummy");
    }
    {
        unique_symbols<int> syms{{"foo", 0}, {"bar", 1}};
        constexpr auto input = "baz"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(syms.parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
}

TEST_CASE("rollback on failed parse (action)")
{
    using x4::eps;
    using x4::int_;
    using x4::_pass;

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(eps[([](auto& ctx) { _pass(ctx) = false; })].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(int_[([](auto& ctx) { _pass(ctx) = false; })].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser itself succeeds; always results in side effect
    }
    {
        constexpr auto input = "42,43"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE((int_ >> ',' >> int_)[([](auto& ctx) { _pass(ctx) = false; })].parse(first, input.end(), unused, dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{42, 43}); // sequence parser itself succeeds; always results in side effect
    }
}

TEST_CASE("rollback on failed parse (auxiliary)")
{
    using x4::attr;
    using x4::eps;
    using x4::eoi;
    using x4::eol;

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE((attr(42) >> eps(false)).parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser has side effect because attribute is not a container
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(eoi.parse(first, input.end(), unused, unused));
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(eol.parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(eps(false).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(eps([]{ return false; }).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
}

TEST_CASE("rollback on failed parse (directive)")
{
    using It = std::string_view::const_iterator;

    using x4::int_;
    using x4::true_;
    using x4::eps;
    using x4::standard::space;

    using x4::expect;
    using x4::lexeme;
    using x4::matches;
    using x4::no_case;
    using x4::no_skip;
    using x4::omit;
    using x4::raw;
    using x4::repeat;
    using x4::seek;
    using x4::skip;
    using x4::reskip;
    using x4::with;

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        x4::expectation_failure<It> failure;
        auto const ctx = x4::make_context<x4::contexts::expectation_failure>(failure);
        REQUIRE_FALSE(expect[eps(false)].parse(first, input.end(), ctx, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        x4::expectation_failure<It> failure;
        auto const ctx = x4::make_context<x4::contexts::expectation_failure>(failure);
        int dummy_int = -1;
        REQUIRE_FALSE(expect[int_].parse(first, input.end(), ctx, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42!"sv;
        auto first = input.begin();
        x4::expectation_failure<It> failure;
        auto const ctx = x4::make_context<x4::contexts::expectation_failure>(failure);
        int dummy_int = -1;
        REQUIRE_FALSE((int_ >> expect['i']).parse(first, input.end(), ctx, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser has side effect because attribute is not a container
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        x4::expectation_failure<It> failure;
        auto const ctx = x4::make_context<x4::contexts::expectation_failure>(failure);
        std::vector<int> dummy_ints;
        REQUIRE_FALSE((int_ >> expect[','] >> int_).parse(first, input.end(), ctx, dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{}); // sequence parser has NO side effect because attribute is a container
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(lexeme[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(lexeme[int_ >> eps(false)].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser has side effect because attribute is not a container
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE(lexeme[int_ >> ',' >> int_].parse(first, input.end(), unused, dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{}); // sequence parser has NO side effect because attribute is a container
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE(matches[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        bool dummy_bool = false;
        REQUIRE(matches[int_].parse(first, input.end(), unused, dummy_bool));
        CHECK(first == input.end());
        CHECK(dummy_bool == true);
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        bool dummy_bool = true;
        REQUIRE(matches[int_].parse(first, input.end(), unused, dummy_bool));
        CHECK(first == input.begin());
        CHECK(dummy_bool == false);
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(no_case[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(no_case[int_].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(no_case[int_ >> eps(false)].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser has side effect because attribute is not a container
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE(no_case[int_ >> ',' >> int_].parse(first, input.end(), unused, dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{}); // sequence parser has NO side effect because attribute is a container
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(no_skip[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(no_skip[int_].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(no_skip[int_ >> eps(false)].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser has side effect because attribute is not a container
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE(no_skip[int_ >> ',' >> int_].parse(first, input.end(), unused, dummy_ints));
        CHECK(first == input.begin()); // sequence parser has NO side effect because attribute is not a container
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(omit[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(omit[int_].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(omit[int_ >> eps(false)].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1); // `omit` never yields an attribute
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        std::ranges::subrange<It> dummy_subrange{input.end(), input.end()};
        REQUIRE_FALSE(raw[eps(false)].parse(first, input.end(), unused, dummy_subrange));
        CHECK(first == input.begin());
        CHECK(dummy_subrange.begin() == input.end());
        CHECK(dummy_subrange.end() == input.end());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        std::ranges::subrange<It> dummy_subrange{input.end(), input.end()};
        REQUIRE_FALSE(raw[int_].parse(first, input.end(), unused, dummy_subrange));
        CHECK(first == input.begin());
        CHECK(dummy_subrange.begin() == input.end());
        CHECK(dummy_subrange.end() == input.end());
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        std::ranges::subrange<It> dummy_subrange{input.end(), input.end()};
        REQUIRE_FALSE(raw[int_ >> eps(false)].parse(first, input.end(), unused, dummy_subrange));
        CHECK(first == input.begin());
        CHECK(dummy_subrange.begin() == input.end());
        CHECK(dummy_subrange.end() == input.end());
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(repeat(1)[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE(repeat(1)[int_ >> eps(false)].parse(first, input.end(), unused, dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{});
    }
    {
        constexpr auto input = "true123"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE(repeat(1)[true_ >> true_].parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{true}); // sequence parser has side effect
    }
    {
        constexpr auto input = "true123"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE(repeat(2)[true_].parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{true}); // sequence parser has side effect
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(seek[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(seek[int_].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(seek[int_ >> eps(false)].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 2); // `seek` has side effect
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(skip(space)[eps(false)].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(skip(space)[int_].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(skip(space)[int_ >> eps(false)].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42 foo"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE(skip(space)[int_ >> int_].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{42}); // sequence parser has side effect
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(reskip[eps(false)].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(reskip[int_].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(reskip[int_ >> eps(false)].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42 foo"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE(reskip[int_ >> int_].parse(first, input.end(), x4::make_context<x4::contexts::skipper>(space), dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{42}); // sequence parser has side effect
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE(with<struct with_id_>(input)[eps(false)].parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(with<struct with_id_>(input)[int_].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE(with<struct with_id_>(input)[int_ >> eps(false)].parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser has side effect
    }
}

TEST_CASE("rollback on failed parse (operator)")
{
    using x4::int_;
    using x4::true_;
    using x4::false_;
    using x4::eps;

    // `sequence` should be tested first because it is the requirement for subsequent test cases
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((eps >> eps(false)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((true_ >> true_).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{true}); // `sequence` parser exposes the side effects
    }

    // -----------------------------------------------

    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE((int_ >> eps(false)).parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == 42); // sequence parser has side effect because attribute is not a container
    }
    {
        constexpr auto input = "42,43"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE((int_ >> eps(false) >> int_).parse(first, input.end(), unused, dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{}); // sequence parser has NO side effect because LHS is `sequence<int_, eps(false)>`, which does not move the result until `eps(false)` is evaluated
    }
    {
        constexpr auto input = "42,43"sv;
        auto first = input.begin();
        std::vector<int> dummy_ints;
        REQUIRE_FALSE((int_ >> (eps(false) >> int_)).parse(first, input.end(), unused, dummy_ints));
        CHECK(first == input.begin());
        CHECK(dummy_ints == std::vector<int>{42}); // sequence parser has side effect, in contrast to above
    }

    // NOLINTBEGIN(misc-redundant-expression)
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((eps(false) | eps(false)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE((eps(false) | int_).parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE((eps(false) | int_ >> eps(false)).parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1); // `alternative` parser shall not expose the side effects on the failed branch
    }
    {
        constexpr auto input = "true"sv;
        auto first = input.begin();
        bool dummy_bool = true;
        REQUIRE_FALSE((false_ | false_).parse(first, input.end(), unused, dummy_bool));
        CHECK(first == input.begin());
        CHECK(dummy_bool == true);
    }
    {
        constexpr auto input = "true"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((true_ >> false_ | true_ >> false_).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{});
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((eps - eps).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE((int_ - eps).parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1);
    }
    {
        constexpr auto input = "42"sv;
        auto first = input.begin();
        int dummy_int = -1;
        REQUIRE_FALSE((int_ - eps).parse(first, input.end(), unused, dummy_int));
        CHECK(first == input.begin());
        CHECK(dummy_int == -1); // `difference` parser shall not expose the side effects
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE(((true_ >> true_) - eps).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{}); // `difference` parser shall not expose the side effects
    }
    {
        constexpr auto input = "truetrue"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE(((true_ >> true_) - eps).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{}); // `difference` parser shall not expose the side effects
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((*eps(false) >> eps(false)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((*true_ >> eps(false)).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{true}); // `kleene` parser (within sequence) exposes the side effects
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((+eps(false)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((+true_ >> eps(false)).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{true}); // `plus` parser (within sequence) exposes the side effects
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((eps(false) % eps(false)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "false"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((true_ % eps(false)).parse(first, input.end(), unused, dummy_bools)); // initial unmatch shall result in total parse failure
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "false"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((true_ % eps(true)).parse(first, input.end(), unused, dummy_bools)); // initial unmatch shall result in total parse failure (even if the delimiter parse succeeds)
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((true_ % eps(false) >> eps(false)).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{true});// `list` parser (within sequence) exposes the side effects
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((&eps(false)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "true"sv;
        auto first = input.begin();
        REQUIRE_FALSE((&false_).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        REQUIRE_FALSE((&(true_ >> true_)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((!eps).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "true"sv;
        auto first = input.begin();
        REQUIRE_FALSE((!true_).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "truetrue"sv;
        auto first = input.begin();
        REQUIRE_FALSE((!(true_ >> true_)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }

    {
        constexpr auto input = "foo"sv;
        auto first = input.begin();
        REQUIRE_FALSE((-eps >> eps(false)).parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "false"sv;
        auto first = input.begin();
        bool dummy_bool = false;
        REQUIRE_FALSE((-true_ >> eps(false)).parse(first, input.end(), unused, dummy_bool));
        CHECK(first == input.begin());
        CHECK(dummy_bool == false);
    }
    {
        constexpr auto input = "true"sv;
        auto first = input.begin();
        bool dummy_bool = false;
        REQUIRE_FALSE((-true_ >> eps(false)).parse(first, input.end(), unused, dummy_bool));
        CHECK(first == input.begin());
        CHECK(dummy_bool == true);
    }
    {
        constexpr auto input = "false"sv;
        auto first = input.begin();
        std::optional<bool> dummy_optional_bool = false;
        REQUIRE_FALSE((-true_ >> eps(false)).parse(first, input.end(), unused, dummy_optional_bool));
        CHECK(first == input.begin());
        REQUIRE(dummy_optional_bool.has_value());
        CHECK(*dummy_optional_bool == false);
    }
    {
        constexpr auto input = "true"sv;
        auto first = input.begin();
        std::optional<bool> dummy_optional_bool = false;
        REQUIRE_FALSE((-true_ >> eps(false)).parse(first, input.end(), unused, dummy_optional_bool));
        CHECK(first == input.begin());
        REQUIRE(dummy_optional_bool.has_value());
        CHECK(*dummy_optional_bool == true);
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        std::vector<bool> dummy_bools;
        REQUIRE_FALSE((-(true_ >> true_) >> eps(false)).parse(first, input.end(), unused, dummy_bools));
        CHECK(first == input.begin());
        CHECK(dummy_bools == std::vector<bool>{true});
    }
    {
        constexpr auto input = "truefalse"sv;
        auto first = input.begin();
        std::optional<std::vector<bool>> dummy_optional_bools;
        REQUIRE_FALSE((-(true_ >> true_) >> eps(false)).parse(first, input.end(), unused, dummy_optional_bools));
        CHECK(first == input.begin());
        CHECK(dummy_optional_bools.has_value() == false); // `optional` parser for `optional<container attribute>` shall not expose the side effects
    }

    // NOLINTEND(misc-redundant-expression)
}

TEST_CASE("rollback on failed parse (rule)")
{
    using x4::lit;

    {
        constexpr auto input = "ab"sv;
        auto first = input.begin();
        constexpr x4::rule<struct _, unused_type> r("r");
        constexpr auto p = r = lit('a') >> r;
        REQUIRE_FALSE(p.parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
    {
        constexpr auto input = "ab"sv;
        auto first = input.begin();
        constexpr x4::rule<struct _, unused_type> r("r");
        constexpr auto p = r %= lit('a') >> r;
        REQUIRE_FALSE(p.parse(first, input.end(), unused, unused));
        CHECK(first == input.begin());
    }
}

TEST_CASE("transform iterator")
{
    using x4::raw;
    using x4::eps;
    using x4::eoi;
    using x4::standard::upper;
    using x4::repeat;

    std::string input = "abcde";
    auto const rng = input | std::views::transform([](char c) {
        return c < 'a' || 'z' < c ? c : static_cast<char>(c - 'a' + 'A');
    });
    using range_type = decltype(rng);
    static_assert(std::ranges::forward_range<range_type>);

    {
        std::string str;
        REQUIRE(parse(std::ranges::begin(rng), std::ranges::end(rng), +upper >> eoi, str));
        CHECK("ABCDE" == str);
    }

    {
        std::ranges::subrange<std::ranges::iterator_t<range_type>> str;

        REQUIRE(parse(std::ranges::begin(rng), std::ranges::end(rng), raw[+upper >> eoi], str));
        CHECK(std::ranges::equal(std::string("ABCDE"), str));
    }

    CHECK(parse(std::ranges::begin(rng), std::ranges::end(rng), (repeat(6)[upper] | repeat(5)[upper]) >> eoi));
}

// NOLINTEND(readability-container-size-empty)
