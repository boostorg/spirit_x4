#ifndef BOOST_SPIRIT_X4_CORE_MOVE_TO_HPP
#define BOOST_SPIRIT_X4_CORE_MOVE_TO_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/traits/attribute_category.hpp>
#include <boost/spirit/x4/traits/tuple_traits.hpp>
#include <boost/spirit/x4/traits/variant_traits.hpp>

#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/include/copy.hpp>

#include <iterator>
#include <ranges>
#include <type_traits>
#include <concepts>
#include <utility>

namespace boost::spirit::x4 {

// Identical types ---------------------------------------
//
// Note that these overloads must be strictly "more constrained"
// than any of the other overloads.
//
// Such resolution should be possible even without an extra
// constraint that checks `std::is_same_v<Source, Dest>`, thanks to
// the ordinary overload resolution rules of C++.

template<traits::NonUnusedAttr T>
constexpr void move_to(T const&& src, T& dest)
    noexcept(std::is_nothrow_assignable_v<T&, T const&&>)
{
    dest = std::move(src);
}

template<traits::NonUnusedAttr T>
constexpr void move_to(T&& src, T& dest)
    noexcept(std::is_nothrow_assignable_v<T&, T&&>)
{
    dest = std::forward<T>(src);
}

template<traits::NonUnusedAttr T>
constexpr void move_to(T const& src, T& dest)
    noexcept(std::is_nothrow_copy_assignable_v<T>)
{
    dest = src;
}

template<traits::NonUnusedAttr T>
constexpr void move_to(T&, T&) noexcept
{
    static_assert(
        !std::is_const_v<T>,
        "`x4::move_to(T const&, T const&)` is not allowed"
    );

    static_assert(
        false,
        "lvalue reference detected on the `src` argument of `x4::move_to`. "
        "The caller is definitely lacking `std::move` or `std::forward`. If you "
        "intend to *copy* the mutable value, apply `x4::move_to(std::as_const(attr_), attr)`."
    );
    // Banned: possible, but bug-prone.
    // dest = std::move(src);
}

// unused_type -------------------------------------------
template<class Source, class Dest>
    requires
        std::same_as<std::remove_cvref_t<Source>, unused_type> ||
        std::same_as<std::remove_const_t<Dest>, unused_type> ||
        std::same_as<std::remove_cvref_t<Source>, unused_container_type> ||
        std::same_as<std::remove_const_t<Dest>, unused_container_type>
constexpr void move_to(Source&&, Dest&) noexcept
{
    // unused attribute
}

template<std::forward_iterator It, std::sentinel_for<It> Se>
constexpr void move_to(It const&, Se const&, unused_container_type const&) noexcept
{
    // unused attribute
}

template<std::forward_iterator It, std::sentinel_for<It> Se>
constexpr void move_to(It const&, Se const&, unused_type&) = delete; // the call site is lacking `x4::assume_container`

template<std::forward_iterator It, std::sentinel_for<It> Se>
constexpr void move_to(It const&, Se const&, unused_type const&) = delete; // the call site is lacking `x4::assume_container`

template<std::forward_iterator It, std::sentinel_for<It> Se>
constexpr void move_to(It const&, Se const&, unused_type&&) = delete; // the call site is lacking `x4::assume_container`

template<std::forward_iterator It, std::sentinel_for<It> Se>
constexpr void move_to(It const&, Se const&, unused_type const&&) = delete; // the call site is lacking `x4::assume_container`

// Category specific --------------------------------------

template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::plain_attr> Dest>
    requires traits::is_size_one_sequence_v<Source>
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(noexcept(dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)))))
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
    dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)));
}

template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::plain_attr> Dest>
    requires (!traits::is_size_one_sequence_v<Source>)
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
    static_assert(std::is_assignable_v<Dest&, Source&&>);
    dest = std::forward<Source>(src);
}

