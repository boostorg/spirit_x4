#ifndef BOOST_SPIRIT_X4_CORE_PARSER_HPP
#define BOOST_SPIRIT_X4_CORE_PARSER_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/attribute.hpp>
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/traits/attribute.hpp>

#include <iterator>
#include <string>
#include <type_traits>
#include <concepts>
#include <utility>

#ifndef BOOST_SPIRIT_X4_NO_RTTI
#include <typeinfo>
#endif

namespace boost::spirit::x4 {

template<class Subject, class Action>
struct action;

namespace detail {

struct parser_base {};
struct parser_id;

} // detail

template<class Derived>
struct parser : private detail::parser_base
{
    static_assert(!std::is_reference_v<Derived>);
    using derived_type = Derived;

    static constexpr bool handles_container = false;
    static constexpr bool is_pass_through_unary = false;
    static constexpr bool has_action = false;

    [[nodiscard]] constexpr Derived& derived() & noexcept
    {
        return static_cast<Derived&>(*this);
    }

    [[nodiscard]] constexpr Derived const& derived() const& noexcept
    {
        return static_cast<Derived const&>(*this);
    }

    [[nodiscard]] constexpr Derived&& derived() && noexcept
    {
        return static_cast<Derived&&>(*this);
    }

    [[nodiscard]] constexpr Derived const&& derived() const&& noexcept
    {
        return static_cast<Derived const&&>(*this);
    }

    template<class Self, class Action>
        requires std::is_constructible_v<
            action<Derived, std::remove_cvref_t<Action>>,
            decltype(std::declval<Self>().derived()),
            Action
        >
    [[nodiscard]] constexpr action<Derived, std::remove_cvref_t<Action>>
    operator[](this Self&& self, Action&& f)
        noexcept(std::is_nothrow_constructible_v<
            action<Derived, std::remove_cvref_t<Action>>,
            decltype(std::forward<Self>(self).derived()),
            Action
        >)
    {
        return {std::forward<Self>(self).derived(), std::forward<Action>(f)};
    }
};

template<class Subject, class Derived>
struct unary_parser : parser<Derived>
{
    using subject_type = Subject;

    static constexpr bool has_action = Subject::has_action;

    constexpr unary_parser() = default;

    template<class SubjectT>
        requires
            (!std::is_same_v<std::remove_cvref_t<SubjectT>, unary_parser>) &&
            std::is_constructible_v<Subject, SubjectT>
    constexpr unary_parser(SubjectT&& subject)
        noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
        : subject(std::forward<SubjectT>(subject))
    {}

    Subject subject;
};

template<class Left, class Right, class Derived>
struct binary_parser : parser<Derived>
{
    using left_type = Left;
    using right_type = Right;

    static constexpr bool has_action = left_type::has_action || right_type::has_action;

    constexpr binary_parser() = default;

    template<class LeftT, class RightT>
        requires std::is_constructible_v<Left, LeftT> && std::is_constructible_v<Right, RightT>
    constexpr binary_parser(LeftT&& left, RightT&& right)
        noexcept(std::is_nothrow_constructible_v<Left, LeftT> && std::is_nothrow_constructible_v<Right, RightT>)
        : left(std::forward<LeftT>(left))
        , right(std::forward<RightT>(right))
    {}

    // TODO: [MSVC 2022 BUG] "overruns" in constexpr, test case in `lit.cpp`
    /*BOOST_SPIRIT_NO_UNIQUE_ADDRESS*/ Left left;
    /*BOOST_SPIRIT_NO_UNIQUE_ADDRESS*/ Right right;
};

// as_parser: convert a type, T, into a parser.
namespace extension {

// In short: if you want to customize the `as_parser(p)` behavior, just
// specialize `x4::extension::as_parser` for your class.
//
// Older versions of X4 specified the signature `as_spirit_parser(p)` to be
// used for the ADL adaptation of user-defined types. However, many parts in
// X4 were instead using ADL of the form `as_parser(p)` to dispatch any types
// in the first place. This makes it impossible for the control to reach the
// `as_spirit_parser(p)` call if any user-defined `as_parser(p)` is *more
// constrained* than `as_spirit_parser(p)`.
//
// In other words, the old signature `as_spirit_parser(p)` has never been
// implemented correctly since its very first introduction to X4.
//
// Additionally, GitHub fulltext search shows that there exists zero usage of
// `as_spirit_parser` except for the unmaintained blog post written by the
// original inventor. Therefore, we simply removed the feature.
//
// Furthermore, the current maintainer of X4 believes there are no practical
// use cases of ADL adaptation on user-defined types. As such, we make the
// `x4::as_parser` (not to be confused with `x4::extension::as_parser`) to
// model a C++20-ish CPO to inhibit undesired ADL in the first place.

template<class T>
struct as_parser; // not defined

template<>
struct as_parser<unused_type>
{
    using value_type [[deprecated("Use x4::as_parser_plain_t")]] = unused_type;

