#ifndef BOOST_SPIRIT_X4_CHAR_ENCODING_UNICODE_HPP
#define BOOST_SPIRIT_X4_CHAR_ENCODING_UNICODE_HPP

/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/x4/char_encoding/unicode/classification.hpp>

#include <cstdint>

namespace boost::spirit::x4::char_encoding {

struct unicode
{
    using char_type = char32_t;
    using classify_type = x4::unicode::classify_type;

    [[nodiscard]] static constexpr bool
    isascii_(classify_type ch) noexcept
    {
        return 0 == (ch & ~0x7f);
    }

    [[nodiscard]] static constexpr bool
    ischar(classify_type ch) noexcept
    {
        // unicode code points in the range 0x00 to 0x10FFFF
        return ch <= 0x10FFFF;
    }

    [[nodiscard]] static constexpr bool
    isalnum(classify_type ch) noexcept
    {
        return x4::unicode::is_alphanumeric(ch);
    }

    [[nodiscard]] static constexpr bool
    isalpha(classify_type ch) noexcept
    {
        return x4::unicode::is_alphabetic(ch);
    }

    [[nodiscard]] static constexpr bool
    isdigit(classify_type ch) noexcept
    {
        return x4::unicode::is_decimal_number(ch);
    }

    [[nodiscard]] static constexpr bool
    isxdigit(classify_type ch) noexcept
    {
        return x4::unicode::is_hex_digit(ch);
    }

    [[nodiscard]] static constexpr bool
    iscntrl(classify_type ch) noexcept
    {
        return x4::unicode::is_control(ch);
    }

    [[nodiscard]] static constexpr bool
    isgraph(classify_type ch) noexcept
    {
        return x4::unicode::is_graph(ch);
    }

    [[nodiscard]] static constexpr bool
    islower(classify_type ch) noexcept
    {
        return x4::unicode::is_lowercase(ch);
    }

    [[nodiscard]] static constexpr bool
    isprint(classify_type ch) noexcept
    {
        return x4::unicode::is_print(ch);
    }

    [[nodiscard]] static constexpr bool
    ispunct(classify_type ch) noexcept
    {
        return x4::unicode::is_punctuation(ch);
    }

    [[nodiscard]] static constexpr bool
    isspace(classify_type ch) noexcept
    {
        return x4::unicode::is_white_space(ch);
    }

    [[nodiscard]] static constexpr bool
    (isblank)(classify_type ch) noexcept
    {
        return x4::unicode::is_blank(ch);
    }

    [[nodiscard]] static constexpr bool
    isupper(classify_type ch) noexcept
    {
        return x4::unicode::is_uppercase(ch);
    }

    // Mixing character encodings is semantically wrong
    static constexpr void isascii_(char) = delete;
    static constexpr void isascii_(wchar_t) = delete;
    static constexpr void ischar(char) = delete;
    static constexpr void ischar(wchar_t) = delete;
    static constexpr void isalnum(char) = delete;
    static constexpr void isalnum(wchar_t) = delete;
    static constexpr void isalpha(char) = delete;
    static constexpr void isalpha(wchar_t) = delete;
    static constexpr void isdigit(char) = delete;
    static constexpr void isdigit(wchar_t) = delete;
    static constexpr void isxdigit(char) = delete;
    static constexpr void isxdigit(wchar_t) = delete;
    static constexpr void iscntrl(char) = delete;
    static constexpr void iscntrl(wchar_t) = delete;
    static constexpr void isgraph(char) = delete;
    static constexpr void isgraph(wchar_t) = delete;
    static constexpr void islower(char) = delete;
    static constexpr void islower(wchar_t) = delete;
    static constexpr void isprint(char) = delete;
    static constexpr void isprint(wchar_t) = delete;
    static constexpr void ispunct(char) = delete;
    static constexpr void ispunct(wchar_t) = delete;
    static constexpr void isspace(char) = delete;
    static constexpr void isspace(wchar_t) = delete;
    static constexpr void isblank(char) = delete;
    static constexpr void isblank(wchar_t) = delete;
    static constexpr void isupper(char) = delete;
    static constexpr void isupper(wchar_t) = delete;

    // Simple character conversions

    [[nodiscard]] static constexpr classify_type
    tolower(classify_type ch) noexcept
    {
        return x4::unicode::to_lowercase(ch);
    }

    [[nodiscard]] static constexpr classify_type
    toupper(classify_type ch) noexcept
    {
        return x4::unicode::to_uppercase(ch);
    }

