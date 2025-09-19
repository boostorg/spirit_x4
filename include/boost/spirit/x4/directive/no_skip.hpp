/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_NO_SKIP_JAN_16_2010_0802PM
#define BOOST_SPIRIT_X4_NO_SKIP_JAN_16_2010_0802PM

#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/parser.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    // Same as `lexeme[...]`, but does not pre-skip
    template <typename Subject>
    struct no_skip_directive : unary_parser<Subject, no_skip_directive<Subject>>
    {
        using base_type = unary_parser<Subject, no_skip_directive<Subject>>;

        static constexpr bool is_pass_through_unary = true;
        static constexpr bool handles_container = Subject::handles_container;

        template <typename SubjectT>
            requires
                (!std::is_same_v<std::remove_cvref_t<SubjectT>, no_skip_directive>) &&
                std::is_constructible_v<Subject, SubjectT>
        constexpr no_skip_directive(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<base_type, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

    private:
        template <typename Context>
        using unused_skipper_context_t = x4::context<
            skipper_tag,
            unused_skipper<
                std::remove_reference_t<decltype(x4::get<skipper_tag>(std::declval<Context const&>()))>
            >,
            Context
        >;

    public:
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires has_skipper_v<Context>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(is_nothrow_parsable_v<
                Subject, It, Se,
                unused_skipper_context_t<Context>,
                RContext,
                Attribute
            >)
        {
            auto const& skipper = x4::get<skipper_tag>(context);

            unused_skipper<std::remove_reference_t<decltype(skipper)>>
            unused_skipper(skipper);

            return this->subject.parse(
                first, last,
                x4::make_context<skipper_tag>(unused_skipper, context),
                rcontext,
                attr
            );
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires (!has_skipper_v<Context>)
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, RContext, Attribute>)
        {
            return this->subject.parse(first, last, context, rcontext, attr);
        }
    };

    namespace detail
    {
        struct no_skip_gen
        {
            template <X4Subject Subject>
            [[nodiscard]] constexpr no_skip_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const // TODO: MSVC can't handle static operator[]
                noexcept(is_parser_nothrow_constructible_v<no_skip_directive<as_parser_plain_t<Subject>>, Subject>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };

    } // detail

    inline namespace cpos
    {
        inline constexpr detail::no_skip_gen no_skip{};

    } // cpos

} // boost::spirit::x4

#endif
