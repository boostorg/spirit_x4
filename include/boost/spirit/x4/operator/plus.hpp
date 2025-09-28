#ifndef BOOST_SPIRIT_X4_OPERATOR_PLUS_HPP
#define BOOST_SPIRIT_X4_OPERATOR_PLUS_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/detail/parse_into_container.hpp>

#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/attribute.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Subject>
struct plus : unary_parser<Subject, plus<Subject>>
{
    using base_type = unary_parser<Subject, plus>;

    static constexpr bool handles_container = true;

    template<class SubjectT>
        requires
            (!std::is_same_v<std::remove_cvref_t<SubjectT>, plus>) &&
            std::is_constructible_v<base_type, SubjectT>
    constexpr plus(SubjectT&& subject)
        noexcept(std::is_nothrow_constructible_v<base_type, SubjectT>)
        : base_type(std::forward<SubjectT>(subject))
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(noexcept(detail::parse_into_container(this->subject, first, last, ctx, x4::assume_container(attr))))
    {
        if (!detail::parse_into_container(this->subject, first, last, ctx, x4::assume_container(attr))) {
            return false;
        }

        while (detail::parse_into_container(this->subject, first, last, ctx, x4::assume_container(attr)))
            /* loop */;

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return !x4::has_expectation_failure(ctx);
        } else {
            return true;
        }
    }
};

template<X4Subject Subject>
[[nodiscard]] constexpr plus<as_parser_plain_t<Subject>>
operator+(Subject&& subject)
    noexcept(is_parser_nothrow_constructible_v<plus<as_parser_plain_t<Subject>>, Subject>)
{
    return {as_parser(std::forward<Subject>(subject))};
}

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

template<class Subject, class Context>
struct attribute_of<x4::plus<Subject>, Context>
    : build_container<attribute_of_t<Subject, Context>>
{};

} // boost::spirit::x4::traits

#endif
