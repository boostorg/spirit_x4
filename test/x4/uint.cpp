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
#include <boost/spirit/x4/numeric/uint.hpp>

#include <string_view>

#include <cstring>
#include <climits>
#include <cstdint>

//
// BEWARE PLATFORM DEPENDENT!!! ***
// The following assumes 32 bit integers and 64 bit long longs.
// Modify these constant strings when appropriate.
//

constexpr std::string_view max_unsigned = "4294967295";
constexpr std::string_view unsigned_overflow = "4294967296";
// constexpr std::string_view max_int = "2147483647";
// constexpr std::string_view int_overflow = "2147483648";
// constexpr std::string_view min_int = "-2147483648";
// constexpr std::string_view int_underflow = "-2147483649";
constexpr std::string_view max_binary = "11111111111111111111111111111111";
constexpr std::string_view binary_overflow = "100000000000000000000000000000000";
constexpr std::string_view max_octal = "37777777777";
constexpr std::string_view octal_overflow = "100000000000";
constexpr std::string_view max_hex = "FFFFFFFF";
constexpr std::string_view hex_overflow = "100000000";

// A custom int type
struct custom_uint
{
    unsigned n;
    custom_uint() : n(0) {}
    explicit custom_uint(unsigned n_) : n(n_) {}
    custom_uint& operator=(unsigned n_) { n = n_; return *this; }
    friend bool operator==(custom_uint a, custom_uint b)
        { return a.n == b.n; }
    friend bool operator==(custom_uint a, unsigned b)
        { return a.n == b; }
    friend custom_uint operator*(custom_uint a, custom_uint b)
        { return custom_uint(a.n * b.n); }
    friend custom_uint operator+(custom_uint a, custom_uint b)
        { return custom_uint(a.n + b.n); }
};

