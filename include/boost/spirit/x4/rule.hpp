/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X4_RULE_JAN_08_2012_0326PM
#define BOOST_SPIRIT_X4_RULE_JAN_08_2012_0326PM

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/context.hpp>

#include <boost/spirit/x4/traits/transform_attribute.hpp>

#ifdef BOOST_SPIRIT_X4_DEBUG
#include <boost/spirit/x4/debug/simple_trace.hpp>
#endif

#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <boost/config/helper_macros.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

#ifndef BOOST_SPIRIT_X4_NO_RTTI
#include <typeinfo>
#endif

#include <cassert>

namespace boost::spirit::x4
{
    template <typename ID, typename Attribute = unused_type, bool force_attribute = false>
    struct rule;

    struct parse_pass_context_tag;

    namespace detail
    {
        template <typename ID>
        struct rule_id {};

        // Placeholder type to detect whether the default `parse_rule(...)` is called
        enum struct default_parse_rule_result : bool {};

        template <typename ID, typename Context>
        constexpr bool context_has_rule_id = !std::is_same_v<
            std::remove_cvref_t<decltype(x4::get<ID>(std::declval<Context const&>()))>,
            unused_type
        >;
    } // detail

    // The default `parse_rule` definition.
    //
    // This overload will only be selected when there exists no user-defined
    // definition for `parse_rule`.
    //
    // When a user invokes `BOOST_SPIRIT_X4_DEFINE_`, an unconstrained overload
    // is generated at the user's namespace scope. It will never conflict with
    // this (vvvvv) overload, as the generated one is never directly called with
    // a context containing `ID`.
    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        requires detail::context_has_rule_id<ID, Context>
    [[nodiscard]] constexpr detail::default_parse_rule_result
    parse_rule(
        detail::rule_id<ID>,
        It& first, Se const& last,
        Context const& context, Attribute& attr
    )
    {
        auto&& rule_ = x4::get<ID>(context);
        return static_cast<detail::default_parse_rule_result>(
            rule_.parse(first, last, context, unused, attr)
        );
    }

    // This overload is selected only when the user *declares* their `parse_rule`
    // in the user's namespace scope AND the function definition is not found.
    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        requires (!detail::context_has_rule_id<ID, Context>)
    constexpr void
    parse_rule(
        detail::rule_id<ID>,
        It&, Se const&,
        Context const&, Attribute&
    ) = delete; // BOOST_SPIRIT_X4_DEFINE undefined for this rule


    namespace detail
    {
#if defined(BOOST_SPIRIT_X4_DEBUG)
        // TODO: This should be customizable by users
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        struct scoped_debug
        {
            scoped_debug(
                char const* rule_name,
                It const& first, Se const& last,
                Attribute const& attr,
                bool const* parse_ok
            )
                : parse_ok(parse_ok)
                , rule_name(rule_name)
                , first(first)
                , last(last)
                , attr(attr)
                , f(detail::get_simple_trace())
            {
                f(first, last, attr, pre_parse, rule_name);
            }

            ~scoped_debug()
            {
                f(first, last, attr, *parse_ok ? successful_parse : failed_parse, rule_name);
            }

            bool const* parse_ok = nullptr;
            char const* rule_name = nullptr;
            It const& first;
            Se const& last;
            Attribute const& attr;
            detail::simple_trace_type& f;
        };
#endif

        template <typename ID, typename It, typename Se, typename Context>
        concept HasImmutableOnErrorOverload =
            std::forward_iterator<It> &&
            std::sentinel_for<Se, It> &&
            requires(ID& id) { // Note: `ID` should be non-const
                id.on_error(
                    std::declval<It const&>(),
                    std::declval<Se const&>(),
                    std::declval<expectation_failure<It> const&>(),
                    std::declval<Context const&>()
                );
            };

        template <typename ID, typename It, typename Se, typename Context>
        concept HasMutableOnErrorOverload =
            std::forward_iterator<It> &&
            std::sentinel_for<Se, It> &&
            requires(ID& id) { // Note: `ID` should be non-const
                id.on_error(
                    std::declval<It&>(),
                    std::declval<Se&>(),
                    std::declval<expectation_failure<It> const&>(),
                    std::declval<Context const&>()
                );
            };

        template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
        struct has_on_error : std::false_type {};

