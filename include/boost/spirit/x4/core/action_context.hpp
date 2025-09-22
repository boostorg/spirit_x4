#ifndef BOOST_SPIRIT_X4_CORE_ACTION_CONTEXT_HPP
#define BOOST_SPIRIT_X4_CORE_ACTION_CONTEXT_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/context.hpp>

namespace boost::spirit::x4 {

namespace contexts {

// _pass
struct parse_pass
{
    static constexpr bool is_unique = true;
};

// _val
struct rule_val
{
    static constexpr bool is_unique = true;
};

// _where
struct where
{
    static constexpr bool is_unique = true;
};

// _attr
struct attr
{
    static constexpr bool is_unique = true;
};

} // contexts

using parse_pass_context_tag [[deprecated("Use `x4::contexts::parse_pass`")]] = contexts::parse_pass;
using rule_val_context_tag [[deprecated("Use `x4::contexts::rule_val`")]] = contexts::rule_val;
using where_context_tag [[deprecated("Use `x4::contexts::where`")]] = contexts::where;
using attr_context_tag [[deprecated("Use `x4::contexts::attr`")]] = contexts::attr;


namespace detail {

struct _pass_fn
{
    template<class Context>
    [[nodiscard]] static constexpr bool&
    operator()(Context const& ctx BOOST_SPIRIT_LIFETIMEBOUND) noexcept
    {
        return x4::get<contexts::parse_pass>(ctx);
    }

    template<class Context>
    static void operator()(Context const&&) = delete; // dangling
};

struct _val_fn
{
    template<class Context>
    [[nodiscard]] static constexpr auto&&
    operator()(Context const& ctx BOOST_SPIRIT_LIFETIMEBOUND) noexcept
    {
        return x4::get<contexts::rule_val>(ctx);
    }

    template<class Context>
    static void operator()(Context const&&) = delete; // dangling
};

struct _where_fn
{
    template<class Context>
    [[nodiscard]] static constexpr auto&&
    operator()(Context const& ctx BOOST_SPIRIT_LIFETIMEBOUND) noexcept
    {
        return x4::get<contexts::where>(ctx);
    }

    template<class Context>
    static void operator()(Context const&&) = delete; // dangling
};

struct _attr_fn
{
    template<class Context>
    [[nodiscard]] static constexpr auto&&
    operator()(Context const& ctx BOOST_SPIRIT_LIFETIMEBOUND) noexcept
    {
        return x4::get<contexts::attr>(ctx);
    }

    template<class Context>
    static void operator()(Context const&&) = delete; // dangling
};

} // detail

inline namespace cpos {

inline constexpr detail::_pass_fn _pass{};
inline constexpr detail::_val_fn _val{};
inline constexpr detail::_where_fn _where{};
inline constexpr detail::_attr_fn _attr{};

} // cpos

} // boost::spirit::x4

#endif
