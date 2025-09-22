#ifndef BOOST_SPIRIT_X4_CORE_CONTEXT_HPP
#define BOOST_SPIRIT_X4_CORE_CONTEXT_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/core/type_traits.hpp>

#include <boost/spirit/x4/core/unused.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

template<class ID, class T, class Next>
struct context;

namespace detail {

struct monostate_context_tag;
using monostate_context = context<monostate_context_tag, void, unused_type>;

} // detail


template<class Context, class ID_To_Search>
struct has_context;

template<class Context, class ID_To_Search>
constexpr bool has_context_v = has_context<Context, ID_To_Search>::value;

template<class ID_To_Search>
struct has_context<unused_type, ID_To_Search>
    : std::false_type
{};

template<class T, class Next, class ID_To_Search>
struct has_context<context<ID_To_Search, T, Next>, ID_To_Search>
    : std::true_type
{};

template<class ID, class T, class Next, class ID_To_Search>
    requires (!std::same_as<ID, ID_To_Search>)
struct has_context<context<ID, T, Next>, ID_To_Search>
    : has_context<std::remove_cvref_t<Next>, ID_To_Search>
{};


template<class ID_To_Get, class ID, class T, class Next>
[[nodiscard]] constexpr decltype(auto)
get(context<ID, T, Next> const& ctx) noexcept
{
    if constexpr (has_context_v<context<ID, T, Next>, ID_To_Get>) {
        return ctx.get(std::type_identity<ID_To_Get>{});
    } else {
        return (unused); // return lvalue
    }
}

template<class ID_To_Get>
[[nodiscard]] constexpr unused_type const&
get(unused_type const&) noexcept
{
    return unused;
}

template<class ID_To_Get, class ID, class T, class Next>
void get(context<ID, T, Next> const&&) = delete; // dangling

template<class ID, class Context>
using get_context_plain_t = std::remove_cvref_t<decltype(x4::get<ID>(std::declval<Context const&>()))>;


template<class Context, class ID, class T>
struct has_context_of : std::false_type {};

template<class Context, class ID, class T>
    requires has_context_v<Context, ID>
struct has_context_of<Context, ID, T>
{
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);
    static constexpr bool value = std::same_as<get_context_plain_t<ID, Context>, T>;
};

template<class Context, class ID, class T>
constexpr bool has_context_of_v = has_context_of<Context, ID, T>::value;

namespace detail {

template<class ID>
concept UniqueContextID = !requires { ID::is_unique; } || requires { requires ID::is_unique; };

template<class ID>
concept AllowUnusedContextID = requires { requires ID::allow_unused; };

template<class ID, class Next>
concept HasNoDuplicateContext = !UniqueContextID<ID> || !has_context_v<Next, ID>;

template<class T>
concept ContextValueType =
    !std::same_as<std::remove_cvref_t<T>, unused_type> &&
    !std::is_reference_v<T> &&
    !is_ttp_specialization_of_v<std::remove_const_t<T>, context>;

template<class Next>
concept ContextNextType =
    !std::same_as<std::remove_cvref_t<Next>, unused_type> &&
    (
        (
            std::is_lvalue_reference_v<Next> &&
            std::is_const_v<std::remove_reference_t<Next>>
        ) ||
        (
            !std::is_lvalue_reference_v<Next> &&
            !std::is_rvalue_reference_v<Next> &&
            !std::is_const_v<Next> &&
            std::move_constructible<Next>
        )
    );

template<class ContextWithCVRef>
using canonical_context_t = std::conditional_t<
    std::same_as<std::remove_cvref_t<ContextWithCVRef>, unused_type>,
    unused_type,
    std::conditional_t<
        std::is_lvalue_reference_v<ContextWithCVRef>,
        std::remove_reference_t<ContextWithCVRef> const&, // Next& -> Next const&
        std::remove_cvref_t<ContextWithCVRef> // Next const -> Next
    >
>;

template<class ID, class T, class Next>
struct context_storage
{
    static_assert(ContextValueType<T>);
    static_assert(ContextNextType<Next>);

    // lvalue{lvalue} or non-reference{lvalue}
    constexpr context_storage(T& val BOOST_SPIRIT_LIFETIMEBOUND, std::remove_cvref_t<Next> const& next) noexcept
        : val(val)
        , next(next)
    {}

    // non-reference{rvalue}
    constexpr context_storage(T& val BOOST_SPIRIT_LIFETIMEBOUND, std::remove_cvref_t<Next>&& next)
        noexcept(std::is_nothrow_constructible_v<Next, std::remove_const_t<Next>>)
        requires (!std::is_lvalue_reference_v<Next>)
        : val(val)
        , next(std::move(next))
    {}

