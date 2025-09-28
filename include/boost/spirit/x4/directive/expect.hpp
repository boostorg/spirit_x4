#ifndef BOOST_SPIRIT_X4_DIRECTIVE_EXPECT_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_EXPECT_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/detail/parse_into_container.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Subject>
struct expect_directive : unary_parser<Subject, expect_directive<Subject>>
{
    using base_type = unary_parser<Subject, expect_directive<Subject>>;

    static constexpr bool is_pass_through_unary = true;

    template<class SubjectT>
        requires
            (!std::is_same_v<std::remove_cvref_t<SubjectT>, expect_directive>) &&
            std::is_constructible_v<base_type, SubjectT>
    constexpr expect_directive(SubjectT&& subject)
        noexcept(std::is_nothrow_constructible_v<base_type, SubjectT>)
        : base_type(std::forward<SubjectT>(subject))
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        // never noexcept; expectation failure requires construction of debug information
    {
        static_assert(
            has_context_v<Context, contexts::expectation_failure>,
            "Context type was not specified for `x4::contexts::expectation_failure`. "
            "You probably forgot: `x4::with<x4::contexts::expectation_failure>(failure)[p]`. "
            "Note that you must also bind the context to your skipper."
        );

        bool const r = this->subject.parse(first, last, ctx, attr);

        // only the first failure is needed
        if (!r && !x4::has_expectation_failure(ctx)) {
            x4::set_expectation_failure(first, this->subject, ctx);
        }
        return r;
    }
};

namespace detail {

struct expect_gen
{
    template<X4Subject Subject>
    [[nodiscard]] constexpr expect_directive<as_parser_plain_t<Subject>>
    operator[](Subject&& subject) const
        noexcept(is_parser_nothrow_constructible_v<expect_directive<as_parser_plain_t<Subject>>, Subject>)
    {
        return {as_parser(std::forward<Subject>(subject))};
    }
};

} // detail

namespace parsers::directive {

inline constexpr detail::expect_gen expect{};

} // parsers::directive

using parsers::directive::expect;

} // boost::spirit::x4

namespace boost::spirit::x4::detail {

// Special case handling for expect expressions.
template<class Subject, class Context>
struct parse_into_container_impl<expect_directive<Subject>, Context>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
    [[nodiscard]] static constexpr bool
    call(
        expect_directive<Subject> const& parser,
        It& first, Se const& last, Context const& ctx, Attr& attr
    ) // never noexcept; expectation failure requires construction of debug information
    {
        static_assert(
            has_context_v<Context, contexts::expectation_failure>,
            "Context type was not specified for `x4::contexts::expectation_failure`. "
            "You probably forgot: `x4::with<x4::contexts::expectation_failure>(failure)[p]`. "
            "Note that you must also bind the context to your skipper."
        );

        bool const r = detail::parse_into_container(parser.subject, first, last, ctx, attr);

        // only the first error is needed
        if (!r && !x4::has_expectation_failure(ctx)) {
            x4::set_expectation_failure(first, parser.subject, ctx);
        }
        return r;
    }
};

} // boost::spirit::x4::detail

#endif