template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::tuple_attr> Dest>
    requires
        traits::is_same_size_sequence_v<Dest, Source> &&
        (!traits::is_size_one_sequence_v<Dest>)
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(
        std::is_rvalue_reference_v<Source&&> ?
        noexcept(fusion::move(std::move(src), dest)) :
        noexcept(fusion::copy(src, dest))
    )
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

    if constexpr (std::is_rvalue_reference_v<Source&&>) {
        fusion::move(std::move(src), dest);
    } else {
        fusion::copy(src, dest);
    }
}

template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::variant_attr> Dest>
    requires traits::is_size_one_sequence_v<Source> && traits::variant_has_substitute_v<Dest, Source>
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

    // dest is a variant, src is a single element fusion sequence that the variant
    // *can* directly hold.
    dest = std::forward<Source>(src);
}

template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::variant_attr> Dest>
    requires traits::is_size_one_sequence_v<Source> && (!traits::variant_has_substitute_v<Dest, Source>)
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(noexcept(dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)))))
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

    // dest is a variant, src is a single element fusion sequence that the variant
    // cannot directly hold. We'll try to unwrap the single element fusion sequence.

    // Make sure that the Dest variant can really hold Source
    static_assert(
        traits::variant_has_substitute_v<Dest, typename fusion::result_of::front<Source>::type>,
        "Error! The destination variant (Dest) cannot hold the source type (Source)"
    );

    dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)));
}

template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::variant_attr> Dest>
    requires (!traits::is_size_one_sequence_v<Source>)
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
    dest = std::forward<Source>(src);
}

template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::optional_attr> Dest>
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
    dest = std::forward<Source>(src);
}

// Containers -------------------------------------------------

template<std::forward_iterator It, std::sentinel_for<It> Se, traits::CategorizedAttr<traits::container_attr> Dest>
constexpr void
move_to(It first, Se last, Dest& dest)
    // never noexcept, requires container insertion
{
    static_assert(!std::same_as<std::remove_const_t<Dest>, unused_container_type>);

    // Be careful, this may lead to converting surprisingly incompatible types,
    // for example, `std::vector<int>` and `std::set<int>`. Such types must be
    // handled *before* invoking `move_to`.

    if (traits::is_empty(dest)) {
        dest = Dest(first, last);
    } else {
        traits::append(dest, first, last);
    }
}

template<std::forward_iterator It, std::sentinel_for<It> Se, std::ranges::subrange_kind Kind>
constexpr void
move_to(It first, Se last, std::ranges::subrange<It, Se, Kind>& rng)
{
    rng = std::ranges::subrange<It, Se, Kind>(std::move(first), std::move(last));
}

template<std::forward_iterator It, std::sentinel_for<It> Se, traits::CategorizedAttr<traits::tuple_attr> Dest>
    requires traits::is_size_one_sequence_v<Dest>
constexpr void
move_to(It first, Se last, Dest& dest)
    noexcept(noexcept(x4::move_to(first, last, fusion::front(dest))))
{
    x4::move_to(first, last, fusion::front(dest));
}

template<traits::X4Container Source, traits::CategorizedAttr<traits::container_attr> Dest>
constexpr void
move_to(Source&& src, Dest& dest)
    // TODO: noexcept
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

    if constexpr (std::is_rvalue_reference_v<Source&&>) {
        x4::move_to(std::make_move_iterator(std::ranges::begin(src)), std::make_move_iterator(std::ranges::end(src)), dest);
    } else {
        x4::move_to(std::ranges::begin(src), std::ranges::end(src), dest);
    }
}

// Size-one fusion tuple forwarding
template<traits::NonUnusedAttr Source, traits::CategorizedAttr<traits::tuple_attr> Dest>
    requires traits::is_size_one_sequence_v<Dest>
constexpr void
move_to(Source&& src, Dest& dest)
    noexcept(noexcept(x4::move_to(std::forward<Source>(src), fusion::front(dest))))
{
    static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

    x4::move_to(std::forward<Source>(src), fusion::front(dest));
}

template<class Source, class Dest>
concept X4Movable = requires {
    x4::move_to(std::declval<Source>(), std::declval<Dest&>());
};

} // boost::spirit::x4

#endif
