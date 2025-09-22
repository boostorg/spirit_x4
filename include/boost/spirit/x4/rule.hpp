#ifndef BOOST_SPIRIT_X4_RULE_HPP
#define BOOST_SPIRIT_X4_RULE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/action_context.hpp>

#include <boost/spirit/x4/traits/transform_attribute.hpp>

#ifdef BOOST_SPIRIT_X4_DEBUG
# include <boost/spirit/x4/debug/simple_trace.hpp>
#endif

#include <boost/spirit/x4/debug/detail/parse_callback.hpp>

#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

#ifndef BOOST_SPIRIT_X4_NO_RTTI
# include <typeinfo>
#endif

#include <cassert>

namespace boost::spirit::x4 {

template<class RuleID, X4Attribute Attr = unused_type, bool ForceAttribute = false>
struct rule;

namespace detail {

template<class RuleID>
struct rule_id
{
    static_assert(UniqueContextID<RuleID>);
};

// Placeholder type to detect whether the default `parse_rule(...)` is called
enum struct default_parse_rule_result : bool {};

// The default `parse_rule` definition.
//
// This overload will only be selected when there exists no user-defined
// definition for `parse_rule`.
//
// When a user invokes `BOOST_SPIRIT_X4_DEFINE_`, an unconstrained overload
// is generated at the user's namespace scope. It will never conflict with
// this (vvvvv) overload, as the generated one is never directly called with
// a context containing `RuleID`.
template<class RuleID, std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    requires has_context_v<Context, RuleID>
[[nodiscard]] constexpr default_parse_rule_result
parse_rule(
    rule_id<RuleID>,
    It& first, Se const& last,
    Context const& ctx, Attr& attr
)
{
    auto&& rule_ = x4::get<RuleID>(ctx);

    return static_cast<default_parse_rule_result>(
        rule_.parse(first, last, ctx, attr)
    );
}

// This overload is selected only when the user *declares* their `parse_rule`
// in the user's namespace scope AND the function definition is not found.
template<class RuleID, std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    requires (!has_context_v<Context, RuleID>)
constexpr void
parse_rule(rule_id<RuleID>, It&, Se const&, Context const&, Attr&)
    = delete; // BOOST_SPIRIT_X4_DEFINE undefined for this rule


template<class RuleID, X4Attribute Attr, bool SkipDefinitionInjection = false>
struct rule_impl
{
    static_assert(UniqueContextID<RuleID>);

    template<
        class RHS, std::forward_iterator It, std::sentinel_for<It> Se,
        class RContext, X4Attribute Exposed
    >
    [[nodiscard]] static constexpr bool
    parse_rhs(
        RHS const& rhs, It& first, Se const& last,
        RContext const& rcontext, Exposed& attr
    ) // never noexcept; requires complex handling
    {
        // See if the user has `BOOST_SPIRIT_X4_DEFINE` for this rule
        constexpr bool is_default_parse_rule = std::same_as<
            decltype(parse_rule( // ADL
                std::declval<rule_id<RuleID>>(), first, last,
                std::declval<decltype(x4::make_context<RuleID>(rhs, rcontext))>(),
                std::declval<Attr&>()
            )),
            default_parse_rule_result
        >;

        It start = first; // backup

        //
        // NOTE: The branches below are intentionally written verbosely to make sure
        // we have the minimal call stack. DON'T extract these procedures into a
        // separate function. That would make the compilation error significantly
        // longer in complex scenario.
        //

        bool ok;
        if constexpr (SkipDefinitionInjection || !is_default_parse_rule) {
            ok = rhs.parse(first, last, rcontext, attr);

        } else {
            // If there is no `BOOST_SPIRIT_X4_DEFINE` for this rule,
            // we'll make a context for this rule tagged by its `RuleID`
            // so we can extract the rule later on in the default
            // `parse_rule` overload.
            auto const rule_id_context = x4::make_context<RuleID>(rhs, rcontext);
            ok = rhs.parse(first, last, rule_id_context, attr);
        }

        constexpr bool rhs_has_on_error = has_on_error<RuleID, It, Se, RContext>::value;

        // Note: this check uses `It, It` because the value is actually iterator-iterator pair
        if constexpr (has_on_success<RuleID, It, It, RContext, Exposed>::value) {
            if (ok) {
                x4::skip_over(start, first, rcontext);
                RuleID{}.on_success(std::as_const(start), std::as_const(first), rcontext, attr);
                return true;
            }

            if constexpr (rhs_has_on_error) {
                if (x4::has_expectation_failure(rcontext)) {
                    auto const& x = x4::get_expectation_failure(rcontext);
                    static_assert(
                        std::is_void_v<decltype(RuleID{}.on_error(std::as_const(first), std::as_const(last), rcontext, x))>,
                        "error handler should not return a value"
                    );
                    RuleID{}.on_error(std::as_const(first), std::as_const(last), rcontext, x);
                }
            }
            return false;

        } else { // does not have `on_success`
            if constexpr (rhs_has_on_error) {
                if (ok) return true;

                if (x4::has_expectation_failure(rcontext)) {
                    auto const& x = x4::get_expectation_failure(rcontext);
                    static_assert(
                        std::is_void_v<decltype(RuleID{}.on_error(std::as_const(first), std::as_const(last), rcontext, x))>,
                        "error handler should not return a value"
                    );
                    RuleID{}.on_error(std::as_const(first), std::as_const(last), rcontext, x);
                }
                return false;

            } else {
                return ok;
            }
        }
    }

