#ifndef BOOST_SPIRIT_X4_CHAR_DETAIL_BASIC_CHSET_HPP
#define BOOST_SPIRIT_X4_CHAR_DETAIL_BASIC_CHSET_HPP

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2009 Daniel Nuffer
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/char/detail/char_range_run.hpp>

#include <bitset>
#include <climits>

namespace boost::spirit::x4::detail {

// basic character set implementation using `char_range_run`
template<class CharT>
struct basic_chset
{
    [[nodiscard]] constexpr bool
    test(CharT v) const noexcept
    {
        return rr.test(v);
    }

    constexpr void
    set(CharT from, CharT to)
    {
        rr.set(char_range<CharT>(from, to));
    }

    constexpr void
    set(CharT c)
    {
        rr.set(char_range<CharT>(c, c));
    }

    constexpr void
    clear(CharT from, CharT to)
    {
        rr.clear(char_range<CharT>(from, to));
    }

    constexpr void
    clear(CharT c)
    {
        rr.clear(char_range<CharT>(c, c));
    }

    constexpr void
    clear() noexcept
    {
        rr.clear();
    }

    constexpr void
    inverse()
    {
        basic_chset inv;
        inv.set(
            (std::numeric_limits<CharT>::min)(),
            (std::numeric_limits<CharT>::max)()
        );
        inv -= *this;
        swap(inv);
    }

    constexpr void
    swap(basic_chset& x) noexcept
    {
        rr.swap(x.rr);
    }


    constexpr basic_chset&
    operator|=(basic_chset const& x)
    {
        using const_iterator = typename char_range_run<CharT>::const_iterator;
        for (const_iterator iter = x.rr.begin(); iter != x.rr.end(); ++iter) {
            rr.set(*iter);
        }
        return *this;
    }

    constexpr basic_chset&
    operator&=(basic_chset const& x)
    {
        basic_chset inv;
        inv.set(
            (std::numeric_limits<CharT>::min)(),
            (std::numeric_limits<CharT>::max)()
        );
        inv -= x;
        *this -= inv;
        return *this;
    }

    constexpr basic_chset&
    operator-=(basic_chset const& x)
    {
        using const_iterator = typename char_range_run<CharT>::const_iterator;
        for (const_iterator iter = x.rr.begin(); iter != x.rr.end(); ++iter) {
            rr.clear(*iter);
        }
        return *this;
    }

    constexpr basic_chset&
    operator^=(basic_chset const& x)
    {
        basic_chset bma = x;
        bma -= *this;
        *this -= x;
        *this |= bma;
        return *this;
    }

private:
    char_range_run<CharT> rr;
};

#if (CHAR_BIT == 8)

// Specializations for 8 bit chars using std::bitset
template<class CharT>
struct basic_chset_8bit
{
    [[nodiscard]] constexpr bool
    test(CharT v) const noexcept
    {
        return bset.test(static_cast<unsigned char>(v));
    }

    constexpr void
    set(CharT from, CharT to) noexcept
    {
        for (int i = from; i <= to; ++i)
            bset.set(static_cast<unsigned char>(i));
    }

    constexpr void
    set(CharT c) noexcept
    {
        bset.set(static_cast<unsigned char>(c));
    }

    constexpr void
    clear(CharT from, CharT to) noexcept
    {
        for (int i = from; i <= to; ++i)
            bset.reset(static_cast<unsigned char>(i));
    }

    constexpr void
    clear(CharT c) noexcept
    {
        bset.reset(static_cast<unsigned char>(c));
    }

    constexpr void
    clear() noexcept
    {
        bset.reset();
    }

    constexpr void
    inverse() noexcept
    {
        bset.flip();
    }

    constexpr void
    swap(basic_chset_8bit& x) noexcept
    {
        std::swap(bset, x.bset);
    }

    constexpr basic_chset_8bit&
    operator|=(basic_chset_8bit const& x) noexcept
    {
        bset |= x.bset;
        return *this;
    }

    constexpr basic_chset_8bit&
    operator&=(basic_chset_8bit const& x) noexcept
    {
        bset &= x.bset;
        return *this;
    }

    constexpr basic_chset_8bit&
    operator-=(basic_chset_8bit const& x) noexcept
    {
        bset &= ~x.bset;
        return *this;
    }

    constexpr basic_chset_8bit&
    operator^=(basic_chset_8bit const& x) noexcept
    {
        bset ^= x.bset;
        return *this;
    }

private:
    std::bitset<256> bset;
};

template<>
struct basic_chset<char>
    : basic_chset_8bit<char> {};

template<>
struct basic_chset<signed char>
    : basic_chset_8bit<signed char> {};

template<>
struct basic_chset<unsigned char>
    : basic_chset_8bit<unsigned char> {};

#endif // #if (CHAR_BIT == 8)

} // boost::spirit::x4::detail

#endif

