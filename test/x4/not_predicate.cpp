/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>

TEST_CASE("not_predicate")
{
    using x4::int_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(!int_);

    CHECK(!parse("1234", !int_));
    CHECK(parse("abcd", !int_).is_partial_match());
    CHECK(!parse("abcd", !!int_));
}
