#ifndef BOOST_SPIRIT_X4_DIRECTIVE_MATCHES_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_MATCHES_HPP

/*=============================================================================
    Copyright (c) 2015 Mario Lang
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/unused.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Subject>
struct matches_directive : unary_parser<Subject, matches_directive<Subject>>
{
    using attribute_type = bool;

    static constexpr bool has_attribute = true;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            is_nothrow_parsable_v<Subject, It, Se, Context, unused_type> &&
            noexcept(x4::move_to(std::declval<bool const&>(), attr))
        )
    {
        bool const matched = this->subject.parse(first, last, ctx, unused);

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            if (x4::has_expectation_failure(ctx)) return false;
        }

        x4::move_to(matched, attr);
        return true;
    }
};

namespace detail {

struct matches_gen
{
    template<X4Subject Subject>
    [[nodiscard]] constexpr matches_directive<as_parser_plain_t<Subject>>
    operator[](Subject&& subject) const
        noexcept(is_parser_nothrow_constructible_v<matches_directive<as_parser_plain_t<Subject>>, Subject>)
    {
        return {as_parser(std::forward<Subject>(subject))};
    }
};

} // detail

namespace parsers::directive {

inline constexpr detail::matches_gen matches{};

} // parsers::directive

using parsers::directive::matches;

} // boost::spirit::x4

#endif
