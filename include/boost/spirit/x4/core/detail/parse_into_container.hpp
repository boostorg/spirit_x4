#ifndef BOOST_SPIRIT_X4_CORE_DETAIL_PARSE_INTO_CONTAINER_HPP
#define BOOST_SPIRIT_X4_CORE_DETAIL_PARSE_INTO_CONTAINER_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/core/parser.hpp>

#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/attribute.hpp>
#include <boost/spirit/x4/traits/substitution.hpp>

#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/front.hpp>
#include <boost/fusion/sequence/intrinsic/back.hpp>

// TODO: remove Boost.Variant usage
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <ranges>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4::detail {

template<class Parser, class Container, class Context>
struct parser_accepts_container
    : traits::is_substitute<traits::attribute_of_t<Parser, Context>, Container>
{};

template<class Parser, class Container, class Context>
constexpr bool parser_accepts_container_v = parser_accepts_container<Parser, Container, Context>::value;

template<class Parser>
struct parse_into_container_base_impl
{
    // Parser has attribute (synthesize; Attribute is a container)
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires (!parser_accepts_container_v<Parser, Attr, Context>)
    [[nodiscard]] static constexpr bool
    call_synthesize(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) // never noexcept (requires container insertion)
    {
        static_assert(!std::same_as<std::remove_const_t<Attr>, unused_container_type>);

        using value_type = traits::container_value_t<Attr>;
        value_type val; // default-initialize

        static_assert(Parsable<Parser, It, Se, Context, value_type>);
        if (!parser.parse(first, last, ctx, val)) return false;

        // push the parsed value into our attribute
        traits::push_back(attr, std::move(val));
        return true;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
        requires (!parser_accepts_container_v<Parser, unused_container_type, Context>)
    [[nodiscard]] static constexpr bool
    call_synthesize(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, unused_container_type const&
    ) noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, unused_type>)
    {
        static_assert(Parsable<Parser, It, Se, Context, unused_type>);
        return parser.parse(first, last, ctx, unused);
    }

    // Parser has attribute (synthesize; Attribute is a container)
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires parser_accepts_container_v<Parser, Attr, Context>
    [[nodiscard]] static constexpr bool
    call_synthesize(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, Attr>)
    {
        static_assert(Parsable<Parser, It, Se, Context, Attr>);
        return parser.parse(first, last, ctx, attr);
    }

    // Parser has attribute && it is NOT fusion sequence
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires
            traits::has_attribute_v<Parser, Context> &&
            (!fusion::traits::is_sequence<Attr>::value)
    [[nodiscard]] static constexpr bool
    call(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    )
    {
        // TODO: reduce call stack while keeping maintainability
        return parse_into_container_base_impl::call_synthesize(parser, first, last, ctx, attr);
    }

    // Parser has attribute && it is fusion sequence
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires
            traits::has_attribute_v<Parser, Context> &&
            fusion::traits::is_sequence<Attr>::value
    [[nodiscard]] static constexpr bool
    call(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) noexcept(noexcept(parse_into_container_base_impl::call_synthesize(parser, first, last, ctx, fusion::front(attr))))
    {
        static_assert(traits::has_size_v<Attr, 1>, "Expecting a single element fusion sequence");
        // TODO: reduce call stack while keeping maintainability
        return parse_into_container_base_impl::call_synthesize(parser, first, last, ctx, fusion::front(attr));
    }

    // Parser has no attribute (pass unused)
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires (!traits::has_attribute_v<Parser, Context>)
    [[nodiscard]] static constexpr bool
    call(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, Attr& /* attr */
    ) noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, unused_container_type>)
    {
        // static_assert(Parsable<Parser, It, Se, Context, unused_container_type>);
        return parser.parse(first, last, ctx, unused_container);
    }
};

template<class Parser, class Context>
struct parse_into_container_impl : parse_into_container_base_impl<Parser> {};


template<class Parser, class Context>
    requires Parser::handles_container
struct parse_into_container_impl<Parser, Context>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
    static constexpr bool pass_attibute_as_is = std::disjunction_v<
        parser_accepts_container<Parser, Attr, Context>,

        std::negation<traits::is_substitute< // parser attribute is substitute for container value?
            traits::pseudo_attribute_t<
                Context,
                traits::attribute_of_t<Parser, Context>,
                It, Se
            >,
            traits::container_value_t<Attr>
        >>
    >;

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
        requires (!pass_attibute_as_is<It, Se, Attr>)
    [[nodiscard]] static constexpr bool
    call(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) noexcept(noexcept(parse_into_container_base_impl<Parser>::call(
        parser, first, last, ctx, attr
    )))
    {
        return parse_into_container_base_impl<Parser>::call(
            parser, first, last, ctx, attr
        );
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se>
        requires pass_attibute_as_is<It, Se, unused_container_type>
    [[nodiscard]] static constexpr bool
    call(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, unused_container_type
    ) noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, unused_container_type>)
    {
        static_assert(Parsable<Parser, It, Se, Context, unused_container_type>);
        return parser.parse(first, last, ctx, unused_container);
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
        requires pass_attibute_as_is<It, Se, Attr>
    [[nodiscard]] static constexpr bool
    call(
        Parser const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) // never noexcept (requires container insertion)
    {
        static_assert(!std::is_same_v<std::remove_const_t<Attr>, unused_type>);
        static_assert(!std::is_same_v<std::remove_const_t<Attr>, unused_container_type>);

        static_assert(Parsable<Parser, It, Se, Context, Attr>);

        if (traits::is_empty(attr)) {
            return parser.parse(first, last, ctx, attr);
        }

        Attr rest; // default-initialize
        if (!parser.parse(first, last, ctx, rest)) return false;

        traits::append(
            attr,
            std::make_move_iterator(std::ranges::begin(rest)),
            std::make_move_iterator(std::ranges::end(rest))
        );
        return true;
    }
};

template<
    class Parser, std::forward_iterator It, std::sentinel_for<It> Se,
    class Context, X4Attribute Attr
>
[[nodiscard]] constexpr bool
parse_into_container(
    Parser const& parser, It& first, Se const& last,
    Context const& ctx, Attr& attr
) noexcept(noexcept(parse_into_container_impl<Parser, Context>::call(
    parser, first, last, ctx, attr
)))
{
    static_assert(
        !std::same_as<Attr, unused_type>,
        "`unused_type` should not be passed to `parse_into_container`. Use `x4::assume_container(attr)`"
    );

    return parse_into_container_impl<Parser, Context>::call(
        parser, first, last, ctx, attr
    );
}

} // boost::spirit::x4::detail

#endif
