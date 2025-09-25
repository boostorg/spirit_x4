/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "real.hpp"

TEST_CASE("real3")
{
    // strict real number tests
    {
        using x4::real_parser;
        using x4::strict_ureal_policies;
        using x4::strict_real_policies;

        constexpr real_parser<double, strict_ureal_policies<double>> strict_udouble;
        constexpr real_parser<double, strict_real_policies<double>> strict_double;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(strict_udouble);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(strict_double);

        CHECK(!parse("1234", strict_udouble));
        {
            double d = 0;
            CHECK(!parse("1234", strict_udouble, d));
        }

        CHECK(parse("1.2", strict_udouble));
        {
            double d = 0;
            REQUIRE(parse("1.2", strict_udouble, d));
            CHECK(compare(d, 1.2));
        }

        CHECK(!parse("-1234", strict_double));
        {
            double d = 0;
            CHECK(!parse("-1234", strict_double, d));
        }

        CHECK(parse("123.", strict_double));
        {
            double d = 0;
            REQUIRE(parse("123.", strict_double, d));
            CHECK(compare(d, 123));
        }

        CHECK(parse("3.E6", strict_double));
        {
            double d = 0;
            REQUIRE(parse("3.E6", strict_double, d));
            CHECK(compare(d, 3e6));
        }

        {
            constexpr real_parser<double, no_trailing_dot_policy<double>> notrdot_real;
            constexpr real_parser<double, no_leading_dot_policy<double>> nolddot_real;

            BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(notrdot_real);
            BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(nolddot_real);

            CHECK(!parse("1234.", notrdot_real)); // Bad trailing dot
            CHECK(!parse(".1234", nolddot_real)); // Bad leading dot
        }
    }

    // Special thousands separated numbers
    {
        using x4::real_parser;
        constexpr real_parser<double, ts_real_policies<double>> ts_real;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(ts_real);

        CHECK(parse("123.01", ts_real));
        {
            double d = 0;
            REQUIRE(parse("123.01", ts_real, d));
            CHECK(compare(d, 123.01));
        }

        CHECK(parse("123,456,789.01", ts_real));
        {
            double d = 0;
            REQUIRE(parse("123,456,789.01", ts_real, d));
            CHECK(compare(d, 123456789.01));
        }

        CHECK(parse("12,345,678.90", ts_real));
        {
            double d = 0;
            REQUIRE(parse("12,345,678.90", ts_real, d));
            CHECK(compare(d, 12345678.90));
        }

        CHECK(parse("1,234,567.89", ts_real));
        {
            double d = 0;
            REQUIRE(parse("1,234,567.89", ts_real, d));
            CHECK(compare(d, 1234567.89));
        }

        CHECK(!parse("1234,567,890", ts_real));
        CHECK(!parse("1,234,5678,9", ts_real));
        CHECK(!parse("1,234,567.89e6", ts_real));
        CHECK(!parse("1,66", ts_real));
    }
}
