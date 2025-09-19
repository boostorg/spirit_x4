/*=============================================================================
    Copyright (c) 2014 Thomas Bernard
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_NO_CASE_SEPT_16_2014_0912PM
#define BOOST_SPIRIT_X4_NO_CASE_SEPT_16_2014_0912PM

#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/parser.hpp>

#include <boost/spirit/x4/string/case_compare.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    // propagate no_case information through the context
    template <typename Subject>
    struct no_case_directive : unary_parser<Subject, no_case_directive<Subject>>
    {
        using base_type = unary_parser<Subject, no_case_directive<Subject>>;

        static constexpr bool is_pass_through_unary = true;
        static constexpr bool handles_container = Subject::handles_container;

        template <typename SubjectT>
            requires
                (!std::is_same_v<std::remove_cvref_t<SubjectT>, no_case_directive>) &&
                std::is_constructible_v<Subject, SubjectT>
        constexpr no_case_directive(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(noexcept(
                this->subject.parse(
                    first, last,
                    x4::make_context<detail::no_case_tag_t>(detail::no_case_tag, context),
                    rcontext,
                    attr
                )
            ))
        {
            return this->subject.parse(
                first, last,
                x4::make_context<detail::no_case_tag_t>(detail::no_case_tag, context),
                rcontext,
                attr
            );
        }
    };

    namespace detail
    {
        struct no_case_gen
        {
            template <X4Subject Subject>
            [[nodiscard]] constexpr no_case_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const
                noexcept(is_parser_nothrow_constructible_v<no_case_directive<as_parser_plain_t<Subject>>, Subject>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };

    } // detail

    inline namespace cpos
    {
        inline constexpr detail::no_case_gen no_case{};

    } // cpos

} // boost::spirit::x4

#endif