    template<
        bool ForceAttr,
        class RHS, std::forward_iterator It, std::sentinel_for<It> Se,
        class Context, X4Attribute Exposed
    >
    [[nodiscard]] static constexpr bool
    call_rule_definition(
        RHS const& rhs, char const* rule_name,
        It& first, Se const& last,
        Context const& ctx, Exposed& attr
    )
    {
        // Do down-stream transformation, provide attribute for `rhs` parser
        using transform = traits::transform_attribute<Attr, Exposed>;
        using transform_attr = typename transform::type;
        transform_attr attr_ = transform::pre(attr);

        // Creates a place to hold the result of parse_rhs
        // called inside the following scope.
        bool parse_ok;
        {
        #ifdef BOOST_SPIRIT_X4_DEBUG
            parse_ok = false;

            // Debug on destructor, i.e., before any modifications are made to the
            // attribute passed to `parse_rhs`. Note: the debug must be done before
            // `transform::post`, where some types do some modifications there;
            // for instance, if `Exposed` is a recursive variant.
            scoped_rule_debug<It, Se, transform_attr>
            dbg(rule_name, first, last, attr_, &parse_ok);
        #else
            (void)rule_name;
        #endif

            // Replace or insert the `_val` context, while avoiding infinite instantiation on
            // recursive grammars.
            //
            // In pre-X4 codebase, this was done by passing the extraneous `rcontext` parameter
            // to every `.parse(...)` invocation on ALL parsers. We dropped that param on X4;
            // now we simply append the rule attribute reference to the `context`, as it can
            // essentially work as the global storage for any parsers.
            //
            // Although the `context` acts like the "global" storage, the actual lifetime of the
            // rule-specific attribute is managed by stack-like structure, similar to that of
            // `x4::locals`. Note that we don't actually use `x4::locals` here; we *already have*
            // the stack-like structure because a recursive grammar inherently creates the nested
            // invocation hierarchy, which ultimately does the same thing.
            //
            //   Dear valued user,
            //   You have discovered the deepest wizardry of X4...
            //   You win!!
            //
            auto const rcontext = x4::replace_first_context<contexts::rule_val>(ctx, attr_);

            // The existence of semantic action inhibits attribute materialization
            // _unless_ it is explicitly required by the user (primarily via `%=`).
            if constexpr (RHS::has_action && !ForceAttr) {
                parse_ok = rule_impl::parse_rhs(rhs, first, last, rcontext, unused);

            } else { // Attribute is required
                parse_ok = rule_impl::parse_rhs(rhs, first, last, rcontext, attr_);
            }
        }

        if (parse_ok) {
            // Integrate the results back into the original attribute value, if appropriate
            transform::post(attr, std::forward<transform_attr>(attr_));
        }
        return parse_ok;
    }
};

template<class RuleID, X4Subject RHS, X4Attribute Attr, bool ForceAttr, bool SkipDefinitionInjection = false>
struct rule_definition : parser<rule_definition<RuleID, RHS, Attr, ForceAttr, SkipDefinitionInjection>>
{
    using this_type = rule_definition;
    using id = RuleID;
    using rhs_type = RHS;
    using lhs_type = rule<RuleID, Attr, ForceAttr>;
    using attribute_type = Attr;

