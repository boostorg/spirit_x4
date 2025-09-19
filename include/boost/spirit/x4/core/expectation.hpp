/*=============================================================================
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_SUPPORT_EXPECTATION_HPP
#define BOOST_SPIRIT_X4_SUPPORT_EXPECTATION_HPP

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/parser.hpp> // for `x4::what`
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/traits/optional_traits.hpp>

#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    struct expectation_failure_tag;

    template <std::forward_iterator It>
    struct expectation_failure
    {
        template <typename Which>
            requires std::is_constructible_v<std::string, Which>
        constexpr expectation_failure(It where, Which&& which)
            noexcept(std::is_nothrow_copy_constructible_v<It> && std::is_nothrow_constructible_v<std::string, Which>)
            : where_(where)
            , which_(std::forward<Which>(which))
        {}

        [[nodiscard]]
        constexpr It const& where() const noexcept { return where_; }

        [[nodiscard]]
        constexpr std::string const& which() const noexcept { return which_; }

    private:
        It where_;
        std::string which_;
    };


    template <typename Context>
    using expectation_failure_t = std::remove_cv_t<std::remove_reference_t<
        decltype(x4::get<expectation_failure_tag>(std::declval<Context const&>()))>>;

    template <std::forward_iterator It>
    using expectation_failure_optional =
        typename traits::build_optional<expectation_failure<It>>::type;

    template <typename Context>
    constexpr bool has_expectation_failure_context = !std::is_same_v<expectation_failure_t<Context>, unused_type>;

    // x4::where(x), x4::which(x)
    // Convenient accessors for absorbing the variation of
    // optional/reference_wrapper interfaces.

    // beware ADL - we should avoid overgeneralization here.

    namespace expectation_failure_helpers
    {
        // bare type
        template <std::forward_iterator It>
        [[nodiscard]]
        constexpr decltype(auto) where(expectation_failure<It> const& failure) noexcept { return failure.where(); }

        template <std::forward_iterator It>
        [[nodiscard]]
        constexpr decltype(auto) which(expectation_failure<It> const& failure) noexcept { return failure.which(); }

        // std::optional
        template <std::forward_iterator It>
        [[nodiscard]]
        constexpr decltype(auto) where(std::optional<expectation_failure<It>> const& failure) noexcept { return failure->where(); }

        template <std::forward_iterator It>
        [[nodiscard]]
        constexpr decltype(auto) which(std::optional<expectation_failure<It>> const& failure) noexcept { return failure->which(); }

        // std::optional + std::reference_wrapper
        template <std::forward_iterator It>
        [[nodiscard]]
        constexpr decltype(auto) where(std::reference_wrapper<std::optional<expectation_failure<It>>> const& failure) noexcept { return failure.get()->where(); }

        template <std::forward_iterator It>
        [[nodiscard]]
        constexpr decltype(auto) which(std::reference_wrapper<std::optional<expectation_failure<It>>> const& failure) noexcept { return failure.get()->which(); }

    } // expectation_failure_helpers

    using expectation_failure_helpers::where;
    using expectation_failure_helpers::which;

    namespace detail {
        inline constexpr bool has_expectation_failure_impl(unused_type const&) noexcept = delete;

        [[nodiscard]] inline constexpr bool has_expectation_failure_impl(bool& failure) noexcept
        {
            return failure;
        }

        template <std::forward_iterator It>
        [[nodiscard]] constexpr bool has_expectation_failure_impl(std::optional<expectation_failure<It>> const& failure) noexcept
        {
            return failure.has_value();
        }

        template <typename T>
        [[nodiscard]] constexpr bool has_expectation_failure_impl(std::reference_wrapper<T> const& ref) noexcept
        {
            return detail::has_expectation_failure_impl(ref.get());
        }


        template <std::forward_iterator It, typename T>
        constexpr void set_expectation_failure_impl(bool& failure, T&& value)
            noexcept(std::is_nothrow_assignable_v<bool&, T>)
        {
            failure = std::forward<T>(value);
        }

        template <std::forward_iterator It, typename T>
        constexpr void set_expectation_failure_impl(std::optional<expectation_failure<It>>& failure, T&& value)
            noexcept(std::is_nothrow_assignable_v<std::optional<expectation_failure<It>>&, T>)
        {
            failure = std::forward<T>(value);
        }

        template <typename AnyExpectationFailure, typename T>
        constexpr void set_expectation_failure_impl(std::reference_wrapper<AnyExpectationFailure>& failure, T&& value)
            noexcept(std::is_nothrow_assignable_v<AnyExpectationFailure&, T>)
        {
            detail::set_expectation_failure_impl(failure.get(), std::forward<T>(value));
        }


        template <std::forward_iterator It>
        constexpr void clear_expectation_failure_impl(unused_type const&) noexcept = delete;

        template <std::forward_iterator It>
        constexpr void clear_expectation_failure_impl(bool& failure) noexcept
        {
            failure = false;
        }

        template <std::forward_iterator It>
        constexpr void clear_expectation_failure_impl(std::optional<expectation_failure<It>>& failure) noexcept
        {
            failure.reset();
        }

        template <typename T>
        constexpr void clear_expectation_failure_impl(std::reference_wrapper<T>& ref) noexcept
        {
            return detail::clear_expectation_failure_impl(ref.get());
        }

    } // detail

    template <typename Context>
    [[nodiscard]]
    constexpr bool has_expectation_failure(Context const& context) noexcept
    {
        using T = expectation_failure_t<Context>;
        static_assert(
            !std::is_same_v<unused_type, T>,
            "Context type was not specified for x4::expectation_failure_tag. "
            "You probably forgot: `x4::with<x4::expectation_failure_tag>(failure)[p]`. "
            "Note that you must also bind the context to your skipper."
        );
        return detail::has_expectation_failure_impl(x4::get<expectation_failure_tag>(context));
    }

    //
    // Creation of a brand new expectation_failure instance.
    // This is the primary overload.
    //
    template <std::forward_iterator It, typename Subject, typename Context>
    constexpr void set_expectation_failure(
        It const& where,
        Subject const& subject,
        Context const& context
    )
    {
        using T = expectation_failure_t<Context>;
        static_assert(
            !std::is_same_v<unused_type, T>,
            "Context type was not specified for x4::expectation_failure_tag. "
            "You probably forgot: `x4::with<x4::expectation_failure_tag>(failure)[p]`. "
            "Note that you must also bind the context to your skipper."
        );

        if constexpr (std::is_same_v<T, bool>)
        {
            (void)where;
            (void)subject;
            detail::set_expectation_failure_impl(
                x4::get<expectation_failure_tag>(context),
                true
            );
        }
        else
        {
            detail::set_expectation_failure_impl(
                x4::get<expectation_failure_tag>(context),
                expectation_failure<It>(where, x4::what(subject))
            );
        }
    }

    //
    // Copy-assignment of existing expectation_failure instance.
    //
    // When you're in the situation where this functionality is
    // *really* needed, it essentially means that you have
    // multiple valid exceptions at the same time.
    //
    // There are only two decent situations that I can think of:
    //
    //   (a) When you are writing a custom parser procedure with very specific characteristics:
    //       1. You're forking a context.
    //       2. Your parser class has delegated some process to child parser(s).
    //       3. The child parser(s) have raised an exceptation_failure.
    //       4. You need to propagate the failure back to the parent context.
    //
    //   (b) When you truly need a nested exception.
    //       That is, you're trying to preserve a nested exception structure
    //       raised by nested directive: e.g. `x4::expect[x4::expect[p]]`.
    //       Note that all builtin primitives just save the first error,
    //       so this structure does not exist in core (as of now).
    //
    template <typename AnyExpectationFailure, typename Context>
    constexpr void set_expectation_failure(
        AnyExpectationFailure const& existing_failure,
        Context const& context
    ) {
        using T = expectation_failure_t<Context>;
        static_assert(
            !std::is_same_v<T, unused_type>,
            "Context type was not specified for x4::expectation_failure_tag. "
            "You probably forgot: `x4::with<x4::expectation_failure_tag>(failure)[p]`. "
            "Note that you must also bind the context to your skipper."
        );

        static_assert(
            std::is_assignable_v<T, AnyExpectationFailure const&>,
            "previous/current expectation failure types should be compatible"
        );

        detail::set_expectation_failure_impl(x4::get<expectation_failure_tag>(context), existing_failure);
    }

    template <typename Context>
    [[nodiscard]]
    constexpr decltype(auto) get_expectation_failure(Context const& context) noexcept
    {
        using T = expectation_failure_t<Context>;
        static_assert(
            !std::is_same_v<T, unused_type>,
            "Context type was not specified for x4::expectation_failure_tag. "
            "You probably forgot: `x4::with<x4::expectation_failure_tag>(failure)[p]`. "
            "Note that you must also bind the context to your skipper."
        );

        return x4::get<expectation_failure_tag>(context);
    }

    template <typename Context>
    constexpr void clear_expectation_failure(Context const& context) noexcept
    {
        using T = expectation_failure_t<Context>;
        static_assert(
            !std::is_same_v<T, unused_type>,
            "Context type was not specified for x4::expectation_failure_tag. "
            "You probably forgot: `x4::with<x4::expectation_failure_tag>(failure)[p]`. "
            "Note that you must also bind the context to your skipper."
        );
        detail::clear_expectation_failure_impl(x4::get<expectation_failure_tag>(context));
    }

} // boost::spirit::x4

#endif
