/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>

#include <concepts>
#include <iterator>
#include <string>
#include <cstring>
#include <type_traits>

TEST_CASE("rule2")
{
    using namespace x4::standard;
    using x4::rule;
    using x4::lit;
    using x4::_attr;

    {
        // context tests

        constexpr auto a = rule<class a_id, char>("a") = alpha;

        {
            char ch{};
            // This semantic action requires the context
            auto f = [&](auto& ctx){ ch = _attr(ctx); };
            REQUIRE(parse("x", a[f]));
            CHECK(ch == 'x');
        }
        {
            char ch{};
            // This semantic action requires the (unused) context
            auto f2 = [&](auto&){ ch = 'y'; };
            REQUIRE(parse("x", a[f2]));
            CHECK(ch == 'y');
        }
        {
            char ch{};
            // The semantic action may optionally not have any arguments at all
            auto f3 = [&]{ ch = 'z'; };
            REQUIRE(parse("x", a[f3]));
            CHECK(ch == 'z');
        }

        {
            char ch{};
            REQUIRE(parse("z", a, ch)); // Attribute is given
            CHECK(ch == 'z');
        }
    }

    {
        // auto rules tests

        constexpr auto a = rule<class a_id, char>("a") = alpha;

        {
            char ch{};
            auto f = [&](auto& ctx){ ch = _attr(ctx); };
            REQUIRE(parse("x", a[f]));
            CHECK(ch == 'x');
        }
        {
            char ch{};
            REQUIRE(parse("z", a, ch)); // attribute is given.
            CHECK(ch == 'z');
        }
        {
            char ch{};
            auto f = [&](auto& ctx) { ch = _attr(ctx); };
            REQUIRE(parse("x", a[f]));
            CHECK(ch == 'x');
        }
        {
            char ch{};
            REQUIRE(parse("z", a, ch)); // attribute is given.
            CHECK(ch == 'z');
        }
    }

    // auto rules tests: allow stl containers as attributes to
    // sequences (in cases where attributes of the elements
    // are convertible to the value_type of the container or if
    // the element itself is an stl container with value_type
    // that is convertible to the value_type of the attribute).
    {
        constexpr auto r = rule<class r_id, std::string>("r") = char_ >> *(',' >> char_);

        std::string s;
        auto f = [&](auto& ctx) { s = _attr(ctx); };
        REQUIRE(parse("a,b,c,d,e,f", r[f]));
        CHECK(s == "abcdef");
    }
    {
        constexpr auto r = rule<class r_id, std::string>("r") = char_ >> *(',' >> char_);

        std::string s;
        auto f = [&](auto& ctx) { s = _attr(ctx); };
        REQUIRE(parse("a,b,c,d,e,f", r[f]));
        CHECK(s == "abcdef");
    }
    {
        auto r = rule<class r_id, std::string>("r") = char_ >> char_ >> char_ >> char_ >> char_ >> char_;

        std::string s;
        auto f = [&](auto& ctx) { s = _attr(ctx); };
        REQUIRE(parse("abcdef", r[f]));
        CHECK(s == "abcdef");
    }
}
