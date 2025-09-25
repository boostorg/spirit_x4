/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/eoi.hpp>

TEST_CASE("eoi")
{
    using x4::eoi;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eoi);

    CHECK(parse("", eoi));
    CHECK(!(parse("x", eoi)));
    CHECK(x4::what(eoi) == "eoi");
}