    static constexpr bool has_attribute = !std::same_as<Attr, unused_type>;
    static constexpr bool handles_container = traits::is_container<Attr>::value;
    static constexpr bool force_attribute = ForceAttr;

    template<class RHS_T>
        requires std::is_constructible_v<RHS, RHS_T>
    constexpr rule_definition(RHS_T&& rhs, char const* name)
        noexcept(std::is_nothrow_constructible_v<RHS, RHS_T>)
        : rhs(std::forward<RHS_T>(rhs))
        , name(name)
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr_>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr_& attr) const
        // never noexcept; requires very complex implementation details
    {
        return rule_impl<RuleID, attribute_type, SkipDefinitionInjection>
            ::template call_rule_definition<ForceAttr>(
                rhs, name, first, last, ctx, attr
            );
    }

    RHS rhs;
    char const* name = "unnamed";
};

template<class Exposed>
struct narrowing_checker
{
    using Dest = Exposed[];

    // emulate `Exposed x[] = {std::forward<T>(t)};`
    template<class T>
    static void operator()(T&&)
        requires requires(T&& t) { { Dest{std::forward<T>(t)} }; };
};

template<class Exposed, typename Attr>
concept RuleAttrNeedsNarrowingConversion =
    X4Attribute<Attr> &&
    !requires { narrowing_checker<std::remove_const_t<Exposed>>::operator()(std::declval<Attr>()); };

// Resolve "The Spirit X3 rule problem" in Boost.Parser's documentation
// https://www.boost.org/doc/libs/1_89_0/doc/html/boost_parser/this_library_s_relationship_to_boost_spirit.html#boost_parser.this_library_s_relationship_to_boost_spirit.the_spirit_x3_rule_problem
// https://github.com/boostorg/spirit_x4/issues/38
template<class Exposed, typename Attr>
concept RuleAttrTransformable =
    X4Attribute<std::remove_const_t<Exposed>> &&
    X4Attribute<Attr> &&
    std::default_initializable<Attr> &&
    std::is_assignable_v<Exposed&, Attr> &&
    !RuleAttrNeedsNarrowingConversion<Exposed, Attr>;

template<class Exposed, typename Attr>
concept RuleAttrCompatible =
    std::same_as<std::remove_const_t<Exposed>, Attr> ||
    RuleAttrTransformable<Exposed, Attr>;

} // detail

template<class RuleID, X4Attribute RuleAttr, bool ForceAttr>
struct rule : parser<rule<RuleID, RuleAttr, ForceAttr>>
{
    static_assert(!std::is_reference_v<RuleAttr>, "reference type is not allowed for rule attribute type");
    static_assert(X4Attribute<RuleAttr>);

    using id = RuleID;
    using attribute_type = RuleAttr;

    static constexpr bool has_attribute = !std::same_as<std::remove_const_t<RuleAttr>, unused_type>;
    static constexpr bool handles_container = traits::is_container_v<std::remove_const_t<RuleAttr>>;
    static constexpr bool force_attribute = ForceAttr;

    char const* name = "unnamed";

#ifndef BOOST_SPIRIT_X4_NO_RTTI
    rule() : name(typeid(rule).name()) {}
#else
    constexpr rule() noexcept : name("unnamed") {}
#endif

    constexpr rule(char const* name) noexcept
        : name(name)
    {}

    constexpr rule(rule const& r) noexcept
        : name(r.name)
    {
        // Assert that we are not copying an uninitialized static rule. If
        // the static is in another TU, it may be initialized after we copy
        // it. If so, its name member will be nullptr.
        assert(r.name != nullptr && "uninitialized rule"); // static initialization order fiasco
    }

