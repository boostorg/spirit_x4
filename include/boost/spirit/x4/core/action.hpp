#ifndef BOOST_SPIRIT_X4_CORE_ACTION_HPP
#define BOOST_SPIRIT_X4_CORE_ACTION_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/action_context.hpp>

#include <boost/spirit/x4/traits/attribute.hpp>

#include <ranges>
#include <iterator>
#include <concepts>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

struct raw_attribute_type; // TODO: move this to detail

namespace detail {

// Compose attr(where(val(pass(context))))
template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
struct action_context_impl;

template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
using action_context_t = typename action_context_impl<It, Se, Context, Attr>::type;

template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4NonUnusedAttribute Attr>
struct action_context_impl<It, Se, Context, Attr>
{
    using type = context<
        contexts::attr,
        Attr,
        context<
            contexts::where,
            std::ranges::subrange<It, Se> const,
            context<
                contexts::parse_pass,
                bool,
                canonical_context_t<Context const&>
            >
        >
    >;
};

template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4UnusedAttribute Attr>
struct action_context_impl<It, Se, Context, Attr>
{
    using type = context<
        contexts::where,
        std::ranges::subrange<It, Se> const,
        context<
            contexts::parse_pass,
            bool,
            canonical_context_t<Context const&>
        >
    >;
};

} // detail

// Note about the constraint on the `Action` parameter:
//
// Ideally we should have a context-agnostic concept that can be used
// like `X4ActionFunctor<F>`, but we technically can't.
//
// In order to check `std::invocable`, we need to know the actual context
// type passed to the `.parse(...)` function but it is unknown until
// runtime.
//
// Even if we make up the most trivial context type (i.e. `unused_type`),
// such concept will be useless because a user-provided functor always
// operates on user-specific precondition that assumes the context
// holds exact specific type provided to the entry point (`x4::parse`).

template<class Subject, class Action>
struct action : unary_parser<Subject, action<Subject, Action>>
{
    static_assert(
        !std::is_reference_v<Action>,
        "Reference type is disallowed for semantic action functor to prevent dangling reference"
    );

    using base_type = unary_parser<Subject, action<Subject, Action>>;
    static constexpr bool is_pass_through_unary = true;
    static constexpr bool has_action = true;

    Action f;

    template<class SubjectT, class ActionT>
        requires std::is_constructible_v<base_type, SubjectT> && std::is_constructible_v<Action, ActionT>
    constexpr action(SubjectT&& subject, ActionT&& f)
        noexcept(std::is_nothrow_constructible_v<base_type, SubjectT> && std::is_nothrow_constructible_v<Action, ActionT>)
        : base_type(std::forward<SubjectT>(subject))
        , f(std::forward<ActionT>(f))
    {
    }

    // attr==unused, action wants attribute
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, unused_type) const
        noexcept(
            std::is_nothrow_default_constructible_v<traits::attribute_of_t<action, Context>> &&
            noexcept(this->parse_main(first, last, ctx, std::declval<traits::attribute_of_t<action, Context>&>()))
        )
    {
        using attribute_type = traits::attribute_of_t<action, Context>;

        // Synthesize the attribute since one is not supplied
        attribute_type attribute; // default-initialize
        return this->parse_main(first, last, ctx, attribute);
    }

    // Catch-all overload for non-unused_type attribute
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(noexcept(this->parse_main(first, last, ctx, attr)))
    {
        return this->parse_main(first, last, ctx, attr);
    }

    constexpr void operator[](auto const&) const = delete; // You can't add semantic action for semantic action

