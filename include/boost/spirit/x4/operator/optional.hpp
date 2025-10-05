#ifndef BOOST_SPIRIT_X4_OPERATOR_OPTIONAL_HPP
#define BOOST_SPIRIT_X4_OPERATOR_OPTIONAL_HPP

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
#include <boost/spirit/x4/core/move_to.hpp>

#include <boost/spirit/x4/traits/attribute.hpp>
#include <boost/spirit/x4/traits/optional_traits.hpp>
#include <boost/spirit/x4/traits/attribute_category.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Subject>
struct optional : unary_parser<Subject, optional<Subject>>
{
    static constexpr bool is_pass_through_unary = false;
    static constexpr bool handles_container = true;

    // catch-all overload
    template<
        std::forward_iterator It, std::sentinel_for<It> Se, class Context,
        class Attr // unconstrained
    >
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, Attr>)
    {
        static_assert(Parsable<Subject, It, Se, Context, Attr>);

        // discard [[nodiscard]]
        (void)this->subject.parse(first, last, ctx, attr);

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return !x4::has_expectation_failure(ctx);
        } else {
            return true;
        }
    }

    // container attribute
    template<
        std::forward_iterator It, std::sentinel_for<It> Se, class Context,
        traits::CategorizedAttr<traits::container_attr> Attr
    >
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(noexcept(detail::parse_into_container(this->subject, first, last, ctx, attr)))
    {
        // discard [[nodiscard]]
        (void)detail::parse_into_container(this->subject, first, last, ctx, attr);

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return !x4::has_expectation_failure(ctx);
        } else {
            return true;
        }
    }

    // optional attribute
    template<
        std::forward_iterator It, std::sentinel_for<It> Se, class Context,
        traits::CategorizedAttr<traits::optional_attr> Attr
    >
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            std::is_nothrow_default_constructible_v<x4::traits::optional_value_t<Attr>> &&
            is_nothrow_parsable_v<Subject, It, Se, Context, x4::traits::optional_value_t<Attr>> &&
            noexcept(x4::move_to(std::declval<x4::traits::optional_value_t<Attr>&&>(), attr))
        )
    {
        using value_type = x4::traits::optional_value_t<Attr>;
        value_type val; // default-initialize

        static_assert(Parsable<Subject, It, Se, Context, value_type>);
        if (this->subject.parse(first, last, ctx, val)) {
            // assign the parsed value into our attribute
            x4::move_to(std::move(val), attr);
            return true;
        }

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return !x4::has_expectation_failure(ctx);
        } else {
            return true;
        }
    }
};

template<X4Subject Subject>
[[nodiscard]] constexpr optional<as_parser_plain_t<Subject>>
operator-(Subject&& subject)
    noexcept(is_parser_nothrow_constructible_v<optional<as_parser_plain_t<Subject>>, Subject>)
{
    return {as_parser(std::forward<Subject>(subject))};
}

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

template<class Subject, class Context>
struct attribute_of<x4::optional<Subject>, Context>
    : build_optional<attribute_of_t<Subject, Context>>
{};

} // boost::spirit::x4::traits

#endif
