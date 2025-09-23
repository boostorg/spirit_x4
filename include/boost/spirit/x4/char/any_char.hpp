#ifndef BOOST_SPIRIT_X4_CHAR_ANY_CHAR_HPP
#define BOOST_SPIRIT_X4_CHAR_ANY_CHAR_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/char/literal_char.hpp>
#include <boost/spirit/x4/char/char_set.hpp>
#include <boost/spirit/x4/traits/string_traits.hpp>

namespace boost::spirit::x4 {

template<class Encoding>
struct any_char : char_parser<Encoding, any_char<Encoding>>
{
    using encoding_type = Encoding;
    using attribute_type = typename Encoding::char_type;
    using char_type = typename Encoding::char_type;
    using classify_type = typename Encoding::classify_type;

    static constexpr bool has_attribute = true;

    [[nodiscard]] static constexpr bool
    test(classify_type classify_ch, auto const& /* ctx */) noexcept
    {
        static_assert(noexcept(encoding_type::ischar(classify_ch)));
        return encoding_type::ischar(classify_ch);
    }

    static constexpr void
    test(auto, auto const& /* ctx */) = delete; // Mixing incompatible char types is not allowed

    template<std::same_as<char_type> CharT>
    [[nodiscard]] static constexpr literal_char<Encoding>
    operator()(CharT ch) noexcept
    {
        return {ch};
    }

    template<traits::CharIncompatibleWith<char_type> CharT>
    static constexpr void operator()(CharT) = delete; // Mixing incompatible char types is not allowed

    [[nodiscard]] static constexpr literal_char<Encoding>
    operator()(char_type const (&ch)[2]) noexcept
    {
        return {ch[0]};
    }

    template<std::size_t N>
    [[nodiscard]] static constexpr char_set<Encoding>
    operator()(char_type const (&ch)[N])
    {
        return char_set<Encoding>{ch};
    }

    template<traits::CharIncompatibleWith<char_type> CharT, std::size_t N>
    static constexpr void
    operator()(CharT const (&)[N]) = delete; // Mixing incompatible char types is not allowed

    template<std::same_as<char_type> CharT>
    [[nodiscard]] static constexpr char_range<Encoding>
    operator()(CharT from, CharT to) noexcept
    {
        return {from, to};
    }

    template<class From, class To>
        requires traits::CharIncompatibleWith<From, char_type> || traits::CharIncompatibleWith<To, char_type>
    static constexpr void operator()(From, To) = delete; // Mixing incompatible char types is not allowed

    template<class From, std::size_t FromN, class To, std::size_t ToN>
    static constexpr void
    operator()(From const (&)[FromN], To const (&)[ToN]) = delete; // Use single character literal to define character range

    [[nodiscard]] static char_set<Encoding>
    operator()(std::basic_string_view<char_type> sv)
    {
        return char_set<Encoding>{std::move(sv)};
    }
};

} // boost::spirit::x4

#endif