    // non-reference{const rvalue}
    constexpr context_storage(T& val BOOST_SPIRIT_LIFETIMEBOUND, std::remove_cvref_t<Next> const&& next)
        noexcept(std::is_nothrow_constructible_v<Next, std::remove_const_t<Next> const>)
        requires (!std::is_lvalue_reference_v<Next>)
        : val(val)
        , next(std::move(next))
    {}

    // lvalue{rvalue}
    context_storage(std::remove_const_t<T> const&, std::remove_cvref_t<Next> const&&) requires std::is_lvalue_reference_v<Next> = delete;
    context_storage(std::remove_const_t<T> const&&, std::remove_cvref_t<Next> const&) = delete;

    [[nodiscard]] constexpr T& get(std::type_identity<ID> const&) const noexcept BOOST_SPIRIT_LIFETIMEBOUND
    {
        return val;
    }

    [[nodiscard]] constexpr decltype(auto) get(auto const& id) const noexcept BOOST_SPIRIT_LIFETIMEBOUND
    {
        return next.get(id);
    }

    T& val;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    BOOST_SPIRIT_NO_UNIQUE_ADDRESS Next next;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

template<class ID, class T, class Next>
    requires std::same_as<std::remove_cvref_t<Next>, unused_type>
struct context_storage<ID, T, Next>
{
    static_assert(ContextValueType<T>);

    constexpr explicit context_storage(T& val BOOST_SPIRIT_LIFETIMEBOUND) noexcept
        : val(val)
    {}

    constexpr context_storage(T& val BOOST_SPIRIT_LIFETIMEBOUND, unused_type const&) noexcept
        : val(val)
    {}

    context_storage(std::remove_const_t<T> const&&) = delete;
    context_storage(std::remove_const_t<T> const&&, unused_type const&) = delete;

    [[nodiscard]] constexpr T& get(std::type_identity<ID> const&) const noexcept BOOST_SPIRIT_LIFETIMEBOUND
    {
        return val;
    }

    static void get(auto const&) = delete; // ID not found

    T& val;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

#define BOOST_SPIRIT_X4_UNUSED_CONTEXT_VALUE_TYPE_ERROR \
    "Assigning `unused` to a unique context does not make sense because " \
    "unique context is binary: it either holds a value or is empty. Introducing " \
    "`unused` makes it tri-state, which is prohibited for maintainability. If you " \
    "want to propagate `unused` as a placeholder (e.g. for debug QoL purpose), " \
    "mark the tag with `allow_unused = true`."

template<class ID, class T, class Next>
    requires std::same_as<std::remove_const_t<T>, unused_type>
struct context_storage<ID, T, Next>
{
    static_assert(ContextNextType<Next>);

    static_assert(
        !detail::UniqueContextID<ID> || detail::AllowUnusedContextID<ID>,
        BOOST_SPIRIT_X4_UNUSED_CONTEXT_VALUE_TYPE_ERROR
    );

    // lvalue{lvalue} or non-reference{lvalue}
    constexpr context_storage(unused_type const&, std::remove_cvref_t<Next> const& next) noexcept
        : next(next)
    {}

    // non-reference{rvalue}
    constexpr context_storage(unused_type const&, std::remove_cvref_t<Next>&& next)
        noexcept(std::is_nothrow_constructible_v<Next, std::remove_cvref_t<Next>>)
        requires (!std::is_lvalue_reference_v<Next>)
        : next(std::move(next))
    {}

    // non-reference{const rvalue}
    constexpr context_storage(unused_type const&, std::remove_cvref_t<Next> const&& next)
        noexcept(std::is_nothrow_constructible_v<Next, std::remove_cvref_t<Next> const>)
        requires (!std::is_lvalue_reference_v<Next>)
        : next(std::move(next))
    {}

    // lvalue{rvalue}
    context_storage(unused_type const&, std::remove_cvref_t<Next> const&&) requires std::is_lvalue_reference_v<Next> = delete;

    [[nodiscard]] static constexpr unused_type const& get(std::type_identity<ID> const&) noexcept
    {
        return unused;
    }

    [[nodiscard]] constexpr decltype(auto) get(auto const& id) const noexcept
    {
        return next.get(id);
    }

