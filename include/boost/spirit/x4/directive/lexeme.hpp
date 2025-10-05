#ifndef BOOST_SPIRIT_X4_DIRECTIVE_LEXEME_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_LEXEME_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/parser.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Subject>
struct lexeme_directive : unary_parser<Subject, lexeme_directive<Subject>>
{
    static constexpr bool is_pass_through_unary = true;
    static constexpr bool handles_container = Subject::handles_container;

private:
    template<class Context>
    using pre_skip_context_t = std::remove_cvref_t<decltype(
        x4::make_context<contexts::skipper>(std::declval<unused_skipper_t<Context>&>(), std::declval<Context const&>())
    )>;

public:
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires has_skipper_v<Context>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            noexcept(x4::skip_over(first, last, ctx)) &&
            is_nothrow_parsable_v<Subject, It, Se, pre_skip_context_t<Context>, Attr>
        )
    {
        x4::skip_over(first, last, ctx);

        auto const& skipper = x4::get<contexts::skipper>(ctx);
        unused_skipper_t<Context> unused_skipper(skipper);

        return this->subject.parse(
            first, last,
            x4::make_context<contexts::skipper>(unused_skipper, ctx),
            attr
        );
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires (!has_skipper_v<Context>)
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, Attr>)
    {
        //  no need to pre-skip if skipper is unused
        return this->subject.parse(first, last, ctx, attr);
    }
};

namespace detail {

struct lexeme_gen
{
    template<X4Subject Subject>
    [[nodiscard]] constexpr lexeme_directive<as_parser_plain_t<Subject>>
    operator[](Subject&& subject) const
        noexcept(is_parser_nothrow_constructible_v<lexeme_directive<as_parser_plain_t<Subject>>, Subject>)
    {
        return {as_parser(std::forward<Subject>(subject))};
    }
};

} // detail

namespace parsers::directive {

inline constexpr detail::lexeme_gen lexeme{};

} // parsers::directive

using parsers::directive::lexeme;

} // boost::spirit::x4

#endif
