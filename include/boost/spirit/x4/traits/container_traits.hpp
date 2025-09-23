#ifndef BOOST_SPIRIT_X4_TRAITS_CONTAINER_TRAITS_HPP
#define BOOST_SPIRIT_X4_TRAITS_CONTAINER_TRAITS_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/core/unused.hpp>

#include <boost/fusion/support/category_of.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/fusion/include/is_sequence.hpp>

#include <ranges>
#include <iterator>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4::traits {

// Customization point
template<class T>
struct is_associative : std::false_type
{
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);
};

template<class T>
    requires requires {
        typename T::key_type;
    }
struct is_associative<T> : std::true_type
{
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);
};

template<class T>
constexpr bool is_associative_v = is_associative<T>::value;


namespace detail {

template<class T>
struct remove_value_const
{
    using type = T;
};

template<class T>
struct remove_value_const<T const> : remove_value_const<T>
{};

template<class F, class S>
struct remove_value_const<std::pair<F, S>>
{
    using type = std::pair<
        typename remove_value_const<F>::type,
        typename remove_value_const<S>::type
    >;
};

} // detail

// Customization point
template<class Container>
struct container_value
    : detail::remove_value_const<typename Container::value_type>
{};

template<class Container>
using container_value_t = typename container_value<Container>::type;

template<class Container>
struct container_value<Container const> : container_value<Container> {};

// There is no single container value for fusion maps, but because output
// of this metafunc is used to check wheter parser's attribute can be
// saved to container, we simply return whole fusion::map as is
// so that check can be done in traits::is_substitute specialisation
template<class T>
    requires
        fusion::traits::is_sequence<T>::value &&
        fusion::traits::is_associative<T>::value
struct container_value<T>
{
    using type = T;
};

template<>
struct container_value<unused_container_type>
{
    using type = unused_type;
};


// Customization point
template<class Container>
    requires requires(Container& c) {
        std::ranges::begin(c);
    }
struct container_iterator
{
    using type = decltype(std::ranges::begin(std::declval<Container&>()));
};

template<class Container>
using container_iterator_t = typename container_iterator<Container>::type;


// Customization point
template<class Container>
    requires requires(Container& c) {
        std::ranges::end(c);
    }
struct container_sentinel
{
    using type = decltype(std::ranges::end(std::declval<Container&>()));
};

template<class Container>
using container_sentinel_t = typename container_sentinel<Container>::type;


// Customization point
template<class Container>
struct push_back_container; // not defined

namespace detail {

struct push_back_fn
{
    template<class Container>
    static constexpr void operator()(Container&, unused_type const&) noexcept
    {
        static_assert(!std::is_same_v<std::remove_const_t<Container>, unused_type>);
        static_assert(!std::is_same_v<std::remove_const_t<Container>, unused_container_type>);
    }

    template<class Container, class T>
        requires requires(Container& c) {
            c.push_back(std::declval<T>());
        }
    static constexpr void operator()(Container& c, T&& val)
        noexcept(noexcept(c.push_back(std::declval<T>())))
    {
        c.push_back(std::forward<T>(val));
    }

    template<class Container, class T>
        requires (!requires(Container& c) {
            c.push_back(std::declval<T>());
        }) && requires(Container& c) {
            c.insert(std::ranges::end(c), std::declval<T>());
        }
    static constexpr void operator()(Container& c, T&& val)
        noexcept(noexcept(c.insert(std::ranges::end(c), std::declval<T>())))
    {
        c.insert(std::ranges::end(c), std::forward<T>(val));
    }

    template<class Container, class T>
        requires requires(Container& c) {
            push_back_container<Container>::call(c, std::declval<T>());
        }
    static constexpr void operator()(Container& c, T&& val)
        noexcept(noexcept(push_back_container<Container>::call(c, std::forward<T>(val))))
    {
        static_assert(!std::is_same_v<std::remove_const_t<Container>, unused_type>);
        static_assert(!std::is_same_v<std::remove_const_t<Container>, unused_container_type>);
        push_back_container<Container>::call(c, std::forward<T>(val));
    }
};

} // detail

inline namespace cpos {

inline constexpr detail::push_back_fn push_back{};

} // cpos


// Customization point
template<class Container>
struct append_container; // not defined

namespace detail {

struct append_fn
{
    template<class Container, std::forward_iterator It, std::sentinel_for<It> Se>
        requires
            is_associative_v<Container> &&
            requires(Container& c, It first, Se last) {
                c.insert(first, last);
            }
    static constexpr void operator()(Container& c, It first, Se last)
        noexcept(noexcept(c.insert(first, last)))
    {
        c.insert(first, last);
    }

    template<class Container, std::forward_iterator It, std::sentinel_for<It> Se>
        requires
            (!is_associative_v<Container>) &&
            requires(Container& c, It first, Se last) {
                c.insert(std::ranges::end(c), first, last);
            }
    static constexpr void operator()(Container& c, It first, Se last)
        noexcept(noexcept(c.insert(std::ranges::end(c), first, last)))
    {
        c.insert(std::ranges::end(c), first, last);
    }

    template<class Container, std::forward_iterator It, std::sentinel_for<It> Se>
        requires requires(Container& c, It first, Se last) {
            append_container<Container>::call(c, first, last);
        }
    static constexpr void operator()(Container& c, It first, Se last)
        noexcept(noexcept(append_container<Container>::call(c, first, last)))
    {
        static_assert(!std::is_same_v<std::remove_const_t<Container>, unused_type>);
        static_assert(!std::is_same_v<std::remove_const_t<Container>, unused_container_type>);
        append_container<Container>::call(c, first, last);
    }
};

} // detail

