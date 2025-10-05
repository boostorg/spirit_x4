#ifndef BOOST_SPIRIT_X4_OPERATOR_NOT_PREDICATE_HPP
#define BOOST_SPIRIT_X4_OPERATOR_NOT_PREDICATE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/expectation.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Subject>
struct not_predicate : unary_parser<Subject, not_predicate<Subject>>
{
    using attribute_type = unused_type;

    static constexpr bool has_attribute = false;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& /*attr*/) const
        noexcept(
            std::is_nothrow_copy_assignable_v<It> &&
            is_nothrow_parsable_v<Subject, It, Se, Context, unused_type>
        )
    {
        It local_first = first;

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return !this->subject.parse(local_first, last, ctx, unused) &&
                !x4::has_expectation_failure(ctx);
        } else {
            return !this->subject.parse(local_first, last, ctx, unused);
        }
    }
};

template<X4Subject Subject>
[[nodiscard]] constexpr not_predicate<as_parser_plain_t<Subject>>
operator!(Subject&& subject)
    noexcept(is_parser_nothrow_constructible_v<not_predicate<as_parser_plain_t<Subject>>, Subject>)
{
    return {as_parser(std::forward<Subject>(subject))};
}

} // boost::spirit::x4

#endif
