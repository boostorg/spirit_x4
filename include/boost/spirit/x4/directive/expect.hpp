/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_EXPECT_MARCH_16_2012_1024PM
#define BOOST_SPIRIT_X4_EXPECT_MARCH_16_2012_1024PM

#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/detail/parse_into_container.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    template <typename Subject>
    struct expect_directive : unary_parser<Subject, expect_directive<Subject>>
    {
        using base_type = unary_parser<Subject, expect_directive<Subject>>;
        static constexpr bool is_pass_through_unary = true;

        template <typename SubjectT>
            requires std::is_constructible_v<Subject, SubjectT>
        constexpr expect_directive(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const // never noexcept; expectation failure requires construction of debug information
        {
            bool const r = this->subject.parse(first, last, context, rcontext, attr);

            // only the first failure is needed
            if (!r && !x4::has_expectation_failure(context))
            {
                x4::set_expectation_failure(first, this->subject, context);
            }
            return r;
        }
    };

    namespace detail
    {
        struct expect_gen
        {
            template <X4Subject Subject>
            [[nodiscard]] constexpr expect_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const
                noexcept(is_parser_nothrow_constructible_v<expect_directive<as_parser_plain_t<Subject>>, Subject>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::expect_gen expect{};
    } // cpos

} // boost::spirit::x4

namespace boost::spirit::x4::detail
{
    // Special case handling for expect expressions.
    template <typename Subject, typename Context, typename RContext>
    struct parse_into_container_impl<expect_directive<Subject>, Context, RContext>
    {
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        call(
            expect_directive<Subject> const& parser,
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) // never noexcept; expectation failure requires construction of debug information
        {
            bool const r = detail::parse_into_container(
                parser.subject, first, last, context, rcontext, attr);

            // only the first error is needed
            if (!r && !x4::has_expectation_failure(context))
            {
                x4::set_expectation_failure(first, parser.subject, context);
            }
            return r;
        }
    };
} // boost::spirit::x4::detail

#endif
