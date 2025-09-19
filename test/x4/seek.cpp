/*=============================================================================
    Copyright (c) 2011 Jamboree
    Copyright (c) 2014 Lee Clagett
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/auxiliary/eoi.hpp>
#include <boost/spirit/x4/directive/seek.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/plus.hpp>

#include <vector>

int main()
{
    using namespace spirit_test;
    namespace x4 = boost::spirit::x4;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::seek['x']);

    // test eoi
    {
        BOOST_TEST(parse("", x4::seek[x4::eoi]));
        BOOST_TEST(parse(" ", x4::seek[x4::eoi], x4::space));
        BOOST_TEST(parse("a", x4::seek[x4::eoi]));
        BOOST_TEST(parse(" a", x4::seek[x4::eoi], x4::space));
    }

    // test literal finding
    {
        int i = 0;

        BOOST_TEST(
            parse("!@#$%^&*KEY:123", x4::seek["KEY:"] >> x4::int_, i)
            && i == 123
        );
    }
    // test sequence finding
    {
        int i = 0;

        BOOST_TEST(
            parse("!@#$%^&* KEY : 123", x4::seek[x4::lit("KEY") >> ':'] >> x4::int_, i, x4::space)
            && i == 123
        );
    }

    // test attr finding
    {
        std::vector<int> v;

        BOOST_TEST(
            parse("a06b78c3d", +x4::seek[x4::int_], v).is_partial_match() &&
            v.size() == 3 && v[0] == 6 && v[1] == 78 && v[2] == 3
        );
    }

    // test action
    {

       bool b = false;
       auto const action = [&b]() { b = true; };

       BOOST_TEST(parse("abcdefg", x4::seek["def"][action]).is_partial_match() && b);
    }

    // test container
    {
        std::vector<int> v;

        BOOST_TEST(
            parse("abcInt:100Int:95Int:44", x4::seek[+("Int:" >> x4::int_)], v)
            && v.size() == 3 && v[0] == 100 && v[1] == 95 && v[2] == 44
        );
    }

    // test failure rollback
    BOOST_TEST(!parse("abcdefg", x4::seek[x4::int_]));

    // past the end regression GH#658
    BOOST_TEST(!parse(" ", x4::seek['x'], x4::space));

    return boost::report_errors();
}
