/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>

int main()
{
    using x4::int_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(!int_);

    {
        BOOST_TEST(!parse("1234", !int_));
        BOOST_TEST(parse("abcd", !int_).is_partial_match());
        BOOST_TEST(!parse("abcd", !!int_));
    }

    return boost::report_errors();
}
