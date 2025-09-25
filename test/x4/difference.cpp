/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/difference.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>

#include <string>

TEST_CASE("difference")
{
    using x4::standard::char_;
    using x4::standard::space;
    using x4::lit;
    using x4::_attr;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ - 'a');

    // Basic tests
    {
        CHECK(parse("b", char_ - 'a'));
        CHECK(!parse("a", char_ - 'a'));
        CHECK(parse("/* abcdefghijk */", "/*" >> *(char_ - "*/") >> "*/"));
        CHECK(!parse("switch", lit("switch") - "switch"));
    }

    // Test attributes
    {
        char attr{};
        REQUIRE(parse("xg", (char_ - 'g') >> 'g', attr));
        CHECK(attr == 'x');
    }

    // Test handling of container attributes
    {
        std::string attr;
        REQUIRE(parse("abcdefg", *(char_ - 'g') >> 'g', attr));
        CHECK(attr == "abcdef");
    }

    {
        std::string s;
        REQUIRE(parse(
            "/*abcdefghijk*/",
            "/*" >> *(char_ - "*/")[([&](auto& ctx){ s += _attr(ctx); })] >> "*/"
        ));
        CHECK(s == "abcdefghijk");
    }
    {
        std::string s;
        REQUIRE(parse(
            "    /*abcdefghijk*/",
            "/*" >> *(char_ - "*/")[([&](auto& ctx){ s += _attr(ctx); })] >> "*/",
            space
        ));
        CHECK(s == "abcdefghijk");
    }
}