int main()
{
    // unsigned tests
    {
        using x4::uint_;
        unsigned u;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint_);

        BOOST_TEST(parse("123456", uint_));
        BOOST_TEST(parse("123456", uint_, u));
        BOOST_TEST(u == 123456);

        BOOST_TEST(parse(max_unsigned, uint_));
        BOOST_TEST(parse(max_unsigned, uint_, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!parse(unsigned_overflow, uint_));
        BOOST_TEST(!parse(unsigned_overflow, uint_, u));
    }

    // binary tests
    {
        using x4::bin;
        unsigned u;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(bin);

        BOOST_TEST(parse("11111110", bin));
        BOOST_TEST(parse("11111110", bin, u));
        BOOST_TEST(u == 0xFE);

        BOOST_TEST(parse(max_binary, bin));
        BOOST_TEST(parse(max_binary, bin, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!parse(binary_overflow, bin));
        BOOST_TEST(!parse(binary_overflow, bin, u));
    }

    // octal tests
    {
        using x4::oct;
        unsigned u;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(oct);

        BOOST_TEST(parse("12545674515", oct));
        BOOST_TEST(parse("12545674515", oct, u));
        BOOST_TEST(u == 012545674515);

        BOOST_TEST(parse(max_octal, oct));
        BOOST_TEST(parse(max_octal, oct, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!parse(octal_overflow, oct));
        BOOST_TEST(!parse(octal_overflow, oct, u));
    }

    // hex tests
    {
        using x4::hex;
        unsigned u;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(hex);

        BOOST_TEST(parse("95BC8DF", hex));
        BOOST_TEST(parse("95BC8DF", hex, u));
        BOOST_TEST(u == 0x95BC8DF);

        BOOST_TEST(parse("abcdef12", hex));
        BOOST_TEST(parse("abcdef12", hex, u));
        BOOST_TEST(u == 0xabcdef12);

        BOOST_TEST(parse(max_hex, hex));
        BOOST_TEST(parse(max_hex, hex, u));
        BOOST_TEST(u == UINT_MAX);

        BOOST_TEST(!parse(hex_overflow, hex));
        BOOST_TEST(!parse(hex_overflow, hex, u));
    }

    // limited fieldwidth
    {
        using x4::uint_parser;

        {
            constexpr uint_parser<unsigned, 10, 1, 3> uint3{};
            BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint3);

            BOOST_TEST(parse("123456", uint3).is_partial_match());

            unsigned u = 0;
            BOOST_TEST(parse("123456", uint3, u).is_partial_match());
            BOOST_TEST(u == 123);
        }
        {
            constexpr uint_parser<unsigned, 10, 2, 4> uint4{};
            BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint4);

            BOOST_TEST(parse("123456", uint4).is_partial_match());

            {
                unsigned u = 0;
                BOOST_TEST(parse("123456", uint4, u).is_partial_match());
                BOOST_TEST(u == 1234);
            }

            BOOST_TEST(!parse("1", uint4));
            {
                unsigned u = 0;
                BOOST_TEST(!parse("1", uint4, u));
            }
            {
                unsigned u = 0;
                BOOST_TEST(parse("014567", uint4, u).is_partial_match() && u == 145);
            }
        }

        constexpr uint_parser<unsigned, 10, 4, 4> uint_exact4{};
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint_exact4);

        {
            unsigned u = 1;
            auto const res = parse("0000000", uint_exact4, u);
            BOOST_TEST(res.is_partial_match() && res.remainder.size() == 3 && u == 0);
        }
        {
            unsigned u = 0;
            auto const res = parse("0001400", uint_exact4, u);
            BOOST_TEST(res.is_partial_match() && res.remainder.size() == 3 && u == 1);
        }
    }

    // action tests
    {
        using x4::_attr;
        using x4::uint_;
        using x4::standard::space;

        int n = 0;
        auto f = [&](auto& ctx){ n = _attr(ctx); };

        BOOST_TEST(parse("123", uint_[f]));
        BOOST_TEST(n == 123);
        BOOST_TEST(parse("   456", uint_[f], space));
        BOOST_TEST(n == 456);
    }

    // Check overflow is parse error
    {
        x4::uint_parser<std::uint8_t> uint8_;
        std::uint8_t u8;

        BOOST_TEST(!parse("999", uint8_, u8));
        BOOST_TEST(!parse("256", uint8_, u8));
        BOOST_TEST(parse("255", uint8_, u8));

        x4::uint_parser<std::uint16_t> uint16_;
        std::uint16_t u16;

        BOOST_TEST(!parse("99999", uint16_, u16));
        BOOST_TEST(!parse("65536", uint16_, u16));
        BOOST_TEST(parse("65535", uint16_, u16));

        x4::uint_parser<std::uint32_t> uint32_;
        std::uint32_t u32;

        BOOST_TEST(!parse("9999999999", uint32_, u32));
        BOOST_TEST(!parse("4294967296", uint32_, u32));
        BOOST_TEST(parse("4294967295", uint32_, u32));

        x4::uint_parser<std::int8_t> u_int8_;

        BOOST_TEST(!parse("999", u_int8_, u8));
        BOOST_TEST(!parse("-1", u_int8_, u8));
        BOOST_TEST(!parse("128", u_int8_, u8));
        BOOST_TEST(parse("127", u_int8_, u8));
        BOOST_TEST(parse("0", u_int8_, u8));

        x4::uint_parser<std::int16_t> u_int16_;

        BOOST_TEST(!parse("99999", u_int16_, u16));
        BOOST_TEST(!parse("-1", u_int16_, u16));
        BOOST_TEST(!parse("32768", u_int16_, u16));
        BOOST_TEST(parse("32767", u_int16_, u16));
        BOOST_TEST(parse("0", u_int16_, u16));

        x4::uint_parser<std::int32_t> u_int32_;

        BOOST_TEST(!parse("9999999999", u_int32_, u32));
        BOOST_TEST(!parse("-1", u_int32_, u32));
        BOOST_TEST(!parse("2147483648", u_int32_, u32));
        BOOST_TEST(parse("2147483647", u_int32_, u32));
        BOOST_TEST(parse("0", u_int32_, u32));
    }

    // custom uint tests
    {
        using x4::uint_;
        using x4::uint_parser;
        custom_uint u;

        BOOST_TEST(parse("123456", uint_, u));
        uint_parser<custom_uint, 10, 1, 2> uint2;
        BOOST_TEST(parse("12", uint2, u));
    }

    return boost::report_errors();
}