        template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
            requires HasImmutableOnErrorOverload<ID, It, Se, Context>
        struct has_on_error<ID, It, Se, Context> : std::true_type
        {
            // We intentionally make this hard error to prevent error-prone definition
            static_assert(
                std::is_void_v<
                    decltype(std::declval<ID&>().on_error(
                        std::declval<It const&>(),
                        std::declval<Se const&>(),
                        std::declval<expectation_failure<It> const&>(),
                        std::declval<Context const&>()
                    ))
                >,
                "The return type of `on_error` should be `void`."
            );
        };

        template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
            requires
                (!HasImmutableOnErrorOverload<ID, It, Se, Context>) &&
                HasMutableOnErrorOverload<ID, It, Se, Context>
        struct has_on_error<ID, It, Se, Context> : std::false_type
        {
            // Historically, Spirit has passed mutable lvalue references of the iterators *as-is*
            // to the `on_success`/`on_error` handlers. This behavior was simply a mistake, because:
            //   (1) `on_success`/`on_error` mechanism was designed to be grammar-agnostic, and
            //   (2) it does not make sense to modify the grammar-specific iterator on the
            //       grammar-agnostic callback.
            //
            // Furthermore, any modification to X4's internal iterator variables may invoke
            // undefined behavior, since we never provide any kind of guarantee on how the
            // intermediate iterator variables are processed in X4's implementation details.
            static_assert(
                false,
                "The `on_error` callback should only accept const reference or passed-by-value iterators."
            );
        };

        template <typename ID, typename It, typename Se, typename Attribute, typename Context>
        concept HasImmutableOnSuccessOverload =
            std::forward_iterator<It> &&
            std::sentinel_for<Se, It> &&
            requires(ID& id) { // Note: `ID` should be non-const
                id.on_success(
                    std::declval<It const&>(),
                    std::declval<Se const&>(),
                    std::declval<Attribute&>(),
                    std::declval<Context const&>()
                );
            };

        template <typename ID, typename It, typename Se, typename Attribute, typename Context>
        concept HasMutableOnSuccessOverload =
            std::forward_iterator<It> &&
            std::sentinel_for<Se, It> &&
            requires(ID& id) { // Note: `ID` should be non-const
                id.on_success(
                    std::declval<It&>(),
                    std::declval<Se&>(),
                    std::declval<Attribute&>(),
                    std::declval<Context const&>()
                );
            };

        template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename Context>
        struct has_on_success : std::false_type {};

        template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename Context>
            requires HasImmutableOnSuccessOverload<ID, It, Se, Attribute, Context>
        struct has_on_success<ID, It, Se, Attribute, Context> : std::true_type
        {
            // We intentionally make this hard error to prevent error-prone definition
            static_assert(
                std::is_void_v<
                    decltype(std::declval<ID&>().on_success(
                        std::declval<It const&>(),
                        std::declval<Se const&>(),
                        std::declval<Attribute&>(),
                        std::declval<Context const&>()
                    ))
                >,
                "The return type of `on_success` should be `void`."
            );
        };

        template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename Context>
            requires
                (!HasImmutableOnSuccessOverload<ID, It, Se, Attribute, Context>) &&
                HasMutableOnSuccessOverload<ID, It, Se, Attribute, Context>
        struct has_on_success<ID, It, Se, Attribute, Context> : std::false_type
        {
            // For details, see the comments on `has_on_error`.
            static_assert(
                false,
                "The `on_success` callback should only accept const reference or passed-by-value iterators."
            );
        };

