#ifndef BOOST_SPIRIT_X4_PARSE_HPP
#define BOOST_SPIRIT_X4_PARSE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/action.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/expectation.hpp>

#include <boost/spirit/x4/traits/string_traits.hpp>

#include <boost/spirit/x4/parse_result.hpp>

#include <iterator>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

enum class root_skipper_flag : char
{
    do_post_skip, // force post-skipping
    post_skip [[deprecated("Use `do_post_skip`")]] = do_post_skip,

    dont_post_skip // inhibit post-skipping
};

using skip_flag [[deprecated("Use `root_skipper_flag`")]] = root_skipper_flag;

// --------------------------------------------
// Default parse context

namespace detail {

template<class ItOrRange>
struct parse_context_for_impl;

template<std::forward_iterator It>
struct parse_context_for_impl<It>
{
    using type = context<
        contexts::expectation_failure,
        expectation_failure<It>
    >;
};

template<std::ranges::forward_range R>
struct parse_context_for_impl<R>
    : parse_context_for_impl<std::ranges::iterator_t<R const>>
{};

template<class Skipper, class ItOrRange, class SeOrRange = ItOrRange>
struct phrase_parse_context_for_impl;

template<class Skipper, std::forward_iterator It, std::sentinel_for<It> Se>
struct phrase_parse_context_for_impl<Skipper, It, Se>
{
    static_assert(X4ExplicitParser<Skipper, It, Se>);

    using type = context<
        contexts::expectation_failure,
        expectation_failure<It>,
        context<contexts::skipper, Skipper const>
    >;
};

template<class Skipper, std::ranges::forward_range R>
struct phrase_parse_context_for_impl<Skipper, R>
    : phrase_parse_context_for_impl<Skipper, std::ranges::iterator_t<R const>, std::ranges::sentinel_t<R const>>
{};

} // detail

// Used for determining the context type required in `BOOST_SPIRIT_X4_INSTANTIATE`.
// Note that sentinel is not required, because only the iterator is needed for error info.
// We keep the empty parameter as the noop placeholder to make the interface consistent with `phrase_parse_context_for`.
template<class ItOrRange, class = void>
using parse_context_for = typename detail::parse_context_for_impl<ItOrRange>::type;

// Used for determining the context type required in `BOOST_SPIRIT_X4_INSTANTIATE`.
template<class Skipper, class ItOrRange, class SeOrRange = ItOrRange>
using phrase_parse_context_for = typename detail::phrase_parse_context_for_impl<Skipper, ItOrRange, SeOrRange>::type;


namespace detail {

template<class Parser, class R>
concept X4RangeParseParser =
    X4Parser<
        Parser,
        typename range_parse_parser_impl<R>::iterator_type,
        typename range_parse_parser_impl<R>::sentinel_type
    >;

template<class Skipper, class R>
concept X4RangeParseSkipper =
    X4ExplicitParser<
        Skipper,
        typename range_parse_parser_impl<R>::iterator_type,
        typename range_parse_parser_impl<R>::sentinel_type
    >;

struct parse_fn_main
{
private:
    template<std::ranges::forward_range R>
        requires (!traits::CharArray<R>)
    [[nodiscard]] static constexpr decltype(auto) as_parse_range(R const& range) noexcept
    {
        return range;
    }

    template<std::ranges::forward_range R>
        requires traits::CharArray<R>
    [[nodiscard]] static constexpr auto as_parse_range(R const& str)
        noexcept(noexcept(std::basic_string_view{str}))
    {
        return std::basic_string_view{str};
    }

public:
    // --------------------------------------------
    // parse(range)

