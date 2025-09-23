#ifndef BOOST_SPIRIT_X4_TRAITS_TUPLE_TRAITS_HPP
#define BOOST_SPIRIT_X4_TRAITS_TUPLE_TRAITS_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
================================================_==============================*/

#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/include/is_view.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/front.hpp>

#include <type_traits>

namespace boost::spirit::x4::traits {

template<class A, class B>
struct has_same_size
    : std::bool_constant<
        fusion::result_of::size<std::remove_cvref_t<A>>::value ==
        fusion::result_of::size<std::remove_cvref_t<B>>::value
    >
{};

template<class A, class B>
constexpr bool has_same_size_v = has_same_size<A, B>::value;

template<class T, std::size_t N>
struct has_size
    : std::bool_constant<fusion::result_of::size<std::remove_cvref_t<T>>::value == N>
{};

template<class T, std::size_t N>
constexpr bool has_size_v = has_size<T, N>::value;

template<class A, class B>
struct is_same_size_sequence
    : std::bool_constant<std::conjunction_v<
        fusion::traits::is_sequence<std::remove_cvref_t<A>>,
        fusion::traits::is_sequence<std::remove_cvref_t<B>>,
        has_same_size<A, B>
    >>
{};

template<class A, class B>
constexpr bool is_same_size_sequence_v = is_same_size_sequence<A, B>::value;

template<class Seq>
struct is_size_one_sequence
    : std::bool_constant<std::conjunction_v<
        fusion::traits::is_sequence<std::remove_cvref_t<Seq>>,
        has_size<Seq, 1>
    >>
{};

template<class Seq>
constexpr bool is_size_one_sequence_v = is_size_one_sequence<Seq>::value;

template<class View>
struct is_size_one_view
    : std::bool_constant<std::conjunction_v<
        fusion::traits::is_view<std::remove_cvref_t<View>>,
        has_size<View, 1>
    >>
{};

template<class View>
constexpr bool is_size_one_view_v = is_size_one_view<View>::value;


template<class T>
struct synthesized_value
{
    using type = T;
};

template<class T>
using synthesized_value_t = typename synthesized_value<T>::type;

template<class T>
    requires is_size_one_sequence_v<std::remove_cvref_t<T>>
struct synthesized_value<T>
{
    using type = std::remove_cvref_t<typename fusion::result_of::front<T>::type>;
};

} // boost::spirit::x4::traits

#endif