    BOOST_SPIRIT_NO_UNIQUE_ADDRESS Next next;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

template<class ID, class T, class Next>
    requires
        std::same_as<std::remove_const_t<T>, unused_type> &&
        std::same_as<std::remove_cvref_t<Next>, unused_type>
struct context_storage<ID, T, Next>
{
    static_assert(
        !detail::UniqueContextID<ID> || detail::AllowUnusedContextID<ID>,
        BOOST_SPIRIT_X4_UNUSED_CONTEXT_VALUE_TYPE_ERROR
    );

    constexpr context_storage() noexcept = default;
    constexpr explicit context_storage(unused_type const&) noexcept {}
    constexpr context_storage(unused_type const&, unused_type const&) noexcept {}

    [[nodiscard]] static constexpr unused_type const& get(std::type_identity<ID> const&) noexcept
    {
        return unused;
    }

    static void get(auto const&) = delete; // ID not found
};

#undef BOOST_SPIRIT_X4_UNUSED_CONTEXT_VALUE_TYPE_ERROR

} // detail

template<class ID, class T, class Next = unused_type>
struct context : detail::context_storage<ID, T, Next>
{
private:
    static_assert(
        !std::same_as<std::remove_cvref_t<Next>, unused_type> || std::same_as<Next, unused_type>,
        "Next cannot be a reference to `unused_type`; use plain type instead"
    );
    static_assert(!std::same_as<std::remove_cvref_t<Next>, detail::monostate_context>);

    using storage_type = detail::context_storage<ID, T, Next>;

public:
    static_assert(detail::HasNoDuplicateContext<ID, std::remove_cvref_t<Next>>);

    using value_type = T;
    using next_type = Next;

    using storage_type::storage_type;
    // TODO: ^^^ Why is clang-tidy complaining about `Rvalue reference parameter "" is never moved`?
};

// Empty context specialization. Materialized when `remove_context` removed everything.
template<class T>
struct context<detail::monostate_context_tag, T>
{
    static_assert(std::is_void_v<T>, "monostate_context's value type must be void");

    using value_type = void;
    using next_type = unused_type;

