/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_PARSE_APRIL_16_2006_0442PM
#define BOOST_SPIRIT_X4_PARSE_APRIL_16_2006_0442PM

#include <boost/spirit/x4/core/config.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/action.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/expectation.hpp>

#include <boost/spirit/x4/traits/string_traits.hpp>

#include <boost/spirit/x4/parse_result.hpp>

#include <optional>
#include <iterator>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    enum class root_skipper_flag : char
    {
        do_post_skip, // force post-skipping
        post_skip [[deprecated("Use `do_post_skip`")]] = do_post_skip,

        dont_post_skip // inhibit post-skipping
    };

    using skip_flag [[deprecated("Use `root_skipper_flag`")]] = root_skipper_flag;

    namespace detail
    {
        template <typename Parser, typename R>
        concept X4RangeParseParser =
            X4Parser<
                Parser,
                typename detail::range_parse_parser_impl<R>::iterator_type,
                typename detail::range_parse_parser_impl<R>::sentinel_type
            >;

        template <typename Skipper, typename R>
        concept X4RangeParseSkipper =
            X4ExplicitParser<
                Skipper,
                typename detail::range_parse_parser_impl<R>::iterator_type,
                typename detail::range_parse_parser_impl<R>::sentinel_type
            >;

        struct parse_fn_main
        {
            // --------------------------------------------
            // parse(it/se)

            // It/Se + Parser + Attribute
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser>
            static constexpr parse_result<It, Se>
            operator()(It first, Se last, Parser&& p, X4Attribute auto& attr)
            {
                std::optional<expectation_failure<It>> expect_failure;
                auto failure_ctx = x4::make_context<expectation_failure_tag>(expect_failure);

                bool const ok = as_parser(std::forward<Parser>(p)).parse(first, last, failure_ctx, unused, attr);
                return parse_result<It, Se>{
                    .ok = ok,
                    .expect_failure = std::move(expect_failure),
                    .remainder = {std::move(first), std::move(last)}
                };
            }

            // parse_result + It/Se + Parser + Attribute
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser>
            static constexpr void
            operator()(parse_result<It, Se>& res, It first, Se last, Parser&& p, X4Attribute auto& attr)
            {
                res.expect_failure.reset();
                auto failure_ctx = x4::make_context<expectation_failure_tag>(res.expect_failure);

                res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, failure_ctx, unused, attr);
                res.remainder = {std::move(first), std::move(last)};
            }

            // --------------------------------------------
            // parse(range)

            // R + Parser + Attribute
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser>
                requires (!traits::CharArray<R>)
            static constexpr parse_result_for<R>
            operator()(R const& range, Parser&& p, X4Attribute auto& attr)
            {
                using It = std::ranges::iterator_t<R const>;
                using Se = std::ranges::sentinel_t<R const>;

                std::optional<expectation_failure<It>> expect_failure;
                auto failure_ctx = x4::make_context<expectation_failure_tag>(expect_failure);

                It first = std::ranges::begin(range);
                Se last = std::ranges::end(range);
                bool const ok = as_parser(std::forward<Parser>(p)).parse(first, last, failure_ctx, unused, attr);
                return parse_result_for<R>{
                    .ok = ok,
                    .expect_failure = std::move(expect_failure),
                    .remainder = {std::move(first), std::move(last)}
                };
            }

            // "str" + Parser + Attribute
            template <traits::CharArray R, X4RangeParseParser<R> Parser>
            static constexpr parse_result_for<R>
            operator()(R const& str, Parser&& p, X4Attribute auto& attr)
            {
                return parse_fn_main::operator()(std::basic_string_view{str}, std::forward<Parser>(p), attr);
            }

            // parse_result + R + Parser + Attribute
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser>
                requires (!traits::CharArray<R>)
            static constexpr void
            operator()(parse_result_for<R>& res, R const& range, Parser&& p, X4Attribute auto& attr)
            {
                using It = std::ranges::iterator_t<R const>;
                using Se = std::ranges::sentinel_t<R const>;

                res.expect_failure.reset();
                auto failure_ctx = x4::make_context<expectation_failure_tag>(res.expect_failure);

                It first = std::ranges::begin(range);
                Se last = std::ranges::end(range);
                res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, failure_ctx, unused, attr);
                res.remainder = {std::move(first), std::move(last)};
            }

            // parse_result + "str" + Parser + Attribute
            template <traits::CharArray R, X4RangeParseParser<R> Parser>
            static constexpr void
            operator()(parse_result_for<R>& res, R const& str, Parser&& p, X4Attribute auto& attr)
            {
                return parse_fn_main::operator()(res, std::basic_string_view{str}, std::forward<Parser>(p), attr);
            }

            // --------------------------------------------
            // phrase_parse(it/se)

            // It/Se + Parser + Skipper + Attribute + (root_skipper_flag)
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper>
            static constexpr parse_result<It, Se>
            operator()(It first, Se last, Parser&& p, X4Attribute auto& attr, Skipper const& s, root_skipper_flag flag = root_skipper_flag::do_post_skip)
            {
                auto skipper_ctx = x4::make_context<skipper_tag>(s);

                std::optional<expectation_failure<It>> expect_failure;
                auto ctx = x4::make_context<expectation_failure_tag>(expect_failure, skipper_ctx);

                bool ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, unused, attr);
                if (ok && flag == root_skipper_flag::do_post_skip)
                {
                    x4::skip_over(first, last, ctx);
                    // ReSharper disable once CppAssignedValueIsNeverUsed
                    if (expect_failure) [[unlikely]] ok = false;
                }
                return parse_result<It, Se>{
                    .ok = ok,
                    .expect_failure = std::move(expect_failure),
                    .remainder = {std::move(first), std::move(last)}
                };
            }

            // parse_result + It/Se + Parser + Skipper + Attribute + (root_skipper_flag)
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper>
            static constexpr void
            operator()(parse_result<It, Se>& res, It first, Se last, Parser&& p, X4Attribute auto& attr, Skipper const& s, root_skipper_flag flag = root_skipper_flag::do_post_skip)
            {
                auto skipper_ctx = x4::make_context<skipper_tag>(s);

                res.expect_failure.reset();
                auto ctx = x4::make_context<expectation_failure_tag>(res.expect_failure, skipper_ctx);

                res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, unused, attr);
                if (res.ok && flag == root_skipper_flag::do_post_skip)
                {
                    x4::skip_over(first, last, ctx);
                    if (res.expect_failure) [[unlikely]] res.ok = false;
                }
                res.remainder = {std::move(first), std::move(last)};
            }

            // --------------------------------------------
            // phrase_parse(range)

            // R + Parser + Attribute + Skipper + (root_skipper_flag)
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
                requires (!traits::CharArray<R>)
            static constexpr parse_result_for<R>
            operator()(R const& range, Parser&& p, X4Attribute auto& attr, Skipper const& s, root_skipper_flag flag = root_skipper_flag::do_post_skip)
            {
                using It = std::ranges::iterator_t<R const>;
                using Se = std::ranges::sentinel_t<R const>;

                auto skipper_ctx = x4::make_context<skipper_tag>(s);

                std::optional<expectation_failure<It>> expect_failure;
                auto ctx = x4::make_context<expectation_failure_tag>(expect_failure, skipper_ctx);

                It first = std::ranges::begin(range);
                Se last = std::ranges::end(range);
                bool ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, unused, attr);
                if (ok && flag == root_skipper_flag::do_post_skip)
                {
                    x4::skip_over(first, last, ctx);
                    // ReSharper disable once CppAssignedValueIsNeverUsed
                    if (expect_failure) [[unlikely]] ok = false;
                }
                return parse_result_for<R>{
                    .ok = ok,
                    .expect_failure = std::move(expect_failure),
                    .remainder = {std::move(first), std::move(last)}
                };
            }

            // "str" + Parser + Attribute + Skipper + (root_skipper_flag)
            template <traits::CharArray R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
            static constexpr parse_result_for<R>
            operator()(R const& str, Parser&& p, X4Attribute auto& attr, Skipper const& s, root_skipper_flag flag = root_skipper_flag::do_post_skip)
            {
                return parse_fn_main::operator()(std::basic_string_view{str}, std::forward<Parser>(p), attr, s, flag);
            }

            // parse_result + R + Parser + Attribute + Skipper
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
                requires (!traits::CharArray<R>)
            static constexpr void
            operator()(parse_result_for<R>& res, R const& range, Parser&& p, X4Attribute auto& attr, Skipper const& s, root_skipper_flag flag = root_skipper_flag::do_post_skip)
            {
                using It = std::ranges::iterator_t<R const>;
                using Se = std::ranges::sentinel_t<R const>;

                auto skipper_ctx = x4::make_context<skipper_tag>(s);

                res.expect_failure.reset();
                auto ctx = x4::make_context<expectation_failure_tag>(res.expect_failure, skipper_ctx);

                It first = std::ranges::begin(range);
                Se last = std::ranges::end(range);
                res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, unused, attr);
                if (res.ok && flag == root_skipper_flag::do_post_skip)
                {
                    x4::skip_over(first, last, ctx);
                    if (res.expect_failure) [[unlikely]] res.ok = false;
                }
                res.remainder = {std::move(first), std::move(last)};
            }

            // parse_result + "str" + Parser + Attribute + Skipper
            template <traits::CharArray R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
            static constexpr void
            operator()(parse_result_for<R>& res, R const& str, Parser&& p, X4Attribute auto& attr, Skipper const& s, root_skipper_flag flag = root_skipper_flag::do_post_skip)
            {
                return parse_fn_main::operator()(res, std::basic_string_view{str}, std::forward<Parser>(p), attr, s, flag);
            }
        }; // parse_fn_main

        struct parse_fn : parse_fn_main
        {
            using parse_fn_main::operator();

            // --------------------------------------------
            // deprecated

            // It/Se + Parser
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser>
            static constexpr void
            operator()(It, Se, Parser&&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

            // parse_result + It/Se + Parser
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser>
            static constexpr void
            operator()(parse_result<It, Se>&, It, Se, Parser&&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

            // R + Parser
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser>
            static constexpr void
            operator()(R&&, Parser&&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

            // parse_result + R + Parser
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser>
            static constexpr void
            operator()(parse_result_for<R>&, R&&, Parser&&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

            // ---------------------------------------------
            // deprecated phrase_parse

            // It/Se + Parser + Skipper
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper>
            static constexpr void
            operator()(It, Se, Parser&&, Skipper const&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

            // parse_result + It/Se + Parser + Skipper
            template <std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper>
            static constexpr void
            operator()(parse_result<It, Se>&, It, Se, Parser&&, Skipper const&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

            // R + Parser + Skipper
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
            static constexpr void
            operator()(R&&, Parser&&, Skipper const&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

            // parse_result + R + Parser + Skipper
            template <std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
            static constexpr void
            operator()(parse_result_for<R>&, R&&, Parser&&, Skipper const&) = delete; // If you don't need Attribute, explicitly pass `x4::unused`.

        }; // parse_fn

    } // detail

    inline namespace cpos
    {
        inline constexpr detail::parse_fn parse{};

        [[maybe_unused, deprecated("Use `parse(...)`. Spirit can now dispatch overloads correctly.")]]
        inline constexpr detail::parse_fn phrase_parse{};

    } // cpos

    // --------------------------------------------
    // context

    namespace detail
    {
        template <typename Skipper, typename ItOrRange, typename SeOrRange = ItOrRange>
        struct phrase_parse_context_for_impl;

        template <typename Skipper, std::forward_iterator It, std::sentinel_for<It> Se>
        struct phrase_parse_context_for_impl<Skipper, It, Se>
        {
            static_assert(X4ExplicitParser<Skipper, It, Se>);

            using skipper_ctx_type = decltype(x4::make_context<skipper_tag>(std::declval<Skipper const&>()));

            using type = decltype(x4::make_context<expectation_failure_tag>(
                std::declval<std::optional<expectation_failure<It>>&>(),
                std::declval<skipper_ctx_type const&>()
            ));
        };

        template <typename Skipper, std::ranges::forward_range R>
        struct phrase_parse_context_for_impl<Skipper, R>
            : phrase_parse_context_for_impl<Skipper, std::ranges::iterator_t<R const>, std::ranges::sentinel_t<R const>>
        {};

    } // detail

    // Used for determining the context type required in `BOOST_SPIRIT_X4_INSTANTIATE`.
    template <typename Skipper, typename ItOrRange, typename SeOrRange = ItOrRange>
    using phrase_parse_context_for = typename detail::phrase_parse_context_for_impl<Skipper, ItOrRange, SeOrRange>::type;

    namespace detail
    {
        template <typename ItOrRange>
        struct parse_context_for_impl;

        template <std::forward_iterator It>
        struct parse_context_for_impl<It>
        {
            using type = decltype(x4::make_context<expectation_failure_tag>(
                std::declval<std::optional<expectation_failure<It>>&>()
            ));
        };

        template <std::ranges::forward_range R>
        struct parse_context_for_impl<R>
            : parse_context_for_impl<std::ranges::iterator_t<R const>>
        {};

    } // detail

    // Used for determining the context type required in `BOOST_SPIRIT_X4_INSTANTIATE`.
    // Note that sentinel is not required, because only the iterator is needed for error info.
    // We keep the empty parameter as the noop placeholder to make the interface consistent with `phrase_parse_context_for`.
    template <typename ItOrRange, typename = void>
    using parse_context_for = typename detail::parse_context_for_impl<ItOrRange>::type;

} // boost::spirit::x4

#endif
