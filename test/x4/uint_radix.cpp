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

namespace
{
//
// BEWARE PLATFORM DEPENDENT!!! ***
// The following assumes 32 bit integers and 64 bit long longs.
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

} // anonymous

int main()
{
    using x4::uint_parser;

    // arbitrary radix test (base 3)
    {
        unsigned int u;
        uint_parser<unsigned int, 3, 1, -1>             base3_parser;

        BOOST_TEST(parse("210112221200",                 base3_parser));
        BOOST_TEST(parse("210112221200",            base3_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("1231",                        base3_parser));
        BOOST_TEST(!parse("1231",                   base3_parser, u));

        BOOST_TEST(parse(max_unsigned_base3,             base3_parser));
        BOOST_TEST(parse(max_unsigned_base3,        base3_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base3,       base3_parser));
        BOOST_TEST(!parse(unsigned_overflow_base3,  base3_parser, u));
        BOOST_TEST(!parse(digit_overflow_base3,          base3_parser));
        BOOST_TEST(!parse(digit_overflow_base3,     base3_parser, u));
    }

    // arbitrary radix test (base 4)
    {
        unsigned int u;

        uint_parser<unsigned int, 4, 1, -1>             base4_parser;

        BOOST_TEST(parse("1213210302",                   base4_parser));
        BOOST_TEST(parse("1213210302",              base4_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("1234",                        base4_parser));
        BOOST_TEST(!parse("1234",                   base4_parser, u));

        BOOST_TEST(parse(max_unsigned_base4,             base4_parser));
        BOOST_TEST(parse(max_unsigned_base4,        base4_parser, u));
        BOOST_TEST(!parse(digit_overflow_base4,          base4_parser));
        BOOST_TEST(!parse(digit_overflow_base4,     base4_parser, u));
    }

    // arbitrary radix test (base 5)
    {
        unsigned int u;

        uint_parser<unsigned int, 5, 1, -1>             base5_parser;

        BOOST_TEST(parse("102033432",                    base5_parser));
        BOOST_TEST(parse("102033432",               base5_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("2345",                        base5_parser));
        BOOST_TEST(!parse("2345",                   base5_parser, u));

        BOOST_TEST(parse(max_unsigned_base5,             base5_parser));
        BOOST_TEST(parse(max_unsigned_base5,        base5_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base5,       base5_parser));
        BOOST_TEST(!parse(unsigned_overflow_base5,  base5_parser, u));
        BOOST_TEST(!parse(digit_overflow_base5,          base5_parser));
        BOOST_TEST(!parse(digit_overflow_base5,     base5_parser, u));
    }

    // arbitrary radix test (base 6)
    {
        unsigned int u;

        uint_parser<unsigned int, 6, 1, -1>             base6_parser;

        BOOST_TEST(parse("13032030",                     base6_parser));
        BOOST_TEST(parse("13032030",                base6_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("3456",                        base6_parser));
        BOOST_TEST(!parse("3456",                   base6_parser, u));

        BOOST_TEST(parse(max_unsigned_base6,             base6_parser));
        BOOST_TEST(parse(max_unsigned_base6,        base6_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base6,       base6_parser));
        BOOST_TEST(!parse(unsigned_overflow_base6,  base6_parser, u));
        BOOST_TEST(!parse(digit_overflow_base6,          base6_parser));
        BOOST_TEST(!parse(digit_overflow_base6,     base6_parser, u));
    }

    // arbitrary radix test (base 7)
    {
        unsigned int u;

        uint_parser<unsigned int, 7, 1, -1>             base7_parser;

        BOOST_TEST(parse("3414600",                      base7_parser));
        BOOST_TEST(parse("3414600",                 base7_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("4567",                        base7_parser));
        BOOST_TEST(!parse("4567",                   base7_parser, u));

        BOOST_TEST(parse(max_unsigned_base7,             base7_parser));
        BOOST_TEST(parse(max_unsigned_base7,        base7_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base7,       base7_parser));
        BOOST_TEST(!parse(unsigned_overflow_base7,  base7_parser, u));
        BOOST_TEST(!parse(digit_overflow_base7,          base7_parser));
        BOOST_TEST(!parse(digit_overflow_base7,     base7_parser, u));
    }

    // arbitrary radix test (base 9)
    {
        unsigned int u;

        uint_parser<unsigned int, 9, 1, -1>             base9_parser;

        BOOST_TEST(parse("715850",                       base9_parser));
        BOOST_TEST(parse("715850",                  base9_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("6789",                        base9_parser));
        BOOST_TEST(!parse("6789",                   base9_parser, u));

        BOOST_TEST(parse(max_unsigned_base9,             base9_parser));
        BOOST_TEST(parse(max_unsigned_base9,        base9_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base9,       base9_parser));
        BOOST_TEST(!parse(unsigned_overflow_base9,  base9_parser, u));
        BOOST_TEST(!parse(digit_overflow_base9,          base9_parser));
        BOOST_TEST(!parse(digit_overflow_base9,     base9_parser, u));
    }

    // arbitrary radix test (base 11)
    {
        unsigned int u;

        uint_parser<unsigned int, 11, 1, -1>            base11_parser;

        BOOST_TEST(parse("26a815",                       base11_parser));
        BOOST_TEST(parse("26a815",                  base11_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("90ab",                        base11_parser));
        BOOST_TEST(!parse("90AB",                   base11_parser, u));

        BOOST_TEST(parse(max_unsigned_base11,            base11_parser));
        BOOST_TEST(parse(max_unsigned_base11,       base11_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base11,      base11_parser));
        BOOST_TEST(!parse(unsigned_overflow_base11, base11_parser, u));
        BOOST_TEST(!parse(digit_overflow_base11,         base11_parser));
        BOOST_TEST(!parse(digit_overflow_base11,    base11_parser, u));
    }

    // arbitrary radix test (base 12)
    {
        unsigned int u;
        uint_parser<unsigned int, 12, 1, -1>            base12_parser;

        BOOST_TEST(parse("185616",                       base12_parser));
        BOOST_TEST(parse("185616",                  base12_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("9abc",                        base12_parser));
        BOOST_TEST(!parse("9ABC",                   base12_parser, u));

        BOOST_TEST(parse(max_unsigned_base12,            base12_parser));
        BOOST_TEST(parse(max_unsigned_base12,       base12_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base12,      base12_parser));
        BOOST_TEST(!parse(unsigned_overflow_base12, base12_parser, u));
        BOOST_TEST(!parse(digit_overflow_base12,         base12_parser));
        BOOST_TEST(!parse(digit_overflow_base12,    base12_parser, u));
    }

    // arbitrary radix test (base 13)
    {
        unsigned int u;
        uint_parser<unsigned int, 13, 1, -1>            base13_parser;

        BOOST_TEST(parse("11b140",                       base13_parser));
        BOOST_TEST(parse("11b140",                  base13_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("abcd",                        base13_parser));
        BOOST_TEST(!parse("ABCD",                   base13_parser, u));

        BOOST_TEST(parse(max_unsigned_base13,            base13_parser));
        BOOST_TEST(parse(max_unsigned_base13,       base13_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base13,      base13_parser));
        BOOST_TEST(!parse(unsigned_overflow_base13, base13_parser, u));
        BOOST_TEST(!parse(digit_overflow_base13,         base13_parser));
        BOOST_TEST(!parse(digit_overflow_base13,    base13_parser, u));
    }

    // arbitrary radix test (base 14)
    {
        unsigned int u;
        uint_parser<unsigned int, 14, 1, -1>            base14_parser;

        BOOST_TEST(parse("b0870",                        base14_parser));
        BOOST_TEST(parse("b0870",                   base14_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("bcde",                        base14_parser));
        BOOST_TEST(!parse("BCDE",                   base14_parser, u));

        BOOST_TEST(parse(max_unsigned_base14,            base14_parser));
        BOOST_TEST(parse(max_unsigned_base14,       base14_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base14,      base14_parser));
        BOOST_TEST(!parse(unsigned_overflow_base14, base14_parser, u));
        BOOST_TEST(!parse(digit_overflow_base14,         base14_parser));
        BOOST_TEST(!parse(digit_overflow_base14,    base14_parser, u));
    }

    // arbitrary radix test (base 15)
    {
        unsigned int u;
        uint_parser<unsigned int, 15, 1, -1>            base15_parser;

        BOOST_TEST(parse("85a7c",                        base15_parser));
        BOOST_TEST(parse("85a7c",                   base15_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("cdef",                        base15_parser));
        BOOST_TEST(!parse("CDEF",                   base15_parser, u));

        BOOST_TEST(parse(max_unsigned_base15,            base15_parser));
        BOOST_TEST(parse(max_unsigned_base15,       base15_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base15,      base15_parser));
        BOOST_TEST(!parse(unsigned_overflow_base15, base15_parser, u));
        BOOST_TEST(!parse(digit_overflow_base15,         base15_parser));
        BOOST_TEST(!parse(digit_overflow_base15,    base15_parser, u));
    }

    // arbitrary radix test (base 17)
    {
        unsigned int u;
        uint_parser<unsigned int, 17, 1, -1>            base17_parser;

        BOOST_TEST(parse("515g7",                        base17_parser));
        BOOST_TEST(parse("515g7",                   base17_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("efgh",                        base17_parser));
        BOOST_TEST(!parse("EFGH",                   base17_parser, u));

        BOOST_TEST(parse(max_unsigned_base17,            base17_parser));
        BOOST_TEST(parse(max_unsigned_base17,       base17_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base17,      base17_parser));
        BOOST_TEST(!parse(unsigned_overflow_base17, base17_parser, u));
        BOOST_TEST(!parse(digit_overflow_base17,         base17_parser));
        BOOST_TEST(!parse(digit_overflow_base17,    base17_parser, u));
    }

    // arbitrary radix test (base 18)
    {
        unsigned int u;
        uint_parser<unsigned int, 18, 1, -1>            base18_parser;

        BOOST_TEST(parse("40d70",                        base18_parser));
        BOOST_TEST(parse("40d70",                   base18_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("fghi",                        base18_parser));
        BOOST_TEST(!parse("FGHI",                   base18_parser, u));

        BOOST_TEST(parse(max_unsigned_base18,            base18_parser));
        BOOST_TEST(parse(max_unsigned_base18,       base18_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base18,      base18_parser));
        BOOST_TEST(!parse(unsigned_overflow_base18, base18_parser, u));
        BOOST_TEST(!parse(digit_overflow_base18,         base18_parser));
        BOOST_TEST(!parse(digit_overflow_base18,    base18_parser, u));
    }

    // arbitrary radix test (base 19)
    {
        unsigned int u;
        uint_parser<unsigned int, 19, 1, -1>            base19_parser;

        BOOST_TEST(parse("34g3a",                        base19_parser));
        BOOST_TEST(parse("34g3a",                   base19_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("ghij",                        base19_parser));
        BOOST_TEST(!parse("GHIJ",                   base19_parser, u));

        BOOST_TEST(parse(max_unsigned_base19,            base19_parser));
        BOOST_TEST(parse(max_unsigned_base19,       base19_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base19,      base19_parser));
        BOOST_TEST(!parse(unsigned_overflow_base19, base19_parser, u));
        BOOST_TEST(!parse(digit_overflow_base19,         base19_parser));
        BOOST_TEST(!parse(digit_overflow_base19,    base19_parser, u));
    }

    // arbitrary radix test (base 20)
    {
        unsigned int u;
        uint_parser<unsigned int, 20, 1, -1>            base20_parser;

        BOOST_TEST(parse("2d0c2",                        base20_parser));
        BOOST_TEST(parse("2d0c2",                   base20_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("hijk",                        base20_parser));
        BOOST_TEST(!parse("HIJK",                   base20_parser, u));

        BOOST_TEST(parse(max_unsigned_base20,            base20_parser));
        BOOST_TEST(parse(max_unsigned_base20,       base20_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base20,      base20_parser));
        BOOST_TEST(!parse(unsigned_overflow_base20, base20_parser, u));
        BOOST_TEST(!parse(digit_overflow_base20,         base20_parser));
        BOOST_TEST(!parse(digit_overflow_base20,    base20_parser, u));
    }

    // arbitrary radix test (base 21)
    {
        unsigned int u;
        uint_parser<unsigned int, 21, 1, -1>            base21_parser;

        BOOST_TEST(parse("23h00",                        base21_parser));
        BOOST_TEST(parse("23h00",                   base21_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("ijkl",                        base21_parser));
        BOOST_TEST(!parse("IJKL",                   base21_parser, u));

        BOOST_TEST(parse(max_unsigned_base21,            base21_parser));
        BOOST_TEST(parse(max_unsigned_base21,       base21_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base21,      base21_parser));
        BOOST_TEST(!parse(unsigned_overflow_base21, base21_parser, u));
        BOOST_TEST(!parse(digit_overflow_base21,         base21_parser));
        BOOST_TEST(!parse(digit_overflow_base21,    base21_parser, u));
    }

    // arbitrary radix test (base 22)
    {
        unsigned int u;
        uint_parser<unsigned int, 22, 1, -1>            base22_parser;

        BOOST_TEST(parse("1hibg",                        base22_parser));
        BOOST_TEST(parse("1hibg",                   base22_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("jklm",                        base22_parser));
        BOOST_TEST(!parse("JKLM",                   base22_parser, u));

        BOOST_TEST(parse(max_unsigned_base22,            base22_parser));
        BOOST_TEST(parse(max_unsigned_base22,       base22_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base22,      base22_parser));
        BOOST_TEST(!parse(unsigned_overflow_base22, base22_parser, u));
        BOOST_TEST(!parse(digit_overflow_base22,         base22_parser));
        BOOST_TEST(!parse(digit_overflow_base22,    base22_parser, u));
    }

    // arbitrary radix test (base 23)
    {
        unsigned int u;
        uint_parser<unsigned int, 23, 1, -1>            base23_parser;

        BOOST_TEST(parse("1bjm7",                        base23_parser));
        BOOST_TEST(parse("1bjm7",                   base23_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("klmn",                        base23_parser));
        BOOST_TEST(!parse("KLMN",                   base23_parser, u));

        BOOST_TEST(parse(max_unsigned_base23,            base23_parser));
        BOOST_TEST(parse(max_unsigned_base23,       base23_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base23,      base23_parser));
        BOOST_TEST(!parse(unsigned_overflow_base23, base23_parser, u));
        BOOST_TEST(!parse(digit_overflow_base23,         base23_parser));
        BOOST_TEST(!parse(digit_overflow_base23,    base23_parser, u));
    }

    // arbitrary radix test (base 24)
    {
        unsigned int u;
        uint_parser<unsigned int, 24, 1, -1>            base24_parser;

        BOOST_TEST(parse("16gci",                        base24_parser));
        BOOST_TEST(parse("16gci",                   base24_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("lmno",                        base24_parser));
        BOOST_TEST(!parse("LMNO",                   base24_parser, u));

        BOOST_TEST(parse(max_unsigned_base24,            base24_parser));
        BOOST_TEST(parse(max_unsigned_base24,       base24_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base24,      base24_parser));
        BOOST_TEST(!parse(unsigned_overflow_base24, base24_parser, u));
        BOOST_TEST(!parse(digit_overflow_base24,         base24_parser));
        BOOST_TEST(!parse(digit_overflow_base24,    base24_parser, u));
    }

    // arbitrary radix test (base 25)
    {
        unsigned int u;
        uint_parser<unsigned int, 25, 1, -1>            base25_parser;

        BOOST_TEST(parse("123jh",                        base25_parser));
        BOOST_TEST(parse("123jh",                   base25_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("mnop",                        base25_parser));
        BOOST_TEST(!parse("MNOP",                   base25_parser, u));

        BOOST_TEST(parse(max_unsigned_base25,            base25_parser));
        BOOST_TEST(parse(max_unsigned_base25,       base25_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base25,      base25_parser));
        BOOST_TEST(!parse(unsigned_overflow_base25, base25_parser, u));
        BOOST_TEST(!parse(digit_overflow_base25,         base25_parser));
        BOOST_TEST(!parse(digit_overflow_base25,    base25_parser, u));
    }

    // arbitrary radix test (base 26)
    {
        unsigned int u;
        uint_parser<unsigned int, 26, 1, -1>            base26_parser;

        BOOST_TEST(parse("o3f0",                         base26_parser));
        BOOST_TEST(parse("o3f0",                    base26_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("nopq",                        base26_parser));
        BOOST_TEST(!parse("NOPQ",                   base26_parser, u));

        BOOST_TEST(parse(max_unsigned_base26,            base26_parser));
        BOOST_TEST(parse(max_unsigned_base26,       base26_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base26,      base26_parser));
        BOOST_TEST(!parse(unsigned_overflow_base26, base26_parser, u));
        BOOST_TEST(!parse(digit_overflow_base26,         base26_parser));
        BOOST_TEST(!parse(digit_overflow_base26,    base26_parser, u));
    }

    // arbitrary radix test (base 27)
    {
        unsigned int u;
        uint_parser<unsigned int, 27, 1, -1>            base27_parser;

        BOOST_TEST(parse("lepi",                         base27_parser));
        BOOST_TEST(parse("lepi",                    base27_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("opqr",                        base27_parser));
        BOOST_TEST(!parse("OPQR",                   base27_parser, u));

        BOOST_TEST(parse(max_unsigned_base27,            base27_parser));
        BOOST_TEST(parse(max_unsigned_base27,       base27_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base27,      base27_parser));
        BOOST_TEST(!parse(unsigned_overflow_base27, base27_parser, u));
        BOOST_TEST(!parse(digit_overflow_base27,         base27_parser));
        BOOST_TEST(!parse(digit_overflow_base27,    base27_parser, u));
    }

    // arbitrary radix test (base 28)
    {
        unsigned int u;
        uint_parser<unsigned int, 28, 1, -1>            base28_parser;

        BOOST_TEST(parse("j93e",                         base28_parser));
        BOOST_TEST(parse("j93e",                    base28_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("pqrs",                        base28_parser));
        BOOST_TEST(!parse("PQRS",                   base28_parser, u));

        BOOST_TEST(parse(max_unsigned_base28,            base28_parser));
        BOOST_TEST(parse(max_unsigned_base28,       base28_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base28,      base28_parser));
        BOOST_TEST(!parse(unsigned_overflow_base28, base28_parser, u));
        BOOST_TEST(!parse(digit_overflow_base28,         base28_parser));
        BOOST_TEST(!parse(digit_overflow_base28,    base28_parser, u));
    }

    // arbitrary radix test (base 29)
    {
        unsigned int u;
        uint_parser<unsigned int, 29, 1, -1>            base29_parser;

        BOOST_TEST(parse("hbd1",                         base29_parser));
        BOOST_TEST(parse("hbd1",                    base29_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("qrst",                        base29_parser));
        BOOST_TEST(!parse("QRST",                   base29_parser, u));

        BOOST_TEST(parse(max_unsigned_base29,            base29_parser));
        BOOST_TEST(parse(max_unsigned_base29,       base29_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base29,      base29_parser));
        BOOST_TEST(!parse(unsigned_overflow_base29, base29_parser, u));
        BOOST_TEST(!parse(digit_overflow_base29,         base29_parser));
        BOOST_TEST(!parse(digit_overflow_base29,    base29_parser, u));
    }

    // arbitrary radix test (base 30)
    {
        unsigned int u;
        uint_parser<unsigned int, 30, 1, -1>            base30_parser;

        BOOST_TEST(parse("flbc",                         base30_parser));
        BOOST_TEST(parse("flbc",                    base30_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("rstu",                        base30_parser));
        BOOST_TEST(!parse("RSTU",                   base30_parser, u));

        BOOST_TEST(parse(max_unsigned_base30,            base30_parser));
        BOOST_TEST(parse(max_unsigned_base30,       base30_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base30,      base30_parser));
        BOOST_TEST(!parse(unsigned_overflow_base30, base30_parser, u));
        BOOST_TEST(!parse(digit_overflow_base30,         base30_parser));
        BOOST_TEST(!parse(digit_overflow_base30,    base30_parser, u));
    }

    // arbitrary radix test (base 31)
    {
        unsigned int u;
        uint_parser<unsigned int, 31, 1, -1>            base31_parser;

        BOOST_TEST(parse("e7e7",                         base31_parser));
        BOOST_TEST(parse("e7e7",                    base31_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("stuv",                        base31_parser));
        BOOST_TEST(!parse("STUV",                   base31_parser, u));

        BOOST_TEST(parse(max_unsigned_base31,            base31_parser));
        BOOST_TEST(parse(max_unsigned_base31,       base31_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base31,      base31_parser));
        BOOST_TEST(!parse(unsigned_overflow_base31, base31_parser, u));
        BOOST_TEST(!parse(digit_overflow_base31,         base31_parser));
        BOOST_TEST(!parse(digit_overflow_base31,    base31_parser, u));
    }

    // arbitrary radix test (base 32)
    {
        unsigned int u;
        uint_parser<unsigned int, 32, 1, -1>            base32_parser;

        BOOST_TEST(parse("cu9i",                         base32_parser));
        BOOST_TEST(parse("cu9i",                    base32_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("tuvw",                        base32_parser));
        BOOST_TEST(!parse("TUVW",                   base32_parser, u));

        BOOST_TEST(parse(max_unsigned_base32,            base32_parser));
        BOOST_TEST(parse(max_unsigned_base32,       base32_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base32,      base32_parser));
        BOOST_TEST(!parse(unsigned_overflow_base32, base32_parser, u));
        BOOST_TEST(!parse(digit_overflow_base32,         base32_parser));
        BOOST_TEST(!parse(digit_overflow_base32,    base32_parser, u));
    }

    // arbitrary radix test (base 33)
    {
        unsigned int u;
        uint_parser<unsigned int, 33, 1, -1>            base33_parser;

        BOOST_TEST(parse("bqir",                         base33_parser));
        BOOST_TEST(parse("bqir",                    base33_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("uvwx",                        base33_parser));
        BOOST_TEST(!parse("UVWX",                   base33_parser, u));

        BOOST_TEST(parse(max_unsigned_base33,            base33_parser));
        BOOST_TEST(parse(max_unsigned_base33,       base33_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base33,      base33_parser));
        BOOST_TEST(!parse(unsigned_overflow_base33, base33_parser, u));
        BOOST_TEST(!parse(digit_overflow_base33,         base33_parser));
        BOOST_TEST(!parse(digit_overflow_base33,    base33_parser, u));
    }

    // arbitrary radix test (base 34)
    {
        unsigned int u;
        uint_parser<unsigned int, 34, 1, -1>            base34_parser;

        BOOST_TEST(parse("aqxo",                         base34_parser));
        BOOST_TEST(parse("aqxo",                    base34_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("vwxy",                        base34_parser));
        BOOST_TEST(!parse("VWXY",                   base34_parser, u));

        BOOST_TEST(parse(max_unsigned_base34,            base34_parser));
        BOOST_TEST(parse(max_unsigned_base34,       base34_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base34,      base34_parser));
        BOOST_TEST(!parse(unsigned_overflow_base34, base34_parser, u));
        BOOST_TEST(!parse(digit_overflow_base34,         base34_parser));
        BOOST_TEST(!parse(digit_overflow_base34,    base34_parser, u));
    }

    // arbitrary radix test (base 35)
    {
        unsigned int u;
        uint_parser<unsigned int, 35, 1, -1>            base35_parser;

        BOOST_TEST(parse("9vb7",                         base35_parser));
        BOOST_TEST(parse("9vb7",                    base35_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!parse("wxyz",                        base35_parser));
        BOOST_TEST(!parse("WXYZ",                   base35_parser, u));

        BOOST_TEST(parse(max_unsigned_base35,            base35_parser));
        BOOST_TEST(parse(max_unsigned_base35,       base35_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base35,      base35_parser));
        BOOST_TEST(!parse(unsigned_overflow_base35, base35_parser, u));
        BOOST_TEST(!parse(digit_overflow_base35,         base35_parser));
        BOOST_TEST(!parse(digit_overflow_base35,    base35_parser, u));
    }

    // arbitrary radix test (base 36)
    {
        unsigned int u;
        uint_parser<unsigned int, 36, 1, -1>            base36_parser;

        BOOST_TEST(parse("93ci",                         base36_parser));
        BOOST_TEST(parse("93ci",                    base36_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(parse(max_unsigned_base36,            base36_parser));
        BOOST_TEST(parse(max_unsigned_base36,       base36_parser, u));

        BOOST_TEST(!parse(unsigned_overflow_base36,      base36_parser));
        BOOST_TEST(!parse(unsigned_overflow_base36, base36_parser, u));
        BOOST_TEST(!parse(digit_overflow_base36,         base36_parser));
        BOOST_TEST(!parse(digit_overflow_base36,    base36_parser, u));
    }

    return boost::report_errors();
}
