#ifndef BOOST_SPIRIT_X4_NUMERIC_BOOL_HPP
#define BOOST_SPIRIT_X4_NUMERIC_BOOL_HPP

/*=============================================================================
    Copyright (c) 2009 Hartmut Kaiser
    Copyright (c) 2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>

#include <boost/spirit/x4/string/detail/string_parse.hpp>

#include <concepts>
#include <string_view>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

//  Default boolean policies
template<class T = bool>
struct bool_policies
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr, class CaseCompare>
    [[nodiscard]] static constexpr bool
    parse_true(It& first, Se const& last, Attr& attr_, CaseCompare const& compare)
        noexcept(noexcept(x4::move_to(T(true), attr_)))
    {
        using namespace std::string_view_literals;
        if (detail::string_parse("true"sv, first, last, unused_container, compare)) {
            x4::move_to(T(true), attr_);
            return true;
        }
        return false;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr, class CaseCompare>
    [[nodiscard]] static constexpr bool
    parse_false(It& first, Se const& last, Attr& attr_, CaseCompare const& compare)
        noexcept(noexcept(x4::move_to(T(false), attr_)))
    {
        using namespace std::string_view_literals;
        if (detail::string_parse("false"sv, first, last, unused_container, compare)) {
            x4::move_to(T(false), attr_);
            return true;
        }
        return false;
    }
};

namespace detail {

template<class Policy, class Encoding>
concept BoolPolicy = requires(
    typename Encoding::char_type const*& first, typename Encoding::char_type const* const& last,
    unused_type const& ctx, case_compare<Encoding> const& compare
) {
    { Policy::parse_true(first, last, ctx, compare) } -> std::same_as<bool>;
    { Policy::parse_false(first, last, ctx, compare) } -> std::same_as<bool>;
};

} // detail

template<class T, class Encoding, class Policy = bool_policies<T>>
struct bool_parser : parser<bool_parser<T, Encoding, Policy>>
{
    static_assert(X4Attribute<T>);
    static_assert(std::default_initializable<T>);
    static_assert(detail::BoolPolicy<Policy, Encoding>);

    using attribute_type = T;
    using encoding = Encoding;
    using policy_type = Policy;

    static constexpr bool has_attribute = true;

    constexpr bool_parser() = default;

    constexpr bool_parser(Policy const&) = delete; // Policy should be stateless

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class U>
        requires
            std::same_as<std::remove_const_t<U>, T> ||
            std::same_as<std::remove_const_t<U>, unused_type>
    [[nodiscard]] static constexpr bool
    parse(It& first, Se const& last, Context const& ctx, U& attr)
        noexcept(
            noexcept(x4::skip_over(first, last, ctx)) &&
            noexcept(Policy::parse_true(first, last, attr, get_case_compare<encoding>(ctx))) &&
            noexcept(Policy::parse_false(first, last, attr, get_case_compare<encoding>(ctx)))
        )
    {
        x4::skip_over(first, last, ctx);
        return Policy::parse_true(first, last, attr, get_case_compare<encoding>(ctx))
            || Policy::parse_false(first, last, attr, get_case_compare<encoding>(ctx));
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Attr>
    [[nodiscard]] static constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr)
        noexcept(
            std::is_nothrow_default_constructible_v<T> &&
            noexcept(bool_parser::parse(first, last, ctx, std::declval<T&>())) &&
            noexcept(x4::move_to(std::declval<T>(), attr))
        )
    {
        static_assert(X4NonUnusedAttribute<Attr>);

        // this case is called when Attribute is not T
        T attr_;
        if (bool_parser::parse(first, last, ctx, attr_)) {
            x4::move_to(std::move(attr_), attr);
            return true;
        }
        return false;
    }
};

template<class T, class Encoding, class Policy = bool_policies<T>>
struct literal_bool_parser : parser<literal_bool_parser<T, Encoding, Policy>>
{
    static_assert(X4Attribute<T>);
    static_assert(std::default_initializable<T>);
    static_assert(detail::BoolPolicy<Policy, Encoding>);

    using attribute_type = T;
    using encoding = Encoding;

    static constexpr bool has_attribute = true;

    template<class U>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, literal_bool_parser>) &&
            std::is_constructible_v<T, U>
    constexpr literal_bool_parser(U&& expected_bool)
        noexcept(std::is_nothrow_constructible_v<T, U>)
        : expected_bool_(std::forward<U>(expected_bool))
    {}

    template<class U>
    constexpr literal_bool_parser(U&&, Policy const&) = delete; // Policy should be stateless

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class U>
        requires
            std::same_as<std::remove_const_t<U>, T> ||
            std::same_as<std::remove_const_t<U>, unused_type>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, U& attr) const
        noexcept(
            noexcept(x4::skip_over(first, last, ctx)) &&
            noexcept(Policy::parse_true(first, last, attr, x4::get_case_compare<encoding>(ctx))) &&
            noexcept(Policy::parse_false(first, last, attr, x4::get_case_compare<encoding>(ctx)))
        )
    {
        x4::skip_over(first, last, ctx);
        return (expected_bool_ && Policy::parse_true(first, last, attr, x4::get_case_compare<encoding>(ctx)))
            || (!expected_bool_ && Policy::parse_false(first, last, attr, x4::get_case_compare<encoding>(ctx)));
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            std::is_nothrow_default_constructible_v<T> &&
            noexcept(literal_bool_parser::parse(first, last, ctx, std::declval<T&>())) &&
            noexcept(x4::move_to(std::declval<T>(), attr))
        )
    {
        static_assert(X4NonUnusedAttribute<Attr>);

        // this case is called when Attribute is not T
        T attr_;
        if (literal_bool_parser::parse(first, last, ctx, attr_)) {
            x4::move_to(std::move(attr_), attr);
            return true;
        }
        return false;
    }

private:
    T expected_bool_;
};

namespace standard {

inline constexpr bool_parser<bool, char_encoding::standard> bool_{};
inline constexpr literal_bool_parser<bool, char_encoding::standard> true_{true};
inline constexpr literal_bool_parser<bool, char_encoding::standard> false_{false};

} // standard

namespace parsers::standard {

using x4::standard::bool_;
using x4::standard::true_;
using x4::standard::false_;

} // parsers::standard

#ifndef BOOST_SPIRIT_X4_NO_STANDARD_WIDE
namespace standard_wide {

inline constexpr bool_parser<bool, char_encoding::standard_wide> bool_{};
inline constexpr literal_bool_parser<bool, char_encoding::standard_wide> true_{true};
inline constexpr literal_bool_parser<bool, char_encoding::standard_wide> false_{false};

} // standard_wide

namespace parsers::standard_wide {

using x4::standard_wide::bool_;
using x4::standard_wide::true_;
using x4::standard_wide::false_;

} // parsers::standard_wide
#endif

// TODO: unicode bool parser

using standard::bool_;
using standard::true_;
using standard::false_;

namespace parsers {

using x4::standard::bool_;
using x4::standard::true_;
using x4::standard::false_;

} // parsers

} // boost::spirit::x4

#endif
