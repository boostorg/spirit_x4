#ifndef BOOST_SPIRIT_X4_OPERATOR_ALTERNATIVE_HPP
#define BOOST_SPIRIT_X4_OPERATOR_ALTERNATIVE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/core/detail/parse_alternative.hpp>

#include <boost/spirit/x4/traits/attribute.hpp>
#include <boost/spirit/x4/traits/container_traits.hpp>

#include <boost/variant/variant.hpp> // TODO: remove this

#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class Left, class Right>
struct alternative : binary_parser<Left, Right, alternative<Left, Right>>
{
    using base_type = binary_parser<Left, Right, alternative>;

    template<class LeftT, class RightT>
        requires std::is_constructible_v<base_type, LeftT, RightT>
    constexpr alternative(LeftT&& left, RightT&& right)
        noexcept(std::is_nothrow_constructible_v<base_type, LeftT, RightT>)
        : base_type(std::forward<LeftT>(left), std::forward<RightT>(right))
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, unused_type) const
        noexcept(
            is_nothrow_parsable_v<Left, It, Se, Context, unused_type> &&
            is_nothrow_parsable_v<Right, It, Se, Context, unused_type>
        )
    {
        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return this->left.parse(first, last, ctx, unused) ||
                (!x4::has_expectation_failure(ctx) && this->right.parse(first, last, ctx, unused));
        } else {
            return this->left.parse(first, last, ctx, unused) ||
                this->right.parse(first, last, ctx, unused);
        }
    }

    // If you're changing these semantics, you should:
    //   - Understand https://github.com/boostorg/spirit/issues/378
    //   - Add test to `attribute.cpp`. This is mandatory.

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Attr>
        requires (!traits::X4Container<Attr>)
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            noexcept(detail::parse_alternative(this->left, first, last, ctx, attr)) &&
            noexcept(detail::parse_alternative(this->right, first, last, ctx, attr)) &&
            noexcept(x4::move_to(std::declval<Attr>(), attr)) &&
            std::is_nothrow_default_constructible_v<Attr>
        )
    {
        static_assert(!std::same_as<std::remove_const_t<Attr>, unused_type>);
        static_assert(!std::same_as<std::remove_const_t<Attr>, unused_container_type>);

        if (Attr attr_temp; detail::parse_alternative(this->left, first, last, ctx, attr_temp)) {
            x4::move_to(std::move(attr_temp), attr);
            return true;
        }
        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            if (x4::has_expectation_failure(ctx)) return false;
        }

        if (Attr attr_temp; detail::parse_alternative(this->right, first, last, ctx, attr_temp)) {
            x4::move_to(std::move(attr_temp), attr);
            return true;
        }
        return false; // `attr` is untouched
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, traits::X4Container ContainerAttr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, ContainerAttr& attr) const
        noexcept(
            noexcept(detail::parse_alternative(this->left, first, last, ctx, attr)) &&
            noexcept(detail::parse_alternative(this->right, first, last, ctx, attr)) &&
            noexcept(x4::move_to(std::declval<ContainerAttr>(), attr)) &&
            std::is_nothrow_default_constructible_v<ContainerAttr> &&
            noexcept(traits::clear(attr))
        )
    {
        static_assert(!std::same_as<std::remove_const_t<ContainerAttr>, unused_type>);
        static_assert(!std::same_as<std::remove_const_t<ContainerAttr>, unused_container_type>);

        // We can (ab)use the exposed attribute as the temporary workspace
        // if and only if the modification or rollback of the exposed attribute
        // does not change the semantic state of the exposed container instance.
        //
        // The only situation we can guarantee such condition is when the container
        // is empty; assuming that the "empty" state of any user-provided container
        // class is monostate.
        if (traits::is_empty(attr)) {
            if (detail::parse_alternative(this->left, first, last, ctx, attr)) {
                return true;
            }
            traits::clear(attr); // Make sure we don't propagate observable side effect

            if constexpr (has_context_v<Context, contexts::expectation_failure>) {
                if (x4::has_expectation_failure(ctx)) return false;
            }

            if (detail::parse_alternative(this->right, first, last, ctx, attr)) {
                return true;
            }
            traits::clear(attr); // Make sure we don't propagate observable side effect
            return false;
        }

        //
        // Non-empty container
        // Since the attribute is a container, we can reuse the buffer when the `left` fails
        //
        if consteval {
            ContainerAttr attr_temp;

            if (detail::parse_alternative(this->left, first, last, ctx, attr_temp)) {
                x4::move_to(std::move(attr_temp), attr);
                return true;
            }

            if constexpr (has_context_v<Context, contexts::expectation_failure>) {
                if (x4::has_expectation_failure(ctx)) return false;
            }
            traits::clear(attr_temp); // Reuse the buffer

            if (detail::parse_alternative(this->right, first, last, ctx, attr_temp)) {
                x4::move_to(std::move(attr_temp), attr);
                return true;
            }
            return false; // `attr` is untouched

        } else { // not consteval
            thread_local ContainerAttr attr_temp;
            traits::clear(attr_temp);

            if (detail::parse_alternative(this->left, first, last, ctx, attr_temp)) {
                x4::move_to(std::move(attr_temp), attr);
                return true;
            }

            if constexpr (has_context_v<Context, contexts::expectation_failure>) {
                if (x4::has_expectation_failure(ctx)) return false;
            }
            traits::clear(attr_temp); // Reuse the buffer

            if (detail::parse_alternative(this->right, first, last, ctx, attr_temp)) {
                x4::move_to(std::move(attr_temp), attr);
                return true;
            }
            return false; // `attr` is untouched
        }
    }
};

template<X4Subject Left, X4Subject Right>
[[nodiscard]] constexpr alternative<as_parser_plain_t<Left>, as_parser_plain_t<Right>>
operator|(Left&& left, Right&& right)
    noexcept(
        is_parser_nothrow_castable_v<Left> &&
        is_parser_nothrow_castable_v<Right> &&
        std::is_nothrow_constructible_v<
            alternative<as_parser_plain_t<Left>, as_parser_plain_t<Right>>,
            as_parser_t<Left>,
            as_parser_t<Right>
        >
    )
{
    return {as_parser(std::forward<Left>(left)), as_parser(std::forward<Right>(right))};
}

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

template<class Left, class Right, class Context>
struct attribute_of<alternative<Left, Right>, Context>
    : x4::detail::attribute_of_binary<boost::variant, alternative, Left, Right, Context>
{};

} // boost::spirit::x4::traits

#endif
