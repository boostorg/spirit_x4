/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Bryce Lelbach
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/numeric/int.hpp>

#include <boost/fusion/include/vector.hpp>

#include <string_view>

#include <climits>
#include <cstdint>

namespace {

//
// BEWARE PLATFORM DEPENDENT!!!
// The following assumes 32-bit or 64-bit integers and 64 bit long longs.
// Modify these constant strings when appropriate.
//

static_assert(sizeof(long long) == 8, "unexpected long long size");

#if INT_MAX != LLONG_MAX
static_assert(sizeof(int) == 4, "unexpected int size");
constexpr std::string_view max_int = "2147483647";
constexpr std::string_view int_overflow = "2147483648";
constexpr std::string_view min_int = "-2147483648";
constexpr std::string_view int_underflow = "-2147483649";
#else
static_assert(sizeof(int) == 8, "unexpected int size");
constexpr std::string_view max_int = "9223372036854775807";
constexpr std::string_view int_overflow = "9223372036854775808";
constexpr std::string_view min_int = "-9223372036854775808";
constexpr std::string_view int_underflow = "-9223372036854775809";
#endif

constexpr std::string_view max_long_long = "9223372036854775807";
constexpr std::string_view long_long_overflow = "9223372036854775808";
constexpr std::string_view min_long_long = "-9223372036854775808";
constexpr std::string_view long_long_underflow = "-9223372036854775809";

// A custom int type
struct custom_int
{
    int n;
    custom_int() : n(0) {}
    explicit custom_int(int n_) : n(n_) {}
    custom_int& operator=(int n_) { n = n_; return *this; }
    //friend bool operator==(custom_int a, custom_int b) { return a.n == b.n; }
    //friend bool operator==(custom_int a, int b) { return a.n == b; }
    friend custom_int operator*(custom_int a, custom_int b) { return custom_int(a.n * b.n); }
    friend custom_int operator+(custom_int a, custom_int b) { return custom_int(a.n + b.n); }
    friend custom_int operator-(custom_int a, custom_int b) { return custom_int(a.n - b.n); }
};

} // anonymous

TEST_CASE("int")
{
    using x4::short_;
    using x4::int_;
    using x4::long_;
    using x4::long_long;
    using x4::int8;
    using x4::_attr;
    using x4::int_parser;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(int8);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(short_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(int_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(long_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(long_long);

    // signed integer tests
    {
        int i = 0;
        CHECK(parse("123456", int_));
        REQUIRE(parse("123456", int_, i));
        CHECK(i == 123456);
    }
    {
        int i = 0;
        CHECK(parse("+123456", int_));
        REQUIRE(parse("+123456", int_, i));
        CHECK(i == 123456);
    }
    {
        int i = 0;
        CHECK(parse("-123456", int_));
        REQUIRE(parse("-123456", int_, i));
        CHECK(i == -123456);
    }
    {
        int i = 0;
        CHECK(parse(max_int, int_));
        REQUIRE(parse(max_int, int_, i));
        CHECK(i == INT_MAX);
    }
    {
        int i = 0;
        CHECK(parse(min_int, int_));
        REQUIRE(parse(min_int, int_, i));
        CHECK(i == INT_MIN);
    }
    {
        int i = 0;
        CHECK(!parse(int_overflow, int_));
        CHECK(!parse(int_overflow, int_, i));
        CHECK(!parse(int_underflow, int_));
        CHECK(!parse(int_underflow, int_, i));

        CHECK(!parse("-", int_));
        CHECK(!parse("-", int_, i));

        CHECK(!parse("+", int_));
        CHECK(!parse("+", int_, i));

        // Bug report from Steve Nutt
        CHECK(!parse("5368709120", int_, i));
    }
    {
        int i = 0;
        // with leading zeros
        CHECK(parse("0000000000123456", int_));
        REQUIRE(parse("0000000000123456", int_, i));
        CHECK(i == 123456);
    }

    // long long tests
    {
        long long ll = 0;
        CHECK(parse("1234567890123456789", long_long));
        REQUIRE(parse("1234567890123456789", long_long, ll));
        CHECK(ll == 1234567890123456789LL);
    }
    {
        long long ll = 0;
        CHECK(parse("-1234567890123456789", long_long));
        REQUIRE(parse("-1234567890123456789", long_long, ll));
        CHECK(ll == -1234567890123456789LL);
    }
    {
        long long ll = 0;
        CHECK(parse(max_long_long, long_long));
        REQUIRE(parse(max_long_long, long_long, ll));
        CHECK(ll == LLONG_MAX);
    }
    {
        long long ll = 0;
        CHECK(parse(min_long_long, long_long));
        REQUIRE(parse(min_long_long, long_long, ll));
        CHECK(ll == LLONG_MIN);
    }

    {
        long long ll = 0;
        CHECK(!parse(long_long_overflow, long_long));
        CHECK(!parse(long_long_overflow, long_long, ll));
        CHECK(!parse(long_long_underflow, long_long));
        CHECK(!parse(long_long_underflow, long_long, ll));
    }

    // short_ and long_ tests
    {
        short s = 0;
        CHECK(parse("12345", short_));
        CHECK(parse("12345", short_, s));
        CHECK(s == 12345);
    }
    {
        long l = 0;
        CHECK(parse("1234567890", long_));
        CHECK(parse("1234567890", long_, l));
        CHECK(l == 1234567890);
    }

    // Check overflow is parse error
    {
        std::int8_t i = 0;
        CHECK(!parse("999", int8, i));
    }
    {
        short s = 0;
        CHECK(!parse("32769", short_, s));
        CHECK(!parse("41234", short_, s));
    }

    // int_parser<unused_type> tests
    {
        constexpr int_parser<unused_type> any_int{};
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(any_int);

        CHECK(parse("123456", any_int));
        CHECK(parse("-123456", any_int));
        CHECK(parse("-1234567890123456789", any_int));
    }

    // action tests
    {
        using x4::standard::space;
        int n = 0, m = 0;

        auto f = [&](auto& ctx){ n = _attr(ctx); };

        REQUIRE(parse("123", int_[f]));
        CHECK(n == 123);
        REQUIRE(parse("789", int_[f], m));
        REQUIRE(n == 789);
        CHECK(m == 789);
        REQUIRE(parse("   456", int_[f], space));
        CHECK(n == 456);
    }

    // custom int tests
    {
        custom_int i{};

        CHECK(parse("-123456", int_, i));
        int_parser<custom_int, 10, 1, 2> int2;
        CHECK(parse("-12", int2, i));
    }

    // single-element fusion vector tests
    {
        boost::fusion::vector<int> i{};

        REQUIRE(parse("-123456", int_, i));
        CHECK(boost::fusion::at_c<0>(i) == -123456);
    }
}
