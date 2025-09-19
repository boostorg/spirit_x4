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

int main()
{
    using x4::standard::char_;
    using x4::standard::space;
    using x4::lit;
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ - 'a');

    // Basic tests
    {
        BOOST_TEST(parse("b", char_ - 'a'));
        BOOST_TEST(!parse("a", char_ - 'a'));
        BOOST_TEST(parse("/* abcdefghijk */", "/*" >> *(char_ - "*/") >> "*/"));
        BOOST_TEST(!parse("switch", lit("switch") - "switch"));
    }

    // Test attributes
    {
        char attr;
        BOOST_TEST(parse("xg", (char_ - 'g') >> 'g', attr));
        BOOST_TEST(attr == 'x');
    }

    // Test handling of container attributes
    {
        std::string attr;
        BOOST_TEST(parse("abcdefg", *(char_ - 'g') >> 'g', attr));
        BOOST_TEST(attr == "abcdef");
    }

    {
        using x4::_attr;

        std::string s;

        BOOST_TEST(parse(
            "/*abcdefghijk*/"
          , "/*" >> *(char_ - "*/")[([&](auto& ctx){ s += _attr(ctx); })] >> "*/"
        ));
        BOOST_TEST(s == "abcdefghijk");
        s.clear();

        BOOST_TEST(parse(
            "    /*abcdefghijk*/"
          , "/*" >> *(char_ - "*/")[([&](auto& ctx){ s += _attr(ctx); })] >> "*/"
          , space
        ));
        BOOST_TEST(s == "abcdefghijk");
    }

    return boost::report_errors();
}
