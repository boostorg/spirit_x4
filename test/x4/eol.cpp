/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/eol.hpp>

int main()
{
    namespace x4 = boost::spirit::x4;
    using x4::eol;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eol);

    {
        BOOST_TEST(parse("\r\n", eol));
        BOOST_TEST(parse("\r", eol));
        BOOST_TEST(parse("\n", eol));
        BOOST_TEST(!parse("\n\r", eol));
        BOOST_TEST(!parse("", eol));
    }

    {
        BOOST_TEST(x4::what(eol) == "eol");
    }

    return boost::report_errors();
}
