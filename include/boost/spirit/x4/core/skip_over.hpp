#ifndef BOOST_SPIRIT_X4_CORE_SKIP_OVER_HPP
#define BOOST_SPIRIT_X4_CORE_SKIP_OVER_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/core/parser.hpp>

#include <iterator>
#include <type_traits>

namespace boost::spirit::x4 {

namespace contexts {

// Tag used to find the skipper from the context
struct skipper
{
    static constexpr bool is_unique = false;
};

} // contexts

using skipper_tag [[deprecated("Use `x4::contexts::skipper`")]] = contexts::skipper;

// Move the /first/ iterator to the first non-matching position
// given a skip-parser. The function is a no-op if unused_type or
// unused_skipper is passed as the skip-parser.
template<X4Subject Skipper>
struct [[nodiscard]] unused_skipper
{
    constexpr explicit unused_skipper(Skipper const&&) = delete; // dangling

    constexpr explicit unused_skipper(Skipper const& skipper) noexcept
        : skipper(skipper)
    {}

    Skipper const& skipper;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

template<class Context>
using unused_skipper_t = unused_skipper<std::remove_cvref_t<decltype(
    x4::get<contexts::skipper>(std::declval<Context const&>())
)>>;

namespace detail {

template<class Skipper>
struct is_unused_skipper : std::false_type
{
    static_assert(X4Subject<Skipper>);
};

template<class Skipper>
struct is_unused_skipper<unused_skipper<Skipper>> : std::true_type {};

template<>
struct is_unused_skipper<unused_type> : std::true_type {};

template<class Skipper>
[[nodiscard]] constexpr Skipper const&
get_unused_skipper(Skipper const& skipper BOOST_SPIRIT_LIFETIMEBOUND) noexcept
{
    static_assert(X4Subject<Skipper>);
    return skipper;
}

template<class Skipper>
void get_unused_skipper(Skipper const&&) = delete; // dangling

template<class Skipper>
[[nodiscard]] constexpr Skipper const&
get_unused_skipper(unused_skipper<Skipper> const& unused_skipper) noexcept
{
    static_assert(X4Subject<Skipper>);
    return unused_skipper.skipper;
}

template<class Skipper>
void get_unused_skipper(unused_skipper<Skipper> const&&) = delete;

template<class Context>
struct skip_over_context
{
    using type = unused_type;
};

template<class Context>
    requires (!std::is_same_v<expectation_failure_t<Context>, unused_type>)
struct skip_over_context<Context>
{
    using type = std::remove_cvref_t<decltype(x4::make_context<contexts::expectation_failure>(
        x4::get<contexts::expectation_failure>(std::declval<Context const&>())
    ))>;
};

template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Subject Skipper>
constexpr void skip_over(It& first, Se const& last, Context const& ctx, Skipper const& skipper)
    noexcept(is_nothrow_parsable_v<Skipper, It, Se, typename skip_over_context<Context>::type, unused_type>)
{
    if constexpr (!has_context_v<Context, contexts::expectation_failure>) {
        // The context given by parent was truly `unused_type`.
        // There exists only one such case in core; that is
        // `x4::phrase_parse(...)` which creates a fresh context
        // for the (post)skipper.
        //
        // In that case, it is perfectly fine to pass `unused`
        // because the skipper should have been wrapped
        // like `x4::with<x4::contexts::expectation_failure>(failure)[skipper]`.
        // (Note that we have plenty of static_asserts in other
        // locations to detect the absence of the context.)
        //
        // If we encounter this branch in any other situations,
        // that should be a BUG of `expectation_failure` logic.

        while (skipper.parse(first, last, unused, unused))
            /* loop */;

    } else {
        // In order to cut the template instantiation chain,
        // we must *forget* the original context at least once
        // during the (recursive) invocation of skippers.
        //
        // Traditionally, implementation detail of `skip_over`
        // was disposing the context because we can clearly assume
        // that any 'context,' including those provided by users,
        // is semantically meaningless as long as we're just
        // *skipping* iterators. As you can see in the other branch,
        // `unused` was passed for that purpose.
        //
        // However, we need to do a quite different thing when the
        // non-throwing expectation_failure mode is enabled.
        //
        // Since the reference bound to `x4::contexts::expectation_failure` is
        // provided by the user in the first place, if we do forget it
        // then it will be impossible to resurrect the value afterward.
        // It will also be problematic for `skip_over` itself because the
        // underlying skipper may (or may not) raise an expectation failure.
        // In traditional mode, the error was thrown by a C++ exception.
        // But how can we propagate that error without throwing?
        //
        // For this reason we're going to cherry-pick the reference
        // and repack it into a brand-new context.

        auto const local_ctx = x4::make_context<contexts::expectation_failure>(
            x4::get<contexts::expectation_failure>(ctx));

        while (skipper.parse(first, last, local_ctx, unused))
            /* loop */;
    }
}

template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
constexpr void skip_over(It&, Se const&, Context const&, unused_type const&) noexcept
{
}

template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Skipper>
constexpr void skip_over(It&, Se const&, Context const&, unused_skipper<Skipper> const&) noexcept
{
}

} // detail

template<class Context>
struct has_skipper
  : std::bool_constant<!detail::is_unused_skipper<
        std::remove_cvref_t<decltype(x4::get<contexts::skipper>(std::declval<Context const&>()))>
    >::value>
{};

template<class Context>
constexpr bool has_skipper_v = has_skipper<Context>::value;

template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
constexpr void skip_over(It& first, Se const& last, Context const& ctx)
    noexcept(noexcept(detail::skip_over(first, last, ctx, x4::get<contexts::skipper>(ctx))))
{
    detail::skip_over(first, last, ctx, x4::get<contexts::skipper>(ctx));
}

} // boost::spirit::x4

#endif
