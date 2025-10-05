#ifndef BOOST_SPIRIT_X4_DIRECTIVE_OMIT_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_OMIT_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/parser.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

// `omit_directive` forces the attribute of subject parser
// to be `unused_type`
template<class Subject>
struct omit_directive : unary_parser<Subject, omit_directive<Subject>>
{
    using attribute_type = unused_type;
    using subject_type = Subject;

    static constexpr bool has_attribute = false;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr const&) const
        noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, unused_type>)
    {
        static_assert(Parsable<Subject, It, Se, Context, unused_type>);
        return this->subject.parse(first, last, ctx, unused);
    }
};

namespace detail {

struct omit_gen
{
    template<X4Subject Subject>
    [[nodiscard]] constexpr omit_directive<as_parser_plain_t<Subject>>
    operator[](Subject&& subject) const
        noexcept(is_parser_nothrow_constructible_v<omit_directive<as_parser_plain_t<Subject>>, Subject>)
    {
        return {as_parser(std::forward<Subject>(subject))};
    }
};

} // detail

namespace parsers::directive {

[[maybe_unused]] inline constexpr detail::omit_gen omit{};

} // parsers::directive

using parsers::directive::omit;

} // boost::spirit::x4

#endif
