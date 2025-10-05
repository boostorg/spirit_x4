#ifndef BOOST_SPIRIT_X4_OPERATOR_LIST_HPP
#define BOOST_SPIRIT_X4_OPERATOR_LIST_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/detail/parse_into_container.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/unused.hpp>

#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/attribute.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Left, class Right>
struct list : binary_parser<Left, Right, list<Left, Right>>
{
    static constexpr bool handles_container = true;

    using binary_parser<Left, Right, list>::binary_parser;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            noexcept(detail::parse_into_container(this->left, first, last, ctx, x4::assume_container(attr))) &&
            std::is_nothrow_copy_assignable_v<It> &&
            is_nothrow_parsable_v<Right, It, Se, Context, unused_type>
        )
    {
        // In order to succeed we need to match at least one element
        if (!detail::parse_into_container(this->left, first, last, ctx, x4::assume_container(attr))) {
            return false;
        }

        It last_parse_it = first;
        while (
            this->right.parse(last_parse_it, last, ctx, unused) &&
            detail::parse_into_container(this->left, last_parse_it, last, ctx, x4::assume_container(attr))
        ) {
            // TODO: can we reduce this copy assignment?
            first = last_parse_it;
        }

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return !x4::has_expectation_failure(ctx);
        } else {
            return true;
        }
    }
};

template<X4Subject Left, X4Subject Right>
[[nodiscard]] constexpr list<as_parser_plain_t<Left>, as_parser_plain_t<Right>>
operator%(Left&& left, Right&& right)
    noexcept(
        is_parser_nothrow_castable_v<Left> &&
        is_parser_nothrow_castable_v<Right> &&
        std::is_nothrow_constructible_v<
            list<as_parser_plain_t<Left>, as_parser_plain_t<Right>>,
            as_parser_t<Left>,
            as_parser_t<Right>
        >
    )
{
    return {as_parser(std::forward<Left>(left)), as_parser(std::forward<Right>(right))};
}

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

template<class Left, class Right, class Context>
struct attribute_of<x4::list<Left, Right>, Context>
    : traits::build_container<attribute_of_t<Left, Context>>
{};

template<class Left, class Right, class Context>
struct has_attribute<x4::list<Left, Right>, Context>
    : has_attribute<Left, Context>
{};

} // boost::spirit::x4::traits

#endif