        template <typename Attribute, typename ID, bool SkipDefinitionInjection = false>
        struct rule_parser
        {
            template <
                typename RHS, std::forward_iterator It, std::sentinel_for<It> Se,
                typename Context, typename RContext, typename Exposed
            >
            [[nodiscard]] static constexpr bool
            parse_rhs(
                RHS const& rhs, It& first, Se const& last,
                Context const& context, RContext& rcontext, Exposed& attr
            ) // never noexcept; requires complex handling
            {
                It start = first;

                // See if the user has a BOOST_SPIRIT_X4_DEFINE for this rule
                using parse_rule_result_type = decltype(
                    parse_rule( // ADL
                        std::declval<detail::rule_id<ID>>(), first, last,
                        std::declval<decltype(x4::make_unique_context<ID>(rhs, context))>(),
                        std::declval<Attribute&>()
                    )
                );
                constexpr bool is_default_parse_rule = std::is_same_v<
                    parse_rule_result_type, default_parse_rule_result
                >;

                bool ok;
                if constexpr (SkipDefinitionInjection || !is_default_parse_rule)
                {
                    ok = rhs.parse(first, last, context, rcontext, attr);
                }
                else
                {
                    // If there is no BOOST_SPIRIT_X4_DEFINE for this rule,
                    // we'll make a context for this rule tagged by its ID
                    // so we can extract the rule later on in the default
                    // parse_rule function.
                    auto rule_id_context = x4::make_unique_context<ID>(rhs, context);
                    ok = rhs.parse(first, last, rule_id_context, rcontext, attr);
                }

                // Note: this check uses `It, It` because the value is actually iterator-iterator pair
                if constexpr (has_on_success<ID, It, It, Exposed, Context>::value)
                {
                    if (!ok) return false;

                    x4::skip_over(start, first, context);
                    bool pass = true;
                    ID().on_success(
                        std::as_const(start), std::as_const(first), attr,
                        x4::make_context<parse_pass_context_tag>(pass, context)
                    );
                    return pass;
                }
                else
                {
                    return ok;
                }
            }

            template <
                typename RHS, std::forward_iterator It, std::sentinel_for<It> Se,
                typename Context, typename RContext, typename Exposed
            >
            [[nodiscard]] static constexpr bool
            parse_rhs_with_on_error(
                RHS const& rhs, It& first, Se const& last,
                Context const& context, RContext& rcontext, Exposed& attr
            ) // never noexcept; requires complex handling
            {
                while (true)
                {
                    if (rule_parser::parse_rhs(rhs, first, last, context, rcontext, attr))
                    {
                        return true;
                    }

                    if (x4::has_expectation_failure(context)) {
                        auto const& x = x4::get_expectation_failure(context);
                        static_assert(
                            std::is_void_v<decltype(ID{}.on_error(std::as_const(first), std::as_const(last), *x, context))>,
                            "error handler should not return a value"
                        );
                        ID{}.on_error(std::as_const(first), std::as_const(last), *x, context);
                        return false;
                    }
                    return false;
                }
            }

            template <
                bool ForceAttribute,
                typename RHS, std::forward_iterator It, std::sentinel_for<It> Se,
                typename Context, typename Exposed
            >
            [[nodiscard]] static constexpr bool
            call_rule_definition(
                RHS const& rhs, char const* rule_name,
                It& first, Se const& last,
                Context const& context, Exposed& attr
            )
            {
                // Do down-stream transformation, provides attribute for rhs parser
                using transform = traits::transform_attribute<Attribute, Exposed>;
                using transform_attr = typename transform::type;
                transform_attr attr_ = transform::pre(attr);

                // Creates a place to hold the result of parse_rhs
                // called inside the following scope.
                bool parse_ok;
                {
                #ifdef BOOST_SPIRIT_X4_DEBUG
                    parse_ok = false;

                    // Create a scope to cause the dbg variable below (within
                    // the #if...#endif) to call it's DTOR before any
                    // modifications are made to the attribute, attr_ passed
                    // to parse_rhs (such as might be done in
                    // transform::post when, for example,
                    // Exposed is a recursive variant).
                    scoped_debug<It, Se, transform_attr>
                    dbg(rule_name, first, last, attr_, &parse_ok);
                #else
                    (void)rule_name;
                #endif

                    // In theory, these calls can be overloaded using tag dispatches.
                    // However we should get over those legacy technique already, as they
                    // lead to enormous amount of call stack, generating unreadable
                    // compilation errors. Even when we have a single layer of tag dispatch,
                    // we should generally avoid it because the "true_type/false_type"
                    // argument placed at the very last param of the overload is virtually
                    // indistinguishable in messages and has serious QoL issue in debuggers.
                    constexpr bool rhs_has_on_error = has_on_error<ID, It, Se, Context>::value;

                    // The existence of semantic action inhibits attribute materialization
                    // _unless_ it is explicitly required by the user (primarily via `%=`).
                    if constexpr (RHS::has_action && !ForceAttribute)
                    {
                        if constexpr (rhs_has_on_error)
                        {
                            parse_ok = rule_parser::parse_rhs_with_on_error(
                                rhs, first, last, context, attr_ /* rcontext */, unused
                            );
                        }
                        else
                        {
                            parse_ok = rule_parser::parse_rhs(
                                rhs, first, last, context, attr_ /* rcontext */, unused
                            );
                        }
                    }
                    else // attribute is required
                    {
                        if constexpr (rhs_has_on_error)
                        {
                            parse_ok = rule_parser::parse_rhs_with_on_error(
                                rhs, first, last, context, attr_ /* rcontext */, attr_
                            );
                        }
                        else
                        {
                            parse_ok = rule_parser::parse_rhs(
                                rhs, first, last, context, attr_ /* rcontext */, attr_
                            );
                        }
                    }
                }
                if (parse_ok)
                {
                    // do up-stream transformation, this integrates the results
                    // back into the original attribute value, if appropriate
                    transform::post(attr, std::forward<transform_attr>(attr_));
                }
                return parse_ok;
            }
        };

    } // detail


