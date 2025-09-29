#ifndef BOOST_SPIRIT_X4_TRAITS_ATTRIBUTE_CATEGORY_HPP
#define BOOST_SPIRIT_X4_TRAITS_ATTRIBUTE_CATEGORY_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/traits/subrange_traits.hpp>
#include <boost/spirit/x4/traits/variant_traits.hpp>
#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/optional_traits.hpp>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/support/category_of.hpp>

#include <concepts>
#include <type_traits>

namespace boost::spirit::x4 {

struct unused_type;
struct unused_container_type;

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

struct unused_attr {};
struct plain_attr {};
struct container_attr {};
struct tuple_attr {};
struct variant_attr {};
struct optional_attr {};
struct subrange_attr {};

template<class T>
struct attribute_category
{
    using type = plain_attr;
};

template<class T>
struct attribute_category<T const> : attribute_category<T> {};

template<class T>
struct attribute_category<T&> : attribute_category<T> {};

template<class T>
struct attribute_category<T const&> : attribute_category<T> {};

template<class T>
struct attribute_category<T&&> : attribute_category<T> {};

template<class T>
struct attribute_category<T const&&> : attribute_category<T> {};

template<class T>
using attribute_category_t = typename attribute_category<T>::type;

template<>
struct attribute_category<unused_type>
{
    using type = unused_attr;
};

template<>
struct attribute_category<unused_container_type>
{
    using type = container_attr;

    // The attribute category type for `unused_container_type` is
    // `container_attribute`, but it does not satisfy `is_container`.
};

template<class T, typename AttrCategoryTag>
concept CategorizedAttr =
    std::same_as<typename attribute_category<std::remove_cvref_t<T>>::type, AttrCategoryTag>;

template<class T>
concept NonUnusedAttr = !CategorizedAttr<T, unused_attr>;

template<class T>
    requires
        fusion::traits::is_sequence<std::remove_cvref_t<T>>::value
struct attribute_category<T>
{
    using type = tuple_attr;
};

template<class T>
    requires is_variant_v<std::remove_cvref_t<T>>
struct attribute_category<T>
{
    using type = variant_attr;
};

template<class T>
    requires is_optional_v<std::remove_cvref_t<T>>
struct attribute_category<T>
{
    using type = optional_attr;
};

template<class T>
    requires is_subrange_v<std::remove_cvref_t<T>>
struct attribute_category<T>
{
    using type = subrange_attr;
};

template<class T>
    requires traits::is_container_v<std::remove_cvref_t<T>>
struct attribute_category<T>
{
    using type = container_attr;
};

} // boost::spirit::x4::traits

#endif
