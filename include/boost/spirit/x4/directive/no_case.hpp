#ifndef BOOST_SPIRIT_X4_DIRECTIVE_NO_CASE_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_NO_CASE_HPP

/*=============================================================================
    Copyright (c) 2014 Thomas Bernard
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/parser.hpp>

#include <boost/spirit/x4/string/case_compare.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

// propagate no_case information through the context
template<class Subject>
struct no_case_directive : unary_parser<Subject, no_case_directive<Subject>>
{
    static constexpr bool is_pass_through_unary = true;
    static constexpr bool handles_container = Subject::handles_container;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(noexcept(
            this->subject.parse(
                first, last,
                x4::make_context<detail::case_compare_tag>(detail::case_compare_no_case, ctx),
                attr
            )
        ))
    {
        return this->subject.parse(
            first, last,
            x4::make_context<detail::case_compare_tag>(detail::case_compare_no_case, ctx),
            attr
        );
    }
};

namespace detail {

struct no_case_gen
{
    template<X4Subject Subject>
    [[nodiscard]] constexpr no_case_directive<as_parser_plain_t<Subject>>
    operator[](Subject&& subject) const
        noexcept(is_parser_nothrow_constructible_v<no_case_directive<as_parser_plain_t<Subject>>, Subject>)
    {
        return {as_parser(std::forward<Subject>(subject))};
    }
};

} // detail

namespace parsers::directive {

inline constexpr detail::no_case_gen no_case{};

} // parsers::directive

using parsers::directive::no_case;

} // boost::spirit::x4

#endif