    // Primary overload
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Exposed>
        requires
            (!std::same_as<std::remove_const_t<Exposed>, unused_type>) &&
            detail::RuleAttrCompatible<Exposed, RuleAttr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Exposed& exposed_attr) const
        // never noexcept; requires very complex implementation details
    {
        static_assert(has_attribute, "A rule must have an attribute. Check your rule definition.");

        // Remove the `_val` context. This makes the actual `context` type passed to
        // the (potentially ADL-found) `parse_rule` function to be rule-agnostic.
        // If we don't do this, the specialized function signature becomes
        // nondeterministic, and we lose the opportunity to do explicit template
        // instantiation in `BOOST_SPIRIT_X4_INSTANTIATE`.
        //
        // Note that this removal is possible only because the actual invocation of
        // `parse_rule` *ALWAYS* results in subsequent invocation of `call_rule_definition`,
        // which resets the `_val` context to the appropriate reference.
        auto&& rule_agnostic_ctx = x4::remove_first_context<contexts::rule_val>(ctx);

        using detail::parse_rule;

        if constexpr (std::same_as<std::remove_const_t<Exposed>, RuleAttr>) {
            return static_cast<bool>(parse_rule(detail::rule_id<RuleID>{}, first, last, rule_agnostic_ctx, exposed_attr));

        } else {
            static_assert(detail::RuleAttrTransformable<Exposed, RuleAttr>);
            static_assert(std::is_assignable_v<Exposed&, RuleAttr>);
            static_assert(!detail::RuleAttrNeedsNarrowingConversion<Exposed, RuleAttr>);

            RuleAttr rule_attr;
            if (!static_cast<bool>(parse_rule(detail::rule_id<RuleID>{}, first, last, rule_agnostic_ctx, rule_attr))) {
                return false;
            }

            // X3's behavior
            // x4::move_to(std::move(rule_attr), exposed_attr);
            exposed_attr = std::move(rule_attr);
            return true;
        }
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Exposed>
        requires
            (!std::same_as<std::remove_const_t<Exposed>, unused_type>) &&
            (!detail::RuleAttrCompatible<Exposed, RuleAttr>) &&
            detail::RuleAttrNeedsNarrowingConversion<Exposed, RuleAttr>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, Exposed&) const = delete; // Rule attribute needs narrowing conversion

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, unused_type const&) const
        // never noexcept; requires very complex implementation details
    {
        // make sure we pass exactly the rule attribute type
        attribute_type no_attr; // default-initialize

        // See the comments on the primary overload of `rule::parse(...)`
        auto&& rule_agnostic_ctx = x4::remove_first_context<contexts::rule_val>(ctx);

        // ADL
        using detail::parse_rule;
        return static_cast<bool>(parse_rule(detail::rule_id<RuleID>{}, first, last, rule_agnostic_ctx, no_attr));
    }

    template<X4Subject RHS>
    [[nodiscard]] constexpr detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, ForceAttr>  // NOLINT(misc-unconventional-assign-operator)
    operator=(RHS&& rhs) const&
        noexcept(
            is_parser_nothrow_castable_v<RHS> &&
            std::is_nothrow_constructible_v<
                detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, ForceAttr>,
                as_parser_t<RHS>, char const*
            >
        )
    {
        return {as_parser(std::forward<RHS>(rhs)), name};
    }

    template<X4Subject RHS>
    [[nodiscard]] constexpr detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, true>
    operator%=(RHS&& rhs) const&
        noexcept(
            is_parser_nothrow_castable_v<RHS> &&
            std::is_nothrow_constructible_v<
                detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, true>,
                as_parser_t<RHS>, char const*
            >
        )
    {
        return {as_parser(std::forward<RHS>(rhs)), name};
    }

    // When a rule placeholder constructed and immediately consumed it cannot be used recursively,
    // that's why the rule definition injection into a parser context can be skipped.
    // This optimization has a huge impact on compile times because immediate rules are commonly
    // used to cast an attribute like `as`/`attr_cast` does in Qi.
    template<X4Subject RHS>
    [[nodiscard]] constexpr detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, ForceAttr, true>  // NOLINT(misc-unconventional-assign-operator)
    operator=(RHS&& rhs) const&&
        noexcept(
            is_parser_nothrow_castable_v<RHS> &&
            std::is_nothrow_constructible_v<
                detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, ForceAttr, true>,
                as_parser_t<RHS>, char const*
            >
        )
    {
        return {as_parser(std::forward<RHS>(rhs)), name};
    }

    template<X4Subject RHS>
    [[nodiscard]] constexpr detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, true, true>
    operator%=(RHS&& rhs) const&&
        noexcept(
            is_parser_nothrow_castable_v<RHS> &&
            std::is_nothrow_constructible_v<
                detail::rule_definition<RuleID, as_parser_plain_t<RHS>, RuleAttr, true, true>,
                as_parser_t<RHS>, char const*
            >
        )
    {
        return {as_parser(std::forward<RHS>(rhs)), name};
    }
};