    template <typename ID, X4Subject RHS, typename Attribute, bool ForceAttribute, bool SkipDefinitionInjection = false>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute, ForceAttribute, SkipDefinitionInjection>>
    {
        using this_type = rule_definition<ID, RHS, Attribute, ForceAttribute, SkipDefinitionInjection>;
        using id = ID;
        using rhs_type = RHS;
        using lhs_type = rule<ID, Attribute, ForceAttribute>;
        using attribute_type = Attribute;

        static constexpr bool has_attribute = !std::is_same_v<Attribute, unused_type>;
        static constexpr bool handles_container = traits::is_container<Attribute>::value;
        static constexpr bool force_attribute = ForceAttribute;

        template <typename RHS_T>
            requires std::is_constructible_v<RHS, RHS_T>
        constexpr rule_definition(RHS_T&& rhs, char const* name)
            noexcept(std::is_nothrow_constructible_v<RHS, RHS_T>)
            : rhs(std::forward<RHS_T>(rhs))
            , name(name)
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute_>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext const&, Attribute_& attr) const
            // never noexcept; requires very complex implementation details
        {
            return detail::rule_parser<attribute_type, ID, SkipDefinitionInjection>
                ::template call_rule_definition<ForceAttribute>(
                    rhs, name, first, last, context, attr
                );
        }

        RHS rhs;
        char const* name = "unnamed";
    };

    template <typename ID, typename Attribute, bool ForceAttribute>
    struct rule : parser<rule<ID, Attribute, ForceAttribute>>
    {
        static_assert(!std::is_reference_v<Attribute>, "Reference qualifier on rule attribute type is meaningless");

        using id = ID;
        using attribute_type = Attribute;
        static constexpr bool has_attribute = !std::is_same_v<std::remove_const_t<Attribute>, unused_type>;
        static constexpr bool handles_container = traits::is_container_v<std::remove_const_t<Attribute>>;
        static constexpr bool force_attribute = ForceAttribute;

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
            // Assert that we are not copying an unitialized static rule. If
            // the static is in another TU, it may be initialized after we copy
            // it. If so, its name member will be nullptr.
            assert(r.name != nullptr && "uninitialized rule"); // static initialization order fiasco
        }

        template <X4Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute>
        operator=(RHS&& rhs) const&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        template <X4Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true>
        operator%=(RHS&& rhs) const&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        // When a rule placeholder constructed and immediately consumed it cannot be used recursively,
        // that's why the rule definition injection into a parser context can be skipped.
        // This optimization has a huge impact on compile times because immediate rules are commonly
        // used to cast an attribute like `as`/`attr_cast` does in Qi.
        template <X4Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute, true>
        operator=(RHS&& rhs) const&&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute, true>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        template <X4Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true, true>
        operator%=(RHS&& rhs) const&&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true, true>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Exposed>
            requires (!std::is_same_v<std::remove_const_t<Exposed>, unused_type>)
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext const&, Exposed& exposed_attr) const
            // never noexcept; requires very complex implementation details
        {
            static_assert(has_attribute, "A rule must have an attribute. Check your rule definition.");

            static_assert(
                traits::Transformable<Attribute, Exposed>,
                "Attribute type mismatch; the rule's attribute is not assignable to "
                "the exposed attribute, and no eligible specialization of "
                "`x4::traits::transform_attribute` was found."
            );

            using transform = traits::transform_attribute<Attribute, Exposed>;
            using transformed_type = typename transform::type;
            transformed_type transformed_attr = transform::pre(exposed_attr);

            // ADL
            if (static_cast<bool>(parse_rule(detail::rule_id<ID>{}, first, last, context, transformed_attr))) {
                transform::post(exposed_attr, std::forward<transformed_type>(transformed_attr));
                return true;
            }
            return false;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext const&, unused_type const&) const
            // never noexcept; requires very complex implementation details
        {
            // make sure we pass exactly the rule attribute type
            attribute_type no_attr; // default-initialize

            // ADL
            return static_cast<bool>(parse_rule(detail::rule_id<ID>{}, first, last, context, no_attr));
        }

        char const* name = "unnamed";
    };

    namespace traits
    {
        template <typename T>
        struct is_rule : std::false_type {};

        template <typename T>
        constexpr bool is_rule_v = is_rule<T>::value;

        template <typename ID, typename Attribute, bool ForceAttribute>
        struct is_rule<rule<ID, Attribute, ForceAttribute>> : std::true_type {};

        template <typename ID, typename Attribute, typename RHS, bool ForceAttribute, bool SkipDefinitionInjection>
        struct is_rule<rule_definition<ID, RHS, Attribute, ForceAttribute, SkipDefinitionInjection>> : std::true_type {};
    }

    template <typename T>
        requires traits::is_rule_v<T>
    struct get_info<T>
    {
        using result_type = std::string;
        [[nodiscard]] std::string operator()(T const& r) const
        {
            assert(r.name != nullptr && "uninitialized rule"); // static initialization order fiasco
            return r.name? r.name : "uninitialized";
        }
    };

