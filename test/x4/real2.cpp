/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Bryce Lelbach
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "real.hpp"

namespace {

template<class T, class P>
void basic_real_parser_test(P parser)
{
    T attr;

    CHECK(parse("-1234", parser));
    REQUIRE(parse("-1234", parser, attr));
    CHECK(compare(attr, T(-1234l)));

    CHECK(parse("-1.2e3", parser));
    REQUIRE(parse("-1.2e3", parser, attr));
    CHECK(compare(attr, T(-1.2e3l)));

    CHECK(parse("+1.2e3", parser));
    REQUIRE(parse("+1.2e3", parser, attr));
    CHECK(compare(attr, T(1.2e3l)));

    CHECK(parse("-0.1", parser));
    REQUIRE(parse("-0.1", parser, attr));
    CHECK(compare(attr, T(-0.1l)));

    CHECK(parse("-1.2e-3", parser));
    REQUIRE(parse("-1.2e-3", parser, attr));
    CHECK(compare(attr, T(-1.2e-3l)));

    CHECK(parse("-1.e2", parser));
    REQUIRE(parse("-1.e2", parser, attr));
    CHECK(compare(attr, T(-1.e2l)));

    CHECK(parse("-.2e3", parser));
    REQUIRE(parse("-.2e3", parser, attr));
    CHECK(compare(attr, T(-.2e3l)));

    CHECK(parse("-2e3", parser));
    REQUIRE(parse("-2e3", parser, attr));
    CHECK(compare(attr, T(-2e3l)));

    CHECK(!parse("-e3", parser));
    CHECK(!parse("-e3", parser, attr));

    CHECK(!parse("-1.2e", parser));
    CHECK(!parse("-1.2e", parser, attr));
}

} // anonymous

TEST_CASE("real2")
{
    using x4::double_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::float_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::double_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::long_double);

    // signed real number tests
    basic_real_parser_test<float>(x4::float_);
    basic_real_parser_test<double>(x4::double_);
    basic_real_parser_test<long double>(x4::long_double);

    {
        double d = 0;
        CHECK(parse("-5.7222349715140557e+307", double_, d));
        CHECK(d == -5.7222349715140557e+307); // exact
    }
    {
        double d = 0;
        CHECK(parse("2.0332938517515416e-308", double_, d));
        CHECK(d == 2.0332938517515416e-308); // exact
    }
    {
        double d = 0;
        CHECK(parse("20332938517515416e291", double_, d));
        CHECK(d == 20332938517515416e291); // exact
    }
    {
        double d = 0;
        CHECK(parse("2.0332938517515416e307", double_, d));
        CHECK(d == 2.0332938517515416e307); // exact
    }

    CHECK(parse("-inf", double_));
    CHECK(parse("-infinity", double_));

    {
        double d = 0;
        REQUIRE(parse("-inf", double_, d));
        REQUIRE(std::isinf(d));
        CHECK(std::signbit(d));
    }
    {
        double d = 0;
        REQUIRE(parse("-infinity", double_, d));
        REQUIRE(std::isinf(d));
        CHECK(std::signbit(d));
    }

    CHECK(parse("-INF", double_));
    CHECK(parse("-INFINITY", double_));

    {
        double d = 0;
        REQUIRE(parse("-INF", double_, d));
        REQUIRE(std::isinf(d));
        CHECK(std::signbit(d));
    }
    {
        double d = 0;
        REQUIRE(parse("-INFINITY", double_, d));
        REQUIRE(std::isinf(d));
        CHECK(std::signbit(d));
    }

    {
        double d = 0;
        CHECK(parse("-nan", double_));
        REQUIRE(parse("-nan", double_, d));
        REQUIRE(std::isnan(d));
        CHECK(std::signbit(d));
    }
    {
        double d = 0;
        CHECK(parse("-NAN", double_));
        REQUIRE(parse("-NAN", double_, d));
        REQUIRE(std::isnan(d));
        CHECK(std::signbit(d));
    }
    {
        double d = 0;
        CHECK(parse("-nan(...)", double_));
        REQUIRE(parse("-nan(...)", double_, d));
        REQUIRE(std::isnan(d));
        CHECK(std::signbit(d));
    }
    {
        double d = 0;
        CHECK(parse("-NAN(...)", double_));
        REQUIRE(parse("-NAN(...)", double_, d));
        REQUIRE(std::isnan(d));
        CHECK(std::signbit(d));
    }

    CHECK(!parse("1e999", double_));
    CHECK(!parse("1e-999", double_));

    {
        double d = 0;
        REQUIRE(parse("2.1111111e-303", double_, d));
        CHECK(compare(d, 2.1111111e-303));
    }
    {
        double d = 0;
        REQUIRE(parse("1.1234e", double_, d).is_partial_match());
        CHECK(compare(d, 1.1234));
    }

    // https://svn.boost.org/trac10/ticket/11608
    {
        // Note Qi has better precision
        // TODO(2025): why?

        {
            double d = 0;
            REQUIRE(parse("1267650600228229401496703205376", double_, d));
            CHECK(compare(d, 1267650600228229401496703205376.));
        }

        {
            double d = 0;
            REQUIRE(parse("12676506.00228229401496703205376", double_, d));
            CHECK(compare(d, 12676506.00228229401496703205376));
        }
        {
            double d = 0;
            REQUIRE(parse("12676506.00228229401496703205376E6", double_, d));
            CHECK(compare(d, 12676506.00228229401496703205376E6));
        }
    }
}
