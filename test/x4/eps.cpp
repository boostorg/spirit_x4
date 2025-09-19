/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>

int main()
{
    using x4::eps;
    using x4::unused_type;

    {
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eps);
        BOOST_TEST(parse("", eps));
        BOOST_TEST(parse("xxx", eps).is_partial_match());
        //~ BOOST_TEST(!parse("", !eps)); // not predicate $$$ Implement me! $$$
    }

    {   // test non-lazy semantic predicate

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eps(true));
        BOOST_TEST(parse("", eps(true)));
        BOOST_TEST(!parse("", eps(false)));
        BOOST_TEST(parse("", !eps(false)));
    }

    {   // test lazy semantic predicate

        auto true_ = [](unused_type) { return true; };
        auto false_ = [](unused_type) { return false; };

        // cannot use lambda in constant expression before C++17
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(eps(std::true_type{}));
        BOOST_TEST(parse("", eps(true_)));
        BOOST_TEST(!parse("", eps(false_)));
        BOOST_TEST(parse("", !eps(false_)));
    }

    return boost::report_errors();
}
