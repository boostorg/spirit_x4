/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/directive/as.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/char/negated_char.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/rule.hpp>

#include <string>
#include <string_view>
#include <concepts>
#include <type_traits>

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("as")
{
    using namespace std::string_view_literals;

    using x4::eps;
    using x4::string;
    using x4::_attr;
    using x4::_val;
    using x4::_as_val;

    using It = std::string_view::const_iterator;
    using Se = It;
    using Context = unused_type;

    // as<unused_type>
    {
        constexpr auto p = x4::as<unused_type>(eps);
        using Underlying = std::remove_const_t<decltype(eps)>;
        using AsParser = std::remove_const_t<decltype(p)>;

        static_assert(std::same_as<x4::traits::attribute_of_t<Underlying, Context>, unused_type>);

        static_assert(x4::is_nothrow_parsable_v<Underlying, It, Se, Context, unused_type>);
        static_assert(x4::is_nothrow_parsable_v<Underlying, It, Se, Context, std::string>); // Arbitrary exposed attribute

        static_assert(x4::is_nothrow_parsable_v<AsParser, It, Se, Context, unused_type>);
        static_assert(x4::is_nothrow_parsable_v<AsParser, It, Se, Context, std::string>); // Arbitrary exposed attribute

        std::string_view input;
        It first = input.begin();
        Se const last = input.end();
        std::string attr;
        (void)p.parse(first, last, unused, unused);
        (void)p.parse(first, last, unused, attr);
    }

    // as<int>
    {
        constexpr auto p = x4::as<int>(eps);
        using Underlying = std::remove_const_t<decltype(eps)>;
        using AsParser = std::remove_const_t<decltype(p)>;

        static_assert(std::same_as<x4::traits::attribute_of_t<Underlying, Context>, unused_type>);

        static_assert(x4::is_nothrow_parsable_v<Underlying, It, Se, Context, unused_type>);
        static_assert(x4::is_nothrow_parsable_v<Underlying, It, Se, Context, std::string>); // Arbitrary exposed attribute

        static_assert(x4::is_nothrow_parsable_v<AsParser, It, Se, Context, unused_type>);
        static_assert(x4::is_nothrow_parsable_v<AsParser, It, Se, Context, long>); // Arbitrary exposed attribute

        std::string_view input;
        It first = input.begin();
        Se const last = input.end();
        long attr = 0;
        (void)p.parse(first, last, unused, unused);
        (void)p.parse(first, last, unused, attr);
    }

    constexpr auto disable_attr = eps[([](auto&) {})];
    constexpr auto quoted_string = '\'' >> *~x4::char_('\'') >> '\'';

    // `as` only
    {
        std::string attr;
        REQUIRE(parse("'foo'", quoted_string, attr));
        CHECK(attr == "foo"sv);
    }
    {
        std::string attr;
        REQUIRE(parse("'foo'", x4::as<std::string>(quoted_string), attr));
        CHECK(attr == "foo"sv);
    }

    {
        std::string attr;
        REQUIRE(parse("'fo", quoted_string | x4::string("'fo"), attr));
        CHECK(attr == "'fo"sv);
    }
    {
        std::string attr;
        REQUIRE(parse("'fo", x4::as<std::string>(quoted_string) | x4::string("'fo"), attr));
        CHECK(attr == "'fo"sv);
    }

    // `as` + `rule`
    {
        constexpr x4::rule<struct _, std::string> rule_maker{"rule_maker"};

        // Non-forced attribute, `operator=`
        {
            // Attribute is disabled because the sub parser has semantic action and the operator is `=`
            constexpr auto rule_without_attr = rule_maker = quoted_string >> disable_attr;
            std::string str;
            REQUIRE(parse("'foo'", rule_without_attr, str));
            CHECK(str == ""sv); // Disabled attribute should yield default-constructed attribute
        }
        {
            // Attribute is disabled because the sub parser has semantic action and the operator is `=`
            constexpr auto rule_without_attr = rule_maker = x4::as<std::string>(quoted_string) >> disable_attr;
            std::string str;
            REQUIRE(parse("'foo'", rule_without_attr, str));
            CHECK(str == ""sv); // Disabled attribute should yield default-constructed attribute
        }
        {
            // Attribute is forced because `as_directive` sets `::has_action` to `false`
            constexpr auto rule_without_attr = rule_maker = x4::as<std::string>(quoted_string >> disable_attr);
            std::string str;
            REQUIRE(parse("'foo'", rule_without_attr, str));
            CHECK(str == "foo"sv); // Forced attribute should hold the parsed value
        }

        // Forced attribute, `operator%=`
        {
            constexpr auto rule_with_forced_attr = rule_maker %= quoted_string >> disable_attr;
            std::string str;
            REQUIRE(parse("'foo'", rule_with_forced_attr, str));
            CHECK(str == "foo"sv); // Forced attribute should hold the parsed value
        }
        {
            constexpr auto rule_with_forced_attr = rule_maker %= x4::as<std::string>(quoted_string) >> disable_attr;
            std::string str;
            REQUIRE(parse("'foo'", rule_with_forced_attr, str));
            CHECK(str == "foo"sv); // `as` should not create a temporary; it should directly parse into the exposed variable
        }
        {
            constexpr auto rule_with_forced_attr = rule_maker %= x4::as<std::string>(quoted_string >> disable_attr);
            std::string str;
            REQUIRE(parse("'foo'", rule_with_forced_attr, str));
            CHECK(str == "foo"sv); // `as` should not create a temporary; it should directly parse into the exposed variable
        }
    }

    // `as_val(ctx)` (with auto attribute propagation)
    {
        std::string result;

        constexpr auto string_rule = x4::rule<struct _, decltype(result)>{""} =
            x4::as<std::string>(
                eps[([](auto& ctx) {
                    _val(ctx) = "default";
                })] >>

                eps[([](auto& ctx) {
                    _as_val(ctx) = "foo";
                })]
            );

        std::string_view const input;
        It first = input.begin();
        Se const last = input.end();

        REQUIRE(string_rule.parse(first, last, unused, result));
        CHECK(result == "foo"sv);
    }
    // `as_val(ctx)` (with disabled attribute)
    {
        std::string result;

        constexpr auto string_rule = x4::rule<struct _, decltype(result)>{""} =
            x4::as<std::string>(
                eps[([](auto& ctx) {
                    _val(ctx) = "default";
                })] >>

                eps[([]([[maybe_unused]] auto& ctx) {
                    static_assert(std::same_as<std::remove_cvref_t<decltype(_as_val(ctx))>, unused_type>);
                })]
            ) >> disable_attr; // <----------

        std::string_view const input;
        It first = input.begin();
        Se const last = input.end();

        REQUIRE(string_rule.parse(first, last, unused, result));
        CHECK(result == "default"sv);
    }
    // `as_val(ctx)` (within `as<unused_type>(as<std::string>(...))`)
    {
        std::string result{"default"};

        constexpr auto unused_rule = x4::as<unused_type>(
            x4::as<std::string>(
                eps[([]([[maybe_unused]] auto& ctx) {
                    static_assert(std::same_as<std::remove_cvref_t<decltype(_as_val(ctx))>, unused_type>);
                })]
            )
        );

        std::string_view const input;
        It first = input.begin();
        Se const last = input.end();

        REQUIRE(unused_rule.parse(first, last, unused, result));
        CHECK(result == "default"sv);
    }
    // `as_val(ctx)` (within `as<std::string>(as<unused_type>(...))`)
    {
        std::string result;

        /*constexpr*/ auto unused_rule = x4::as<std::string>(
            x4::attr("default") >>

            eps[([]([[maybe_unused]] auto& ctx) {
                static_assert(std::same_as<std::remove_cvref_t<decltype(_as_val(ctx))>, std::string>);
            })] >>

            x4::as<unused_type>(
                eps[([]([[maybe_unused]] auto& ctx) {
                    static_assert(std::same_as<std::remove_cvref_t<decltype(_as_val(ctx))>, unused_type>);
                })]
            )
        );

        std::string_view const input;
        It first = input.begin();
        Se const last = input.end();

        REQUIRE(unused_rule.parse(first, last, unused, result));
        CHECK(result == "default"sv);
    }

    // Use `_val(ctx)` inside `as<T>(...)`
    {
        struct StringLiteral
        {
            bool is_quoted = false;
            std::string text;
        };

        StringLiteral result;

        constexpr auto string_literal = x4::rule<struct _, StringLiteral>{"StringLiteral"} =
            eps[([](auto& ctx) { _val(ctx).is_quoted = false; })] >>
            x4::as<std::string>(
                x4::lit('"')[([](auto& ctx) {
                    StringLiteral& rule_val = _val(ctx);
                    rule_val.is_quoted = true;
                })] >>
                *~x4::char_('"') >>
                '"'
            )[([](auto& ctx) { _val(ctx).text = std::move(_attr(ctx)); })];

        std::string_view input = R"("foo")";
        It first = input.begin();
        Se const last = input.end();

        REQUIRE(string_literal.parse(first, last, unused, result));
        CHECK(result.is_quoted == true);
        CHECK(result.text == "foo"sv);
    }
}

// NOLINTEND(readability-container-size-empty)
