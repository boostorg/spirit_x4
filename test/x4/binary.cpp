/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/binary.hpp>

#include <bit>

#include <cstdint>

namespace {

template <std::size_t N, typename... Args>
constexpr auto binary_test(char const (&binstr)[N], Args&&... args)
{
    return spirit_test::parse(std::string_view{binstr, N - 1}, std::forward<Args>(args)...);
}

} // anonymous

int main()
{
    using x4::byte_;
    using x4::word;
    using x4::dword;
    using x4::big_word;
    using x4::big_dword;
    using x4::little_word;
    using x4::little_dword;

    using x4::qword;
    using x4::big_qword;
    using x4::little_qword;

    using x4::bin_float;
    using x4::big_bin_float;
    using x4::little_bin_float;
    using x4::bin_double;
    using x4::big_bin_double;
    using x4::little_bin_double;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(byte_);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(word);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(dword);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_word);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_dword);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_word);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_dword);

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(qword);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_qword);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_qword);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(bin_float);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_bin_float);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_bin_float);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(bin_double);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_bin_double);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_bin_double);

// TODO: boost::endian::endian_arithmetic value constructor is not constexpr
#if 0
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(byte_(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(word(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(dword(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_word(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_dword(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_word(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_dword(1));

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(qword(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_qword(1));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_qword(1));

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(bin_float(1.0f));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_bin_float(1.0f));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_bin_float(1.0f));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(bin_double(1.0));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(big_bin_double(1.0));
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(little_bin_double(1.0));
#endif

    if constexpr (std::endian::native == std::endian::little)
    {
        {
            std::uint8_t uc = 0;
            BOOST_TEST(parse("\x01", byte_, uc) && uc == 0x01);
        }
        {
            std::uint16_t us = 0;
            BOOST_TEST(parse("\x01\x02", word, us) && us == 0x0201);
        }
        {
            std::uint32_t ui = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04", dword, ui) && ui == 0x04030201);
        }
        {
            std::uint64_t ul = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", qword, ul) && ul == 0x0807060504030201LL);
        }
        {
            float f = 0;
            BOOST_TEST(binary_test("\x00\x00\x80\x3f", bin_float, f) && f == 1.0f);
        }
        {
            double d = 0;
            BOOST_TEST(binary_test("\x00\x00\x00\x00\x00\x00\xf0\x3f", bin_double, d) && d == 1.0);
        }
    }
    else // big endian
    {
        {
            std::uint8_t uc = 0;
            BOOST_TEST(parse("\x01", byte_, uc) && uc == 0x01);
        }
        {
            std::uint16_t us = 0;
            BOOST_TEST(parse("\x01\x02", word, us) && us ==  0x0102);
        }
        {
            std::uint32_t ui = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04", dword, ui) && ui == 0x01020304);
        }
        {
            std::uint64_t ul = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", qword, ul) && ul == 0x0102030405060708LL);
        }
        {
            float f = 0;
            BOOST_TEST(binary_test("\x3f\x80\x00\x00", bin_float, f) && f == 1.0f);
        }
        {
            double d = 0;
            BOOST_TEST(binary_test("\x3f\xf0\x00\x00\x00\x00\x00\x00", bin_double, d) && d == 1.0);
        }
    }

    if constexpr (std::endian::native == std::endian::little)
    {
        BOOST_TEST(parse("\x01", byte_(0x01)));
        BOOST_TEST(parse("\x01\x02", word(0x0201)));
        BOOST_TEST(parse("\x01\x02\x03\x04", dword(0x04030201)));

        BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", qword(0x0807060504030201LL)));

        BOOST_TEST(binary_test("\x00\x00\x80\x3f", bin_float(1.0f)));
        BOOST_TEST(binary_test("\x00\x00\x00\x00\x00\x00\xf0\x3f", bin_double(1.0)));
    }
    else // big endian
    {
        BOOST_TEST(parse("\x01", byte_(0x01)));
        BOOST_TEST(parse("\x01\x02", word(0x0102)));
        BOOST_TEST(parse("\x01\x02\x03\x04", dword(0x01020304)));

        BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", qword(0x0102030405060708LL)));

        BOOST_TEST(binary_test("\x3f\x80\x00\x00", bin_float(1.0f)));
        BOOST_TEST(binary_test("\x3f\xf0\x00\x00\x00\x00\x00\x00", bin_double(1.0)));
    }

    {
        // big endian binaries
        {
            std::uint16_t us = 0;
            BOOST_TEST(parse("\x01\x02", big_word, us) && us == 0x0102);
        }
        {
            std::uint32_t ui = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04", big_dword, ui) && ui == 0x01020304);
        }
        {
            std::uint64_t ul = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", big_qword, ul) && ul == 0x0102030405060708LL);
        }
        {
            float f = 0;
            BOOST_TEST(binary_test("\x3f\x80\x00\x00", big_bin_float, f) && f == 1.0f);
        }
        {
            double d = 0;
            BOOST_TEST(binary_test("\x3f\xf0\x00\x00\x00\x00\x00\x00", big_bin_double, d) && d == 1.0);
        }
    }

    {
        BOOST_TEST(parse("\x01\x02", big_word(0x0102)));
        BOOST_TEST(parse("\x01\x02\x03\x04", big_dword(0x01020304)));

        BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", big_qword(0x0102030405060708LL)));

        BOOST_TEST(binary_test("\x3f\x80\x00\x00", big_bin_float(1.0f)));
        BOOST_TEST(binary_test("\x3f\xf0\x00\x00\x00\x00\x00\x00", big_bin_double(1.0)));
    }

    {
        // little endian binaries
        {
            std::uint16_t us = 0;
            BOOST_TEST(parse("\x01\x02", little_word, us) && us == 0x0201);
        }
        {
            std::uint32_t ui = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04", little_dword, ui) && ui == 0x04030201);
        }

        {
            std::uint64_t ul = 0;
            BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", little_qword, ul) && ul == 0x0807060504030201LL);
        }

        {
            float f = 0;
            BOOST_TEST(binary_test("\x00\x00\x80\x3f", little_bin_float, f) && f == 1.0f);
        }
        {
            double d = 0;
            BOOST_TEST(binary_test("\x00\x00\x00\x00\x00\x00\xf0\x3f", little_bin_double, d) && d == 1.0);
        }
    }

    {
        BOOST_TEST(parse("\x01\x02", little_word(0x0201)));
        BOOST_TEST(parse("\x01\x02\x03\x04", little_dword(0x04030201)));

        BOOST_TEST(parse("\x01\x02\x03\x04\x05\x06\x07\x08", little_qword(0x0807060504030201LL)));

        BOOST_TEST(binary_test("\x00\x00\x80\x3f", little_bin_float(1.0f)));
        BOOST_TEST(binary_test("\x00\x00\x00\x00\x00\x00\xf0\x3f", little_bin_double(1.0)));
    }

    return boost::report_errors();
}
