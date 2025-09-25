/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/char/negated_char.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/directive/no_skip.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

TEST_CASE("no_skip")
{
    using x4::standard::space;
    using x4::standard::char_;
    using x4::lexeme;
    using x4::no_skip;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(no_skip['x']);

    // without skipping no_skip is equivalent to lexeme
    {
        std::string str;
        REQUIRE(parse("'  abc '", '\'' >> no_skip[+~char_('\'')] >> '\'', str));
        CHECK(str == "  abc ");
    }
    {
        std::string str;
        REQUIRE(parse("'  abc '", '\'' >> lexeme[+~char_('\'')] >> '\'', str));
        CHECK(str == "  abc ");
    }

    // with skipping, no_skip allows to match a leading skipper
    {
        std::string str;
        REQUIRE(parse("'  abc '", '\'' >> no_skip[+~char_('\'')] >> '\'', space, str));
        CHECK(str == "  abc ");
    }
    {
        std::string str;
        REQUIRE(parse("'  abc '", '\'' >> lexeme[+~char_('\'')] >> '\'', space, str));
        CHECK(str == "abc ");
    }
}