inline namespace cpos {

inline constexpr detail::append_fn append{};

} // cpos


// Customization point
template<class Container>
struct is_empty_container; // not defined

namespace detail {

struct is_empty_fn
{
    template<class Container>
        requires requires(Container const& c) {
            std::ranges::empty(c);
        }
    [[nodiscard]] static constexpr bool
    operator()(Container const& c) noexcept
    {
        static_assert(!std::is_same_v<Container, unused_type>);
        static_assert(!std::is_same_v<Container, unused_container_type>);
        return std::ranges::empty(c);
    }

    template<class Container>
        requires requires(Container const& c) {
            { is_empty_container<Container>::call(c) } -> std::same_as<bool>;
        }
    [[nodiscard]] static constexpr bool
    operator()(Container const& c)
        noexcept(noexcept(is_empty_container<Container>::call(c)))
    {
        static_assert(!std::is_same_v<Container, unused_type>);
        static_assert(!std::is_same_v<Container, unused_container_type>);
        return is_empty_container<Container>::call(c);
    }
};

} // detail

inline namespace cpos {

inline constexpr detail::is_empty_fn is_empty{};

} // cpos


// Customization point
template<class Container>
struct begin_container; // not defined

namespace detail {

struct begin_fn
{
    template<class Container>
        requires requires(Container& c) {
            std::ranges::begin(c);
        }
    [[nodiscard]] static constexpr container_iterator_t<Container>
    operator()(Container& c)
        noexcept(noexcept(std::ranges::begin(c)))
    {
        static_assert(std::same_as<decltype(std::ranges::begin(c)), container_iterator_t<Container>>);
        return std::ranges::begin(c);
    }

    template<class Container>
        requires requires (Container& c) {
            begin_container<Container>::call(c);
        }
    [[nodiscard]] static constexpr container_iterator_t<Container>
    operator()(Container& c)
        noexcept(noexcept(begin_container<Container>::call(c)))
    {
        static_assert(std::same_as<decltype(begin_container<Container>::call(c)), container_iterator_t<Container>>);
        return begin_container<Container>::call(c);
    }
};

} // detail

inline namespace cpos {

inline constexpr detail::begin_fn begin{};

} // cpos

// Customization point
template<class Container>
struct end_container; // not defined

namespace detail {

struct end_fn
{
    template<class Container>
        requires requires(Container& c) {
            std::ranges::end(c);
        }
    [[nodiscard]] static constexpr container_sentinel_t<Container>
    operator()(Container& c)
        noexcept(noexcept(std::ranges::end(c)))
    {
        static_assert(std::same_as<decltype(std::ranges::end(c)), container_sentinel_t<Container>>);
        return std::ranges::end(c);
    }

    template<class Container>
        requires requires (Container& c) {
            end_container<Container>::call(c);
        }
    [[nodiscard]] static constexpr container_sentinel_t<Container>
    operator()(Container& c)
        noexcept(noexcept(end_container<Container>::call(c)))
    {
        static_assert(std::same_as<decltype(end_container<Container>::call(c)), container_sentinel_t<Container>>);
        return end_container<Container>::call(c);
    }
};

} // detail

inline namespace cpos {

inline constexpr detail::end_fn end{};

} // cpos

// -------------------------------------------------

// This is NOT a customization point. Don't specialize this.
template<class T>
struct is_container : std::false_type
{
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);
};

template<class T>
    requires
        // required; fusion pollutes ADL on `size`, which is called by `std::ranges::empty` on Clang 22
        (!fusion::traits::is_sequence<std::remove_cvref_t<T>>::value) &&
        requires(T& c) {
            typename T::value_type; // required
            traits::begin(c);
            requires std::forward_iterator<decltype(traits::begin(c))>;
            traits::end(c);
            requires std::sentinel_for<decltype(traits::end(c)), decltype(traits::begin(c))>;
            traits::is_empty(c);
            traits::push_back(c, std::declval<typename T::value_type>());
            traits::append(
                c,
                std::declval<decltype(std::make_move_iterator(traits::begin(c)))>(),
                std::declval<decltype(std::make_move_iterator(traits::end(c)))>()
            );
        }
struct is_container<T> : std::true_type
{};

// The attribute category type for `unused_container_type` is
// `container_attribute`, but it does not satisfy `is_container`.

template<class T>
constexpr bool is_container_v = is_container<T>::value;

template<class T>
concept X4Container = is_container_v<std::remove_cvref_t<T>>;

// -------------------------------------------------

// Customization point
template<class T>
struct build_container
{
    using type = std::vector<T>;
};

template<class T>
struct build_container<boost::fusion::deque<T>> : build_container<T> {};

template<>
struct build_container<unused_type>
{
    using type = unused_container_type;
};

template<>
struct build_container<unused_container_type>
{
    using type = unused_container_type;
};

template<>
struct build_container<char>
{
    using type = std::basic_string<char>;
};

template<>
struct build_container<wchar_t>
{
    using type = std::basic_string<wchar_t>;
};

template<>
struct build_container<char8_t>
{
    using type = std::basic_string<char8_t>;
};

template<>
struct build_container<char16_t>
{
    using type = std::basic_string<char16_t>;
};

template<>
struct build_container<char32_t>
{
    using type = std::basic_string<char32_t>;
};

} // boost::spirit::x4::traits

#endif
