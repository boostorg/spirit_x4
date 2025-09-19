/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_CALL_CONTEXT_MAY_26_2014_0234PM
#define BOOST_SPIRIT_X4_CALL_CONTEXT_MAY_26_2014_0234PM

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/context.hpp>

namespace boost::spirit::x4
{
    struct parse_pass_context_tag; // _pass
    struct rule_val_context_tag; // _val
    struct where_context_tag; // _where
    struct attr_context_tag; // _attr

    template <typename Context>
    [[nodiscard]] constexpr bool& _pass(Context const& context) noexcept
    {
        return x4::get<parse_pass_context_tag>(context);
    }

    template <typename Context>
    [[nodiscard]] constexpr auto&& _val(Context const& context) noexcept
    {
        return x4::get<rule_val_context_tag>(context);
    }

    template <typename Context>
    [[nodiscard]] constexpr auto&& _where(Context const& context) noexcept
    {
        return x4::get<where_context_tag>(context);
    }

    template <typename Context>
    [[nodiscard]] constexpr auto&& _attr(Context const& context) noexcept
    {
        return x4::get<attr_context_tag>(context);
    }
} // boost::spirit::x4

#endif
