/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Bryce Lelbach
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "real.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

TEST_CASE("real1")
{
    STATIC_CHECK(x4::detail::RealPolicy<x4::real_policies<float>>);
    STATIC_CHECK(x4::detail::RealPolicy<x4::strict_ureal_policies<float>>);
    STATIC_CHECK(x4::detail::RealPolicy<x4::strict_real_policies<float>>);

    STATIC_CHECK(x4::detail::RealPolicy<x4::real_policies<double>>);
    STATIC_CHECK(x4::detail::RealPolicy<x4::strict_ureal_policies<double>>);
    STATIC_CHECK(x4::detail::RealPolicy<x4::strict_real_policies<double>>);

    STATIC_CHECK(x4::detail::RealPolicy<x4::real_policies<long double>>);
    STATIC_CHECK(x4::detail::RealPolicy<x4::strict_ureal_policies<long double>>);
    STATIC_CHECK(x4::detail::RealPolicy<x4::strict_real_policies<long double>>);

    // 3-digit separated numbers
    {
        using x4::uint_parser;

        constexpr uint_parser<unsigned, 10, 1, 3> uint3;
        constexpr uint_parser<unsigned, 10, 3, 3> uint3_3;

        constexpr auto r = uint3 >> *(',' >> uint3_3);

        CHECK(parse("1,234,567,890", r));
        CHECK(parse("12,345,678,900", r));
        CHECK(parse("123,456,789,000", r));
        CHECK(!parse("1000,234,567,890", r));
        CHECK(!parse("1,234,56,890", r));
        CHECK(!parse("1,66", r));
    }

    // unsigned real number tests
    {
        using x4::real_parser;
        using x4::ureal_policies;

        constexpr real_parser<double, ureal_policies<double>> udouble;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(udouble);

        {
            double d = 0;
            CHECK(parse("1234", udouble));
            REQUIRE(parse("1234", udouble, d));
            CHECK(compare(d, 1234));
        }
        {
            double d = 0;
            CHECK(parse("1.2e3", udouble));
            REQUIRE(parse("1.2e3", udouble, d));
            CHECK(compare(d, 1.2e3));
        }
        {
            double d = 0;
            CHECK(parse("1.2e-3", udouble));
            REQUIRE(parse("1.2e-3", udouble, d));
            CHECK(compare(d, 1.2e-3));
        }
        {
            double d = 0;
            CHECK(parse("1.e2", udouble));
            REQUIRE(parse("1.e2", udouble, d));
            CHECK(compare(d, 1.e2));
        }
        {
            double d = 0;
            CHECK(parse("1.", udouble));
            REQUIRE(parse("1.", udouble, d));
            CHECK(compare(d, 1.));
        }
        {
            double d = 0;
            CHECK(parse(".2e3", udouble));
            REQUIRE(parse(".2e3", udouble, d));
            CHECK(compare(d, .2e3));
        }
        {
            double d = 0;
            CHECK(parse("2e3", udouble));
            REQUIRE(parse("2e3", udouble, d));
            CHECK(compare(d, 2e3));
        }
        {
            double d = 0;
            CHECK(parse("2", udouble));
            REQUIRE(parse("2", udouble, d));
            CHECK(compare(d, 2));
        }
        {
            CHECK(parse("inf", udouble));
            CHECK(parse("infinity", udouble));
            CHECK(parse("INF", udouble));
            CHECK(parse("INFINITY", udouble));
        }
        {
            double d = 0;
            REQUIRE(parse("inf", udouble, d));
            CHECK(std::isinf(d));
            REQUIRE(parse("INF", udouble, d));
            CHECK(std::isinf(d));
            REQUIRE(parse("infinity", udouble, d));
            CHECK(std::isinf(d));
            REQUIRE(parse("INFINITY", udouble, d));
            CHECK(std::isinf(d));
        }
        {
            double d = 0;
            CHECK(parse("nan", udouble));
            REQUIRE(parse("nan", udouble, d));
            CHECK(std::isnan(d));
            CHECK(parse("NAN", udouble));
            REQUIRE(parse("NAN", udouble, d));
            CHECK(std::isnan(d));
            CHECK(parse("nan(...)", udouble));
            REQUIRE(parse("nan(...)", udouble, d));
            CHECK(std::isnan(d));
            CHECK(parse("NAN(...)", udouble));
            REQUIRE(parse("NAN(...)", udouble, d));
            CHECK(std::isnan(d));
        }
        {
            double d = 0;
            CHECK(!parse("e3", udouble));
            CHECK(!parse("e3", udouble, d));
        }
        {
            double d = 0;
            CHECK(!parse("-1.2e3", udouble));
            CHECK(!parse("-1.2e3", udouble, d));
        }
        {
            double d = 0;
            CHECK(!parse("+1.2e3", udouble));
            CHECK(!parse("+1.2e3", udouble, d));
        }
        {
            double d = 0;
            CHECK(!parse("1.2e", udouble));
            CHECK(!parse("1.2e", udouble, d));
        }
        {
            double d = 0;
            CHECK(!parse("-.3", udouble));
            CHECK(!parse("-.3", udouble, d));
        }
    }
}
