#ifndef BOOST_SPIRIT_X4_CHAR_CHAR_SET_HPP
#define BOOST_SPIRIT_X4_CHAR_CHAR_SET_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/char/char_parser.hpp>
#include <boost/spirit/x4/char/detail/cast_char.hpp>
#include <boost/spirit/x4/char/detail/basic_chset.hpp>
#include <boost/spirit/x4/string/utf8.hpp>
#include <boost/spirit/x4/string/case_compare.hpp>

#include <ranges>
#include <type_traits>

namespace boost::spirit::x4 {

// Parser for a character range
template<class Encoding, X4Attribute Attr = typename Encoding::char_type>
struct char_range : char_parser<Encoding, char_range<Encoding, Attr>>
{
    using char_type = typename Encoding::char_type;
    using encoding_type = Encoding;
    using attribute_type = Attr;

    static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;

    constexpr char_range(char_type from_, char_type to_) noexcept
        : from(from_), to(to_)
    {}

    template<class Char, class Context>
        requires (std::is_convertible_v<std::remove_cvref_t<Char>, char_type>)
    [[nodiscard]] constexpr bool test(Char ch_, Context const& ctx) const noexcept
    {
        char_type ch = static_cast<char_type>(ch_);  // optimize for token based parsing
        static_assert(noexcept(x4::get_case_compare<encoding_type>(ctx)(ch, from)));

        return x4::get_case_compare<encoding_type>(ctx)(ch, from) >= 0
            && x4::get_case_compare<encoding_type>(ctx)(ch , to) <= 0;
    }

    char_type from, to;
};

// Parser for a character set
template<class Encoding, X4Attribute Attr = typename Encoding::char_type>
struct char_set : char_parser<Encoding, char_set<Encoding, Attr>>
{
    using char_type = typename Encoding::char_type;
    using encoding_type = Encoding;
    using attribute_type = Attr;

    static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;

    template<std::ranges::forward_range R>
    constexpr explicit char_set(R const& str)
        noexcept(detail::cast_char_noexcept<std::ranges::range_value_t<R>, char_type>)
    {
        static_assert(detail::cast_char_viable<std::ranges::range_value_t<R>, char_type>);

        using detail::cast_char; // ADL introduction

        for (auto definition = std::ranges::begin(str); definition != std::ranges::end(str);) {
            auto const ch = *definition;
            auto next_definition = std::next(definition);
            if (next_definition == std::ranges::end(str)) {
                chset.set(cast_char<char_type>(ch));
                break;
            }

            auto next_ch = *next_definition;
            if (next_ch == '-') {
                next_definition = std::next(next_definition);
                if (next_definition == std::ranges::end(str)) {
                    chset.set(cast_char<char_type>(ch));
                    chset.set('-');
                    break;
                }
                chset.set(
                    cast_char<char_type>(ch),
                    cast_char<char_type>(*next_definition)
                );
            } else {
                chset.set(cast_char<char_type>(ch));
            }

            definition = next_definition;
        }
    }

    template<class Char, class Context>
    [[nodiscard]] constexpr bool test(Char ch_, Context const& ctx) const noexcept
    {
        static_assert(noexcept(x4::get_case_compare<encoding_type>(ctx).in_set(ch_, chset)));
        return x4::get_case_compare<encoding_type>(ctx).in_set(ch_, chset);
    }

    detail::basic_chset<char_type> chset;
};

template<class Encoding, X4Attribute Attr>
struct get_info<char_set<Encoding, Attr>>
{
    using result_type = std::string;
    [[nodiscard]] constexpr std::string operator()(char_set<Encoding, Attr> const& /* p */) const
    {
        return "char-set"; // TODO: make more user-friendly
    }
};

template<class Encoding, X4Attribute Attr>
struct get_info<char_range<Encoding, Attr>>
{
    using result_type = std::string;
    [[nodiscard]] constexpr std::string operator()(char_range<Encoding, Attr> const& p) const
    {
        // TODO: make more user-friendly && make the format consistent with above
        return "char_range \"" + x4::to_utf8(Encoding::toucs4(p.from)) + '-' + x4::to_utf8(Encoding::toucs4(p.to))+ '"';
    }
};

} // boost::spirit::x4

#endif
