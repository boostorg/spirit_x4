#ifndef BOOST_SPIRIT_X4_DIRECTIVE_REPEAT_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_REPEAT_HPP

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2014 Thomas Bernard
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/core/expectation.hpp>

#include <iterator>
#include <type_traits>
#include <concepts>
#include <utility>

namespace boost::spirit::x4 {

namespace detail {

// Infinite loop tag type
struct repeat_inf_type
{
    constexpr explicit repeat_inf_type() noexcept = default;
};

template<std::integral T>
struct exact_count // handles repeat(exact)[p]
{
    using value_type = T;
    [[nodiscard]] constexpr bool got_max(T i) const noexcept { return i >= exact_value; }
    [[nodiscard]] constexpr bool got_min(T i) const noexcept { return i >= exact_value; }

    T exact_value;
};

template<std::integral T>
struct finite_count // handles repeat(min, max)[p]
{
    using value_type = T;
    [[nodiscard]] constexpr bool got_max(T i) const noexcept { return i >= max_value; }
    [[nodiscard]] constexpr bool got_min(T i) const noexcept { return i >= min_value; }

    T min_value;
    T max_value;
};

template<std::integral T>
struct infinite_count // handles repeat(min, inf)[p]
{
    using value_type = T;
    [[nodiscard]] constexpr bool got_max(T /*i*/) const noexcept { return false; }
    [[nodiscard]] constexpr bool got_min(T i) const noexcept { return i >= min_value; }

    T min_value;
};

template<class Bounds>
concept RepeatBounds = requires(std::remove_cvref_t<Bounds> const& bounds) {
    typename std::remove_cvref_t<Bounds>::value_type;
    { bounds.got_max(std::declval<typename std::remove_cvref_t<Bounds>::value_type>()) } -> std::same_as<bool>;
    { bounds.got_min(std::declval<typename std::remove_cvref_t<Bounds>::value_type>()) } -> std::same_as<bool>;
};

} // detail

inline namespace cpos {

// Infinite loop tag type
[[deprecated("Use `x4::repeat_inf`")]]
inline constexpr detail::repeat_inf_type inf{};

// Infinite loop tag type
inline constexpr detail::repeat_inf_type repeat_inf{};

} // cpos

template<class Subject, detail::RepeatBounds Bounds>
struct repeat_directive : unary_parser<Subject, repeat_directive<Subject, Bounds>>
{
    using base_type = unary_parser<Subject, repeat_directive<Subject, Bounds>>;
    static constexpr bool is_pass_through_unary = true;
    static constexpr bool handles_container = true;

    template<class SubjectT, detail::RepeatBounds BoundsT>
        requires std::is_constructible_v<base_type, SubjectT> && std::is_constructible_v<Bounds, BoundsT>
    constexpr repeat_directive(SubjectT&& subject, BoundsT&& bounds)
        noexcept(std::is_nothrow_constructible_v<base_type, SubjectT> && std::is_nothrow_constructible_v<Bounds, BoundsT>)
        : base_type(std::forward<SubjectT>(subject))
        , bounds_(std::forward<BoundsT>(bounds))
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        // never noexcept (requires container insertion)
    {
        It local_it = first;
        typename Bounds::value_type i{};
        for (; !bounds_.got_min(i); ++i) {
            if (!detail::parse_into_container(this->subject, local_it, last, ctx, x4::assume_container(attr))) {
                return false;
            }
        }

        first = local_it;
        // parse some more up to the maximum specified
        for (; !bounds_.got_max(i); ++i) {
            if (!detail::parse_into_container(this->subject, first, last, ctx, x4::assume_container(attr))) {
                break;
            }
        }

        if constexpr (has_context_v<Context, contexts::expectation_failure>) {
            return !x4::has_expectation_failure(ctx);
        } else {
            return true;
        }
    }

private:
    Bounds bounds_;
};

namespace detail {

struct repeat_gen
{
    template<X4Subject Subject>
    [[nodiscard, deprecated("`repeat[p]` has the exact same meaning as `*p`. Use `*p` instead.")]]
    constexpr auto operator[](Subject&& subject) const
        noexcept(noexcept(*as_parser(std::forward<Subject>(subject))))
    {
        return *as_parser(std::forward<Subject>(subject));
    }

    template<RepeatBounds Bounds>
    struct [[nodiscard]] repeat_gen_impl
    {
        template<X4Subject Subject>
        [[nodiscard]] constexpr repeat_directive<as_parser_plain_t<Subject>, Bounds>
        operator[](Subject&& subject) const
            noexcept(
                is_parser_nothrow_castable_v<Subject> &&
                std::is_nothrow_constructible_v<
                    repeat_directive<as_parser_plain_t<Subject>, Bounds>,
                    as_parser_t<Subject>,
                    Bounds const&
                >
            )
        {
            return {as_parser(std::forward<Subject>(subject)), bounds};
        }

        Bounds bounds;
    };

    template<std::integral T>
    static constexpr repeat_gen_impl<exact_count<T>>
    operator()(T const exact) noexcept
    {
        return {exact_count<T>{exact}};
    }

    template<std::integral T>
    static constexpr repeat_gen_impl<finite_count<T>>
    operator()(T const min_val, T const max_val) noexcept
    {
        return {finite_count<T>{min_val, max_val}};
    }

    template<std::integral T>
    static constexpr repeat_gen_impl<infinite_count<T>>
    operator()(T const min_val, repeat_inf_type const&) noexcept
    {
        return {infinite_count<T>{min_val}};
    }
};

} // detail

namespace parsers::directive {

inline constexpr detail::repeat_gen repeat{};

} // parsers::directive

using parsers::directive::repeat;

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

template<class Subject, class Bounds, class Context>
struct attribute_of<x4::repeat_directive<Subject, Bounds>, Context>
    : build_container<attribute_of_t<Subject, Context>>
{};

} // boost::spirit::x4::traits

#endif