    template<class T>
    [[nodiscard]] static constexpr auto&& call(T&& unused_) noexcept
    {
        return std::forward<T>(unused_);
    }
};

template<class Derived>
    requires std::is_base_of_v<detail::parser_base, std::remove_cvref_t<Derived>>
struct as_parser<Derived>
{
    using value_type [[deprecated("Use x4::as_parser_plain_t")]] = std::remove_cvref_t<Derived>;

    template<class T>
    [[nodiscard]] static constexpr auto&& call(T&& p) noexcept
    {
        return std::forward<T>(p);
    }
};

template<class Derived>
struct as_parser<parser<Derived>>
{
    using value_type [[deprecated("Use x4::as_parser_plain_t")]] = Derived;

    template<class T>
    [[nodiscard]] static constexpr auto&& call(T&& p) noexcept
    {
        return std::forward<T>(p).derived();
    }
};

} // extension

namespace detail {

struct as_parser_fn
{
    template<class T>
    static void operator()(T&&) = delete; // If you reach here, your specialization of `x4::extension::as_parser` has a wrong signature, or the type is simply incompatible.

    // catch-all default fallback
    template<class T>
        requires std::is_base_of_v<
            parser_base,
            std::remove_cvref_t<decltype(extension::as_parser<std::remove_cvref_t<T>>::call(std::declval<T>()))>
        >
    [[nodiscard]] static constexpr decltype(auto)
    operator()(T&& x) noexcept(noexcept(extension::as_parser<std::remove_cvref_t<T>>::call(std::forward<T>(x))))
    {
        return extension::as_parser<std::remove_cvref_t<T>>::call(std::forward<T>(x));
    }

    template<class Derived>
    [[nodiscard]] static constexpr auto&&
    operator()(parser<Derived>& p) noexcept
    {
        return p.derived();
    }

    template<class Derived>
    [[nodiscard]] static constexpr auto&&
    operator()(parser<Derived> const& p) noexcept
    {
        return p.derived();
    }

    template<class Derived>
    [[nodiscard]] static constexpr auto&&
    operator()(parser<Derived>&& p) noexcept
    {
        return std::move(p).derived();
    }

