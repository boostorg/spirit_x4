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

int main()
{
    static_assert(x4::detail::RealPolicy<x4::real_policies<float>>);
    static_assert(x4::detail::RealPolicy<x4::strict_ureal_policies<float>>);
    static_assert(x4::detail::RealPolicy<x4::strict_real_policies<float>>);

    static_assert(x4::detail::RealPolicy<x4::real_policies<double>>);
    static_assert(x4::detail::RealPolicy<x4::strict_ureal_policies<double>>);
    static_assert(x4::detail::RealPolicy<x4::strict_real_policies<double>>);

    static_assert(x4::detail::RealPolicy<x4::real_policies<long double>>);
    static_assert(x4::detail::RealPolicy<x4::strict_ureal_policies<long double>>);
    static_assert(x4::detail::RealPolicy<x4::strict_real_policies<long double>>);

    // 3-digit separated numbers
    {
        using x4::uint_parser;

        uint_parser<unsigned, 10, 1, 3> uint3;
        uint_parser<unsigned, 10, 3, 3> uint3_3;

    #define r (uint3 >> *(',' >> uint3_3))

        BOOST_TEST(parse("1,234,567,890", r));
        BOOST_TEST(parse("12,345,678,900", r));
        BOOST_TEST(parse("123,456,789,000", r));
        BOOST_TEST(!parse("1000,234,567,890", r));
        BOOST_TEST(!parse("1,234,56,890", r));
        BOOST_TEST(!parse("1,66", r));
    }

    // unsigned real number tests
    {
        using x4::real_parser;
        using x4::ureal_policies;

        constexpr real_parser<double, ureal_policies<double>> udouble;
        double d;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(udouble);

        BOOST_TEST(parse("1234", udouble));
        BOOST_TEST(parse("1234", udouble, d) && compare(d, 1234));

        BOOST_TEST(parse("1.2e3", udouble));
        BOOST_TEST(parse("1.2e3", udouble, d) && compare(d, 1.2e3));

        BOOST_TEST(parse("1.2e-3", udouble));
        BOOST_TEST(parse("1.2e-3", udouble, d) && compare(d, 1.2e-3));

        BOOST_TEST(parse("1.e2", udouble));
        BOOST_TEST(parse("1.e2", udouble, d) && compare(d, 1.e2));

        BOOST_TEST(parse("1.", udouble));
        BOOST_TEST(parse("1.", udouble, d) && compare(d, 1.));

        BOOST_TEST(parse(".2e3", udouble));
        BOOST_TEST(parse(".2e3", udouble, d) && compare(d, .2e3));

        BOOST_TEST(parse("2e3", udouble));
        BOOST_TEST(parse("2e3", udouble, d) && compare(d, 2e3));

        BOOST_TEST(parse("2", udouble));
        BOOST_TEST(parse("2", udouble, d) && compare(d, 2));

        BOOST_TEST(parse("inf", udouble));
        BOOST_TEST(parse("infinity", udouble));
        BOOST_TEST(parse("INF", udouble));
        BOOST_TEST(parse("INFINITY", udouble));

        BOOST_TEST(parse("inf", udouble, d) && std::isinf(d));
        BOOST_TEST(parse("INF", udouble, d) && std::isinf(d));
        BOOST_TEST(parse("infinity", udouble, d) && std::isinf(d));
        BOOST_TEST(parse("INFINITY", udouble, d) && std::isinf(d));

        BOOST_TEST(parse("nan", udouble));
        BOOST_TEST(parse("nan", udouble, d) && std::isnan(d));
        BOOST_TEST(parse("NAN", udouble));
        BOOST_TEST(parse("NAN", udouble, d) && std::isnan(d));
        BOOST_TEST(parse("nan(...)", udouble));
        BOOST_TEST(parse("nan(...)", udouble, d) && std::isnan(d));
        BOOST_TEST(parse("NAN(...)", udouble));
        BOOST_TEST(parse("NAN(...)", udouble, d) && std::isnan(d));

        BOOST_TEST(!parse("e3", udouble));
        BOOST_TEST(!parse("e3", udouble, d));

        BOOST_TEST(!parse("-1.2e3", udouble));
        BOOST_TEST(!parse("-1.2e3", udouble, d));

        BOOST_TEST(!parse("+1.2e3", udouble));
        BOOST_TEST(!parse("+1.2e3", udouble, d));

        BOOST_TEST(!parse("1.2e", udouble));
        BOOST_TEST(!parse("1.2e", udouble, d));

        BOOST_TEST(!parse("-.3", udouble));
        BOOST_TEST(!parse("-.3", udouble, d));
    }

    return boost::report_errors();
}