private:
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires std::invocable<Action const&, detail::action_context_t<It, Se, Context, Attr> const&>
    [[nodiscard]] constexpr bool
    call_action(
        It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) const noexcept(false) // construction of `subrange` is never noexcept as per the standard
    {
        static_assert(
            std::is_void_v<std::invoke_result_t<Action const&, detail::action_context_t<It, Se, Context, Attr> const&>>,
            "Semantic action should not return value. Check your function signature."
        );

        bool pass = true;

        // TODO:
        // Provide some trait to detect whether this is actually needed for
        // each semantic actions.
        //
        // Although this can be assumed to be eliminated in optimized code,
        // this still may introduce compile time overhead (and also runtime
        // overhead, as constructing `subrange` is never noexcept).
        std::ranges::subrange<It, Se> const where_rng(first, last);

        // Inject `_attr` only when `Attr` is not `unused_type`
        if constexpr (X4UnusedAttribute<Attr>) {
            auto const where_ctx = x4::make_context<contexts::where>(
                where_rng,
                x4::make_context<contexts::parse_pass>(pass, ctx)
            );

            // Sanity check (internal check for detecting implementation divergence)
            static_assert(std::same_as<
                std::remove_const_t<decltype(where_ctx)>,
                detail::action_context_t<It, Se, Context, Attr>
            >);
            this->f(where_ctx);

        } else {
            auto const attr_ctx = x4::make_context<contexts::attr>(
                attr,
                x4::make_context<contexts::where>(
                    where_rng,
                    x4::make_context<contexts::parse_pass>(pass, ctx)
                )
            );

            // Sanity check (internal check for detecting implementation divergence)
            static_assert(std::same_as<
                std::remove_const_t<decltype(attr_ctx)>,
                detail::action_context_t<It, Se, Context, Attr>
            >);

            this->f(attr_ctx);
        }
        return pass;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires (!std::invocable<Action const&, detail::action_context_t<It, Se, Context, Attr> const&>)
    [[nodiscard]] constexpr bool
    call_action(
        It&, Se const&,
        Context const&, Attr&
    ) const noexcept(std::is_nothrow_invocable_v<Action const&>)
    {
        // Explicitly make this hard error instead of emitting "no matching overload".
        // This provides much more human-friendly errors.
        static_assert(
            std::invocable<Action const&>,
            "Neither `f(ctx)` nor `f()` is well-formed for your semantic action. "
            "Check your function signature. Note that some functors might need "
            "`const` qualifier to satisfy the constraints."
        );

        static_assert(
            std::is_void_v<std::invoke_result_t<Action const&>>,
            "Semantic action should not return value. Check your function signature."
        );
        this->f();
        return true;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse_main(
        It& first, Se const& last, Context const& ctx, Attr& attr
    ) const noexcept(
        std::is_copy_assignable_v<It> &&
        is_nothrow_parsable_v<Subject, It, Se, Context, Attr> &&
        noexcept(this->call_action(first, last, ctx, attr))
    )
    {
        It const saved_first = first;
        if (!this->subject.parse(first, last, ctx, attr)) return false;

        if (this->call_action(first, last, ctx, attr)) {
            return true;
        }
        // reset iterators if semantic action failed the match
        // retrospectively
        first = saved_first;
        return false;
    }

    // attr==raw_attribute_type, action wants iterator_range (see raw.hpp)
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse_main(
        It& first, Se const& last, Context const& ctx, raw_attribute_type&
    ) const noexcept(false) // construction of `subrange` is never noexcept as per the standard
    {
        // synthesize the attribute since one is not supplied
        std::ranges::subrange<It, Se> rng;
        return this->parse_main(first, last, ctx, rng);
    }
};

template<X4Subject Subject, class Action>
[[nodiscard, deprecated(
    "Use `operator[]` instead. The symbol `/` normally means \"ordered choice\" "
    "in PEG, and is irrelevant to semantic actions. Furthermore, using C++'s "
    "`operator/` for this purpose may introduce surprising behavior when it's "
    "mixed with ordinary PEG operators, for instance, the unary `operator+`, "
    "due to precedence."
)]]
constexpr action<as_parser_plain_t<Subject>, std::remove_cvref_t<Action>>
operator/(Subject&& p, Action&& f)
    noexcept(
        is_parser_nothrow_castable_v<Subject> &&
        std::is_nothrow_constructible_v<
            action<as_parser_plain_t<Subject>, std::remove_cvref_t<Action>>,
            as_parser_t<Subject>, Action
        >
    )
{
    return {as_parser(std::forward<Subject>(p)), std::forward<Action>(f)};
}

} // boost::spirit::x4

#endif