    template<class Derived>
    [[nodiscard]] static constexpr auto&&
    operator()(parser<Derived> const&& p) noexcept
    {
        return std::move(p).derived();
    }
}; // as_parser_fn

} // detail

inline namespace cpos {

inline constexpr detail::as_parser_fn as_parser{};

} // cpos

template<class T>
using as_parser_t = decltype(as_parser(std::declval<T>())); // If you see an error here, your `T` is not castable to X4's parser type.

template<class T>
using as_parser_plain_t = std::remove_cvref_t<as_parser_t<T>>;


// This is a very low level API provided for consistency with
// `is_parser_nothrow_castable`. Most users should use `X4Subject`
// instead.
template<class T>
struct is_parser_castable
{
    static constexpr bool value = requires {
        { x4::as_parser(std::declval<T>()) };
    };
};

template<class T>
constexpr bool is_parser_castable_v = is_parser_castable<T>::value;

// This trait can be used primarily for multi-parameter constructors.
// For example, `a op b` normally requires the below condition to
// become fully noexcept:
//    is_parser_nothrow_castable_v<A> &&
//    is_parser_nothrow_castable_v<B> &&
//    std::is_nothrow_constructible_v<op_parser, as_parser_t<A>, as_parser_t<B>>
template<class T>
struct is_parser_nothrow_castable
{
    static constexpr bool value = requires {
        { x4::as_parser(std::declval<T>()) } noexcept;
    };
};

template<class T>
constexpr bool is_parser_nothrow_castable_v = is_parser_nothrow_castable<T>::value;


template<class T>
concept X4ExplicitSubject =
    std::is_base_of_v<detail::parser_base, std::remove_cvref_t<T>> &&
    std::move_constructible<std::remove_cvref_t<T>>;
    // Note: a lambda with a capture has a deleted move assignment operator,
    // thus requiring move assignable here would make such `x4::action` to
    // not satisfy this trait; we consider it too strict for now.

template<class T>
concept X4ImplicitSubject =
    !std::is_base_of_v<detail::parser_base, std::remove_cvref_t<T>> &&
    is_parser_castable_v<T> && // `as_parser(t)` is valid?
    X4ExplicitSubject<as_parser_t<T>>;

// A type that models `X4Subject` can be used in generic directives
// and operators. Note that this concept is iterator-agnostic.
//
// For example, let `p` denote an object of `T`. Then, `!p` is a
// well-formed NOT predicate in X4's domain (with "NOT predicate"
// referring to that of the PEG semantics) if and only if `T`
// models `X4Subject`.
template<class T>
concept X4Subject = X4ExplicitSubject<T> || X4ImplicitSubject<T>;


// Checks whether `Parser(as_parser(t))` is valid.
//
// This trait can be used for checking whether a "Parser" is constructible
// with some arbitrary argument `T`. In our (X4 core) use cases, the `Parser`
// is usually a concrete type (e.g. `some_parser<as_parser_plain_t<Subject>>`)
// whereas the `T` is some user-provided arbitrary type (e.g. `X4Subject Subject`).
//
// This interface can only be used to check whether `Parser`'s single-parameter
// constructor is available. For multi-parameter construction, manually combine
// `is_parser_castable` with `std::is_constructible`.
template<X4Subject Parser, X4Subject T>
struct is_parser_constructible : std::false_type {};

template<X4Subject Parser, X4Subject T>
    requires std::is_constructible_v<Parser, as_parser_t<T>>
struct is_parser_constructible<Parser, T> : std::true_type {};

template<X4Subject Parser, X4Subject T>
constexpr bool is_parser_constructible_v = is_parser_constructible<Parser, T>::value;

// Checks whether `Parser(as_parser(t))` is noexcept.
//
// This interface can only be used to check whether `Parser`'s single-parameter
// constructor is available. For multi-parameter construction, manually combine
// `is_parser_nothrow_castable` with `std::is_nothrow_constructible`.
template<X4Subject Parser, X4Subject T>
struct is_parser_nothrow_constructible : std::false_type {};

template<X4Subject Parser, X4Subject T>
    requires
        is_parser_nothrow_castable_v<T> &&
        std::is_nothrow_constructible_v<Parser, as_parser_t<T>>
struct is_parser_nothrow_constructible<Parser, T> : std::true_type {};

template<X4Subject Parser, X4Subject T>
constexpr bool is_parser_nothrow_constructible_v = is_parser_nothrow_constructible<Parser, T>::value;


template<class Parser, class It, class Se, class Context, class Attr>
struct is_parsable
{
    static_assert(X4ExplicitSubject<Parser>);
    static_assert(!std::is_reference_v<It>);
    static_assert(std::forward_iterator<It>);
    static_assert(std::sentinel_for<Se, It>);
    static_assert(!std::is_reference_v<Context>);
    static_assert(!std::is_reference_v<Attr>);
    static_assert(X4Attribute<Attr>);

    static constexpr bool value = requires(Parser const& p) { // mutable parser use case is currently unknown
        {
            p.parse(
                std::declval<It&>(), // first
                std::declval<Se>(), // last
                std::declval<Context const&>(), // context
                std::declval<Attr&>() // attr
            )
        } -> std::same_as<bool>;
    };

    static_assert(!requires(Parser const& p) {
        {
            p.parse(
                std::declval<It&>(), // first
                std::declval<Se>(), // last
                std::declval<Context const&>(), // context
                std::declval<unused_type const&>(), // rcontext
                std::declval<Attr&>() // attr
            )
        } -> std::same_as<bool>;
    }, "X4 can now determine `RContext` automatically. Remove `RContext` from your parser.");
};

template<class Parser, class It, class Se, class Context, class Attr>
constexpr bool is_parsable_v = is_parsable<Parser, It, Se, Context, Attr>::value;

template<class Parser, class It, class Se, class Context, class Attr>
concept Parsable = is_parsable<Parser, It, Se, Context, Attr>::value;
// ^^^ this must be concept in order to provide better diagnostics (e.g. on MSVC)

template<class Parser, class It, class Se, class Context, class Attr>
struct is_nothrow_parsable
{
    static_assert(X4ExplicitSubject<Parser>);
    static_assert(!std::is_reference_v<It>);
    static_assert(std::forward_iterator<It>);
    static_assert(std::sentinel_for<Se, It>);
    static_assert(!std::is_reference_v<Context>);
    static_assert(!std::is_reference_v<Attr>);
    static_assert(X4Attribute<Attr>);