    void get(auto const&) = delete;
};

template<class ID, class T, class Next>
    requires
        (!std::same_as<std::remove_cvref_t<Next>, unused_type>) &&
        (!std::same_as<std::remove_cvref_t<Next>, detail::monostate_context>)
[[nodiscard]] constexpr context<ID, T, detail::canonical_context_t<Next>>
make_context(T& val, Next&& next)
    noexcept(std::is_nothrow_constructible_v<detail::canonical_context_t<Next>, Next>)
{
    if constexpr (std::is_lvalue_reference_v<Next>) {
        // class `context` can only hold const lvalue reference
        return {val, std::as_const(next)};

    } else {
        return {val, std::forward<Next>(next)};
    }
}

template<class ID, class T>
[[nodiscard]] constexpr context<ID, T>
make_context(T& val, unused_type const&) noexcept
{
    return context<ID, T>{val};
}

template<class ID, class T>
[[nodiscard]] constexpr context<ID, T>
make_context(T& val, detail::monostate_context const&) noexcept
{
    return context<ID, T>{val};
}

template<class ID, class T>
[[nodiscard]] constexpr context<ID, T>
make_context(T& val) noexcept
{
    return context<ID, T>{val};
}

template<class ID, class T, class Next>
void make_context(T const&&, Next const&) = delete; // dangling

template<class ID, class T>
void make_context(T const&&) = delete; // dangling


// Remove the contained reference of the leftmost context having the id `ID_To_Remove`.
template<class ID_To_Remove, class ID, class T, class Next>
[[nodiscard]] constexpr decltype(auto) // may return existing reference in some cases
remove_first_context(context<ID, T, Next> const& ctx) noexcept
{
    static_assert(!std::same_as<ID_To_Remove, detail::monostate_context_tag>);

    // It does not make sense to remove the "first" context of non-unique ID,
    // because if you do so, you just can't determine the meaning of the remaining
    // (duplicate) entries.
    //
    // For example, let the context have below structure:
    //    context<
    //      tag, int,
    //      context<
    //        tag, float,
    //        context<
    //          tag, double
    //        >
    //      >
    //    >
    //
    // Applying `remove_first_context<tag>` results in this:
    //    context<
    //      tag, float,
    //      context<
    //        tag, double
    //      >
    //    >
    //
    // Although the operation is valid in type level, here's the question:
    // what do you *mean* by the remaining `float` and `double` contexts?
    //
    // As you can see, a context may have arbitrary number of duplicate
    // entries, and it just does not make sense to remove the "first" entry.
    //
    // For this reason, a "remove" operation on an any context is semantically
    // valid if and only if the ID is unique. In such case, `remove_first_context`
    // is essentially equal to `remove_*all*_context`, which is our intention.
    static_assert(detail::UniqueContextID<ID_To_Remove>);

    if constexpr (std::same_as<ID, ID_To_Remove>) { // Match
        if constexpr (std::same_as<Next, unused_type>) {
            // Existing context found; removing it will result in an
            // empty context, so create a monostate placeholder.
            return detail::monostate_context{};

        } else {
            // Existing context found; remove it and end the search.
            return (ctx.next); // Parenthesis is important
        }

    } else { // No match
        if constexpr (std::same_as<ID, detail::monostate_context_tag> || std::same_as<Next, unused_type>) {
            // No match at all. Return as-is.
            return ctx;

        } else {
            // No match. Continue the replacement recursively.
            using NewNext = decltype(x4::remove_first_context<ID_To_Remove>(ctx.next));

            if constexpr (std::same_as<std::remove_cvref_t<NewNext>, std::remove_cvref_t<Next>>) {
                // Avoid creating copy on exact same type
                return ctx;

            } else {
                // If the recursive replacement resulted in a monostate context,
                // prevent appending it; return the context without `next`.
                if constexpr (std::same_as<std::remove_cvref_t<NewNext>, detail::monostate_context>) {
                    return context<ID, T>{ctx.val};

                } else if constexpr (std::is_reference_v<NewNext>) {
                    // Assert `decltype(auto)` is working as intended; i.e., no dangling reference
                    static_assert(std::is_lvalue_reference_v<NewNext>);

                    return context<ID, T, NewNext>{
                        ctx.val, x4::remove_first_context<ID_To_Remove>(ctx.next)
                    };

                } else { // prvalue context
                    return context<ID, T, NewNext>{
                        ctx.val, x4::remove_first_context<ID_To_Remove>(ctx.next)
                    };
                }
            }
        }
    }
}

template<class ID_To_Remove, class ID, class T, class Next>
void remove_first_context(context<ID, T, Next> const&&) = delete; // dangling

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
template<class ID_To_Replace, class ID, class T, class Next, class NewVal>
[[nodiscard]] constexpr decltype(auto)
replace_first_context(
    context<ID, T, Next> const& ctx,
    NewVal& new_val BOOST_SPIRIT_LIFETIMEBOUND
) noexcept
{
    static_assert(!is_ttp_specialization_of_v<std::remove_const_t<NewVal>, context>, "context's value type cannot be context");
    static_assert(!std::same_as<ID_To_Replace, detail::monostate_context_tag>);

    if constexpr (
        detail::UniqueContextID<ID_To_Replace> &&
        !detail::AllowUnusedContextID<ID_To_Replace> &&
        std::same_as<std::remove_const_t<NewVal>, unused_type>
    ) {
        (void)new_val; // == unused
        return x4::remove_first_context<ID_To_Replace>(ctx);

    } else {
        if constexpr (std::same_as<ID, ID_To_Replace>) { // Match
            if constexpr (std::same_as<Next, unused_type>) {
                // Existing context found; replace it and end the search.
                return context<ID, NewVal, Next>{new_val};

            } else {
                // Existing context found; replace it and end the search.
                //
                // This implementation does not replace succeeding (duplicate) entries,
                // because it is `replace_*first*_context`.
                return context<ID, NewVal, Next>{new_val, ctx.next};
            }

        } else { // No match
            if constexpr (std::same_as<ID, detail::monostate_context_tag>) {
                // No match at all. Create a brand-new context.
                return context<ID_To_Replace, NewVal>{new_val};

            } else if constexpr (std::same_as<Next, unused_type>) {
                // No match at all. Append a new one and return.
                // Since we're doing the search from left to right,
                // this branch means there was no existing context
                // for `ID_To_Replace`.
                return context<ID, T, context<ID_To_Replace, NewVal>>{
                    ctx.val, context<ID_To_Replace, NewVal>{new_val}
                };

            } else {
                // No match. Continue the replacement recursively.
                return context<ID, T, decltype(x4::replace_first_context<ID_To_Replace>(ctx.next, new_val))>{
                    ctx.val, x4::replace_first_context<ID_To_Replace>(ctx.next, new_val)
                };
            }
        }
    }
}

template<class ID_To_Replace, class ID, class T, class Next, class NewVal>
void replace_first_context(context<ID, T, Next> const&, NewVal const&&) = delete; // dangling

} // boost::spirit::x4

#endif
