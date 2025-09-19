/*=============================================================================
    Copyright (c) 2009 Hartmut Kaiser
    Copyright (c) 2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X4_NUMERIC_BOOL_HPP
#define BOOST_SPIRIT_X4_NUMERIC_BOOL_HPP

#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>

#include <boost/spirit/x4/string/detail/string_parse.hpp>

#include <string_view>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    //  Default boolean policies
    template <typename T = bool>
    struct bool_policies
    {
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename CaseCompare>
        [[nodiscard]] static constexpr bool
        parse_true(It& first, Se const& last, Attribute& attr_, CaseCompare const& case_compare)
            noexcept(std::is_nothrow_constructible_v<Attribute, T>)
        {
            using namespace std::string_view_literals;
            if (detail::string_parse("true"sv, first, last, unused, case_compare))
            {
                x4::move_to(T(true), attr_);    // result is true
                return true;
            }
            return false;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename CaseCompare>
        [[nodiscard]] static constexpr bool
        parse_false(It& first, Se const& last, Attribute& attr_, CaseCompare const& case_compare)
            noexcept(std::is_nothrow_constructible_v<Attribute, T>)
        {
            using namespace std::string_view_literals;
            if (detail::string_parse("false"sv, first, last, unused, case_compare))
            {
                x4::move_to(T(false), attr_);   // result is false
                return true;
            }
            return false;
        }
    };

    template <typename T, typename Encoding, typename BoolPolicies = bool_policies<T>>
    struct bool_parser : parser<bool_parser<T, Encoding, BoolPolicies>>
    {
        using encoding = Encoding;
        using attribute_type = T;

        static constexpr bool has_attribute = true;

        constexpr bool_parser() = default;

        template <typename BoolPoliciesT>
            requires
                (!std::is_same_v<std::remove_cvref_t<BoolPoliciesT>, bool_parser>) &&
                std::is_constructible_v<BoolPolicies, BoolPoliciesT>
        constexpr bool_parser(BoolPoliciesT&& policies)
            noexcept(std::is_nothrow_constructible_v<BoolPolicies, BoolPoliciesT>)
        	: policies_(std::forward<BoolPoliciesT>(policies))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext const&, T& attr) const
            // TODO: noexcet
        {
            x4::skip_over(first, last, context);
            return policies_.parse_true(first, last, attr, get_case_compare<encoding>(context))
                || policies_.parse_false(first, last, attr, get_case_compare<encoding>(context));
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext const&, Attribute& attr_param) const
            // TODO: noexcept
        {
            // this case is called when Attribute is not T
            T attr_;
            if (bool_parser::parse(first, last, context, unused, attr_))
            {
                x4::move_to(std::move(attr_), attr_param);
                return true;
            }
            return false;
        }

    private:
        BoolPolicies policies_{};
    };

    template <typename T, typename Encoding, typename BoolPolicies = bool_policies<T>>
    struct literal_bool_parser : parser<literal_bool_parser<T, Encoding, BoolPolicies>>
    {
        using encoding = Encoding;
        using attribute_type = T;

        static constexpr bool has_attribute = true;

        template <typename Value>
            requires
                (!std::is_same_v<std::remove_cvref_t<Value>, literal_bool_parser>) &&
                std::is_constructible_v<T, Value>
        constexpr literal_bool_parser(Value&& n)
            noexcept(std::is_nothrow_default_constructible_v<BoolPolicies> && std::is_nothrow_constructible_v<T, Value>)
        	: policies_()
            , n_(std::forward<Value>(n))
        {}

        template <typename Value, typename BoolPoliciesT>
            requires
                std::is_constructible_v<T, Value> &&
                std::is_constructible_v<BoolPolicies, BoolPoliciesT>
        constexpr literal_bool_parser(Value&& n, BoolPoliciesT&& policies)
            noexcept(std::is_nothrow_constructible_v<BoolPolicies, BoolPoliciesT> && std::is_nothrow_constructible_v<T, Value>)
        	: policies_(std::forward<BoolPoliciesT>(policies))
            , n_(std::forward<Value>(n))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext const&, T& attr) const
            // TODO: noexcept
        {
            x4::skip_over(first, last, context);
            return (n_ && policies_.parse_true(first, last, attr, x4::get_case_compare<encoding>(context)))
                || (!n_ && policies_.parse_false(first, last, attr, x4::get_case_compare<encoding>(context)));
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext const& rcontext, Attribute& attr_param) const
            // TODO: noexcept
        {
            // this case is called when Attribute is not T
            T attr_;
            if (literal_bool_parser::parse(first, last, context, rcontext, attr_))
            {
                x4::move_to(std::move(attr_), attr_param);
                return true;
            }
            return false;
        }

    private:
        BoolPolicies policies_{};
        T n_;
    };

    namespace standard
    {
        using bool_type = bool_parser<bool, char_encoding::standard>;
        inline constexpr bool_type bool_{};

        using true_type = literal_bool_parser<bool, char_encoding::standard>;
        inline constexpr true_type true_{true};

        using false_type = literal_bool_parser<bool, char_encoding::standard>;
        inline constexpr false_type false_{false};
    }

#ifndef BOOST_SPIRIT_X4_NO_STANDARD_WIDE
    namespace standard_wide
    {
        using bool_type = bool_parser<bool, char_encoding::standard_wide>;
        inline constexpr bool_type bool_{};

        using true_type = literal_bool_parser<bool, char_encoding::standard_wide>;
        inline constexpr true_type true_{true};

        using false_type = literal_bool_parser<bool, char_encoding::standard_wide>;
        inline constexpr false_type false_{false};
    }
#endif

    // TODO: unicode bool parser

    using standard::bool_;
    using standard::true_;
    using standard::false_;

} // boost::spirit::x4

#endif
