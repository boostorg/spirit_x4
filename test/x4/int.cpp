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

//
// BEWARE PLATFORM DEPENDENT!!!
// The following assumes 32 bit or 64 bit integers and 64 bit long longs.
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
    friend bool operator==(custom_int a, custom_int b) { return a.n == b.n; }
    friend bool operator==(custom_int a, int b) { return a.n == b; }
    friend custom_int operator*(custom_int a, custom_int b) { return custom_int(a.n * b.n); }
    friend custom_int operator+(custom_int a, custom_int b) { return custom_int(a.n + b.n); }
    friend custom_int operator-(custom_int a, custom_int b) { return custom_int(a.n - b.n); }
};

int main()
{
    // signed integer tests
    {
        using x4::int_;
        int i;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(int_);

        BOOST_TEST(parse("123456", int_));
        BOOST_TEST(parse("123456", int_, i));
        BOOST_TEST(i == 123456);

        BOOST_TEST(parse("+123456", int_));
        BOOST_TEST(parse("+123456", int_, i));
        BOOST_TEST(i == 123456);

        BOOST_TEST(parse("-123456", int_));
        BOOST_TEST(parse("-123456", int_, i));
        BOOST_TEST(i == -123456);

        BOOST_TEST(parse(max_int, int_));
        BOOST_TEST(parse(max_int, int_, i));
        BOOST_TEST(i == INT_MAX);

        BOOST_TEST(parse(min_int, int_));
        BOOST_TEST(parse(min_int, int_, i));
        BOOST_TEST(i == INT_MIN);

        BOOST_TEST(!parse(int_overflow, int_));
        BOOST_TEST(!parse(int_overflow, int_, i));
        BOOST_TEST(!parse(int_underflow, int_));
        BOOST_TEST(!parse(int_underflow, int_, i));

        BOOST_TEST(!parse("-", int_));
        BOOST_TEST(!parse("-", int_, i));

        BOOST_TEST(!parse("+", int_));
        BOOST_TEST(!parse("+", int_, i));

        // Bug report from Steve Nutt
        BOOST_TEST(!parse("5368709120", int_, i));

        // with leading zeros
        BOOST_TEST(parse("0000000000123456", int_));
        BOOST_TEST(parse("0000000000123456", int_, i));
        BOOST_TEST(i == 123456);
    }

    // long long tests
    {
        using x4::long_long;
        boost::long_long_type ll;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(long_long);

        BOOST_TEST(parse("1234567890123456789", long_long));
        BOOST_TEST(parse("1234567890123456789", long_long, ll));
        BOOST_TEST(ll == 1234567890123456789LL);

        BOOST_TEST(parse("-1234567890123456789", long_long));
        BOOST_TEST(parse("-1234567890123456789", long_long, ll));
        BOOST_TEST(ll == -1234567890123456789LL);

        BOOST_TEST(parse(max_long_long, long_long));
        BOOST_TEST(parse(max_long_long, long_long, ll));
        BOOST_TEST(ll == LLONG_MAX);

        BOOST_TEST(parse(min_long_long, long_long));
        BOOST_TEST(parse(min_long_long, long_long, ll));
        BOOST_TEST(ll == LLONG_MIN);

        BOOST_TEST(!parse(long_long_overflow, long_long));
        BOOST_TEST(!parse(long_long_overflow, long_long, ll));
        BOOST_TEST(!parse(long_long_underflow, long_long));
        BOOST_TEST(!parse(long_long_underflow, long_long, ll));
    }

    // short_ and long_ tests
    {
        using x4::short_;
        using x4::long_;
        int i;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(short_);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(long_);

        BOOST_TEST(parse("12345", short_));
        BOOST_TEST(parse("12345", short_, i));
        BOOST_TEST(i == 12345);

        BOOST_TEST(parse("1234567890", long_));
        BOOST_TEST(parse("1234567890", long_, i));
        BOOST_TEST(i == 1234567890);
    }

    // Check overflow is parse error
    {
        constexpr x4::int_parser<std::int8_t> int8_{};
        char c;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(int8_);

        BOOST_TEST(!parse("999", int8_, c));

        int i;
        using x4::short_;
        BOOST_TEST(!parse("32769", short_, i).is_partial_match());
        BOOST_TEST(!parse("41234", short_, i).is_partial_match());
    }

    // int_parser<unused_type> tests
    {
        using x4::int_parser;
        using x4::unused_type;
        constexpr int_parser<unused_type> any_int{};

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(any_int);

        BOOST_TEST(parse("123456", any_int));
        BOOST_TEST(parse("-123456", any_int));
        BOOST_TEST(parse("-1234567890123456789", any_int));
    }

    // action tests
    {
        using x4::_attr;
        using x4::standard::space;
        using x4::int_;
        int n = 0, m = 0;

        auto f = [&](auto& ctx){ n = _attr(ctx); };

        BOOST_TEST(parse("123", int_[f]));
        BOOST_TEST(n == 123);
        BOOST_TEST(parse("789", int_[f], m));
        BOOST_TEST(n == 789 && m == 789);
        BOOST_TEST(parse("   456", int_[f], space));
        BOOST_TEST(n == 456);
    }

    // custom int tests
    {
        using x4::int_;
        using x4::int_parser;
        custom_int i;

        BOOST_TEST(parse("-123456", int_, i));
        int_parser<custom_int, 10, 1, 2> int2;
        BOOST_TEST(parse("-12", int2, i));
    }

    // single-element fusion vector tests
    {
        using x4::int_;
        using x4::int_parser;
        boost::fusion::vector<int> i;

        BOOST_TEST(parse("-123456", int_, i));
        BOOST_TEST(boost::fusion::at_c<0>(i) == -123456);
    }

    return boost::report_errors();
}
