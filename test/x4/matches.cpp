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

int main()
{
    using x4::matches;
    using x4::char_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(matches['x']);

    {
        BOOST_TEST(parse("x", matches[char_]));
        bool result = false;
        BOOST_TEST(parse("x", matches[char_], result) && result);
    }

    {
        BOOST_TEST(!parse("y", matches[char_('x')]));
        BOOST_TEST(!parse("y", matches['x']));
        bool result = true;
        BOOST_TEST(parse("y", matches[char_('x')], result).is_partial_match() && !result);
        result = true;
        BOOST_TEST(parse("y", matches['x'], result).is_partial_match() && !result);
    }

    return boost::report_errors();
}
