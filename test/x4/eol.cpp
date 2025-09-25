/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/eol.hpp>

TEST_CASE("eol")
{
    using x4::eol;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eol);

    CHECK(parse("\r\n", eol));
    CHECK(parse("\r", eol));
    CHECK(parse("\n", eol));
    CHECK(!parse("\n\r", eol));
    CHECK(!parse("", eol));
    CHECK(x4::what(eol) == "eol");
}
