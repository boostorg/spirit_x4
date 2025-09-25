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

namespace {

//
// BEWARE PLATFORM DEPENDENT!!!
// The following assumes 32-bit integers and 64-bit long longs.
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
    //friend bool operator==(custom_uint a, custom_uint b) { return a.n == b.n; }
    //friend bool operator==(custom_uint a, unsigned b) { return a.n == b; }
    friend custom_uint operator*(custom_uint a, custom_uint b) { return custom_uint(a.n * b.n); }
    friend custom_uint operator+(custom_uint a, custom_uint b) { return custom_uint(a.n + b.n); }
};

} // anonymous

TEST_CASE("uint")
{
    // unsigned tests
    {
        using x4::uint_;
        unsigned u = 0;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint_);

        CHECK(parse("123456", uint_));
        REQUIRE(parse("123456", uint_, u));
        CHECK(u == 123456);

        CHECK(parse(max_unsigned, uint_));
        REQUIRE(parse(max_unsigned, uint_, u));
        CHECK(u == UINT_MAX);

        CHECK(!parse(unsigned_overflow, uint_));
        CHECK(!parse(unsigned_overflow, uint_, u));
    }

    // binary tests
    {
        using x4::bin;
        unsigned u = 0;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(bin);

        CHECK(parse("11111110", bin));
        REQUIRE(parse("11111110", bin, u));
        CHECK(u == 0xFE);

        CHECK(parse(max_binary, bin));
        REQUIRE(parse(max_binary, bin, u));
        CHECK(u == UINT_MAX);

        CHECK(!parse(binary_overflow, bin));
        CHECK(!parse(binary_overflow, bin, u));
    }

    // octal tests
    {
        using x4::oct;
        unsigned u = 0;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(oct);

        CHECK(parse("12545674515", oct));
        REQUIRE(parse("12545674515", oct, u));
        CHECK(u == 012545674515);

        CHECK(parse(max_octal, oct));
        REQUIRE(parse(max_octal, oct, u));
        CHECK(u == UINT_MAX);

        CHECK(!parse(octal_overflow, oct));
        CHECK(!parse(octal_overflow, oct, u));
    }

    // hex tests
    {
        using x4::hex;
        unsigned u = 0;

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(hex);

        CHECK(parse("95BC8DF", hex));
        REQUIRE(parse("95BC8DF", hex, u));
        CHECK(u == 0x95BC8DF);

        CHECK(parse("abcdef12", hex));
        REQUIRE(parse("abcdef12", hex, u));
        CHECK(u == 0xabcdef12);

        CHECK(parse(max_hex, hex));
        REQUIRE(parse(max_hex, hex, u));
        CHECK(u == UINT_MAX);

        CHECK(!parse(hex_overflow, hex));
        CHECK(!parse(hex_overflow, hex, u));
    }

    // limited fieldwidth
    {
        using x4::uint_parser;

        {
            constexpr uint_parser<unsigned, 10, 1, 3> uint3{};
            BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint3);

            CHECK(parse("123456", uint3).is_partial_match());

            unsigned u = 0;
            REQUIRE(parse("123456", uint3, u).is_partial_match());
            CHECK(u == 123);
        }
        {
            constexpr uint_parser<unsigned, 10, 2, 4> uint4{};
            BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint4);

            CHECK(parse("123456", uint4).is_partial_match());

            {
                unsigned u = 0;
                REQUIRE(parse("123456", uint4, u).is_partial_match());
                CHECK(u == 1234);
            }

            CHECK(!parse("1", uint4));
            {
                unsigned u = 0;
                CHECK(!parse("1", uint4, u));
            }
            {
                unsigned u = 0;
                REQUIRE(parse("014567", uint4, u).is_partial_match());
                CHECK(u == 145);
            }
        }

        constexpr uint_parser<unsigned, 10, 4, 4> uint_exact4{};
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(uint_exact4);

        {
            unsigned u = 1;
            auto const res = parse("0000000", uint_exact4, u);
            REQUIRE(res.is_partial_match());
            REQUIRE(res.remainder.size() == 3);
            CHECK(u == 0);
        }
        {
            unsigned u = 0;
            auto const res = parse("0001400", uint_exact4, u);
            REQUIRE(res.is_partial_match());
            REQUIRE(res.remainder.size() == 3);
            CHECK(u == 1);
        }
    }

    // action tests
    {
        using x4::_attr;
        using x4::uint_;
        using x4::standard::space;

        int n = 0;
        auto f = [&](auto& ctx){ n = _attr(ctx); };

        REQUIRE(parse("123", uint_[f]));
        CHECK(n == 123);
        REQUIRE(parse("   456", uint_[f], space));
        CHECK(n == 456);
    }

    // Check overflow is parse error
    {
        x4::uint_parser<std::uint8_t> uint8_;
        std::uint8_t u8 = 0;

        CHECK(!parse("999", uint8_, u8));
        CHECK(!parse("256", uint8_, u8));
        CHECK(parse("255", uint8_, u8));
    }
    {
        x4::uint_parser<std::uint16_t> uint16_;
        std::uint16_t u16 = 0;

        CHECK(!parse("99999", uint16_, u16));
        CHECK(!parse("65536", uint16_, u16));
        CHECK(parse("65535", uint16_, u16));
    }
    {
        x4::uint_parser<std::uint32_t> uint32_;
        std::uint32_t u32 = 0;

        CHECK(!parse("9999999999", uint32_, u32));
        CHECK(!parse("4294967296", uint32_, u32));
        CHECK(parse("4294967295", uint32_, u32));
    }
    {
        x4::uint_parser<std::int8_t> u_int8_;
        std::uint8_t u8 = 0;

        CHECK(!parse("999", u_int8_, u8));
        CHECK(!parse("-1", u_int8_, u8));
        CHECK(!parse("128", u_int8_, u8));
        CHECK(parse("127", u_int8_, u8));
        CHECK(parse("0", u_int8_, u8));
    }
    {
        x4::uint_parser<std::int16_t> u_int16_;
        std::uint16_t u16 = 0;

        CHECK(!parse("99999", u_int16_, u16));
        CHECK(!parse("-1", u_int16_, u16));
        CHECK(!parse("32768", u_int16_, u16));
        CHECK(parse("32767", u_int16_, u16));
        CHECK(parse("0", u_int16_, u16));
    }
    {
        x4::uint_parser<std::int32_t> u_int32_;
        std::uint32_t u32 = 0;

        CHECK(!parse("9999999999", u_int32_, u32));
        CHECK(!parse("-1", u_int32_, u32));
        CHECK(!parse("2147483648", u_int32_, u32));
        CHECK(parse("2147483647", u_int32_, u32));
        CHECK(parse("0", u_int32_, u32));
    }

    // custom uint tests
    {
        using x4::uint_;
        using x4::uint_parser;
        custom_uint u{};

        CHECK(parse("123456", uint_, u));
        uint_parser<custom_uint, 10, 1, 2> uint2;
        CHECK(parse("12", uint2, u));
    }
}