namespace detail {

struct rule_get_info
{
    using result_type = std::string;

    template<class RuleT> // `rule` or `rule_definition`
    [[nodiscard]] static std::string operator()(RuleT const& rule_like)
    {
        assert(rule_like.name != nullptr && "uninitialized rule"); // static initialization order fiasco
        return rule_like.name ? rule_like.name : "uninitialized";
    }
};

} // detail

template<class RuleID, X4Attribute Attr, bool ForceAttr>
struct get_info<rule<RuleID, Attr, ForceAttr>> : detail::rule_get_info {};

template<class RuleID, X4Attribute Attr, class RHS, bool ForceAttr, bool SkipDefinitionInjection>
struct get_info<detail::rule_definition<RuleID, RHS, Attr, ForceAttr, SkipDefinitionInjection>> : detail::rule_get_info {};

// -------------------------------------------------------------

#define BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN_I(x) _Pragma(#x)
#define BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN(msg) BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN_I(message(msg))

#define BOOST_SPIRIT_X4_DECLARE_(r, constexpr_, rule_type) \
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context> \
    [[nodiscard]] constexpr_ bool \
    parse_rule( \
        ::boost::spirit::x4::detail::rule_id<typename std::remove_cvref_t<rule_type>::id>, \
        It& first, Se const& last, \
        Context const& ctx, \
        typename std::remove_cvref_t<rule_type>::attribute_type& attr \
    );

#define BOOST_SPIRIT_X4_DECLARE(rule_type) BOOST_SPIRIT_X4_DECLARE_(,, rule_type)
#define BOOST_SPIRIT_X4_DECLARE_CONSTEXPR(rule_type) BOOST_SPIRIT_X4_DECLARE_(, constexpr, rule_type)