    static constexpr bool value = requires(Parser const& p) { // mutable parser use case is currently unknown
        {
            p.parse(
                std::declval<It&>(), // first
                std::declval<Se>(), // last
                std::declval<Context const&>(), // context
                std::declval<Attr&>() // attr
            )
        } noexcept -> std::same_as<bool>;
    };

    static_assert(!requires(Parser const& p) {
        {
            p.parse(
                std::declval<It&>(), // first
                std::declval<Se>(), // last
                std::declval<Context const&>(), // context
                std::declval<unused_type const&>(), // rcontext
                std::declval<Attr&>() // attr
            )
        } /*noexcept*/ -> std::same_as<bool>;
    }, "X4 can now determine `RContext` automatically. Remove `RContext` from your parser.");
};

template<class Parser, class It, class Se, class Context, class Attr>
constexpr bool is_nothrow_parsable_v = is_nothrow_parsable<Parser, It, Se, Context, Attr>::value;


template<class Parser, class It, class Se>
concept X4ExplicitParser =
    X4ExplicitSubject<Parser> &&
    is_parsable_v<std::remove_cvref_t<Parser>, It, Se, unused_type, unused_type>;

template<class Parser, class It, class Se>
concept X4ImplicitParser =
    X4ImplicitSubject<Parser> &&
    is_parsable_v<as_parser_plain_t<Parser>, It, Se, unused_type, unused_type>;

// The primary "parser" concept of X4, applicable in iterator-aware contexts.
//
// Let `it` denote an lvalue reference of `It`, and let `se` denote a prvalue of `Se`.
// Let `p` denote an lvalue reference of `Parser`.
//
// For `Parser` to model `X4Parser`, the following conditions must be satisfied:
//   -- the expression `x4::as_parser(p)` is well-formed in unevaluated context, and
//   -- the expression `cp.parse(it, se, x4::unused, x4::unused)`
//      is well-formed and the return type is same as `bool`, where `cp` denotes a
//      const lvalue reference to the result of the expression `x4::as_parser(p)`.
//
// Although some exotic user-defined parser could be designed to operate on the very
// specific context type and/or attribute type, we require the parser to at least
// accept `unused_type` for `Context` and `Attribute`. This is because
// core parsers of Spirit have historically been assuming natural use of `unused_type`
// in many locations.
template<class Parser, class It, class Se>
concept X4Parser = X4ExplicitParser<Parser, It, Se> || X4ImplicitParser<Parser, It, Se>;


// The runtime type info that can be obtained via `x4::what(p)`.
template<class Subject>
struct get_info
{
    static_assert(X4Subject<Subject>);

    [[nodiscard]] static constexpr std::string operator()(Subject const& subject)
    {
        if constexpr (requires {
            { subject.get_x4_info() } -> std::convertible_to<std::string>;
        }) {
            return subject.get_x4_info();

        } else {
            (void)subject;
    #ifndef BOOST_SPIRIT_X4_NO_RTTI
            return typeid(Subject).name();
    #else
            return "(get_info undefined)";
    #endif
        }
    }
};

namespace detail {

// "what" is an extremely common identifier that can be defined in many user-specific
// namespaces. We should avoid ADL usage for such generic names in the first place.
// (Note: CPO inhibits ADL in general.)
struct what_fn
{
    template<X4Subject Subject>
    [[nodiscard]] static constexpr std::string operator()(Subject const& p)
    {
        return get_info<Subject>{}(p);
    }
};

} // detail

inline namespace cpos {

inline constexpr detail::what_fn what{}; // no ADL

} // cpos

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

template<class Subject, class Derived, class Context>
struct has_attribute<unary_parser<Subject, Derived>, Context>
    : has_attribute<Subject, Context> {};

template<class Left, class Right, class Derived, class Context>
struct has_attribute<binary_parser<Left, Right, Derived>, Context>
    : std::disjunction<has_attribute<Left, Context>, has_attribute<Right, Context>> {};

} // boost::spirit::x4::traits

#endif
