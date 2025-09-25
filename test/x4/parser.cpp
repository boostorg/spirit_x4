/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/char/char_class.hpp>

#include <iterator>
#include <string_view>

namespace {

struct minimal_parser
    : x4::parser<minimal_parser>
{
    constexpr minimal_parser() = default;

    constexpr minimal_parser(minimal_parser const&) = delete;
    constexpr minimal_parser& operator=(minimal_parser const&) = delete;

    constexpr minimal_parser(minimal_parser&&) = default;
    constexpr minimal_parser& operator=(minimal_parser&&) = default;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, x4::X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, Attr&) const
    {
        return true;
    }
};

struct minimal_unary_parser
    : x4::unary_parser<minimal_parser, minimal_unary_parser>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, x4::X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, Attr&) const
    {
        return true;
    }
};

struct minimal_binary_parser
    : x4::binary_parser<minimal_parser, minimal_parser, minimal_binary_parser>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, x4::X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, Attr&) const
    {
        return true;
    }
};

struct minimal_unused_parser
    : x4::parser<minimal_unused_parser>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, unused_type const&) const
    {
        return true;
    }
};

struct minimal_unary_unused_parser
    : x4::unary_parser<minimal_unused_parser, minimal_unary_unused_parser>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, unused_type const&) const
    {
        return true;
    }
};

struct minimal_binary_unused_parser
    : x4::binary_parser<minimal_unused_parser, minimal_unused_parser, minimal_binary_unused_parser>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse(It&, Se const&, Context const&, unused_type const&) const
    {
        return true;
    }
};

} // anonymous

TEST_CASE("parser")
{
    using x4::blank;

    {
        using It = std::string_view::const_iterator;
        using Se = It;
        static_assert(std::forward_iterator<It>);
        static_assert(std::sentinel_for<Se, It>);

        STATIC_CHECK(x4::X4Subject<minimal_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_parser, It, Se>);

        // parse
        {
            // str
            STATIC_CHECK(x4::parse("", minimal_parser{}, unused));
            // str + res
            STATIC_CHECK([] { x4::parse_result<It, Se> res{}; x4::parse(res, "", minimal_parser{}, unused); return res; }());

            // It/Se
            STATIC_CHECK(x4::parse(It{}, Se{}, minimal_parser{}, unused));
            // It/Se + res
            STATIC_CHECK([] { x4::parse_result<It, Se> res{}; x4::parse(res, It{}, Se{}, minimal_parser{}, unused); return res; }());
        }

        // phrase_parse
        {
            // str
            STATIC_CHECK(x4::parse("", minimal_parser{}, blank, unused));
            // str + res
            STATIC_CHECK([] { x4::parse_result<It, Se> res{}; x4::parse(res, "", minimal_parser{}, blank, unused); return res; }());

            // It/Se
            STATIC_CHECK(x4::parse(It{}, Se{}, minimal_parser{}, blank, unused));
            // It/Se + res
            STATIC_CHECK([] { x4::parse_result<It, Se> res{}; x4::parse(res, It{}, Se{}, minimal_parser{}, blank, unused); return res; }());
        }

        // expectation
        STATIC_CHECK(x4::parse("", minimal_parser{} > minimal_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_unary_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_unary_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_unary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_unary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_unary_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_unary_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_unary_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_unary_parser{} > minimal_unary_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_binary_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_binary_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_binary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_binary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_binary_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_binary_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_binary_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_binary_parser{} > minimal_binary_parser{}, unused));


        STATIC_CHECK(x4::X4Subject<minimal_unused_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_unused_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_unused_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_unused_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_unused_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_unused_parser{} > minimal_unused_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_unary_unused_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_unary_unused_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_unary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_unary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_unary_unused_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_unary_unused_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_unary_unused_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_unary_unused_parser{} > minimal_unary_unused_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_binary_unused_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_binary_unused_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_binary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_binary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_binary_unused_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_binary_unused_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_binary_unused_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_binary_unused_parser{} > minimal_binary_unused_parser{}, unused));
    }

    {
        using It = std::counted_iterator<char const*>;
        using Se = std::default_sentinel_t;
        static_assert(std::forward_iterator<It>);
        static_assert(std::sentinel_for<Se, It>);

        STATIC_CHECK(x4::X4Subject<minimal_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_parser{} > minimal_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_unary_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_unary_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_unary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_unary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_unary_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_unary_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_unary_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_unary_parser{} > minimal_unary_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_binary_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_binary_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_binary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_binary_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_binary_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_binary_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_binary_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_binary_parser{} > minimal_binary_parser{}, unused));


        STATIC_CHECK(x4::X4Subject<minimal_unused_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_unused_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_unused_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_unused_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_unused_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_unused_parser{} > minimal_unused_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_unary_unused_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_unary_unused_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_unary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_unary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_unary_unused_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_unary_unused_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_unary_unused_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_unary_unused_parser{} > minimal_unary_unused_parser{}, unused));

        STATIC_CHECK(x4::X4Subject<minimal_binary_unused_parser>);
        STATIC_CHECK(x4::X4ExplicitSubject<minimal_binary_unused_parser>);
        STATIC_CHECK(x4::is_parsable_v<minimal_binary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(!x4::is_nothrow_parsable_v<minimal_binary_unused_parser, It, Se, unused_type, unused_type>);
        STATIC_CHECK(x4::X4Parser<minimal_binary_unused_parser, It, Se>);
        STATIC_CHECK(x4::X4ExplicitParser<minimal_binary_unused_parser, It, Se>);
        STATIC_CHECK(x4::parse("", minimal_binary_unused_parser{}, unused));
        STATIC_CHECK(x4::parse("", minimal_binary_unused_parser{} > minimal_binary_unused_parser{}, unused));
    }
}
