#ifndef BOOST_SPIRIT_X4_AUXILIARY_ATTR_HPP
#define BOOST_SPIRIT_X4_AUXILIARY_ATTR_HPP

/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/move_to.hpp>

#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/string_traits.hpp>

#include <string>
#include <string_view>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class T, class HeldValueT = T>
struct attr_parser : parser<attr_parser<T, HeldValueT>>
{
    static_assert(X4Attribute<T>);
    static_assert(!X4UnusedAttribute<T>, "attr_parser with `unused_type` is meaningless");

    // `HeldValueT` is almost always equal to `T`.
    //
    // The most notable situation where they differ is when `attr_parser` is initialized
    // by `char const (&)[N]`. In such case, `attr_parser` must hold the value by
    // `std::string_view`, instead of `std::string`, to be constexpr.

    static_assert(X4Movable<HeldValueT const&, T>);

    using attribute_type = T;
    using held_value_type = HeldValueT;

    static constexpr bool has_attribute = !std::is_same_v<T, unused_type>;
    static constexpr bool handles_container = traits::is_container_v<T>;

    template<class U>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, attr_parser>) &&
            std::is_constructible_v<HeldValueT, U>
    constexpr explicit attr_parser(U&& value)
        noexcept(std::is_nothrow_constructible_v<HeldValueT, U>)
        : held_value_(std::forward<U>(value))
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, Attr& attr) const
        noexcept(noexcept(x4::move_to(std::as_const(held_value_), attr)))
    {
        // Always copy (need reuse in repetitive invocations)
        x4::move_to(std::as_const(held_value_), attr);
        return true;
    }

private:
    HeldValueT held_value_;
};

namespace detail {

template<traits::CharArray R>
using string_array_attr_parser_t = attr_parser<
    std::basic_string<std::remove_extent_t<std::remove_cvref_t<R>>>,
    std::basic_string_view<std::remove_extent_t<std::remove_cvref_t<R>>>
>;

} // detail

template<traits::CharArray R>
attr_parser(R const&) -> attr_parser<
    std::basic_string<std::remove_extent_t<std::remove_cvref_t<R>>>,
    std::basic_string_view<std::remove_extent_t<std::remove_cvref_t<R>>>
>;

template<class T, class HeldValueT>
struct get_info<attr_parser<T, HeldValueT>>
{
    using result_type = std::string;
    [[nodiscard]] constexpr std::string
    operator()(attr_parser<T, HeldValueT> const&) const
    {
        return "attr";
    }
};

namespace detail {

struct attr_gen
{
    template<class T>
    [[nodiscard]] static constexpr attr_parser<std::remove_cvref_t<T>>
    operator()(T&& value)
        noexcept(std::is_nothrow_constructible_v<attr_parser<std::remove_cvref_t<T>>, T>)
    {
        return attr_parser<std::remove_cvref_t<T>>{std::forward<T>(value)};
    }

    template<traits::CharArray R>
    [[nodiscard]] static constexpr string_array_attr_parser_t<R>
    operator()(R&& value)
        noexcept(std::is_nothrow_constructible_v<string_array_attr_parser_t<R>, R>)
    {
        return string_array_attr_parser_t<R>{std::forward<R>(value)};
    }
};

} // detail

namespace parsers {

inline constexpr detail::attr_gen attr{};

} // parsers

using parsers::attr;

} // boost::spirit::x4

#endif
