#ifndef BOOST_SPIRIT_X4_STRING_DETAIL_STRING_PARSE_HPP
#define BOOST_SPIRIT_X4_STRING_DETAIL_STRING_PARSE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/traits/string_traits.hpp>
#include <boost/spirit/x4/traits/tuple_traits.hpp>
#include <boost/spirit/x4/traits/container_traits.hpp>

#include <concepts>
#include <string_view>
#include <iterator>
#include <type_traits>

namespace boost::spirit::x4::detail {

template<class CharT, class CharTraitsT, std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr, class CaseCompareFunc>
[[nodiscard]] constexpr bool
string_parse(
    std::basic_string_view<CharT, CharTraitsT> const str,
    It& first, Se const& last,
    Attr& attr, CaseCompareFunc const& compare
) noexcept(std::same_as<std::remove_const_t<Attr>, unused_container_type>)
{
    using synthesized_value_type = traits::synthesized_value_t<Attr>;
    static_assert(std::same_as<traits::attribute_category_t<synthesized_value_type>, traits::container_attr>);
    using value_type = traits::container_value_t<synthesized_value_type>;
    static_assert(!traits::CharLike<value_type> || std::same_as<value_type, CharT>, "Mixing incompatible char types is not allowed");

    It it = first;
    auto stri = str.begin();
    auto str_last = str.end();

    for (; stri != str_last; ++stri, ++it) {
        if (it == last || compare(*stri, *it) != 0) {
            return false;
        }
    }

    x4::move_to(first, it, attr);
    first = it;
    return true;
}

template<class CharT, class CharTraitsT, std::forward_iterator It, std::sentinel_for<It> Se, class CaseCompareFunc>
constexpr void
string_parse(
    std::basic_string_view<CharT, CharTraitsT> const,
    It&, Se const&,
    unused_type const&, CaseCompareFunc const&
) = delete; // The call site is lacking `x4::assume_container(attr)`

template<class CharT, class CharTraitsT, std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr, class CaseCompareFunc>
[[nodiscard]] constexpr bool
string_parse(
    std::basic_string<CharT, CharTraitsT> const& str,
    It& first, Se const& last,
    Attr& attr, CaseCompareFunc const& compare
) noexcept(noexcept(detail::string_parse(std::basic_string_view{str}, first, last, attr, compare)))
{
    return detail::string_parse(std::basic_string_view{str}, first, last, attr, compare);
}

template<class CharT, class CharTraitsT, std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
[[nodiscard]] constexpr bool
string_parse(
    std::basic_string_view<CharT, CharTraitsT> const ucstr,
    std::basic_string_view<CharT, CharTraitsT> const lcstr,
    It& first, Se const& last, Attr& attr
) noexcept(std::same_as<std::remove_const_t<Attr>, unused_container_type>)
{
    using synthesized_value_type = traits::synthesized_value_t<Attr>;
    static_assert(std::same_as<traits::attribute_category_t<synthesized_value_type>, traits::container_attr>);
    using value_type = traits::container_value_t<synthesized_value_type>;
    static_assert(!traits::CharLike<value_type> || std::same_as<value_type, CharT>, "Mixing incompatible char types is not allowed");

    auto uc_it = ucstr.begin();
    auto uc_last = ucstr.end();
    auto lc_it = lcstr.begin();
    It it = first;

    for (; uc_it != uc_last; ++uc_it, ++lc_it, ++it) {
        if (it == last || (*uc_it != *it && *lc_it != *it)) {
            return false;
        }
    }
    x4::move_to(first, it, attr);
    first = it;
    return true;
}

template<class CharT, class CharTraitsT, std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
[[nodiscard]] constexpr bool
string_parse(
    std::basic_string<CharT, CharTraitsT> const& ucstr,
    std::basic_string<CharT, CharTraitsT> const& lcstr,
    It& first, Se const& last, Attr& attr
) noexcept(noexcept(detail::string_parse(std::basic_string_view{ucstr}, std::basic_string_view{lcstr}, first, last, attr)))
{
    return detail::string_parse(std::basic_string_view{ucstr}, std::basic_string_view{lcstr}, first, last, attr);
}

template<class CharT, class CharTraitsT, std::forward_iterator It, std::sentinel_for<It> Se>
[[nodiscard]] constexpr bool
string_parse(
    std::basic_string_view<CharT, CharTraitsT> const,
    std::basic_string_view<CharT, CharTraitsT> const,
    It&, Se const&, unused_type const&
) = delete; // The call site is lacking `x4::assume_container(attr)`

} // boost::spirit::x4::detail

#endif
