/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/directive/matches.hpp>

TEST_CASE("matches")
{
    using x4::matches;
    using x4::char_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(matches['x']);

    {
        CHECK(parse("x", matches[char_]));
        bool result = false;
        REQUIRE(parse("x", matches[char_], result));
        CHECK(result == true);
    }

    CHECK(!parse("y", matches[char_('x')]));
    CHECK(!parse("y", matches['x']));

    {
        bool result = true;
        REQUIRE(parse("y", matches[char_('x')], result).is_partial_match());
        CHECK(result == false);
    }
    {
        bool result = true;
        REQUIRE(parse("y", matches['x'], result).is_partial_match());
        CHECK(result == false);
    }
}