    [[nodiscard]] static constexpr std::uint32_t
    toucs4(classify_type ch) noexcept
    {
        return ch;
    }

    static constexpr void tolower(char) = delete;
    static constexpr void tolower(wchar_t) = delete;
    static constexpr void toupper(char) = delete;
    static constexpr void toupper(wchar_t) = delete;
    static constexpr void toucs4(char) = delete;
    static constexpr void toucs4(wchar_t) = delete;

    // Major Categories
#define BOOST_SPIRIT_X4_MAJOR_CATEGORY(name) \
    [[nodiscard]] static constexpr bool \
    is_##name(classify_type ch) noexcept \
    { \
        return x4::unicode::get_major_category(ch) == x4::unicode::properties::name; \
    } \
    static constexpr void is_##name(char) = delete; \
    static constexpr void is_##name(wchar_t) = delete;

    BOOST_SPIRIT_X4_MAJOR_CATEGORY(letter)
    BOOST_SPIRIT_X4_MAJOR_CATEGORY(mark)
    BOOST_SPIRIT_X4_MAJOR_CATEGORY(number)
    BOOST_SPIRIT_X4_MAJOR_CATEGORY(separator)
    BOOST_SPIRIT_X4_MAJOR_CATEGORY(other)
    BOOST_SPIRIT_X4_MAJOR_CATEGORY(punctuation)
    BOOST_SPIRIT_X4_MAJOR_CATEGORY(symbol)

#undef BOOST_SPIRIT_X4_MAJOR_CATEGORY

    // General Categories
#define BOOST_SPIRIT_X4_CATEGORY(name) \
    [[nodiscard]] static constexpr bool \
    is_##name(classify_type ch) noexcept \
    { \
        return x4::unicode::get_category(ch) == x4::unicode::properties::name; \
    } \
    static constexpr void is_##name(char) = delete; \
    static constexpr void is_##name(wchar_t) = delete;

    BOOST_SPIRIT_X4_CATEGORY(uppercase_letter)
    BOOST_SPIRIT_X4_CATEGORY(lowercase_letter)
    BOOST_SPIRIT_X4_CATEGORY(titlecase_letter)
    BOOST_SPIRIT_X4_CATEGORY(modifier_letter)
    BOOST_SPIRIT_X4_CATEGORY(other_letter)

    BOOST_SPIRIT_X4_CATEGORY(nonspacing_mark)
    BOOST_SPIRIT_X4_CATEGORY(enclosing_mark)
    BOOST_SPIRIT_X4_CATEGORY(spacing_mark)

    BOOST_SPIRIT_X4_CATEGORY(decimal_number)
    BOOST_SPIRIT_X4_CATEGORY(letter_number)
    BOOST_SPIRIT_X4_CATEGORY(other_number)

    BOOST_SPIRIT_X4_CATEGORY(space_separator)
    BOOST_SPIRIT_X4_CATEGORY(line_separator)
    BOOST_SPIRIT_X4_CATEGORY(paragraph_separator)

    BOOST_SPIRIT_X4_CATEGORY(control)
    BOOST_SPIRIT_X4_CATEGORY(format)
    BOOST_SPIRIT_X4_CATEGORY(private_use)
    BOOST_SPIRIT_X4_CATEGORY(surrogate)
    BOOST_SPIRIT_X4_CATEGORY(unassigned)

    BOOST_SPIRIT_X4_CATEGORY(dash_punctuation)
    BOOST_SPIRIT_X4_CATEGORY(open_punctuation)
    BOOST_SPIRIT_X4_CATEGORY(close_punctuation)
    BOOST_SPIRIT_X4_CATEGORY(connector_punctuation)
    BOOST_SPIRIT_X4_CATEGORY(other_punctuation)
    BOOST_SPIRIT_X4_CATEGORY(initial_punctuation)
    BOOST_SPIRIT_X4_CATEGORY(final_punctuation)

    BOOST_SPIRIT_X4_CATEGORY(math_symbol)
    BOOST_SPIRIT_X4_CATEGORY(currency_symbol)
    BOOST_SPIRIT_X4_CATEGORY(modifier_symbol)
    BOOST_SPIRIT_X4_CATEGORY(other_symbol)

#undef BOOST_SPIRIT_X4_CATEGORY

    // Derived Categories
#define BOOST_SPIRIT_X4_DERIVED_CATEGORY(name) \
    [[nodiscard]] static constexpr bool \
    is_##name(classify_type ch) noexcept \
    { \
        return x4::unicode::is_##name(ch); \
    } \
    static constexpr void is_##name(char) = delete; \
    static constexpr void is_##name(wchar_t) = delete;

