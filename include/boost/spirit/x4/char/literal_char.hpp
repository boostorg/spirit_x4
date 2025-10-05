#ifndef BOOST_SPIRIT_X4_CHAR_LITERAL_CHAR_HPP
#define BOOST_SPIRIT_X4_CHAR_LITERAL_CHAR_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/char/char_parser.hpp>
#include <boost/spirit/x4/string/utf8.hpp>
#include <boost/spirit/x4/string/case_compare.hpp>

#include <type_traits>
#include <concepts>

namespace boost::spirit::x4 {

template<class Encoding, X4Attribute Attr = typename Encoding::char_type>
struct literal_char : char_parser<Encoding, literal_char<Encoding, Attr>>
{
    using encoding_type = Encoding;
    using attribute_type = Attr;
    using char_type = typename Encoding::char_type;
    using classify_type = typename Encoding::classify_type;

    static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;

    template<class Char>
        requires
            (!std::is_same_v<std::remove_cvref_t<Char>, literal_char>) &&
            std::convertible_to<Char, classify_type>
    constexpr literal_char(Char ch) noexcept
        : classify_ch_(static_cast<classify_type>(ch))
    {
        static_assert(std::same_as<Char, char_type>, "Mixing incompatible character types is not allowed");
    }

    [[nodiscard]] constexpr bool
    test(classify_type const test_classify_ch, auto const& ctx) const noexcept
    {
        static_assert(noexcept(x4::get_case_compare<encoding_type>(ctx)(classify_ch_, test_classify_ch)));
        return x4::get_case_compare<encoding_type>(ctx)(classify_ch_, test_classify_ch) == 0;
    }

    constexpr void
    test(auto const, auto const&) const = delete; // Mixing incompatible character types is not allowed

    [[nodiscard]] constexpr classify_type classify_ch() const noexcept { return classify_ch_; }

private:
    classify_type classify_ch_{};
};

template<class Encoding, X4Attribute Attr>
struct get_info<literal_char<Encoding, Attr>>
{
    using result_type = std::string;
    [[nodiscard]] std::string operator()(literal_char<Encoding, Attr> const& p) const
    {
        return '\'' + x4::to_utf8(Encoding::toucs4(p.classify_ch())) + '\'';
    }
};

} // boost::spirit::x4

#endif
