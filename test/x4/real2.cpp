/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Bryce Lelbach
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "real.hpp"

template <typename T, typename P>
void basic_real_parser_test(P parser)
{
    T attr;

    BOOST_TEST(parse("-1234", parser));
    BOOST_TEST(parse("-1234", parser, attr) && compare(attr, T(-1234l)));

    BOOST_TEST(parse("-1.2e3", parser));
    BOOST_TEST(parse("-1.2e3", parser, attr) && compare(attr, T(-1.2e3l)));

    BOOST_TEST(parse("+1.2e3", parser));
    BOOST_TEST(parse("+1.2e3", parser, attr) && compare(attr, T(1.2e3l)));

    BOOST_TEST(parse("-0.1", parser));
    BOOST_TEST(parse("-0.1", parser, attr) && compare(attr, T(-0.1l)));

    BOOST_TEST(parse("-1.2e-3", parser));
    BOOST_TEST(parse("-1.2e-3", parser, attr) && compare(attr, T(-1.2e-3l)));

    BOOST_TEST(parse("-1.e2", parser));
    BOOST_TEST(parse("-1.e2", parser, attr) && compare(attr, T(-1.e2l)));

    BOOST_TEST(parse("-.2e3", parser));
    BOOST_TEST(parse("-.2e3", parser, attr) && compare(attr, T(-.2e3l)));

    BOOST_TEST(parse("-2e3", parser));
    BOOST_TEST(parse("-2e3", parser, attr) && compare(attr, T(-2e3l)));

    BOOST_TEST(!parse("-e3", parser));
    BOOST_TEST(!parse("-e3", parser, attr));

    BOOST_TEST(!parse("-1.2e", parser));
    BOOST_TEST(!parse("-1.2e", parser, attr));
}

int main()
{
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::float_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::double_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::long_double);

    // signed real number tests
    {
        basic_real_parser_test<float>(x4::float_);
        basic_real_parser_test<double>(x4::double_);
        basic_real_parser_test<long double>(x4::long_double);
    }

    {
        using x4::double_;
        double  d;

#if defined(BOOST_SPIRIT_TEST_REAL_PRECISION)
        BOOST_TEST(parse("-5.7222349715140557e+307", double_, d));
        BOOST_TEST(d == -5.7222349715140557e+307); // exact!

        BOOST_TEST(parse("2.0332938517515416e-308", double_, d));
        BOOST_TEST(d == 2.0332938517515416e-308); // exact!

        BOOST_TEST(parse("20332938517515416e291", double_, d));
        BOOST_TEST(d == 20332938517515416e291); // exact!

        BOOST_TEST(parse("2.0332938517515416e307", double_, d));
        BOOST_TEST(d == 2.0332938517515416e307); // exact!
#endif

        BOOST_TEST(parse("-inf", double_));
        BOOST_TEST(parse("-infinity", double_));
        BOOST_TEST(parse("-inf", double_, d) &&
            std::isinf(d) && std::signbit(d));
        BOOST_TEST(parse("-infinity", double_, d) &&
            std::isinf(d) && std::signbit(d));
        BOOST_TEST(parse("-INF", double_));
        BOOST_TEST(parse("-INFINITY", double_));
        BOOST_TEST(parse("-INF", double_, d) &&
            std::isinf(d) && std::signbit(d));
        BOOST_TEST(parse("-INFINITY", double_, d) &&
            std::isinf(d) && std::signbit(d));

        BOOST_TEST(parse("-nan", double_));
        BOOST_TEST(parse("-nan", double_, d) &&
            std::isnan(d) && std::signbit(d));
        BOOST_TEST(parse("-NAN", double_));
        BOOST_TEST(parse("-NAN", double_, d) &&
            std::isnan(d) && std::signbit(d));

        BOOST_TEST(parse("-nan(...)", double_));
        BOOST_TEST(parse("-nan(...)", double_, d) &&
            std::isnan(d) && std::signbit(d));
        BOOST_TEST(parse("-NAN(...)", double_));
        BOOST_TEST(parse("-NAN(...)", double_, d) &&
            std::isnan(d) && std::signbit(d));

        BOOST_TEST(!parse("1e999", double_));
        BOOST_TEST(!parse("1e-999", double_));
        BOOST_TEST(parse("2.1111111e-303", double_, d) &&
            compare(d, 2.1111111e-303));
        BOOST_TEST(!parse("1.1234e", double_, d) && compare(d, 1.1234));

        // https://svn.boost.org/trac10/ticket/11608
        BOOST_TEST(parse("1267650600228229401496703205376", double_, d) &&
            compare(d, 1267650600228229401496703205376.));    // Note Qi has better precision

        BOOST_TEST(parse("12676506.00228229401496703205376", double_, d) &&
            compare(d, 12676506.00228229401496703205376));    // Note Qi has better precision

        BOOST_TEST(parse("12676506.00228229401496703205376E6", double_, d) &&
            compare(d, 12676506.00228229401496703205376E6));  // Note Qi has better precision
    }

    return boost::report_errors();
}
