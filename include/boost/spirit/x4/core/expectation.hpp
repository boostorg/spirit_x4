#ifndef BOOST_SPIRIT_X4_CORE_EXPECTATION_HPP
#define BOOST_SPIRIT_X4_CORE_EXPECTATION_HPP

/*=============================================================================
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/parser.hpp> // for `x4::what`
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/context.hpp>

#include <concepts>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include <cassert>

namespace boost::spirit::x4 {

namespace contexts {

struct expectation_failure
{
    static constexpr bool is_unique = true;
};

} // contexts

using expectation_failure_tag [[deprecated("Use `x4::contexts::expectation_failure`")]] = contexts::expectation_failure;

template<std::forward_iterator It>
struct expectation_failure
{
    constexpr expectation_failure() = default;

    template<class WhichT>
        requires std::is_constructible_v<std::string, WhichT>
    constexpr expectation_failure(It where, WhichT&& which)
        noexcept(std::is_nothrow_copy_constructible_v<It> && std::is_nothrow_constructible_v<std::string, WhichT>)
        : where_(where)
        , which_(std::forward<WhichT>(which))
    {
        if (which_.empty()) {
            which_ = "(unknown location)";
        }
    }

    [[nodiscard]]
    constexpr It const& where() const noexcept
    {
        assert(this->has_value());
        return where_;
    }

    [[nodiscard]]
    constexpr std::string const& which() const noexcept
    {
        assert(this->has_value());
        return which_;
    }

    constexpr void clear() noexcept
    {
        which_.clear();
    }

    template<class WhichT>
        requires std::is_constructible_v<std::string, WhichT>
    constexpr void emplace(It where, WhichT&& which)
        noexcept(std::is_nothrow_move_assignable_v<It> && std::is_nothrow_assignable_v<std::string&, WhichT>)
    {
        where_ = std::move(where);
        which_ = std::forward<WhichT>(which);
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept { return !which_.empty(); }
    [[nodiscard]] constexpr bool has_value() const noexcept { return !which_.empty(); }

    constexpr void swap(expectation_failure& other)
        noexcept(std::is_nothrow_swappable_v<It> && std::is_nothrow_swappable_v<std::string>)
    {
        using std::swap;
        swap(where_, other.where_);
        swap(which_, other.which_);
    }

private:
    It where_{};
    std::string which_;
};

template<std::forward_iterator It>
constexpr void swap(expectation_failure<It>& a, expectation_failure<It>& b)
    noexcept(std::is_nothrow_swappable_v<It> && std::is_nothrow_swappable_v<std::string>)
{
    a.swap(b);
}

template<class Context>
using expectation_failure_t = get_context_plain_t<contexts::expectation_failure, Context>;

template<class Context>
[[nodiscard]]
constexpr bool has_expectation_failure(Context const& ctx) noexcept
{
    static_assert(
        has_context_v<Context, contexts::expectation_failure>,
        "Context type was not specified for `x4::contexts::expectation_failure`. "
        "You probably forgot: `x4::with<x4::contexts::expectation_failure>(failure)[p]`. "
        "Note that you must also bind the context to your skipper."
    );
    return x4::get<contexts::expectation_failure>(ctx).has_value();
}

//
// Creation of a brand-new expectation_failure instance.
// This is the primary overload.
//
template<std::forward_iterator It, class Subject, class Context>
constexpr void set_expectation_failure(
    It where,
    Subject const& subject,
    Context const& ctx
)
    noexcept(noexcept(x4::get<contexts::expectation_failure>(ctx).emplace(std::move(where), x4::what(subject))))
{
    static_assert(
        has_context_v<Context, contexts::expectation_failure>,
        "Context type was not specified for `x4::contexts::expectation_failure`. "
        "You probably forgot: `x4::with<x4::contexts::expectation_failure>(failure)[p]`. "
        "Note that you must also bind the context to your skipper."
    );
    x4::get<contexts::expectation_failure>(ctx).emplace(std::move(where), x4::what(subject));
}

template<class Context>
[[nodiscard]]
constexpr decltype(auto) get_expectation_failure(Context const& ctx) noexcept
{
    static_assert(
        has_context_v<Context, contexts::expectation_failure>,
        "Context type was not specified for `x4::contexts::expectation_failure`. "
        "You probably forgot: `x4::with<x4::contexts::expectation_failure>(failure)[p]`. "
        "Note that you must also bind the context to your skipper."
    );

    return x4::get<contexts::expectation_failure>(ctx);
}

template<class Context>
constexpr void clear_expectation_failure(Context const& ctx) noexcept
{
    static_assert(
        has_context_v<Context, contexts::expectation_failure>,
        "Context type was not specified for `x4::contexts::expectation_failure`. "
        "You probably forgot: `x4::with<x4::contexts::expectation_failure>(failure)[p]`. "
        "Note that you must also bind the context to your skipper."
    );
    x4::get<contexts::expectation_failure>(ctx).clear();
}

} // boost::spirit::x4

#endif
