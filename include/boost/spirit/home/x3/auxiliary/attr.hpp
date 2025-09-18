/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_ATTR_JUL_23_2008_0956AM
#define BOOST_SPIRIT_X3_ATTR_JUL_23_2008_0956AM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/traits/container_traits.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/traits/string_traits.hpp>

#include <string>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename T>
    struct attr_parser : parser<attr_parser<T>>
    {
        static_assert(X3Attribute<T>);
        using attribute_type = T;

        static constexpr bool has_attribute = !std::is_same_v<T, unused_type>;
        static constexpr bool handles_container = traits::is_container_v<T>;

        template <typename U>
            requires
                (!std::is_same_v<std::remove_cvref_t<U>, attr_parser>) &&
                std::is_constructible_v<T, U>
        constexpr attr_parser(U&& value)
            noexcept(std::is_nothrow_constructible_v<T, U>)
            : value_(std::forward<U>(value))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It&, Se const&, Context const&, RContext const&, Attribute& attr_) const
            noexcept(noexcept(traits::move_to(std::as_const(value_), attr_)))
        {
            // Always copy (need reuse in repetitive invocations)
            traits::move_to(std::as_const(value_), attr_);
            return true;
        }

    private:
        T value_;
    };

    template <traits::CharArray R>
    attr_parser(R const&) -> attr_parser<std::basic_string<std::remove_extent_t<std::remove_cvref_t<R>>>>;

    template <typename T>
    struct get_info<attr_parser<T>>
    {
        using result_type = std::string;
        [[nodiscard]] constexpr std::string
        operator()(attr_parser<T> const&) const
        {
            return "attr";
        }
    };

    namespace detail
    {
        struct attr_gen
        {
            template <typename T>
            [[nodiscard]] static constexpr attr_parser<std::remove_cvref_t<T>>
            operator()(T&& value)
                noexcept(std::is_nothrow_constructible_v<attr_parser<std::remove_cvref_t<T>>, T>)
            {
                return { std::forward<T>(value) };
            }

            template <traits::CharArray R>
            [[nodiscard]] static constexpr attr_parser<std::basic_string<std::remove_extent_t<std::remove_cvref_t<R>>>>
            operator()(R&& value)
                noexcept(std::is_nothrow_constructible_v<attr_parser<std::basic_string<std::remove_extent_t<std::remove_cvref_t<R>>>>, R>)
            {
                return { std::forward<R>(value) };
            }
        };

    } // detail

    inline namespace cpos
    {
        inline constexpr detail::attr_gen attr{};

    } // cpos

} // boost::spirit::x3

#endif
