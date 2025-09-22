#ifndef BOOST_SPIRIT_X4_DEBUG_ANNOTATE_ON_SUCCESS_HPP
#define BOOST_SPIRIT_X4_DEBUG_ANNOTATE_ON_SUCCESS_HPP

/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/attribute.hpp>

#include <concepts>
#include <iterator>
#include <type_traits>

namespace boost::spirit::x4 {

namespace contexts {

struct error_handler;

} // contexts

//  The on_success handler tags the AST with the iterator position
//  for error handling.
//
//  The on_success handler also ties the AST to a vector of iterator
//  positions for the purpose of subsequent semantic error handling
//  when the program is being compiled. See also `x4::position_cache`.
//
//  We'll ask the X4's error_handler utility to do these.

struct annotate_on_success
{
    // Catch-all default overload
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    constexpr void
    on_success(It const& first, Se const& last, Context const& ctx, Attr& ast)
    {
        auto&& error_handler_ref = x4::get<contexts::error_handler>(ctx);

        static_assert(
            !std::same_as<std::remove_cvref_t<decltype(error_handler_ref)>, unused_type>,
            "This rule is derived from `x4::annotate_on_success`, but no reference was bound to "
            "`x4::contexts::error_handler`. You must provide a viable error handler via `x4::with`."
        );

        // unwrap `reference_wrapper` if necessary
        if constexpr (requires {
            error_handler_ref.get().tag(ast, first, last);
        }) {
            error_handler_ref.get().tag(ast, first, last);

        } else {
            error_handler_ref.tag(ast, first, last);
        }
    }
};

} // boost::spirit::x4

#endif