// NB: This can't be `constexpr`, because a constexpr declaration
// cannot be used with explicit template instantiation. We simply
// can't drop (legit) use cases of `BOOST_SPIRIT_X4_INSTANTIATE`, so
// this is a pure technical limitation. If you need `constexpr`
// support in your rule, use `BOOST_SPIRIT_X4_DECLARE_CONSTEXPR`.
#define BOOST_SPIRIT_DECLARE(...) \
    BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN( \
        "Use of variadic arguments with `BOOST_SPIRIT_DECLARE` is deprecated due to the heavy compile-time cost of " \
    "`BOOST_PP_SEQ_*`. Just apply `BOOST_SPIRIT_X4_DECLARE` for each of your rules." \
    ) \
    BOOST_PP_SEQ_FOR_EACH(BOOST_SPIRIT_X4_DECLARE_,, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

// -------------------------------------------------------------

#define BOOST_SPIRIT_X4_DEFINE_(r, constexpr_, rule_name) \
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context> \
    [[nodiscard]] constexpr_ bool \
    parse_rule( \
        ::boost::spirit::x4::detail::rule_id<typename std::remove_cvref_t<decltype(rule_name)>::id>, \
        It& first, Se const& last, \
        Context const& ctx, \
        typename std::remove_cvref_t<decltype(rule_name)>::attribute_type& attr \
    ) { \
        using rule_t = std::remove_cvref_t<decltype(rule_name)>; \
        return ::boost::spirit::x4::detail::rule_impl< \
            typename rule_t::id, typename rule_t::attribute_type, true \
        >::call_rule_definition<rule_t::force_attribute>( \
            BOOST_SPIRIT_CONCAT(rule_name, _def), rule_name.name, \
            first, last, ctx, attr \
        ); \
    }

#define BOOST_SPIRIT_X4_DEFINE(rule_type) BOOST_SPIRIT_X4_DEFINE_(,, rule_type)
#define BOOST_SPIRIT_X4_DEFINE_CONSTEXPR(rule_type) BOOST_SPIRIT_X4_DEFINE_(, constexpr, rule_type)

// NB: This can't be `constexpr`, because a constexpr declaration
// cannot be used with explicit template instantiation. We simply
// can't drop (legit) use cases of `BOOST_SPIRIT_INSTANTIATE`, so
// this is a pure technical limitation. If you need `constexpr`
// support in your rule, use `BOOST_SPIRIT_X4_DEFINE_CONSTEXPR`.
#define BOOST_SPIRIT_DEFINE(...) \
    BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN( \
        "Use of variadic arguments with `BOOST_SPIRIT_DEFINE` is deprecated due to the heavy compile-time cost of " \
    "`BOOST_PP_SEQ_*`. Just apply `BOOST_SPIRIT_X4_DEFINE` for each of your rules." \
    ) \
    BOOST_PP_SEQ_FOR_EACH(BOOST_SPIRIT_X4_DEFINE_,, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

// -------------------------------------------------------------

namespace detail {

// New API
template<class RuleT, std::forward_iterator It, class A, class B = void>
struct instantiate_macro_helper
{
    using rule_type = RuleT;
    using iterator_type = It;

    using sentinel_type = A;
    static_assert(std::sentinel_for<sentinel_type, It>);

    using context_type = B;
};

// Old API
template<class RuleT, std::forward_iterator It, class Context>
struct instantiate_macro_helper<RuleT, It, Context, void>
{
    using rule_type = RuleT;
    using iterator_type = It;
    using sentinel_type = It;
    using context_type = Context;
};

} // detail

#define BOOST_SPIRIT_X4_INSTANTIATE_(rule_type, It, Se, Context) \
    template bool parse_rule<It, Se, Context>( \
        ::boost::spirit::x4::detail::rule_id<typename std::remove_cvref_t<rule_type>::id>, \
        It&, Se const&, Context const&, \
        typename std::remove_cvref_t<rule_type>::attribute_type& \
    );

#define BOOST_SPIRIT_X4_INSTANTIATE_WRAP(...) __VA_ARGS__

// NB: This can't be `constexpr`, because a constexpr declaration
// cannot be used with explicit template instantiation.
#define BOOST_SPIRIT_X4_INSTANTIATE(...) \
    BOOST_SPIRIT_X4_INSTANTIATE_( \
        BOOST_SPIRIT_X4_INSTANTIATE_WRAP(typename ::boost::spirit::x4::detail::instantiate_macro_helper<__VA_ARGS__>::rule_type), \
        BOOST_SPIRIT_X4_INSTANTIATE_WRAP(typename ::boost::spirit::x4::detail::instantiate_macro_helper<__VA_ARGS__>::iterator_type), \
        BOOST_SPIRIT_X4_INSTANTIATE_WRAP(typename ::boost::spirit::x4::detail::instantiate_macro_helper<__VA_ARGS__>::sentinel_type), \
        BOOST_SPIRIT_X4_INSTANTIATE_WRAP(typename ::boost::spirit::x4::detail::instantiate_macro_helper<__VA_ARGS__>::context_type) \
    )

#define BOOST_SPIRIT_INSTANTIATE(...) \
    BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN( \
        "Use `BOOST_SPIRIT_X4_INSTANTIATE`. `BOOST_SPIRIT_INSTANTIATE` is deprecated because " \
        "the name was not correctly prefixed with `X4`." \
    ) \
    BOOST_SPIRIT_X4_INSTANTIATE(__VA_ARGS__)

} // boost::spirit::x4

#endif
