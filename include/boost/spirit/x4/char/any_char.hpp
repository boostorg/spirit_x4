/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X4_ANY_CHAR_APRIL_16_2006_1051AM
#define BOOST_SPIRIT_X4_ANY_CHAR_APRIL_16_2006_1051AM

#include <boost/spirit/x4/char/literal_char.hpp>
#include <boost/spirit/x4/char/char_set.hpp>
#include <boost/spirit/x4/traits/string_traits.hpp>

namespace boost::spirit::x4
{
    template <typename Encoding>
    struct any_char : char_parser<any_char<Encoding>>
    {
        using char_type = typename Encoding::char_type;
        using encoding = Encoding;
        using attribute_type = char_type;

        static constexpr bool has_attribute = true;

        template <typename Context>
        [[nodiscard]] static constexpr bool
        test(char_type ch, Context const&) noexcept
        {
            return encoding::ischar(ch);
        }

        [[nodiscard]] static constexpr literal_char<Encoding>
        operator()(char_type ch) noexcept
        {
            return { ch };
        }

        [[nodiscard]] static constexpr literal_char<Encoding>
        operator()(char_type const (&ch)[2]) noexcept
        {
            return { ch[0] };
        }

        template <std::size_t N>
        [[nodiscard]] static constexpr char_set<Encoding>
        operator()(char_type const (&ch)[N])
        {
            return { ch };
        }

        [[nodiscard]] static constexpr char_range<Encoding>
        operator()(char_type from, char_type to) noexcept
        {
            return { from, to };
        }

        [[nodiscard]] static constexpr char_range<Encoding>
        operator()(char_type const (&from)[2], char_type const (&to)[2]) noexcept
        {
            return { static_cast<char_type>(from[0]), static_cast<char_type>(to[0]) };
        }

        [[nodiscard]] static char_set<Encoding>
        operator()(std::basic_string_view<char_type> sv)
        {
            return { std::move(sv) };
        }

        static constexpr void
        test(traits::CharIncompatibleWith<char_type> auto const&, auto const&) = delete; // Mixing incompatible char types is not allowed

        static constexpr void
        operator()(traits::CharIncompatibleWith<char_type> auto const&) = delete; // Mixing incompatible char types is not allowed

        template <std::size_t N>
        static constexpr void
        operator()(traits::CharIncompatibleWith<char_type> auto const (&)[N]) = delete; // Mixing incompatible char types is not allowed

        template <typename FromT, typename ToT>
            requires traits::CharIncompatibleWith<FromT, char_type> || traits::CharIncompatibleWith<ToT, char_type>
        static constexpr void
        operator()(FromT const&, ToT const&) = delete; // Mixing incompatible char types is not allowed

        template <typename FromT, typename ToT>
            requires traits::CharIncompatibleWith<FromT, char_type> || traits::CharIncompatibleWith<ToT, char_type>
        static constexpr void
        operator()(FromT const (&)[2], ToT const (&)[2]) = delete; // Mixing incompatible char types is not allowed
    };

} // boost::spirit::x4

#endif