    // R + Parser + Attribute
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4Attribute ParseAttr>
    static constexpr parse_result_for<R>
    operator()(R const& range, Parser&& p, ParseAttr& attr)
    {
        // Treat "str" as `string_view`
        auto const& range_ = parse_fn_main::as_parse_range(range);

        using It = typename parse_result_for<R>::iterator_type;
        using Se = typename parse_result_for<R>::sentinel_type;
        It first = std::ranges::begin(range_);
        Se last = std::ranges::end(range_);

        expectation_failure<It> expect_failure;

        bool const ok = as_parser(std::forward<Parser>(p)).parse(
            first, last,
            x4::make_context<contexts::expectation_failure>(expect_failure),
            attr
        );
        return parse_result_for<R>{
            .ok = ok,
            .expect_failure = std::move(expect_failure),
            .remainder = {std::move(first), std::move(last)}
        };
    }

    // parse_result + R + Parser + Attribute
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4Attribute ParseAttr>
    static constexpr void
    operator()(parse_result_for<R>& res, R const& range, Parser&& p, ParseAttr& attr)
    {
        // Treat "str" as `string_view`
        auto const& range_ = parse_fn_main::as_parse_range(range);

        using It = typename parse_result_for<R>::iterator_type;
        using Se = typename parse_result_for<R>::sentinel_type;
        It first = std::ranges::begin(range_);
        Se last = std::ranges::end(range_);

        res.expect_failure.clear();
        res.ok = as_parser(std::forward<Parser>(p)).parse(
            first, last,
            x4::make_context<contexts::expectation_failure>(res.expect_failure),
            attr
        );
        res.remainder = {std::move(first), std::move(last)};
    }

    // --------------------------------------------
    // phrase_parse(range)

    // R + Parser + Skipper + Attribute + (root_skipper_flag)
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper, X4Attribute ParseAttr>
    static constexpr parse_result_for<R>
    operator()(R const& range, Parser&& p, Skipper const& s, ParseAttr& attr, root_skipper_flag flag = root_skipper_flag::do_post_skip)
    {
        // Treat "str" as `string_view`
        auto const& range_ = parse_fn_main::as_parse_range(range);

        using It = typename parse_result_for<R>::iterator_type;
        using Se = typename parse_result_for<R>::sentinel_type;
        It first = std::ranges::begin(range_);
        Se last = std::ranges::end(range_);

        expectation_failure<It> expect_failure;
        auto const ctx = x4::make_context<contexts::expectation_failure>(
            expect_failure, x4::make_context<contexts::skipper>(s)
        );

        bool ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, attr);
        if (ok && flag == root_skipper_flag::do_post_skip) {
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

    // parse_result + R + Parser + Skipper + Attribute
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper, X4Attribute ParseAttr>
    static constexpr void
    operator()(parse_result_for<R>& res, R const& range, Parser&& p, Skipper const& s, ParseAttr& attr, root_skipper_flag flag = root_skipper_flag::do_post_skip)
    {
        // Treat "str" as `string_view`
        auto const& range_ = parse_fn_main::as_parse_range(range);

        using It = typename parse_result_for<R>::iterator_type;
        using Se = typename parse_result_for<R>::sentinel_type;
        It first = std::ranges::begin(range_);
        Se last = std::ranges::end(range_);

        res.expect_failure.clear();
        auto const ctx = x4::make_context<contexts::expectation_failure>(
            res.expect_failure, x4::make_context<contexts::skipper>(s)
        );

        res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, attr);
        if (res.ok && flag == root_skipper_flag::do_post_skip) {
            x4::skip_over(first, last, ctx);
            if (res.expect_failure) [[unlikely]] res.ok = false;
        }
        res.remainder = {std::move(first), std::move(last)};
    }

    // --------------------------------------------
    // parse(it/se)

    // It/Se + Parser + Attribute
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4Attribute ParseAttr>
    static constexpr parse_result<It, Se>
    operator()(It first, Se last, Parser&& p, ParseAttr& attr)
    {
        expectation_failure<It> expect_failure;
        bool const ok = as_parser(std::forward<Parser>(p)).parse(
            first, last,
            x4::make_context<contexts::expectation_failure>(expect_failure),
            attr
        );
        return parse_result<It, Se>{
            .ok = ok,
            .expect_failure = std::move(expect_failure),
            .remainder = {std::move(first), std::move(last)}
        };
    }