// -------------------------------------------------------------

#define BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN_I(x) _Pragma(#x)
#define BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN(msg) BOOST_SPIRIT_X4_DEPRECATED_MACRO_WARN_I(message(msg))

#define BOOST_SPIRIT_X4_DECLARE_(r, constexpr_, rule_type)                                       \
    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>           \
    [[nodiscard]] constexpr_ bool                                                             \
    parse_rule(                                                                               \
        ::boost::spirit::x4::detail::rule_id<typename std::remove_cvref_t<rule_type>::id>,    \
        It& first, Se const& last,                                                            \
        Context const& context, typename std::remove_cvref_t<rule_type>::attribute_type& attr \
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


#define BOOST_SPIRIT_X4_DEFINE_(r, constexpr_, rule_name)                                      \
    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>         \
    [[nodiscard]] constexpr_ bool                                                           \
    parse_rule(                                                                             \
        ::boost::spirit::x4::detail::rule_id<std::remove_cvref_t<decltype(rule_name)>::id>, \
        It& first, Se const& last,                                                          \
        Context const& context,                                                             \
        std::remove_cvref_t<decltype(rule_name)>::attribute_type& attr                      \
    ) {                                                                                     \
        using rule_t = std::remove_cvref_t<decltype(rule_name)>;                            \
        return ::boost::spirit::x4::detail::rule_parser<                                    \
            typename rule_t::attribute_type, typename rule_t::id, true                      \
        >::call_rule_definition<rule_t::force_attribute>(                                   \
            BOOST_JOIN(rule_name, _def), rule_name.name,                                    \
            first, last, context, attr                                                      \
        );                                                                                  \
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

    namespace detail
    {
        template <typename RuleT, std::forward_iterator It, typename A, typename B = void>
        struct instantiate_macro_helper
        {
            using rule_type = RuleT;
            using iterator_type = It;

            // Old API:
            //  A => Context
            //  B => void

            // New API:
            //  A => Se
            //  B => Context

            using sentinel_type = std::conditional_t<std::is_void_v<B>, It, A>;
            static_assert(std::sentinel_for<sentinel_type, It>);

            using context_type = std::conditional_t<std::is_void_v<B>, A, B>;
        };
    } // detail

#define BOOST_SPIRIT_X4_INSTANTIATE_(rule_type, It, Se, Context)                           \
    template bool parse_rule<It, Se, Context>(                                             \
        ::boost::spirit::x4::detail::rule_id<typename std::remove_cvref_t<rule_type>::id>, \
        It&, Se const&, Context const&,                                                    \
        typename std::remove_cvref_t<rule_type>::attribute_type&                           \
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
