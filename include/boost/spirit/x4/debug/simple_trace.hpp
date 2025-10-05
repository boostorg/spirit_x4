#ifndef BOOST_SPIRIT_X4_DEBUG_SIMPLE_TRACE_HPP
#define BOOST_SPIRIT_X4_DEBUG_SIMPLE_TRACE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/unused.hpp>

#include <boost/spirit/x4/debug/print_token.hpp>
#include <boost/spirit/x4/debug/print_attribute.hpp>
#include <boost/spirit/x4/debug/debug_handler_state.hpp>

#include <string_view>
#include <concepts>
#include <iostream>

//  The stream to use for debug output
#ifndef BOOST_SPIRIT_X4_DEBUG_OUT
#define BOOST_SPIRIT_X4_DEBUG_OUT std::cerr
#endif

//  number of tokens to print while debugging
#ifndef BOOST_SPIRIT_X4_DEBUG_PRINT_SOME
#define BOOST_SPIRIT_X4_DEBUG_PRINT_SOME 20
#endif

//  number of spaces to indent
#ifndef BOOST_SPIRIT_X4_DEBUG_INDENT
#define BOOST_SPIRIT_X4_DEBUG_INDENT 2
#endif

namespace boost::spirit::x4 {

namespace detail {

template<class Char>
void token_printer(std::ostream& o, Char c)
{
    // allow customization of the token printer routine
    traits::print_token(o, c);
}

} // detail

template<int IndentSpaces = 2, int CharsToPrint = 20>
struct simple_trace
{
    explicit simple_trace(std::ostream& out) noexcept
        : out(out)
    {}

    void print_indent(int n) const
    {
        n *= IndentSpaces;
        for (int i = 0; i != n; ++i) {
            out << ' ';
        }
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se>
    void print_some(std::string_view tag, It first, Se last) const
    {
        simple_trace::print_indent(indent);

        out << '<' << tag << '>';

        for (int i = 0; first != last && i != CharsToPrint && *first; ++i, ++first) {
            detail::token_printer(out, *first);
        }
        out << "</" << tag << '>' << std::endl;

        // TODO: convert invalid xml characters (e.g. '<') to valid character entities
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
    void operator()(
        It first,
        Se last,
        Attr const& attr,
        debug_handler_state const state,
        std::string_view rule_name
    ) const
    {
        using enum debug_handler_state;
        switch (state) {
        case pre_parse:
            simple_trace::print_indent(indent++);
            out << '<' << rule_name << '>' << std::endl;
            simple_trace::print_some("try", first, last);
            break;

        case successful_parse:
            simple_trace::print_some("success", first, last);
            if constexpr (!std::same_as<Attr, unused_type>) {
                simple_trace::print_indent(indent);
                out << "<attributes>";
                traits::print_attribute(out, attr);
                out << "</attributes>";
                out << std::endl;
            }
            simple_trace::print_indent(--indent);
            out << "</" << rule_name << '>' << std::endl;
            break;

        case failed_parse:
            simple_trace::print_indent(indent);
            out << "<fail/>" << std::endl;
            simple_trace::print_indent(--indent);
            out << "</" << rule_name << '>' << std::endl;
            break;
        }
    }

    std::ostream& out;
    mutable int indent = 0;
};

namespace detail {

using simple_trace_type = simple_trace<
    BOOST_SPIRIT_X4_DEBUG_INDENT,
    BOOST_SPIRIT_X4_DEBUG_PRINT_SOME
>;

[[nodiscard]] inline simple_trace_type&
get_simple_trace()
{
    static simple_trace_type tracer(BOOST_SPIRIT_X4_DEBUG_OUT);
    return tracer;
}


// TODO: This should be customizable by users
template<std::forward_iterator It, std::sentinel_for<It> Se, class Attr>
struct [[nodiscard]] scoped_rule_debug
{
    static_assert(X4Attribute<std::remove_reference_t<Attr>>);

    scoped_rule_debug(
        std::string_view rule_name,
        It const& first, Se const& last,
        Attr const& attr,
        bool const* parse_ok
    )
        : parse_ok(parse_ok)
        , rule_name(rule_name)
        , first(first)
        , last(last)
        , attr(attr)
        , f(detail::get_simple_trace())
    {
        f(first, last, attr, debug_handler_state::pre_parse, rule_name);
    }

    ~scoped_rule_debug()
    {
        f(
            first, last,
            attr,
            *parse_ok ? debug_handler_state::successful_parse : debug_handler_state::failed_parse,
            rule_name
        );
    }

    bool const* parse_ok = nullptr;
    std::string_view rule_name;
    It const& first;
    Se const& last;
    Attr const& attr;
    simple_trace_type& f;
};

} // detail

} // boost::spirit::x4

#endif
