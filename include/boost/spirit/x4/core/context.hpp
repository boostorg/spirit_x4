/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_CONTEXT_JAN_4_2012_1215PM
#define BOOST_SPIRIT_X4_CONTEXT_JAN_4_2012_1215PM

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/unused.hpp>

#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    template <typename T>
    struct owning_context_tag {};

    template <typename ID, typename T, typename Next = unused_type>
    struct context
    {
        static_assert(!std::is_reference_v<T>);
        static_assert(!std::is_reference_v<Next>);
        static_assert(!std::is_const_v<Next>);

        constexpr context(T& val BOOST_SPIRIT_LIFETIMEBOUND, Next const& next BOOST_SPIRIT_LIFETIMEBOUND) noexcept
            : val(val)
            , next(next)
        {}

        context(T&, Next const&&) = delete; // dangling
        context(std::remove_const_t<T> const&&, Next const&) = delete; // dangling
        context(std::remove_const_t<T> const&&, Next const&&) = delete; // dangling

        [[nodiscard]] constexpr T& get(std::type_identity<ID>) const noexcept BOOST_SPIRIT_LIFETIMEBOUND
        {
            return val;
        }

        [[nodiscard]] constexpr decltype(auto) get(auto id) const noexcept
        {
            return next.get(id);
        }

        T& val;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        Next const& next;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    };

    template <typename ID, typename T>
    struct context<ID, T, unused_type>
    {
        constexpr context(T& val BOOST_SPIRIT_LIFETIMEBOUND) noexcept
            requires (!std::is_same_v<std::remove_const_t<T>, context>)
            : val(val)
        {}

        context(std::remove_const_t<T> const&&) = delete; // dangling

        constexpr context(T& val BOOST_SPIRIT_LIFETIMEBOUND, unused_type) noexcept
            : val(val)
        {}

        [[nodiscard]] constexpr T& get(std::type_identity<ID>) const noexcept BOOST_SPIRIT_LIFETIMEBOUND
        {
            return val;
        }

        [[nodiscard]] static constexpr unused_type get(auto) noexcept
        {
            return unused_type{};
        }

        T& val;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    };

    template <typename ID, typename T, typename Next>
    struct context<ID, T, owning_context_tag<Next>>
    {
        static_assert(!std::is_reference_v<Next>);

        template <typename OwningNext>
            requires std::is_constructible_v<Next, OwningNext>
        constexpr context(T& val BOOST_SPIRIT_LIFETIMEBOUND, OwningNext&& owning_next)
            noexcept(std::is_nothrow_constructible_v<Next, OwningNext>)
            : val(val)
            , next(std::forward<OwningNext>(owning_next))
        {}

        template <typename OwningNext>
            requires std::is_constructible_v<Next, OwningNext>
        context(std::remove_const_t<T> const&&, OwningNext&&) = delete; // dangling

        [[nodiscard]] constexpr T& get(std::type_identity<ID>) const noexcept BOOST_SPIRIT_LIFETIMEBOUND
        {
            return val;
        }

        [[nodiscard]] constexpr decltype(auto) get(auto id) const noexcept
        {
            return next.get(id);
        }

        T& val;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        Next next; // not reference
    };

    template <typename ID, typename Context>
    [[nodiscard]] constexpr decltype(auto)
    get(Context const& context) noexcept
    {
        return context.get(std::type_identity<ID>{});
    }

    template <typename ID, typename Context>
    void get(Context const&&) = delete; // dangling

    template <typename ID, typename T, typename Next>
    [[nodiscard]] constexpr context<ID, T, Next>
    make_context(T& val, Next const& next) noexcept
    {
        return { val, next };
    }

    template <typename ID, typename T, typename Next>
    void make_context(T const&&, Next const&) = delete; // dangling

    template <typename ID, typename T, typename Next>
    void make_context(T const&&, Next const&&) = delete; // dangling

    template <typename ID, typename T, typename Next>
    void make_context(T&, Next const&&) = delete; // dangling


    template <typename ID, typename T>
    [[nodiscard]] constexpr context<ID, T>
    make_context(T& val) noexcept
    {
        return { val };
    }

    template <typename ID, typename T>
    void make_context(T const&&) = delete; // dangling

    namespace detail
    {
        template <typename ID, typename T, typename Next, typename FoundVal>
        [[nodiscard]] constexpr Next const&
        make_unique_context(T&, Next const& next BOOST_SPIRIT_LIFETIMEBOUND, FoundVal&) noexcept
        {
            return next;
        }

        template <typename ID, typename T, typename Next>
        [[nodiscard]] constexpr context<ID, T, Next>
        make_unique_context(T& val, Next const& next, unused_type) noexcept
        {
            return { val, next };
        }
    } // detail

    template <typename ID, typename T, typename Next>
    [[nodiscard]] constexpr auto
    make_unique_context(T& val, Next const& next) noexcept
    {
        return detail::make_unique_context<ID>(val, next, x4::get<ID>(next));
    }

    template <typename ID, typename T, typename Next>
    void make_unique_context(T const&&, Next const&) = delete; // dangling

    template <typename ID, typename T, typename Next>
    void make_unique_context(T const&&, Next const&&) = delete; // dangling

    template <typename ID, typename T, typename Next>
    void make_unique_context(T&, Next const&&) = delete; // dangling


    // Replaces the contained reference of the leftmost context
    // having the id `ID_To_Replace`. If no such context exists,
    // append a new one.
    //
    // This helper makes it possible to dynamically update the
    // reference bound to the (runtime) context, while avoiding
    // infinite instantiation in recursive grammars.
    //
    // The most notable example of a parser that requires this
    // operation is `x4::locals`. Without this helper, it would
    // inevitably trigger infinite instantiation when binding
    // a local variable instance to the context.
    template <typename ID_To_Replace, typename ID, typename T, typename Next, typename NewVal>
    [[nodiscard]] constexpr auto replace_context(
        context<ID, T, Next> const& ctx, NewVal& new_val BOOST_SPIRIT_LIFETIMEBOUND
    ) noexcept
    {
        if constexpr (std::is_same_v<ID, ID_To_Replace>)
        {
            if constexpr (std::is_same_v<Next, unused_type>)
            {
                // Existing context found; replace it and end the search.
                return context<ID, T, Next>{ new_val };
            }
            else
            {
                // Existing context found; replace it and end the search.
                //
                // The current implementation does not replace
                // succeeding items, even when there exists
                // duplicate contexts that share the same `ID`.
                // We currently choose this strategy for the sake
                // of compilation speed, and we are not aware of
                // a practical use case of redundant contexts in
                // general.
                return context<ID, T, Next>{ new_val, ctx.next };
            }
        }
        else // No match
        {
            if constexpr (std::is_same_v<Next, unused_type>)
            {
                // No match at all. Append a new one and return.
                // Since we're doing the search from left to right,
                // this branch means there was no existing context
                // for `ID_To_Replace`.
                using NewContext = context<ID_To_Replace, NewVal>;
                return context<ID, T, owning_context_tag<NewContext>>{
                    ctx.val, NewContext{ new_val }
                };
            }
            else
            {
                // No match. Continue the replacement recursively.
                using NewNext = decltype(x4::replace_context<ID_To_Replace>(ctx.next, new_val));
                return context<ID, T, owning_context_tag<NewNext>>{
                    ctx.val, x4::replace_context<ID_To_Replace>(ctx.next, new_val)
                };
            }
        }
    }

    template <typename ID_To_Replace, typename ID, typename T, typename Next, typename NewVal>
    void replace_context(context<ID, T, Next> const&, NewVal const&&) = delete; // dangling

} // boost::spirit::x4

#endif
