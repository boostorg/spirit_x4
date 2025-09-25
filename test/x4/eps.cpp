/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>

TEST_CASE("eps")
{
    using x4::eps;

    {
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eps);
        CHECK(parse("", eps));
        CHECK(parse("xxx", eps).is_partial_match());
        CHECK(!parse("", !eps));
    }
    {
        // test non-lazy semantic predicate

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eps(true));
        CHECK(parse("", eps(true)));
        CHECK(!parse("", eps(false)));
        CHECK(parse("", !eps(false)));
    }
    {
        // test lazy semantic predicate

        constexpr auto true_fn = [] { return true; };
        constexpr auto false_Fn = [] { return false; };

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eps(std::true_type{}));
        CHECK(parse("", eps(true_fn)));
        CHECK(!parse("", eps(false_Fn)));
        CHECK(parse("", !eps(false_Fn)));
    }
}
