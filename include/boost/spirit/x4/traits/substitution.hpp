#ifndef BOOST_SPIRIT_X4_TRAITS_SUBSTITUTION_HPP
#define BOOST_SPIRIT_X4_TRAITS_SUBSTITUTION_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/attribute.hpp>
#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/tuple_traits.hpp>

#include <boost/fusion/include/is_sequence.hpp>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>

#include <optional>
#include <type_traits>

namespace boost::spirit::x4::traits {

template<class T>
struct is_variant;

// Find out if T can be a (strong) substitute for Attribute
template<class T, X4Attribute Attr>
struct is_substitute;

template<class T, X4Attribute Attr>
constexpr bool is_substitute_v = is_substitute<T, Attr>::value;

template<class Variant, X4Attribute Attr>
struct variant_has_substitute;

// TODO: reduce MPL usage

namespace detail {

template<class T, X4Attribute Attr>
struct value_type_is_substitute
    : is_substitute<container_value_t<T>, container_value_t<Attr>>
{};

template<class T, X4Attribute Attr>
struct is_substitute_impl : std::false_type {};

template<class T, X4Attribute Attr>
    requires std::conjunction_v<
        fusion::traits::is_sequence<T>,
        fusion::traits::is_sequence<Attr>
    >
struct is_substitute_impl<T, Attr>
    : mpl::equal<T, Attr, is_substitute<mpl::_1, mpl::_2>>
{};

template<class T, X4Attribute Attr>
    requires
        is_container_v<std::remove_const_t<T>> &&
        is_container_v<std::remove_const_t<Attr>>
struct is_substitute_impl<T, Attr>
    : value_type_is_substitute<T, Attr>
{};

template<class T, X4Attribute Attr>
    requires is_variant<std::remove_const_t<Attr>>::value
struct is_substitute_impl<T, Attr>
    : variant_has_substitute<Attr, T>
{};

} // detail

template<class T, X4Attribute Attr>
struct is_substitute
    : std::disjunction<
          std::is_same<T, Attr>,
          detail::is_substitute_impl<T, Attr>
    >
{};

template<class T, X4UnusedAttribute Attr>
struct is_substitute<T, Attr>
    : std::false_type
{};

// for reference T
template<class T, X4Attribute Attr>
struct is_substitute<T&, Attr>
    : is_substitute<T, Attr>
{};

// for reference Attribute
template<class T, X4Attribute Attr>
struct is_substitute<T, Attr&>
    : is_substitute<T, Attr>
{};

template<class T, X4Attribute Attr>
struct is_substitute<std::optional<T>, std::optional<Attr>>
    : is_substitute<T, Attr>
{};

} // boost::spirit::x4::traits

#endif