    BOOST_SPIRIT_X4_DERIVED_CATEGORY(alphabetic)
    BOOST_SPIRIT_X4_DERIVED_CATEGORY(uppercase)
    BOOST_SPIRIT_X4_DERIVED_CATEGORY(lowercase)
    BOOST_SPIRIT_X4_DERIVED_CATEGORY(white_space)
    BOOST_SPIRIT_X4_DERIVED_CATEGORY(hex_digit)
    BOOST_SPIRIT_X4_DERIVED_CATEGORY(noncharacter_code_point)
    BOOST_SPIRIT_X4_DERIVED_CATEGORY(default_ignorable_code_point)

#undef BOOST_SPIRIT_X4_DERIVED_CATEGORY

    // Scripts
#define BOOST_SPIRIT_X4_SCRIPT(name) \
    [[nodiscard]] static constexpr bool \
    is_##name(classify_type ch) noexcept \
    { \
        return x4::unicode::get_script(ch) == x4::unicode::properties::name; \
    } \
    static constexpr void is_##name(char) = delete; \
    static constexpr void is_##name(wchar_t) = delete;

    BOOST_SPIRIT_X4_SCRIPT(adlam)
    BOOST_SPIRIT_X4_SCRIPT(caucasian_albanian)
    BOOST_SPIRIT_X4_SCRIPT(ahom)
    BOOST_SPIRIT_X4_SCRIPT(arabic)
    BOOST_SPIRIT_X4_SCRIPT(imperial_aramaic)
    BOOST_SPIRIT_X4_SCRIPT(armenian)
    BOOST_SPIRIT_X4_SCRIPT(avestan)
    BOOST_SPIRIT_X4_SCRIPT(balinese)
    BOOST_SPIRIT_X4_SCRIPT(bamum)
    BOOST_SPIRIT_X4_SCRIPT(bassa_vah)
    BOOST_SPIRIT_X4_SCRIPT(batak)
    BOOST_SPIRIT_X4_SCRIPT(bengali)
    BOOST_SPIRIT_X4_SCRIPT(bhaiksuki)
    BOOST_SPIRIT_X4_SCRIPT(bopomofo)
    BOOST_SPIRIT_X4_SCRIPT(brahmi)
    BOOST_SPIRIT_X4_SCRIPT(braille)
    BOOST_SPIRIT_X4_SCRIPT(buginese)
    BOOST_SPIRIT_X4_SCRIPT(buhid)
    BOOST_SPIRIT_X4_SCRIPT(chakma)
    BOOST_SPIRIT_X4_SCRIPT(canadian_aboriginal)
    BOOST_SPIRIT_X4_SCRIPT(carian)
    BOOST_SPIRIT_X4_SCRIPT(cham)
    BOOST_SPIRIT_X4_SCRIPT(cherokee)
    BOOST_SPIRIT_X4_SCRIPT(chorasmian)
    BOOST_SPIRIT_X4_SCRIPT(coptic)
    BOOST_SPIRIT_X4_SCRIPT(cypro_minoan)
    BOOST_SPIRIT_X4_SCRIPT(cypriot)
    BOOST_SPIRIT_X4_SCRIPT(cyrillic)
    BOOST_SPIRIT_X4_SCRIPT(devanagari)
    BOOST_SPIRIT_X4_SCRIPT(dives_akuru)
    BOOST_SPIRIT_X4_SCRIPT(dogra)
    BOOST_SPIRIT_X4_SCRIPT(deseret)
    BOOST_SPIRIT_X4_SCRIPT(duployan)
    BOOST_SPIRIT_X4_SCRIPT(egyptian_hieroglyphs)
    BOOST_SPIRIT_X4_SCRIPT(elbasan)
    BOOST_SPIRIT_X4_SCRIPT(elymaic)
    BOOST_SPIRIT_X4_SCRIPT(ethiopic)
    BOOST_SPIRIT_X4_SCRIPT(georgian)
    BOOST_SPIRIT_X4_SCRIPT(glagolitic)
    BOOST_SPIRIT_X4_SCRIPT(gunjala_gondi)
    BOOST_SPIRIT_X4_SCRIPT(masaram_gondi)
    BOOST_SPIRIT_X4_SCRIPT(gothic)
    BOOST_SPIRIT_X4_SCRIPT(grantha)
    BOOST_SPIRIT_X4_SCRIPT(greek)
    BOOST_SPIRIT_X4_SCRIPT(gujarati)
    BOOST_SPIRIT_X4_SCRIPT(gurmukhi)
    BOOST_SPIRIT_X4_SCRIPT(hangul)
    BOOST_SPIRIT_X4_SCRIPT(han)
    BOOST_SPIRIT_X4_SCRIPT(hanunoo)
    BOOST_SPIRIT_X4_SCRIPT(hatran)
    BOOST_SPIRIT_X4_SCRIPT(hebrew)
    BOOST_SPIRIT_X4_SCRIPT(hiragana)
    BOOST_SPIRIT_X4_SCRIPT(anatolian_hieroglyphs)
    BOOST_SPIRIT_X4_SCRIPT(pahawh_hmong)
    BOOST_SPIRIT_X4_SCRIPT(nyiakeng_puachue_hmong)
    BOOST_SPIRIT_X4_SCRIPT(katakana_or_hiragana)
    BOOST_SPIRIT_X4_SCRIPT(old_hungarian)
    BOOST_SPIRIT_X4_SCRIPT(old_italic)
    BOOST_SPIRIT_X4_SCRIPT(javanese)
    BOOST_SPIRIT_X4_SCRIPT(kayah_li)
    BOOST_SPIRIT_X4_SCRIPT(katakana)
    BOOST_SPIRIT_X4_SCRIPT(kawi)
    BOOST_SPIRIT_X4_SCRIPT(kharoshthi)
    BOOST_SPIRIT_X4_SCRIPT(khmer)
    BOOST_SPIRIT_X4_SCRIPT(khojki)
    BOOST_SPIRIT_X4_SCRIPT(khitan_small_script)
    BOOST_SPIRIT_X4_SCRIPT(kannada)
    BOOST_SPIRIT_X4_SCRIPT(kaithi)
    BOOST_SPIRIT_X4_SCRIPT(tai_tham)
    BOOST_SPIRIT_X4_SCRIPT(lao)
    BOOST_SPIRIT_X4_SCRIPT(latin)
    BOOST_SPIRIT_X4_SCRIPT(lepcha)
    BOOST_SPIRIT_X4_SCRIPT(limbu)
    BOOST_SPIRIT_X4_SCRIPT(linear_a)
    BOOST_SPIRIT_X4_SCRIPT(linear_b)
    BOOST_SPIRIT_X4_SCRIPT(lisu)
    BOOST_SPIRIT_X4_SCRIPT(lycian)
    BOOST_SPIRIT_X4_SCRIPT(lydian)
    BOOST_SPIRIT_X4_SCRIPT(mahajani)
    BOOST_SPIRIT_X4_SCRIPT(makasar)
    BOOST_SPIRIT_X4_SCRIPT(mandaic)
    BOOST_SPIRIT_X4_SCRIPT(manichaean)
    BOOST_SPIRIT_X4_SCRIPT(marchen)
    BOOST_SPIRIT_X4_SCRIPT(medefaidrin)
    BOOST_SPIRIT_X4_SCRIPT(mende_kikakui)
    BOOST_SPIRIT_X4_SCRIPT(meroitic_cursive)
    BOOST_SPIRIT_X4_SCRIPT(meroitic_hieroglyphs)
    BOOST_SPIRIT_X4_SCRIPT(malayalam)
    BOOST_SPIRIT_X4_SCRIPT(modi)
    BOOST_SPIRIT_X4_SCRIPT(mongolian)
    BOOST_SPIRIT_X4_SCRIPT(mro)
    BOOST_SPIRIT_X4_SCRIPT(meetei_mayek)
    BOOST_SPIRIT_X4_SCRIPT(multani)
    BOOST_SPIRIT_X4_SCRIPT(myanmar)
    BOOST_SPIRIT_X4_SCRIPT(nag_mundari)
    BOOST_SPIRIT_X4_SCRIPT(nandinagari)
    BOOST_SPIRIT_X4_SCRIPT(old_north_arabian)
    BOOST_SPIRIT_X4_SCRIPT(nabataean)
    BOOST_SPIRIT_X4_SCRIPT(newa)
    BOOST_SPIRIT_X4_SCRIPT(nko)
    BOOST_SPIRIT_X4_SCRIPT(nushu)
    BOOST_SPIRIT_X4_SCRIPT(ogham)
    BOOST_SPIRIT_X4_SCRIPT(ol_chiki)
    BOOST_SPIRIT_X4_SCRIPT(old_turkic)
    BOOST_SPIRIT_X4_SCRIPT(oriya)
    BOOST_SPIRIT_X4_SCRIPT(osage)
    BOOST_SPIRIT_X4_SCRIPT(osmanya)
    BOOST_SPIRIT_X4_SCRIPT(old_uyghur)
    BOOST_SPIRIT_X4_SCRIPT(palmyrene)
    BOOST_SPIRIT_X4_SCRIPT(pau_cin_hau)
    BOOST_SPIRIT_X4_SCRIPT(old_permic)
    BOOST_SPIRIT_X4_SCRIPT(phags_pa)
    BOOST_SPIRIT_X4_SCRIPT(inscriptional_pahlavi)
    BOOST_SPIRIT_X4_SCRIPT(psalter_pahlavi)
    BOOST_SPIRIT_X4_SCRIPT(phoenician)
    BOOST_SPIRIT_X4_SCRIPT(miao)
    BOOST_SPIRIT_X4_SCRIPT(inscriptional_parthian)
    BOOST_SPIRIT_X4_SCRIPT(rejang)
    BOOST_SPIRIT_X4_SCRIPT(hanifi_rohingya)
    BOOST_SPIRIT_X4_SCRIPT(runic)
    BOOST_SPIRIT_X4_SCRIPT(samaritan)
    BOOST_SPIRIT_X4_SCRIPT(old_south_arabian)
    BOOST_SPIRIT_X4_SCRIPT(saurashtra)
    BOOST_SPIRIT_X4_SCRIPT(signwriting)
    BOOST_SPIRIT_X4_SCRIPT(shavian)
    BOOST_SPIRIT_X4_SCRIPT(sharada)
    BOOST_SPIRIT_X4_SCRIPT(siddham)
    BOOST_SPIRIT_X4_SCRIPT(khudawadi)
    BOOST_SPIRIT_X4_SCRIPT(sinhala)
    BOOST_SPIRIT_X4_SCRIPT(sogdian)
    BOOST_SPIRIT_X4_SCRIPT(old_sogdian)
    BOOST_SPIRIT_X4_SCRIPT(sora_sompeng)
    BOOST_SPIRIT_X4_SCRIPT(soyombo)
    BOOST_SPIRIT_X4_SCRIPT(sundanese)
    BOOST_SPIRIT_X4_SCRIPT(syloti_nagri)
    BOOST_SPIRIT_X4_SCRIPT(syriac)
    BOOST_SPIRIT_X4_SCRIPT(tagbanwa)
    BOOST_SPIRIT_X4_SCRIPT(takri)
    BOOST_SPIRIT_X4_SCRIPT(tai_le)
    BOOST_SPIRIT_X4_SCRIPT(new_tai_lue)
    BOOST_SPIRIT_X4_SCRIPT(tamil)
    BOOST_SPIRIT_X4_SCRIPT(tangut)
    BOOST_SPIRIT_X4_SCRIPT(tai_viet)
    BOOST_SPIRIT_X4_SCRIPT(telugu)
    BOOST_SPIRIT_X4_SCRIPT(tifinagh)
    BOOST_SPIRIT_X4_SCRIPT(tagalog)
    BOOST_SPIRIT_X4_SCRIPT(thaana)
    BOOST_SPIRIT_X4_SCRIPT(thai)
    BOOST_SPIRIT_X4_SCRIPT(tibetan)
    BOOST_SPIRIT_X4_SCRIPT(tirhuta)
    BOOST_SPIRIT_X4_SCRIPT(tangsa)
    BOOST_SPIRIT_X4_SCRIPT(toto)
    BOOST_SPIRIT_X4_SCRIPT(ugaritic)
    BOOST_SPIRIT_X4_SCRIPT(vai)
    BOOST_SPIRIT_X4_SCRIPT(vithkuqi)
    BOOST_SPIRIT_X4_SCRIPT(warang_citi)
    BOOST_SPIRIT_X4_SCRIPT(wancho)
    BOOST_SPIRIT_X4_SCRIPT(old_persian)
    BOOST_SPIRIT_X4_SCRIPT(cuneiform)
    BOOST_SPIRIT_X4_SCRIPT(yezidi)
    BOOST_SPIRIT_X4_SCRIPT(yi)
    BOOST_SPIRIT_X4_SCRIPT(zanabazar_square)
    BOOST_SPIRIT_X4_SCRIPT(inherited)
    BOOST_SPIRIT_X4_SCRIPT(common)
    BOOST_SPIRIT_X4_SCRIPT(unknown)

#undef BOOST_SPIRIT_X4_SCRIPT
};

} // boost::spirit::x4::char_encoding

#endif
