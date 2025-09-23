#ifndef BOOST_SPIRIT_X4_TEST_X4_TEST_HPP
#define BOOST_SPIRIT_X4_TEST_X4_TEST_HPP

/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/parse.hpp>

#include <boost/core/lightweight_test.hpp>

#include <iterator>
#include <string_view>
#include <type_traits>
#include <utility>
#include <print>

namespace spirit = boost::spirit;
namespace x4 = spirit::x4;

using x4::unused_type;
using x4::unused;
using x4::unused_container_type;
using x4::unused_container;

namespace spirit_test {

namespace detail {

// Provide `x4::unused` default arg fallback
struct parse_overloads : x4::detail::parse_fn_main
{
    using x4::detail::parse_fn_main::operator();

    // It/Se + Parser
    template<std::forward_iterator It, std::sentinel_for<It> Se, x4::X4Parser<It, Se> Parser>
    static constexpr x4::parse_result<It, Se>
    operator()(It first, Se last, Parser&& p)
    {
        return x4::parse(first, last, std::forward<Parser>(p), x4::unused);
    }

    // parse_result + It/Se + Parser
    template<std::forward_iterator It, std::sentinel_for<It> Se, x4::X4Parser<It, Se> Parser>
    static constexpr void
    operator()(x4::parse_result<It, Se>& res, It first, Se last, Parser&& p)
    {
        return x4::parse(res, first, last, std::forward<Parser>(p), x4::unused);
    }

    // R + Parser
    template<std::ranges::forward_range R, x4::detail::X4RangeParseParser<R> Parser>
    static constexpr x4::parse_result_for<R>
    operator()(R const& r, Parser&& p)
    {
        return x4::parse(r, std::forward<Parser>(p), x4::unused);
    }

    // parse_result + R + Parser
    template<std::ranges::forward_range R, x4::detail::X4RangeParseParser<R> Parser>
    static constexpr void
    operator()(x4::parse_result_for<R>& res, R const& r, Parser&& p)
    {
        return x4::parse(res, r, std::forward<Parser>(p), x4::unused);
    }

    // It/Se + Parser + Skipper
    template<std::forward_iterator It, std::sentinel_for<It> Se, x4::X4Parser<It, Se> Parser, x4::X4ExplicitParser<It, Se> Skipper>
    static constexpr x4::parse_result<It, Se>
    operator()(It first, Se last, Parser&& p, Skipper&& s, x4::root_skipper_flag flag = x4::root_skipper_flag::do_post_skip)
    {
        return x4::parse(first, last, std::forward<Parser>(p), std::forward<Skipper>(s), x4::unused, flag);
    }

    // parse_result + It/Se + Parser + Skipper
    template<std::forward_iterator It, std::sentinel_for<It> Se, x4::X4Parser<It, Se> Parser, x4::X4ExplicitParser<It, Se> Skipper>
    static constexpr void
    operator()(x4::parse_result<It, Se>& res, It first, Se last, Parser&& p, Skipper&& s, x4::root_skipper_flag flag = x4::root_skipper_flag::do_post_skip)
    {
        return x4::parse(res, first, last, std::forward<Parser>(p), std::forward<Skipper>(s), x4::unused, flag);
    }

    // R + Parser + Skipper
    template<
        std::ranges::forward_range R,
        x4::detail::X4RangeParseParser<R> Parser,
        x4::detail::X4RangeParseSkipper<R> Skipper
    >
    static constexpr x4::parse_result_for<R>
    operator()(R const& r, Parser&& p, Skipper&& s, x4::root_skipper_flag flag = x4::root_skipper_flag::do_post_skip)
    {
        return x4::parse(r, std::forward<Parser>(p), std::forward<Skipper>(s), x4::unused, flag);
    }

    // parse_result + R + Parser + Skipper
    template<
        std::ranges::forward_range R,
        x4::detail::X4RangeParseParser<R> Parser,
        x4::detail::X4RangeParseSkipper<R> Skipper
    >
    static constexpr void
    operator()(x4::parse_result_for<R>& res, R const& r, Parser&& p, Skipper&& s, x4::root_skipper_flag flag = x4::root_skipper_flag::do_post_skip)
    {
        return x4::parse(res, r, std::forward<Parser>(p), std::forward<Skipper>(s), x4::unused, flag);
    }
}; // parse_overload

} // detail

inline namespace cpos {

inline constexpr detail::parse_overloads parse{};

} // cpos

template<class Parser>
[[nodiscard]] constexpr bool test_constexpr_copy_move_ctors(Parser const& p)
{
    [[maybe_unused]] Parser copy_ctor(p);
    [[maybe_unused]] Parser move_ctor(std::move(copy_ctor));
    return true;
}

struct stationary
{
    stationary() = default;

    explicit stationary(int i) : val{i} {}
    stationary(stationary const&) = delete;
    stationary(stationary&&) = default;
    stationary& operator=(stationary const&) = delete;
    stationary& operator=(stationary&&) = default;

    stationary& operator=(int i) { val = i; return *this; }

    int val = 0;
};

struct move_only
{
    move_only() = default;
    move_only(move_only&&) = default;
    move_only& operator=(move_only&&) = default;
};

template<class T>
struct synth_parser : x4::parser<synth_parser<T>>
{
    using attribute_type = T;

    static constexpr bool has_attribute = true;
    static constexpr bool handles_container = false;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Attr>
    [[nodiscard]] constexpr bool
    parse(It& iter, Se const& last, Context const&, Attr& attr) const
    {
        if (iter != last && *iter == 's') {
            ++iter;
            x4::move_to(attribute_type{}, attr);
            return true;
        }
        return false;
    }
};

template<class T>
constexpr synth_parser<T> synth{};

constexpr synth_parser<move_only> synth_move_only{};

} // spirit_test

using spirit_test::parse;

#define BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(...) \
    static_assert(::spirit_test::test_constexpr_copy_move_ctors(__VA_ARGS__))

#endif