    // parse_result + It/Se + Parser + Attribute
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4Attribute ParseAttr>
    static constexpr void
    operator()(parse_result<It, Se>& res, It first, Se last, Parser&& p, ParseAttr& attr)
    {
        res.expect_failure.clear();
        res.ok = as_parser(std::forward<Parser>(p)).parse(
            first, last,
            x4::make_context<contexts::expectation_failure>(res.expect_failure),
            attr
        );
        res.remainder = {std::move(first), std::move(last)};
    }

    // --------------------------------------------
    // phrase_parse(it/se)

    // It/Se + Parser + Skipper + Attribute + (root_skipper_flag)
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper, X4Attribute ParseAttr>
    static constexpr parse_result<It, Se>
    operator()(It first, Se last, Parser&& p, Skipper const& s, ParseAttr& attr, root_skipper_flag flag = root_skipper_flag::do_post_skip)
    {
        expectation_failure<It> expect_failure;
        auto const ctx = x4::make_context<contexts::expectation_failure>(
            expect_failure, x4::make_context<contexts::skipper>(s)
        );

        bool ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, attr);
        if (ok && flag == root_skipper_flag::do_post_skip) {
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
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper, X4Attribute ParseAttr>
    static constexpr void
    operator()(parse_result<It, Se>& res, It first, Se last, Parser&& p, Skipper const& s, ParseAttr& attr, root_skipper_flag flag = root_skipper_flag::do_post_skip)
    {
        res.expect_failure.clear();
        auto const ctx = x4::make_context<contexts::expectation_failure>(
            res.expect_failure, x4::make_context<contexts::skipper>(s)
        );

        res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, attr);
        if (res.ok && flag == root_skipper_flag::do_post_skip) {
            x4::skip_over(first, last, ctx);
            if (res.expect_failure) [[unlikely]] res.ok = false;
        }
        res.remainder = {std::move(first), std::move(last)};
    }

}; // parse_fn_main

struct parse_fn_deprecated
{
    // --------------------------------------------
    // deprecated `parse`

    // It/Se + Parser
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser>
    static constexpr void
    operator()(It, Se, Parser&&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.

    // parse_result + It/Se + Parser
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser>
    static constexpr void
    operator()(parse_result<It, Se>&, It, Se, Parser&&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.

    // R + Parser
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser>
    static constexpr void
    operator()(R&&, Parser&&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.

    // parse_result + R + Parser
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser>
    static constexpr void
    operator()(parse_result_for<R>&, R&&, Parser&&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.

    // ---------------------------------------------
    // deprecated `phrase_parse`

    // It/Se + Parser + Skipper
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper>
    static constexpr void
    operator()(It, Se, Parser&&, Skipper const&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.

    // parse_result + It/Se + Parser + Skipper
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Parser<It, Se> Parser, X4ExplicitParser<It, Se> Skipper>
    static constexpr void
    operator()(parse_result<It, Se>&, It, Se, Parser&&, Skipper const&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.

    // R + Parser + Skipper
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
    static constexpr void
    operator()(R&&, Parser&&, Skipper const&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.

    // parse_result + R + Parser + Skipper
    template<std::ranges::forward_range R, X4RangeParseParser<R> Parser, X4RangeParseSkipper<R> Skipper>
    static constexpr void
    operator()(parse_result_for<R>&, R&&, Parser&&, Skipper const&) = delete; // If you don't need Attr, explicitly pass `x4::unused`.
};

struct parse_fn : parse_fn_main, parse_fn_deprecated
{
    using parse_fn_main::operator();
    using parse_fn_deprecated::operator();
};

} // detail

inline namespace cpos {

inline constexpr detail::parse_fn parse{};

[[maybe_unused, deprecated("Use `parse(...)`. Spirit can now dispatch overloads correctly.")]]
inline constexpr detail::parse_fn phrase_parse{};

} // cpos

} // boost::spirit::x4

#endif
