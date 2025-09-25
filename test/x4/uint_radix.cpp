/*=============================================================================
    Copyright (c) 2011 Jan Frederick Eick
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/numeric/uint.hpp>

#include <string_view>

#include <climits>
#include <cstring>

namespace {

//
// BEWARE PLATFORM DEPENDENT!!!
// The following assumes 32-bit integers and 64-bit long longs.
// Modify these constant strings when appropriate.
//

constexpr std::string_view max_unsigned_base3 =                "102002022201221111210";
constexpr std::string_view unsigned_overflow_base3 =           "102002022201221111211";
constexpr std::string_view digit_overflow_base3 =              "1020020222012211112100";

constexpr std::string_view max_unsigned_base4 =                "3333333333333333";
constexpr std::string_view digit_overflow_base4 =              "33333333333333330";

constexpr std::string_view max_unsigned_base5 =                "32244002423140";
constexpr std::string_view unsigned_overflow_base5 =           "32244002423141";
constexpr std::string_view digit_overflow_base5 =              "322440024231400";

constexpr std::string_view max_unsigned_base6 =                "1550104015503";
constexpr std::string_view unsigned_overflow_base6 =           "1550104015504";
constexpr std::string_view digit_overflow_base6 =              "15501040155030";

constexpr std::string_view max_unsigned_base7 =                "211301422353";
constexpr std::string_view unsigned_overflow_base7 =           "211301422354";
constexpr std::string_view digit_overflow_base7 =              "2113014223530";

constexpr std::string_view max_unsigned_base9 =                "12068657453";
constexpr std::string_view unsigned_overflow_base9 =           "12068657454";
constexpr std::string_view digit_overflow_base9 =              "120686574530";

constexpr std::string_view max_unsigned_base11 =               "1904440553";
constexpr std::string_view unsigned_overflow_base11 =          "1904440554";
constexpr std::string_view digit_overflow_base11 =             "19044405530";

constexpr std::string_view max_unsigned_base12 =               "9BA461593";
constexpr std::string_view unsigned_overflow_base12 =          "9BA461594";
constexpr std::string_view digit_overflow_base12 =             "9BA4615930";

constexpr std::string_view max_unsigned_base13 =               "535A79888";
constexpr std::string_view unsigned_overflow_base13 =          "535A79889";
constexpr std::string_view digit_overflow_base13 =             "535A798880";

constexpr std::string_view max_unsigned_base14 =               "2CA5B7463";
constexpr std::string_view unsigned_overflow_base14 =          "2CA5B7464";
constexpr std::string_view digit_overflow_base14 =             "2CA5B74630";

constexpr std::string_view max_unsigned_base15 =               "1A20DCD80";
constexpr std::string_view unsigned_overflow_base15 =          "1A20DCD81";
constexpr std::string_view digit_overflow_base15 =             "1A20DCD800";

constexpr std::string_view max_unsigned_base17 =               "A7FFDA90";
constexpr std::string_view unsigned_overflow_base17 =          "A7FFDA91";
constexpr std::string_view digit_overflow_base17 =             "A7FFDA900";

constexpr std::string_view max_unsigned_base18 =               "704HE7G3";
constexpr std::string_view unsigned_overflow_base18 =          "704HE7G4";
constexpr std::string_view digit_overflow_base18 =             "704HE7G30";

constexpr std::string_view max_unsigned_base19 =               "4F5AFF65";
constexpr std::string_view unsigned_overflow_base19 =          "4F5AFF66";
constexpr std::string_view digit_overflow_base19 =             "4F5AFF650";

constexpr std::string_view max_unsigned_base20 =               "3723AI4F";
constexpr std::string_view unsigned_overflow_base20 =          "3723AI4G";
constexpr std::string_view digit_overflow_base20 =             "3723AI4G0";

constexpr std::string_view max_unsigned_base21 =               "281D55I3";
constexpr std::string_view unsigned_overflow_base21 =          "281D55I4";
constexpr std::string_view digit_overflow_base21 =             "281D55I30";

constexpr std::string_view max_unsigned_base22 =               "1FJ8B183";
constexpr std::string_view unsigned_overflow_base22 =          "1FJ8B184";
constexpr std::string_view digit_overflow_base22 =             "1FJ8B1830";

constexpr std::string_view max_unsigned_base23 =               "1606K7IB";
constexpr std::string_view unsigned_overflow_base23 =          "1606K7IC";
constexpr std::string_view digit_overflow_base23 =             "1606K7IB0";

constexpr std::string_view max_unsigned_base24 =               "MB994AF";
constexpr std::string_view unsigned_overflow_base24 =          "MB994AG";
constexpr std::string_view digit_overflow_base24 =             "MB994AF0";

constexpr std::string_view max_unsigned_base25 =               "HEK2MGK";
constexpr std::string_view unsigned_overflow_base25 =          "HEK2MGL";
constexpr std::string_view digit_overflow_base25 =             "HEK2MGK0";

constexpr std::string_view max_unsigned_base26 =               "DNCHBNL";
constexpr std::string_view unsigned_overflow_base26 =          "DNCHBNM";
constexpr std::string_view digit_overflow_base26 =             "DNCHBNL0";

constexpr std::string_view max_unsigned_base27 =               "B28JPDL";
constexpr std::string_view unsigned_overflow_base27 =          "B28JPDM";
constexpr std::string_view digit_overflow_base27 =             "B28JPDL0";

constexpr std::string_view max_unsigned_base28 =               "8PFGIH3";
constexpr std::string_view unsigned_overflow_base28 =          "8PFGIH4";
constexpr std::string_view digit_overflow_base28 =             "8PFGIH30";

constexpr std::string_view max_unsigned_base29 =               "76BEIGF";
constexpr std::string_view unsigned_overflow_base29 =          "76BEIGH";
constexpr std::string_view digit_overflow_base29 =             "76BEIGF0";

constexpr std::string_view max_unsigned_base30 =               "5QMCPQF";
constexpr std::string_view unsigned_overflow_base30 =          "5QMCPQG";
constexpr std::string_view digit_overflow_base30 =             "5QMCPQF0";

constexpr std::string_view max_unsigned_base31 =               "4Q0JTO3";
constexpr std::string_view unsigned_overflow_base31 =          "4Q0JTO4";
constexpr std::string_view digit_overflow_base31 =             "4Q0JTO30";

constexpr std::string_view max_unsigned_base32 =               "3VVVVVV";
constexpr std::string_view unsigned_overflow_base32 =          "3VVVVVW";
constexpr std::string_view digit_overflow_base32 =             "3VVVVVV0";

constexpr std::string_view max_unsigned_base33 =               "3AOKQ93";
constexpr std::string_view unsigned_overflow_base33 =          "3AOKQ94";
constexpr std::string_view digit_overflow_base33 =             "3AOKQ930";

constexpr std::string_view max_unsigned_base34 =               "2QHXJLH";
constexpr std::string_view unsigned_overflow_base34 =          "2QHXJLI";
constexpr std::string_view digit_overflow_base34 =             "2QHXJLH0";

constexpr std::string_view max_unsigned_base35 =               "2BR45QA";
constexpr std::string_view unsigned_overflow_base35 =          "2BR45QB";
constexpr std::string_view digit_overflow_base35 =             "2BR45QA0";

constexpr std::string_view max_unsigned_base36 =               "1Z141Z3";
constexpr std::string_view unsigned_overflow_base36 =          "1Z141Z4";
constexpr std::string_view digit_overflow_base36 =             "1Z141Z30";

using x4::uint_parser;

} // anonymous


TEST_CASE("uint_radix_3-15")
{
    // arbitrary radix test (base 3)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 3, 1, -1>             base3_parser;

        CHECK(parse("210112221200",                 base3_parser));
        REQUIRE(parse("210112221200",            base3_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("1231",                        base3_parser));
        CHECK(!parse("1231",                   base3_parser, u));

        CHECK(parse(max_unsigned_base3,             base3_parser));
        CHECK(parse(max_unsigned_base3,        base3_parser, u));

        CHECK(!parse(unsigned_overflow_base3,       base3_parser));
        CHECK(!parse(unsigned_overflow_base3,  base3_parser, u));
        CHECK(!parse(digit_overflow_base3,          base3_parser));
        CHECK(!parse(digit_overflow_base3,     base3_parser, u));
    }

    // arbitrary radix test (base 4)
    {
        unsigned int u = 0;

        uint_parser<unsigned int, 4, 1, -1>             base4_parser;

        CHECK(parse("1213210302",                   base4_parser));
        REQUIRE(parse("1213210302",              base4_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("1234",                        base4_parser));
        CHECK(!parse("1234",                   base4_parser, u));

        CHECK(parse(max_unsigned_base4,             base4_parser));
        CHECK(parse(max_unsigned_base4,        base4_parser, u));
        CHECK(!parse(digit_overflow_base4,          base4_parser));
        CHECK(!parse(digit_overflow_base4,     base4_parser, u));
    }

    // arbitrary radix test (base 5)
    {
        unsigned int u = 0;

        uint_parser<unsigned int, 5, 1, -1>             base5_parser;

        CHECK(parse("102033432",                    base5_parser));
        REQUIRE(parse("102033432",               base5_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("2345",                        base5_parser));
        CHECK(!parse("2345",                   base5_parser, u));

        CHECK(parse(max_unsigned_base5,             base5_parser));
        CHECK(parse(max_unsigned_base5,        base5_parser, u));

        CHECK(!parse(unsigned_overflow_base5,       base5_parser));
        CHECK(!parse(unsigned_overflow_base5,  base5_parser, u));
        CHECK(!parse(digit_overflow_base5,          base5_parser));
        CHECK(!parse(digit_overflow_base5,     base5_parser, u));
    }

    // arbitrary radix test (base 6)
    {
        unsigned int u = 0;

        uint_parser<unsigned int, 6, 1, -1>             base6_parser;

        CHECK(parse("13032030",                     base6_parser));
        REQUIRE(parse("13032030",                base6_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("3456",                        base6_parser));
        CHECK(!parse("3456",                   base6_parser, u));

        CHECK(parse(max_unsigned_base6,             base6_parser));
        CHECK(parse(max_unsigned_base6,        base6_parser, u));

        CHECK(!parse(unsigned_overflow_base6,       base6_parser));
        CHECK(!parse(unsigned_overflow_base6,  base6_parser, u));
        CHECK(!parse(digit_overflow_base6,          base6_parser));
        CHECK(!parse(digit_overflow_base6,     base6_parser, u));
    }

    // arbitrary radix test (base 7)
    {
        unsigned int u = 0;

        uint_parser<unsigned int, 7, 1, -1>             base7_parser;

        CHECK(parse("3414600",                      base7_parser));
        REQUIRE(parse("3414600",                 base7_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("4567",                        base7_parser));
        CHECK(!parse("4567",                   base7_parser, u));

        CHECK(parse(max_unsigned_base7,             base7_parser));
        CHECK(parse(max_unsigned_base7,        base7_parser, u));

        CHECK(!parse(unsigned_overflow_base7,       base7_parser));
        CHECK(!parse(unsigned_overflow_base7,  base7_parser, u));
        CHECK(!parse(digit_overflow_base7,          base7_parser));
        CHECK(!parse(digit_overflow_base7,     base7_parser, u));
    }

    // arbitrary radix test (base 9)
    {
        unsigned int u = 0;

        uint_parser<unsigned int, 9, 1, -1>             base9_parser;

        CHECK(parse("715850",                       base9_parser));
        REQUIRE(parse("715850",                  base9_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("6789",                        base9_parser));
        CHECK(!parse("6789",                   base9_parser, u));

        CHECK(parse(max_unsigned_base9,             base9_parser));
        CHECK(parse(max_unsigned_base9,        base9_parser, u));

        CHECK(!parse(unsigned_overflow_base9,       base9_parser));
        CHECK(!parse(unsigned_overflow_base9,  base9_parser, u));
        CHECK(!parse(digit_overflow_base9,          base9_parser));
        CHECK(!parse(digit_overflow_base9,     base9_parser, u));
    }

    // arbitrary radix test (base 11)
    {
        unsigned int u = 0;

        uint_parser<unsigned int, 11, 1, -1>            base11_parser;

        CHECK(parse("26a815",                       base11_parser));
        REQUIRE(parse("26a815",                  base11_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("90ab",                        base11_parser));
        CHECK(!parse("90AB",                   base11_parser, u));

        CHECK(parse(max_unsigned_base11,            base11_parser));
        CHECK(parse(max_unsigned_base11,       base11_parser, u));

        CHECK(!parse(unsigned_overflow_base11,      base11_parser));
        CHECK(!parse(unsigned_overflow_base11, base11_parser, u));
        CHECK(!parse(digit_overflow_base11,         base11_parser));
        CHECK(!parse(digit_overflow_base11,    base11_parser, u));
    }

    // arbitrary radix test (base 12)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 12, 1, -1>            base12_parser;

        CHECK(parse("185616",                       base12_parser));
        REQUIRE(parse("185616",                  base12_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("9abc",                        base12_parser));
        CHECK(!parse("9ABC",                   base12_parser, u));

        CHECK(parse(max_unsigned_base12,            base12_parser));
        CHECK(parse(max_unsigned_base12,       base12_parser, u));

        CHECK(!parse(unsigned_overflow_base12,      base12_parser));
        CHECK(!parse(unsigned_overflow_base12, base12_parser, u));
        CHECK(!parse(digit_overflow_base12,         base12_parser));
        CHECK(!parse(digit_overflow_base12,    base12_parser, u));
    }

    // arbitrary radix test (base 13)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 13, 1, -1>            base13_parser;

        CHECK(parse("11b140",                       base13_parser));
        REQUIRE(parse("11b140",                  base13_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("abcd",                        base13_parser));
        CHECK(!parse("ABCD",                   base13_parser, u));

        CHECK(parse(max_unsigned_base13,            base13_parser));
        CHECK(parse(max_unsigned_base13,       base13_parser, u));

        CHECK(!parse(unsigned_overflow_base13,      base13_parser));
        CHECK(!parse(unsigned_overflow_base13, base13_parser, u));
        CHECK(!parse(digit_overflow_base13,         base13_parser));
        CHECK(!parse(digit_overflow_base13,    base13_parser, u));
    }

    // arbitrary radix test (base 14)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 14, 1, -1>            base14_parser;

        CHECK(parse("b0870",                        base14_parser));
        REQUIRE(parse("b0870",                   base14_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("bcde",                        base14_parser));
        CHECK(!parse("BCDE",                   base14_parser, u));

        CHECK(parse(max_unsigned_base14,            base14_parser));
        CHECK(parse(max_unsigned_base14,       base14_parser, u));

        CHECK(!parse(unsigned_overflow_base14,      base14_parser));
        CHECK(!parse(unsigned_overflow_base14, base14_parser, u));
        CHECK(!parse(digit_overflow_base14,         base14_parser));
        CHECK(!parse(digit_overflow_base14,    base14_parser, u));
    }

    // arbitrary radix test (base 15)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 15, 1, -1>            base15_parser;

        CHECK(parse("85a7c",                        base15_parser));
        REQUIRE(parse("85a7c",                   base15_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("cdef",                        base15_parser));
        CHECK(!parse("CDEF",                   base15_parser, u));

        CHECK(parse(max_unsigned_base15,            base15_parser));
        CHECK(parse(max_unsigned_base15,       base15_parser, u));

        CHECK(!parse(unsigned_overflow_base15,      base15_parser));
        CHECK(!parse(unsigned_overflow_base15, base15_parser, u));
        CHECK(!parse(digit_overflow_base15,         base15_parser));
        CHECK(!parse(digit_overflow_base15,    base15_parser, u));
    }
}

TEST_CASE("uint_radix_17-31")
{
    // arbitrary radix test (base 17)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 17, 1, -1>            base17_parser;

        CHECK(parse("515g7",                        base17_parser));
        REQUIRE(parse("515g7",                   base17_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("efgh",                        base17_parser));
        CHECK(!parse("EFGH",                   base17_parser, u));

        CHECK(parse(max_unsigned_base17,            base17_parser));
        CHECK(parse(max_unsigned_base17,       base17_parser, u));

        CHECK(!parse(unsigned_overflow_base17,      base17_parser));
        CHECK(!parse(unsigned_overflow_base17, base17_parser, u));
        CHECK(!parse(digit_overflow_base17,         base17_parser));
        CHECK(!parse(digit_overflow_base17,    base17_parser, u));
    }

    // arbitrary radix test (base 18)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 18, 1, -1>            base18_parser;

        CHECK(parse("40d70",                        base18_parser));
        REQUIRE(parse("40d70",                   base18_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("fghi",                        base18_parser));
        CHECK(!parse("FGHI",                   base18_parser, u));

        CHECK(parse(max_unsigned_base18,            base18_parser));
        CHECK(parse(max_unsigned_base18,       base18_parser, u));

        CHECK(!parse(unsigned_overflow_base18,      base18_parser));
        CHECK(!parse(unsigned_overflow_base18, base18_parser, u));
        CHECK(!parse(digit_overflow_base18,         base18_parser));
        CHECK(!parse(digit_overflow_base18,    base18_parser, u));
    }

    // arbitrary radix test (base 19)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 19, 1, -1>            base19_parser;

        CHECK(parse("34g3a",                        base19_parser));
        REQUIRE(parse("34g3a",                   base19_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("ghij",                        base19_parser));
        CHECK(!parse("GHIJ",                   base19_parser, u));

        CHECK(parse(max_unsigned_base19,            base19_parser));
        CHECK(parse(max_unsigned_base19,       base19_parser, u));

        CHECK(!parse(unsigned_overflow_base19,      base19_parser));
        CHECK(!parse(unsigned_overflow_base19, base19_parser, u));
        CHECK(!parse(digit_overflow_base19,         base19_parser));
        CHECK(!parse(digit_overflow_base19,    base19_parser, u));
    }

    // arbitrary radix test (base 20)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 20, 1, -1>            base20_parser;

        CHECK(parse("2d0c2",                        base20_parser));
        REQUIRE(parse("2d0c2",                   base20_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("hijk",                        base20_parser));
        CHECK(!parse("HIJK",                   base20_parser, u));

        CHECK(parse(max_unsigned_base20,            base20_parser));
        CHECK(parse(max_unsigned_base20,       base20_parser, u));

        CHECK(!parse(unsigned_overflow_base20,      base20_parser));
        CHECK(!parse(unsigned_overflow_base20, base20_parser, u));
        CHECK(!parse(digit_overflow_base20,         base20_parser));
        CHECK(!parse(digit_overflow_base20,    base20_parser, u));
    }

    // arbitrary radix test (base 21)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 21, 1, -1>            base21_parser;

        CHECK(parse("23h00",                        base21_parser));
        REQUIRE(parse("23h00",                   base21_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("ijkl",                        base21_parser));
        CHECK(!parse("IJKL",                   base21_parser, u));

        CHECK(parse(max_unsigned_base21,            base21_parser));
        CHECK(parse(max_unsigned_base21,       base21_parser, u));

        CHECK(!parse(unsigned_overflow_base21,      base21_parser));
        CHECK(!parse(unsigned_overflow_base21, base21_parser, u));
        CHECK(!parse(digit_overflow_base21,         base21_parser));
        CHECK(!parse(digit_overflow_base21,    base21_parser, u));
    }

    // arbitrary radix test (base 22)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 22, 1, -1>            base22_parser;

        CHECK(parse("1hibg",                        base22_parser));
        REQUIRE(parse("1hibg",                   base22_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("jklm",                        base22_parser));
        CHECK(!parse("JKLM",                   base22_parser, u));

        CHECK(parse(max_unsigned_base22,            base22_parser));
        CHECK(parse(max_unsigned_base22,       base22_parser, u));

        CHECK(!parse(unsigned_overflow_base22,      base22_parser));
        CHECK(!parse(unsigned_overflow_base22, base22_parser, u));
        CHECK(!parse(digit_overflow_base22,         base22_parser));
        CHECK(!parse(digit_overflow_base22,    base22_parser, u));
    }

    // arbitrary radix test (base 23)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 23, 1, -1>            base23_parser;

        CHECK(parse("1bjm7",                        base23_parser));
        REQUIRE(parse("1bjm7",                   base23_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("klmn",                        base23_parser));
        CHECK(!parse("KLMN",                   base23_parser, u));

        CHECK(parse(max_unsigned_base23,            base23_parser));
        CHECK(parse(max_unsigned_base23,       base23_parser, u));

        CHECK(!parse(unsigned_overflow_base23,      base23_parser));
        CHECK(!parse(unsigned_overflow_base23, base23_parser, u));
        CHECK(!parse(digit_overflow_base23,         base23_parser));
        CHECK(!parse(digit_overflow_base23,    base23_parser, u));
    }

    // arbitrary radix test (base 24)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 24, 1, -1>            base24_parser;

        CHECK(parse("16gci",                        base24_parser));
        REQUIRE(parse("16gci",                   base24_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("lmno",                        base24_parser));
        CHECK(!parse("LMNO",                   base24_parser, u));

        CHECK(parse(max_unsigned_base24,            base24_parser));
        CHECK(parse(max_unsigned_base24,       base24_parser, u));

        CHECK(!parse(unsigned_overflow_base24,      base24_parser));
        CHECK(!parse(unsigned_overflow_base24, base24_parser, u));
        CHECK(!parse(digit_overflow_base24,         base24_parser));
        CHECK(!parse(digit_overflow_base24,    base24_parser, u));
    }

    // arbitrary radix test (base 25)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 25, 1, -1>            base25_parser;

        CHECK(parse("123jh",                        base25_parser));
        REQUIRE(parse("123jh",                   base25_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("mnop",                        base25_parser));
        CHECK(!parse("MNOP",                   base25_parser, u));

        CHECK(parse(max_unsigned_base25,            base25_parser));
        CHECK(parse(max_unsigned_base25,       base25_parser, u));

        CHECK(!parse(unsigned_overflow_base25,      base25_parser));
        CHECK(!parse(unsigned_overflow_base25, base25_parser, u));
        CHECK(!parse(digit_overflow_base25,         base25_parser));
        CHECK(!parse(digit_overflow_base25,    base25_parser, u));
    }

    // arbitrary radix test (base 26)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 26, 1, -1>            base26_parser;

        CHECK(parse("o3f0",                         base26_parser));
        REQUIRE(parse("o3f0",                    base26_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("nopq",                        base26_parser));
        CHECK(!parse("NOPQ",                   base26_parser, u));

        CHECK(parse(max_unsigned_base26,            base26_parser));
        CHECK(parse(max_unsigned_base26,       base26_parser, u));

        CHECK(!parse(unsigned_overflow_base26,      base26_parser));
        CHECK(!parse(unsigned_overflow_base26, base26_parser, u));
        CHECK(!parse(digit_overflow_base26,         base26_parser));
        CHECK(!parse(digit_overflow_base26,    base26_parser, u));
    }

    // arbitrary radix test (base 27)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 27, 1, -1>            base27_parser;

        CHECK(parse("lepi",                         base27_parser));
        REQUIRE(parse("lepi",                    base27_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("opqr",                        base27_parser));
        CHECK(!parse("OPQR",                   base27_parser, u));

        CHECK(parse(max_unsigned_base27,            base27_parser));
        CHECK(parse(max_unsigned_base27,       base27_parser, u));

        CHECK(!parse(unsigned_overflow_base27,      base27_parser));
        CHECK(!parse(unsigned_overflow_base27, base27_parser, u));
        CHECK(!parse(digit_overflow_base27,         base27_parser));
        CHECK(!parse(digit_overflow_base27,    base27_parser, u));
    }

    // arbitrary radix test (base 28)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 28, 1, -1>            base28_parser;

        CHECK(parse("j93e",                         base28_parser));
        REQUIRE(parse("j93e",                    base28_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("pqrs",                        base28_parser));
        CHECK(!parse("PQRS",                   base28_parser, u));

        CHECK(parse(max_unsigned_base28,            base28_parser));
        CHECK(parse(max_unsigned_base28,       base28_parser, u));

        CHECK(!parse(unsigned_overflow_base28,      base28_parser));
        CHECK(!parse(unsigned_overflow_base28, base28_parser, u));
        CHECK(!parse(digit_overflow_base28,         base28_parser));
        CHECK(!parse(digit_overflow_base28,    base28_parser, u));
    }

    // arbitrary radix test (base 29)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 29, 1, -1>            base29_parser;

        CHECK(parse("hbd1",                         base29_parser));
        REQUIRE(parse("hbd1",                    base29_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("qrst",                        base29_parser));
        CHECK(!parse("QRST",                   base29_parser, u));

        CHECK(parse(max_unsigned_base29,            base29_parser));
        CHECK(parse(max_unsigned_base29,       base29_parser, u));

        CHECK(!parse(unsigned_overflow_base29,      base29_parser));
        CHECK(!parse(unsigned_overflow_base29, base29_parser, u));
        CHECK(!parse(digit_overflow_base29,         base29_parser));
        CHECK(!parse(digit_overflow_base29,    base29_parser, u));
    }

    // arbitrary radix test (base 30)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 30, 1, -1>            base30_parser;

        CHECK(parse("flbc",                         base30_parser));
        REQUIRE(parse("flbc",                    base30_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("rstu",                        base30_parser));
        CHECK(!parse("RSTU",                   base30_parser, u));

        CHECK(parse(max_unsigned_base30,            base30_parser));
        CHECK(parse(max_unsigned_base30,       base30_parser, u));

        CHECK(!parse(unsigned_overflow_base30,      base30_parser));
        CHECK(!parse(unsigned_overflow_base30, base30_parser, u));
        CHECK(!parse(digit_overflow_base30,         base30_parser));
        CHECK(!parse(digit_overflow_base30,    base30_parser, u));
    }

    // arbitrary radix test (base 31)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 31, 1, -1>            base31_parser;

        CHECK(parse("e7e7",                         base31_parser));
        REQUIRE(parse("e7e7",                    base31_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("stuv",                        base31_parser));
        CHECK(!parse("STUV",                   base31_parser, u));

        CHECK(parse(max_unsigned_base31,            base31_parser));
        CHECK(parse(max_unsigned_base31,       base31_parser, u));

        CHECK(!parse(unsigned_overflow_base31,      base31_parser));
        CHECK(!parse(unsigned_overflow_base31, base31_parser, u));
        CHECK(!parse(digit_overflow_base31,         base31_parser));
        CHECK(!parse(digit_overflow_base31,    base31_parser, u));
    }
}

TEST_CASE("uint_radix_32-36")
{
    // arbitrary radix test (base 32)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 32, 1, -1>            base32_parser;

        CHECK(parse("cu9i",                         base32_parser));
        REQUIRE(parse("cu9i",                    base32_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("tuvw",                        base32_parser));
        CHECK(!parse("TUVW",                   base32_parser, u));

        CHECK(parse(max_unsigned_base32,            base32_parser));
        CHECK(parse(max_unsigned_base32,       base32_parser, u));

        CHECK(!parse(unsigned_overflow_base32,      base32_parser));
        CHECK(!parse(unsigned_overflow_base32, base32_parser, u));
        CHECK(!parse(digit_overflow_base32,         base32_parser));
        CHECK(!parse(digit_overflow_base32,    base32_parser, u));
    }

    // arbitrary radix test (base 33)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 33, 1, -1>            base33_parser;

        CHECK(parse("bqir",                         base33_parser));
        REQUIRE(parse("bqir",                    base33_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("uvwx",                        base33_parser));
        CHECK(!parse("UVWX",                   base33_parser, u));

        CHECK(parse(max_unsigned_base33,            base33_parser));
        CHECK(parse(max_unsigned_base33,       base33_parser, u));

        CHECK(!parse(unsigned_overflow_base33,      base33_parser));
        CHECK(!parse(unsigned_overflow_base33, base33_parser, u));
        CHECK(!parse(digit_overflow_base33,         base33_parser));
        CHECK(!parse(digit_overflow_base33,    base33_parser, u));
    }

    // arbitrary radix test (base 34)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 34, 1, -1>            base34_parser;

        CHECK(parse("aqxo",                         base34_parser));
        REQUIRE(parse("aqxo",                    base34_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("vwxy",                        base34_parser));
        CHECK(!parse("VWXY",                   base34_parser, u));

        CHECK(parse(max_unsigned_base34,            base34_parser));
        CHECK(parse(max_unsigned_base34,       base34_parser, u));

        CHECK(!parse(unsigned_overflow_base34,      base34_parser));
        CHECK(!parse(unsigned_overflow_base34, base34_parser, u));
        CHECK(!parse(digit_overflow_base34,         base34_parser));
        CHECK(!parse(digit_overflow_base34,    base34_parser, u));
    }

    // arbitrary radix test (base 35)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 35, 1, -1>            base35_parser;

        CHECK(parse("9vb7",                         base35_parser));
        REQUIRE(parse("9vb7",                    base35_parser, u));
        CHECK(424242 == u);

        CHECK(!parse("wxyz",                        base35_parser));
        CHECK(!parse("WXYZ",                   base35_parser, u));

        CHECK(parse(max_unsigned_base35,            base35_parser));
        CHECK(parse(max_unsigned_base35,       base35_parser, u));

        CHECK(!parse(unsigned_overflow_base35,      base35_parser));
        CHECK(!parse(unsigned_overflow_base35, base35_parser, u));
        CHECK(!parse(digit_overflow_base35,         base35_parser));
        CHECK(!parse(digit_overflow_base35,    base35_parser, u));
    }

    // arbitrary radix test (base 36)
    {
        unsigned int u = 0;
        uint_parser<unsigned int, 36, 1, -1>            base36_parser;

        CHECK(parse("93ci",                         base36_parser));
        REQUIRE(parse("93ci",                    base36_parser, u));
        CHECK(424242 == u);

        CHECK(parse(max_unsigned_base36,            base36_parser));
        CHECK(parse(max_unsigned_base36,       base36_parser, u));

        CHECK(!parse(unsigned_overflow_base36,      base36_parser));
        CHECK(!parse(unsigned_overflow_base36, base36_parser, u));
        CHECK(!parse(digit_overflow_base36,         base36_parser));
        CHECK(!parse(digit_overflow_base36,    base36_parser, u));
    }
}
