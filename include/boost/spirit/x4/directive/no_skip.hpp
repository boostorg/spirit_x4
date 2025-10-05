#ifndef BOOST_SPIRIT_X4_DIRECTIVE_NO_SKIP_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_NO_SKIP_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2013 Agustin Berge
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

// Same as `lexeme[...]`, but does not pre-skip
template<class Subject>
struct no_skip_directive : unary_parser<Subject, no_skip_directive<Subject>>
{
    static constexpr bool is_pass_through_unary = true;
    static constexpr bool handles_container = Subject::handles_container;

private:
    template<class Context>
    using unused_skipper_context_t = x4::context<
        contexts::skipper,
        unused_skipper<
            std::remove_reference_t<decltype(x4::get<contexts::skipper>(std::declval<Context const&>()))>
        >,
        Context
    >;

public:
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires has_skipper_v<Context>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(is_nothrow_parsable_v<
            Subject, It, Se,
            unused_skipper_context_t<Context>,
            Attr
        >)
    {
        auto const& skipper = x4::get<contexts::skipper>(ctx);

        unused_skipper<std::remove_reference_t<decltype(skipper)>>
        unused_skipper(skipper);

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
        return this->subject.parse(first, last, ctx, attr);
    }
};

namespace detail {

struct no_skip_gen
{
    template<X4Subject Subject>
    [[nodiscard]] constexpr no_skip_directive<as_parser_plain_t<Subject>>
    operator[](Subject&& subject) const // TODO: MSVC can't handle static operator[]
        noexcept(is_parser_nothrow_constructible_v<no_skip_directive<as_parser_plain_t<Subject>>, Subject>)
    {
        return {as_parser(std::forward<Subject>(subject))};
    }
};

} // detail

namespace parsers::directive {

inline constexpr detail::no_skip_gen no_skip{};

} // parsers::directive

using parsers::directive::no_skip;

} // boost::spirit::x4

#endif
