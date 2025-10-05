#ifndef BOOST_SPIRIT_X4_BINARY_HPP
#define BOOST_SPIRIT_X4_BINARY_HPP

/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/move_to.hpp>

#include <boost/endian/conversion.hpp>
#include <boost/endian/arithmetic.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

#include <cstdint>
#include <climits>

namespace boost::spirit::x4 {

template<class T, boost::endian::order endian, std::size_t bits>
struct binary_lit_parser : parser<binary_lit_parser<T, endian, bits>>
{
    using attribute_type = unused_type;

    static constexpr bool has_attribute = false;

    template<class U>
        requires
            (!std::is_same_v<std::remove_cvref_t<U>, binary_lit_parser>) &&
            std::is_constructible_v<T, U>
    constexpr explicit binary_lit_parser(U&& n)
        noexcept(std::is_nothrow_constructible_v<T, U>)
        : n_(std::forward<U>(n))
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr_param) const
        // TODO: noexcept
    {
        x4::skip_over(first, last, ctx);

        unsigned char const* bytes = n_.data();

        It it = first;
        for (unsigned int i = 0; i < sizeof(n_); ++i) {
            if (it == last || *bytes++ != static_cast<unsigned char>(*it++)) {
                return false;
            }
        }

        first = it;
        x4::move_to(n_, attr_param);
        return true;
    }

private:
    boost::endian::endian_arithmetic<endian, T, bits> n_;
};

template<class T, boost::endian::order endian, std::size_t bits>
struct any_binary_parser : parser<any_binary_parser<T, endian, bits>>
{
    static_assert(std::is_trivially_copyable_v<T>);

    using attribute_type = T;

    static constexpr bool has_attribute = !std::is_same_v<unused_type, T>;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr_param) const
        // TODO: noexcept
    {
        x4::skip_over(first, last, ctx);

        // Properly align the buffer for performance reasons
        alignas(T) unsigned char buf[sizeof(T)];
        unsigned char* bytes = buf;

        It it = first;
        for (unsigned int i = 0; i < sizeof(T); ++i) {
            if (it == last) return false;
            *bytes++ = *it++;
        }

        first = it;

        static_assert(bits % CHAR_BIT == 0, "Boost.Endian supports only multiples of CHAR_BIT");
        x4::move_to(
            endian::endian_load<T, bits / CHAR_BIT, endian>(buf),
            attr_param
        );
        return true;
    }

    [[nodiscard]] static constexpr binary_lit_parser<std::remove_cvref_t<T>, endian, bits>
    operator()(T&& n)
        noexcept(std::is_nothrow_constructible_v<binary_lit_parser<std::remove_cvref_t<T>, endian, bits>, T>)
    {
        return binary_lit_parser<std::remove_cvref_t<T>, endian, bits>{std::forward<T>(n)};
    }
};


#define BOOST_SPIRIT_X4_BINARY_PARSER(name, endiantype, attrtype, bits) \
    namespace parsers { \
    inline constexpr any_binary_parser<attrtype, boost::endian::order::endiantype, bits> name{}; \
    } /* parsers */ \
    using parsers::name;

BOOST_SPIRIT_X4_BINARY_PARSER(byte_, native, uint_least8_t, 8)
BOOST_SPIRIT_X4_BINARY_PARSER(word, native, uint_least16_t, 16)
BOOST_SPIRIT_X4_BINARY_PARSER(big_word, big, uint_least16_t, 16)
BOOST_SPIRIT_X4_BINARY_PARSER(little_word, little, uint_least16_t, 16)
BOOST_SPIRIT_X4_BINARY_PARSER(dword, native, uint_least32_t, 32)
BOOST_SPIRIT_X4_BINARY_PARSER(big_dword, big, uint_least32_t, 32)
BOOST_SPIRIT_X4_BINARY_PARSER(little_dword, little, uint_least32_t, 32)

#ifdef BOOST_HAS_LONG_LONG
BOOST_SPIRIT_X4_BINARY_PARSER(qword, native, uint_least64_t, 64)
BOOST_SPIRIT_X4_BINARY_PARSER(big_qword, big, uint_least64_t, 64)
BOOST_SPIRIT_X4_BINARY_PARSER(little_qword, little, uint_least64_t, 64)
#endif

BOOST_SPIRIT_X4_BINARY_PARSER(bin_float, native, float, sizeof(float) * CHAR_BIT)
BOOST_SPIRIT_X4_BINARY_PARSER(big_bin_float, big, float, sizeof(float) * CHAR_BIT)
BOOST_SPIRIT_X4_BINARY_PARSER(little_bin_float, little, float, sizeof(float) * CHAR_BIT)
BOOST_SPIRIT_X4_BINARY_PARSER(bin_double, native, double, sizeof(double) * CHAR_BIT)
BOOST_SPIRIT_X4_BINARY_PARSER(big_bin_double, big, double, sizeof(double) * CHAR_BIT)
BOOST_SPIRIT_X4_BINARY_PARSER(little_bin_double, little, double, sizeof(double) * CHAR_BIT)

#undef BOOST_SPIRIT_X4_BINARY_PARSER


template<class T, std::size_t bits>
struct get_info<any_binary_parser<T, endian::order::little, bits>>
{
    using result_type = std::string;

    [[nodiscard]] constexpr std::string
    operator()(any_binary_parser<T, endian::order::little, bits> const&) const
    {
        return "little-endian binary";
    }
};

template<class T, std::size_t bits>
struct get_info<any_binary_parser<T, endian::order::big, bits>>
{
    using result_type = std::string;

    [[nodiscard]] constexpr std::string
    operator()(any_binary_parser<T, endian::order::big, bits> const&) const
    {
        return "big-endian binary";
    }
};

template<class T, std::size_t bits>
struct get_info<binary_lit_parser<T, endian::order::little, bits>>
{
    using result_type = std::string;

    [[nodiscard]] constexpr std::string
    operator()(binary_lit_parser<T, endian::order::little, bits> const&) const
    {
        return "little-endian binary";
    }
};

template<class T, std::size_t bits>
struct get_info<binary_lit_parser<T, endian::order::big, bits>>
{
    using result_type = std::string;

    [[nodiscard]] constexpr
    std::string operator()(binary_lit_parser<T, endian::order::big, bits> const&) const
    {
        return "big-endian binary";
    }
};

} // boost::spirit::x4

#endif
