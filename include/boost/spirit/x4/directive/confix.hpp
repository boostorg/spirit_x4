#ifndef BOOST_SPIRIT_X4_DIRECTIVE_CONFIX_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_CONFIX_HPP

/*=============================================================================
    Copyright (c) 2009 Chris Hoeppler
    Copyright (c) 2014 Lee Clagett
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/unused.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Prefix, class Subject, class Postfix>
struct confix_directive
    : unary_parser<Subject, confix_directive<Prefix, Subject, Postfix>>
{
    using base_type = unary_parser<Subject, confix_directive>;

    static constexpr bool is_pass_through_unary = true;
    static constexpr bool handles_container = Subject::handles_container;

    template<class PrefixT, class SubjectT, class PostfixT>
        requires
            std::is_constructible_v<Prefix, PrefixT> &&
            std::is_constructible_v<base_type, SubjectT> &&
            std::is_constructible_v<Postfix, PostfixT>
    constexpr confix_directive(PrefixT&& prefix, SubjectT&& subject, PostfixT&& postfix)
        noexcept(
            std::is_nothrow_constructible_v<Prefix, PrefixT> &&
            std::is_nothrow_constructible_v<base_type, SubjectT> &&
            std::is_nothrow_constructible_v<Postfix, PostfixT>
        )
        : base_type(std::forward<SubjectT>(subject))
        , prefix_(std::forward<PrefixT>(prefix))
        , postfix_(std::forward<PostfixT>(postfix))
    {
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            std::is_nothrow_copy_assignable_v<It> &&
            is_nothrow_parsable_v<Prefix, It, Se, Context, unused_type> &&
            is_nothrow_parsable_v<Subject, It, Se, Context, Attr> &&
            is_nothrow_parsable_v<Postfix, It, Se, Context, unused_type>
        )
    {
        It const saved_first = first;

        if (!(prefix_.parse(first, last, ctx, unused) &&
              this->subject.parse(first, last, ctx, attr) &&
              postfix_.parse(first, last, ctx, unused))
        ) {
            if constexpr (has_context_v<Context, contexts::expectation_failure>) {
                if (x4::has_expectation_failure(ctx)) {
                    // don't rollback iterator (mimicking exception-like behavior)
                    return false;
                }
            }

            first = saved_first;
            return false;
        }

        return true;
    }

private:
    Prefix prefix_;
    Postfix postfix_;
};

namespace detail {

template<X4Subject Prefix, X4Subject Postfix>
struct [[nodiscard]] confix_gen
{
    template<X4Subject Subject>
    [[nodiscard]] constexpr confix_directive<Prefix, as_parser_plain_t<Subject>, Postfix>
    operator[](Subject&& subject) const
        noexcept(
            is_parser_nothrow_castable_v<Subject> &&
            std::is_nothrow_constructible_v<
                confix_directive<Prefix, as_parser_plain_t<Subject>, Postfix>,
                Prefix const&,
                as_parser_t<Subject>,
                Postfix const&
            >
        )
    {
        return {prefix, as_parser(std::forward<Subject>(subject)), postfix};
    }

    Prefix prefix;
    Postfix postfix;
};

struct confix_fn
{
    template<X4Subject Prefix, X4Subject Postfix>
    static constexpr confix_gen<as_parser_plain_t<Prefix>, as_parser_plain_t<Postfix>>
    operator()(Prefix&& prefix, Postfix&& postfix)
        noexcept(
            is_parser_nothrow_castable_v<Prefix> &&
            is_parser_nothrow_castable_v<Postfix> &&
            std::is_nothrow_constructible_v<
                detail::confix_gen<as_parser_plain_t<Prefix>, as_parser_plain_t<Postfix>>,
                as_parser_t<Prefix>, as_parser_t<Postfix>
            >
        )
    {
        return {as_parser(std::forward<Prefix>(prefix)), as_parser(std::forward<Postfix>(postfix))};
    }
};

} // detail

namespace parsers::directive {

inline constexpr detail::confix_fn confix{};

} // parsers::directive

using parsers::directive::confix;

} // boost::spirit::x4

#endif
