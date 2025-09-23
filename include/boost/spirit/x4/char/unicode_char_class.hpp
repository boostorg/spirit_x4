#ifndef BOOST_SPIRIT_X4_CHAR_UNICODE_CHAR_CLASS_HPP
#define BOOST_SPIRIT_X4_CHAR_UNICODE_CHAR_CLASS_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/char/char_class_tags.hpp>
#include <boost/spirit/x4/char/char_parser.hpp>
#include <boost/spirit/x4/char/detail/cast_char.hpp>
#include <boost/spirit/x4/char_encoding/unicode.hpp>

namespace boost::spirit::x4 {

namespace char_classes::unicode {

// Common Categories (semantically compatible with `standard::*` variants)

using char_tag = char_classes::char_tag;
using alnum_tag = char_classes::alnum_tag;
using alpha_tag = char_classes::alpha_tag;
using digit_tag = char_classes::digit_tag;
using xdigit_tag = char_classes::xdigit_tag;
using cntrl_tag = char_classes::cntrl_tag;
using graph_tag = char_classes::graph_tag;
using lower_tag = char_classes::lower_tag;
using print_tag = char_classes::print_tag;
using punct_tag = char_classes::punct_tag;
using space_tag = char_classes::space_tag;
using blank_tag = char_classes::blank_tag;
using upper_tag = char_classes::upper_tag;

// Unicode Major Categories

struct letter_tag {};
struct mark_tag {};
struct number_tag {};
struct separator_tag {};
struct other_tag {};
struct punctuation_tag {};
struct symbol_tag {};

// Unicode General Categories

struct uppercase_letter_tag {};
struct lowercase_letter_tag {};
struct titlecase_letter_tag {};
struct modifier_letter_tag {};
struct other_letter_tag {};

struct nonspacing_mark_tag {};
struct enclosing_mark_tag {};
struct spacing_mark_tag {};

struct decimal_number_tag {};
struct letter_number_tag {};
struct other_number_tag {};

struct space_separator_tag {};
struct line_separator_tag {};
struct paragraph_separator_tag {};

struct control_tag {};
struct format_tag {};
struct private_use_tag {};
struct surrogate_tag {};
struct unassigned_tag {};

struct dash_punctuation_tag {};
struct open_punctuation_tag {};
struct close_punctuation_tag {};
struct connector_punctuation_tag {};
struct other_punctuation_tag {};
struct initial_punctuation_tag {};
struct final_punctuation_tag {};

struct math_symbol_tag {};
struct currency_symbol_tag {};
struct modifier_symbol_tag {};
struct other_symbol_tag {};

// Unicode Derived Categories

struct alphabetic_tag {};
struct uppercase_tag {};
struct lowercase_tag {};
struct white_space_tag {};
struct hex_digit_tag {};
struct noncharacter_code_point_tag {};
struct default_ignorable_code_point_tag {};

// Unicode Scripts

struct adlam_tag {};
struct caucasian_albanian_tag {};
struct ahom_tag {};
struct arabic_tag {};
struct imperial_aramaic_tag {};
struct armenian_tag {};
struct avestan_tag {};
struct balinese_tag {};
struct bamum_tag {};
struct bassa_vah_tag {};
struct batak_tag {};
struct bengali_tag {};
struct bhaiksuki_tag {};
struct bopomofo_tag {};
struct brahmi_tag {};
struct braille_tag {};
struct buginese_tag {};
struct buhid_tag {};
struct chakma_tag {};
struct canadian_aboriginal_tag {};
struct carian_tag {};
struct cham_tag {};
struct cherokee_tag {};
struct chorasmian_tag {};
struct coptic_tag {};
struct cypro_minoan_tag {};
struct cypriot_tag {};
struct cyrillic_tag {};
struct devanagari_tag {};
struct dives_akuru_tag {};
struct dogra_tag {};
struct deseret_tag {};
struct duployan_tag {};
struct egyptian_hieroglyphs_tag {};
struct elbasan_tag {};
struct elymaic_tag {};
struct ethiopic_tag {};
struct georgian_tag {};
struct glagolitic_tag {};
struct gunjala_gondi_tag {};
struct masaram_gondi_tag {};
struct gothic_tag {};
struct grantha_tag {};
struct greek_tag {};
struct gujarati_tag {};
struct gurmukhi_tag {};
struct hangul_tag {};
struct han_tag {};
struct hanunoo_tag {};
struct hatran_tag {};
struct hebrew_tag {};
struct hiragana_tag {};
struct anatolian_hieroglyphs_tag {};
struct pahawh_hmong_tag {};
struct nyiakeng_puachue_hmong_tag {};
struct katakana_or_hiragana_tag {};
struct old_hungarian_tag {};
struct old_italic_tag {};
struct javanese_tag {};
struct kayah_li_tag {};
struct katakana_tag {};
struct kawi_tag {};
struct kharoshthi_tag {};
struct khmer_tag {};
struct khojki_tag {};
struct khitan_small_script_tag {};
struct kannada_tag {};
struct kaithi_tag {};
struct tai_tham_tag {};
struct lao_tag {};
struct latin_tag {};
struct lepcha_tag {};
struct limbu_tag {};
struct linear_a_tag {};
struct linear_b_tag {};
struct lisu_tag {};
struct lycian_tag {};
struct lydian_tag {};
struct mahajani_tag {};
struct makasar_tag {};
struct mandaic_tag {};
struct manichaean_tag {};
struct marchen_tag {};
struct medefaidrin_tag {};
struct mende_kikakui_tag {};
struct meroitic_cursive_tag {};
struct meroitic_hieroglyphs_tag {};
struct malayalam_tag {};
struct modi_tag {};
struct mongolian_tag {};
struct mro_tag {};
struct meetei_mayek_tag {};
struct multani_tag {};
struct myanmar_tag {};
struct nag_mundari_tag {};
struct nandinagari_tag {};
struct old_north_arabian_tag {};
struct nabataean_tag {};
struct newa_tag {};
struct nko_tag {};
struct nushu_tag {};
struct ogham_tag {};
struct ol_chiki_tag {};
struct old_turkic_tag {};
struct oriya_tag {};
struct osage_tag {};
struct osmanya_tag {};
struct old_uyghur_tag {};
struct palmyrene_tag {};
struct pau_cin_hau_tag {};
struct old_permic_tag {};
struct phags_pa_tag {};
struct inscriptional_pahlavi_tag {};
struct psalter_pahlavi_tag {};
struct phoenician_tag {};
struct miao_tag {};
struct inscriptional_parthian_tag {};
struct rejang_tag {};
struct hanifi_rohingya_tag {};
struct runic_tag {};
struct samaritan_tag {};
struct old_south_arabian_tag {};
struct saurashtra_tag {};
struct signwriting_tag {};
struct shavian_tag {};
struct sharada_tag {};
struct siddham_tag {};
struct khudawadi_tag {};
struct sinhala_tag {};
struct sogdian_tag {};
struct old_sogdian_tag {};
struct sora_sompeng_tag {};
struct soyombo_tag {};
struct sundanese_tag {};
struct syloti_nagri_tag {};
struct syriac_tag {};
struct tagbanwa_tag {};
struct takri_tag {};
struct tai_le_tag {};
struct new_tai_lue_tag {};
struct tamil_tag {};
struct tangut_tag {};
struct tai_viet_tag {};
struct telugu_tag {};
struct tifinagh_tag {};
struct tagalog_tag {};
struct thaana_tag {};
struct thai_tag {};
struct tibetan_tag {};
struct tirhuta_tag {};
struct tangsa_tag {};
struct toto_tag {};
struct ugaritic_tag {};
struct vai_tag {};
struct vithkuqi_tag {};
struct warang_citi_tag {};
struct wancho_tag {};
struct old_persian_tag {};
struct cuneiform_tag {};
struct yezidi_tag {};
struct yi_tag {};
struct zanabazar_square_tag {};
struct inherited_tag {};
struct common_tag {};
struct unknown_tag {};

} // char_classes::unicode

namespace detail {

struct unicode_char_class_base
{
    using encoding_type = char_encoding::unicode;
    using char_type = char_encoding::unicode::char_type;

#define BOOST_SPIRIT_X4_BASIC_CLASSIFY(name) \
    template<class Char> \
    static constexpr bool \
    is(char_classes::unicode::name##_tag, Char ch) noexcept \
    { \
        return (encoding_type::is##name)(detail::cast_char<char_type>(ch)); \
    }

#define BOOST_SPIRIT_X4_CLASSIFY(name) \
    template<class Char> \
    static constexpr bool \
    is(char_classes::unicode::name##_tag, Char ch) noexcept \
    { \
        return (encoding_type::is_##name)(detail::cast_char<char_type>(ch)); \
    }

    // Unicode Major Categories

    BOOST_SPIRIT_X4_BASIC_CLASSIFY(char)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(alnum)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(alpha)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(digit)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(xdigit)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(cntrl)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(graph)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(lower)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(print)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(punct)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(space)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(blank)
    BOOST_SPIRIT_X4_BASIC_CLASSIFY(upper)

    // Unicode Major Categories

    BOOST_SPIRIT_X4_CLASSIFY(letter)
    BOOST_SPIRIT_X4_CLASSIFY(mark)
    BOOST_SPIRIT_X4_CLASSIFY(number)
    BOOST_SPIRIT_X4_CLASSIFY(separator)
    BOOST_SPIRIT_X4_CLASSIFY(other)
    BOOST_SPIRIT_X4_CLASSIFY(punctuation)
    BOOST_SPIRIT_X4_CLASSIFY(symbol)

    // Unicode General Categories

    BOOST_SPIRIT_X4_CLASSIFY(uppercase_letter)
    BOOST_SPIRIT_X4_CLASSIFY(lowercase_letter)
    BOOST_SPIRIT_X4_CLASSIFY(titlecase_letter)
    BOOST_SPIRIT_X4_CLASSIFY(modifier_letter)
    BOOST_SPIRIT_X4_CLASSIFY(other_letter)

    BOOST_SPIRIT_X4_CLASSIFY(nonspacing_mark)
    BOOST_SPIRIT_X4_CLASSIFY(enclosing_mark)
    BOOST_SPIRIT_X4_CLASSIFY(spacing_mark)

    BOOST_SPIRIT_X4_CLASSIFY(decimal_number)
    BOOST_SPIRIT_X4_CLASSIFY(letter_number)
    BOOST_SPIRIT_X4_CLASSIFY(other_number)

    BOOST_SPIRIT_X4_CLASSIFY(space_separator)
    BOOST_SPIRIT_X4_CLASSIFY(line_separator)
    BOOST_SPIRIT_X4_CLASSIFY(paragraph_separator)

    BOOST_SPIRIT_X4_CLASSIFY(control)
    BOOST_SPIRIT_X4_CLASSIFY(format)
    BOOST_SPIRIT_X4_CLASSIFY(private_use)
    BOOST_SPIRIT_X4_CLASSIFY(surrogate)
    BOOST_SPIRIT_X4_CLASSIFY(unassigned)

    BOOST_SPIRIT_X4_CLASSIFY(dash_punctuation)
    BOOST_SPIRIT_X4_CLASSIFY(open_punctuation)
    BOOST_SPIRIT_X4_CLASSIFY(close_punctuation)
    BOOST_SPIRIT_X4_CLASSIFY(connector_punctuation)
    BOOST_SPIRIT_X4_CLASSIFY(other_punctuation)
    BOOST_SPIRIT_X4_CLASSIFY(initial_punctuation)
    BOOST_SPIRIT_X4_CLASSIFY(final_punctuation)

    BOOST_SPIRIT_X4_CLASSIFY(math_symbol)
    BOOST_SPIRIT_X4_CLASSIFY(currency_symbol)
    BOOST_SPIRIT_X4_CLASSIFY(modifier_symbol)
    BOOST_SPIRIT_X4_CLASSIFY(other_symbol)

    // Unicode Derived Categories

    BOOST_SPIRIT_X4_CLASSIFY(alphabetic)
    BOOST_SPIRIT_X4_CLASSIFY(uppercase)
    BOOST_SPIRIT_X4_CLASSIFY(lowercase)
    BOOST_SPIRIT_X4_CLASSIFY(white_space)
    BOOST_SPIRIT_X4_CLASSIFY(hex_digit)
    BOOST_SPIRIT_X4_CLASSIFY(noncharacter_code_point)
    BOOST_SPIRIT_X4_CLASSIFY(default_ignorable_code_point)

    // Unicode Scripts

    BOOST_SPIRIT_X4_CLASSIFY(adlam)
    BOOST_SPIRIT_X4_CLASSIFY(caucasian_albanian)
    BOOST_SPIRIT_X4_CLASSIFY(ahom)
    BOOST_SPIRIT_X4_CLASSIFY(arabic)
    BOOST_SPIRIT_X4_CLASSIFY(imperial_aramaic)
    BOOST_SPIRIT_X4_CLASSIFY(armenian)
    BOOST_SPIRIT_X4_CLASSIFY(avestan)
    BOOST_SPIRIT_X4_CLASSIFY(balinese)
    BOOST_SPIRIT_X4_CLASSIFY(bamum)
    BOOST_SPIRIT_X4_CLASSIFY(bassa_vah)
    BOOST_SPIRIT_X4_CLASSIFY(batak)
    BOOST_SPIRIT_X4_CLASSIFY(bengali)
    BOOST_SPIRIT_X4_CLASSIFY(bhaiksuki)
    BOOST_SPIRIT_X4_CLASSIFY(bopomofo)
    BOOST_SPIRIT_X4_CLASSIFY(brahmi)
    BOOST_SPIRIT_X4_CLASSIFY(braille)
    BOOST_SPIRIT_X4_CLASSIFY(buginese)
    BOOST_SPIRIT_X4_CLASSIFY(buhid)
    BOOST_SPIRIT_X4_CLASSIFY(chakma)
    BOOST_SPIRIT_X4_CLASSIFY(canadian_aboriginal)
    BOOST_SPIRIT_X4_CLASSIFY(carian)
    BOOST_SPIRIT_X4_CLASSIFY(cham)
    BOOST_SPIRIT_X4_CLASSIFY(cherokee)
    BOOST_SPIRIT_X4_CLASSIFY(chorasmian)
    BOOST_SPIRIT_X4_CLASSIFY(coptic)
    BOOST_SPIRIT_X4_CLASSIFY(cypro_minoan)
    BOOST_SPIRIT_X4_CLASSIFY(cypriot)
    BOOST_SPIRIT_X4_CLASSIFY(cyrillic)
    BOOST_SPIRIT_X4_CLASSIFY(devanagari)
    BOOST_SPIRIT_X4_CLASSIFY(dives_akuru)
    BOOST_SPIRIT_X4_CLASSIFY(dogra)
    BOOST_SPIRIT_X4_CLASSIFY(deseret)
    BOOST_SPIRIT_X4_CLASSIFY(duployan)
    BOOST_SPIRIT_X4_CLASSIFY(egyptian_hieroglyphs)
    BOOST_SPIRIT_X4_CLASSIFY(elbasan)
    BOOST_SPIRIT_X4_CLASSIFY(elymaic)
    BOOST_SPIRIT_X4_CLASSIFY(ethiopic)
    BOOST_SPIRIT_X4_CLASSIFY(georgian)
    BOOST_SPIRIT_X4_CLASSIFY(glagolitic)
    BOOST_SPIRIT_X4_CLASSIFY(gunjala_gondi)
    BOOST_SPIRIT_X4_CLASSIFY(masaram_gondi)
    BOOST_SPIRIT_X4_CLASSIFY(gothic)
    BOOST_SPIRIT_X4_CLASSIFY(grantha)
    BOOST_SPIRIT_X4_CLASSIFY(greek)
    BOOST_SPIRIT_X4_CLASSIFY(gujarati)
    BOOST_SPIRIT_X4_CLASSIFY(gurmukhi)
    BOOST_SPIRIT_X4_CLASSIFY(hangul)
    BOOST_SPIRIT_X4_CLASSIFY(han)
    BOOST_SPIRIT_X4_CLASSIFY(hanunoo)
    BOOST_SPIRIT_X4_CLASSIFY(hatran)
    BOOST_SPIRIT_X4_CLASSIFY(hebrew)
    BOOST_SPIRIT_X4_CLASSIFY(hiragana)
    BOOST_SPIRIT_X4_CLASSIFY(anatolian_hieroglyphs)
    BOOST_SPIRIT_X4_CLASSIFY(pahawh_hmong)
    BOOST_SPIRIT_X4_CLASSIFY(nyiakeng_puachue_hmong)
    BOOST_SPIRIT_X4_CLASSIFY(katakana_or_hiragana)
    BOOST_SPIRIT_X4_CLASSIFY(old_hungarian)
    BOOST_SPIRIT_X4_CLASSIFY(old_italic)
    BOOST_SPIRIT_X4_CLASSIFY(javanese)
    BOOST_SPIRIT_X4_CLASSIFY(kayah_li)
    BOOST_SPIRIT_X4_CLASSIFY(katakana)
    BOOST_SPIRIT_X4_CLASSIFY(kawi)
    BOOST_SPIRIT_X4_CLASSIFY(kharoshthi)
    BOOST_SPIRIT_X4_CLASSIFY(khmer)
    BOOST_SPIRIT_X4_CLASSIFY(khojki)
    BOOST_SPIRIT_X4_CLASSIFY(khitan_small_script)
    BOOST_SPIRIT_X4_CLASSIFY(kannada)
    BOOST_SPIRIT_X4_CLASSIFY(kaithi)
    BOOST_SPIRIT_X4_CLASSIFY(tai_tham)
    BOOST_SPIRIT_X4_CLASSIFY(lao)
    BOOST_SPIRIT_X4_CLASSIFY(latin)
    BOOST_SPIRIT_X4_CLASSIFY(lepcha)
    BOOST_SPIRIT_X4_CLASSIFY(limbu)
    BOOST_SPIRIT_X4_CLASSIFY(linear_a)
    BOOST_SPIRIT_X4_CLASSIFY(linear_b)
    BOOST_SPIRIT_X4_CLASSIFY(lisu)
    BOOST_SPIRIT_X4_CLASSIFY(lycian)
    BOOST_SPIRIT_X4_CLASSIFY(lydian)
    BOOST_SPIRIT_X4_CLASSIFY(mahajani)
    BOOST_SPIRIT_X4_CLASSIFY(makasar)
    BOOST_SPIRIT_X4_CLASSIFY(mandaic)
    BOOST_SPIRIT_X4_CLASSIFY(manichaean)
    BOOST_SPIRIT_X4_CLASSIFY(marchen)
    BOOST_SPIRIT_X4_CLASSIFY(medefaidrin)
    BOOST_SPIRIT_X4_CLASSIFY(mende_kikakui)
    BOOST_SPIRIT_X4_CLASSIFY(meroitic_cursive)
    BOOST_SPIRIT_X4_CLASSIFY(meroitic_hieroglyphs)
    BOOST_SPIRIT_X4_CLASSIFY(malayalam)
    BOOST_SPIRIT_X4_CLASSIFY(modi)
    BOOST_SPIRIT_X4_CLASSIFY(mongolian)
    BOOST_SPIRIT_X4_CLASSIFY(mro)
    BOOST_SPIRIT_X4_CLASSIFY(meetei_mayek)
    BOOST_SPIRIT_X4_CLASSIFY(multani)
    BOOST_SPIRIT_X4_CLASSIFY(myanmar)
    BOOST_SPIRIT_X4_CLASSIFY(nag_mundari)
    BOOST_SPIRIT_X4_CLASSIFY(nandinagari)
    BOOST_SPIRIT_X4_CLASSIFY(old_north_arabian)
    BOOST_SPIRIT_X4_CLASSIFY(nabataean)
    BOOST_SPIRIT_X4_CLASSIFY(newa)
    BOOST_SPIRIT_X4_CLASSIFY(nko)
    BOOST_SPIRIT_X4_CLASSIFY(nushu)
    BOOST_SPIRIT_X4_CLASSIFY(ogham)
    BOOST_SPIRIT_X4_CLASSIFY(ol_chiki)
    BOOST_SPIRIT_X4_CLASSIFY(old_turkic)
    BOOST_SPIRIT_X4_CLASSIFY(oriya)
    BOOST_SPIRIT_X4_CLASSIFY(osage)
    BOOST_SPIRIT_X4_CLASSIFY(osmanya)
    BOOST_SPIRIT_X4_CLASSIFY(old_uyghur)
    BOOST_SPIRIT_X4_CLASSIFY(palmyrene)
    BOOST_SPIRIT_X4_CLASSIFY(pau_cin_hau)
    BOOST_SPIRIT_X4_CLASSIFY(old_permic)
    BOOST_SPIRIT_X4_CLASSIFY(phags_pa)
    BOOST_SPIRIT_X4_CLASSIFY(inscriptional_pahlavi)
    BOOST_SPIRIT_X4_CLASSIFY(psalter_pahlavi)
    BOOST_SPIRIT_X4_CLASSIFY(phoenician)
    BOOST_SPIRIT_X4_CLASSIFY(miao)
    BOOST_SPIRIT_X4_CLASSIFY(inscriptional_parthian)
    BOOST_SPIRIT_X4_CLASSIFY(rejang)
    BOOST_SPIRIT_X4_CLASSIFY(hanifi_rohingya)
    BOOST_SPIRIT_X4_CLASSIFY(runic)
    BOOST_SPIRIT_X4_CLASSIFY(samaritan)
    BOOST_SPIRIT_X4_CLASSIFY(old_south_arabian)
    BOOST_SPIRIT_X4_CLASSIFY(saurashtra)
    BOOST_SPIRIT_X4_CLASSIFY(signwriting)
    BOOST_SPIRIT_X4_CLASSIFY(shavian)
    BOOST_SPIRIT_X4_CLASSIFY(sharada)
    BOOST_SPIRIT_X4_CLASSIFY(siddham)
    BOOST_SPIRIT_X4_CLASSIFY(khudawadi)
    BOOST_SPIRIT_X4_CLASSIFY(sinhala)
    BOOST_SPIRIT_X4_CLASSIFY(sogdian)
    BOOST_SPIRIT_X4_CLASSIFY(old_sogdian)
    BOOST_SPIRIT_X4_CLASSIFY(sora_sompeng)
    BOOST_SPIRIT_X4_CLASSIFY(soyombo)
    BOOST_SPIRIT_X4_CLASSIFY(sundanese)
    BOOST_SPIRIT_X4_CLASSIFY(syloti_nagri)
    BOOST_SPIRIT_X4_CLASSIFY(syriac)
    BOOST_SPIRIT_X4_CLASSIFY(tagbanwa)
    BOOST_SPIRIT_X4_CLASSIFY(takri)
    BOOST_SPIRIT_X4_CLASSIFY(tai_le)
    BOOST_SPIRIT_X4_CLASSIFY(new_tai_lue)
    BOOST_SPIRIT_X4_CLASSIFY(tamil)
    BOOST_SPIRIT_X4_CLASSIFY(tangut)
    BOOST_SPIRIT_X4_CLASSIFY(tai_viet)
    BOOST_SPIRIT_X4_CLASSIFY(telugu)
    BOOST_SPIRIT_X4_CLASSIFY(tifinagh)
    BOOST_SPIRIT_X4_CLASSIFY(tagalog)
    BOOST_SPIRIT_X4_CLASSIFY(thaana)
    BOOST_SPIRIT_X4_CLASSIFY(thai)
    BOOST_SPIRIT_X4_CLASSIFY(tibetan)
    BOOST_SPIRIT_X4_CLASSIFY(tirhuta)
    BOOST_SPIRIT_X4_CLASSIFY(tangsa)
    BOOST_SPIRIT_X4_CLASSIFY(toto)
    BOOST_SPIRIT_X4_CLASSIFY(ugaritic)
    BOOST_SPIRIT_X4_CLASSIFY(vai)
    BOOST_SPIRIT_X4_CLASSIFY(vithkuqi)
    BOOST_SPIRIT_X4_CLASSIFY(warang_citi)
    BOOST_SPIRIT_X4_CLASSIFY(wancho)
    BOOST_SPIRIT_X4_CLASSIFY(old_persian)
    BOOST_SPIRIT_X4_CLASSIFY(cuneiform)
    BOOST_SPIRIT_X4_CLASSIFY(yezidi)
    BOOST_SPIRIT_X4_CLASSIFY(yi)
    BOOST_SPIRIT_X4_CLASSIFY(zanabazar_square)
    BOOST_SPIRIT_X4_CLASSIFY(inherited)
    BOOST_SPIRIT_X4_CLASSIFY(common)
    BOOST_SPIRIT_X4_CLASSIFY(unknown)

#undef BOOST_SPIRIT_X4_BASIC_CLASSIFY
#undef BOOST_SPIRIT_X4_CLASSIFY
};

} // detail

template<class Tag>
struct unicode_char_class : char_parser<char_encoding::unicode, unicode_char_class<Tag>>
{
    using encoding_type = char_encoding::unicode;
    using tag = Tag;
    using char_type = typename encoding_type::char_type;
    using attribute_type = char_type;

    static constexpr bool has_attribute = true;

    [[nodiscard]] static constexpr bool
    test(char_encoding::unicode::classify_type const classify_ch, auto const& /* ctx */) noexcept
    {
        static_assert(noexcept(encoding_type::ischar(classify_ch) && detail::unicode_char_class_base::is(tag{}, classify_ch)));
        return encoding_type::ischar(classify_ch) && detail::unicode_char_class_base::is(tag{}, classify_ch);
    }

    static constexpr void test(auto const, auto const&) = delete; // Mixing incompatible char types is not allowed
};

#define BOOST_SPIRIT_X4_CHAR_CLASS(name) \
    namespace unicode { \
    [[maybe_unused]] inline constexpr unicode_char_class<char_classes::unicode::name##_tag> name{}; \
    } /* unicode */ \
    namespace parsers::unicode { \
    using x4::unicode::name; \
    } /* parsers::unicode */

// Unicode Major Categories

BOOST_SPIRIT_X4_CHAR_CLASS(alnum)
BOOST_SPIRIT_X4_CHAR_CLASS(alpha)
BOOST_SPIRIT_X4_CHAR_CLASS(digit)
BOOST_SPIRIT_X4_CHAR_CLASS(xdigit)
BOOST_SPIRIT_X4_CHAR_CLASS(cntrl)
BOOST_SPIRIT_X4_CHAR_CLASS(graph)
BOOST_SPIRIT_X4_CHAR_CLASS(lower)
BOOST_SPIRIT_X4_CHAR_CLASS(print)
BOOST_SPIRIT_X4_CHAR_CLASS(punct)
BOOST_SPIRIT_X4_CHAR_CLASS(space)
BOOST_SPIRIT_X4_CHAR_CLASS(blank)
BOOST_SPIRIT_X4_CHAR_CLASS(upper)

// Unicode Major Categories

BOOST_SPIRIT_X4_CHAR_CLASS(letter)
BOOST_SPIRIT_X4_CHAR_CLASS(mark)
BOOST_SPIRIT_X4_CHAR_CLASS(number)
BOOST_SPIRIT_X4_CHAR_CLASS(separator)
BOOST_SPIRIT_X4_CHAR_CLASS(other)
BOOST_SPIRIT_X4_CHAR_CLASS(punctuation)
BOOST_SPIRIT_X4_CHAR_CLASS(symbol)

// Unicode General Categories

BOOST_SPIRIT_X4_CHAR_CLASS(uppercase_letter)
BOOST_SPIRIT_X4_CHAR_CLASS(lowercase_letter)
BOOST_SPIRIT_X4_CHAR_CLASS(titlecase_letter)
BOOST_SPIRIT_X4_CHAR_CLASS(modifier_letter)
BOOST_SPIRIT_X4_CHAR_CLASS(other_letter)

BOOST_SPIRIT_X4_CHAR_CLASS(nonspacing_mark)
BOOST_SPIRIT_X4_CHAR_CLASS(enclosing_mark)
BOOST_SPIRIT_X4_CHAR_CLASS(spacing_mark)

BOOST_SPIRIT_X4_CHAR_CLASS(decimal_number)
BOOST_SPIRIT_X4_CHAR_CLASS(letter_number)
BOOST_SPIRIT_X4_CHAR_CLASS(other_number)

BOOST_SPIRIT_X4_CHAR_CLASS(space_separator)
BOOST_SPIRIT_X4_CHAR_CLASS(line_separator)
BOOST_SPIRIT_X4_CHAR_CLASS(paragraph_separator)

BOOST_SPIRIT_X4_CHAR_CLASS(control)
BOOST_SPIRIT_X4_CHAR_CLASS(format)
BOOST_SPIRIT_X4_CHAR_CLASS(private_use)
BOOST_SPIRIT_X4_CHAR_CLASS(surrogate)
BOOST_SPIRIT_X4_CHAR_CLASS(unassigned)

BOOST_SPIRIT_X4_CHAR_CLASS(dash_punctuation)
BOOST_SPIRIT_X4_CHAR_CLASS(open_punctuation)
BOOST_SPIRIT_X4_CHAR_CLASS(close_punctuation)
BOOST_SPIRIT_X4_CHAR_CLASS(connector_punctuation)
BOOST_SPIRIT_X4_CHAR_CLASS(other_punctuation)
BOOST_SPIRIT_X4_CHAR_CLASS(initial_punctuation)
BOOST_SPIRIT_X4_CHAR_CLASS(final_punctuation)

BOOST_SPIRIT_X4_CHAR_CLASS(math_symbol)
BOOST_SPIRIT_X4_CHAR_CLASS(currency_symbol)
BOOST_SPIRIT_X4_CHAR_CLASS(modifier_symbol)
BOOST_SPIRIT_X4_CHAR_CLASS(other_symbol)

// Unicode Derived Categories

BOOST_SPIRIT_X4_CHAR_CLASS(alphabetic)
BOOST_SPIRIT_X4_CHAR_CLASS(uppercase)
BOOST_SPIRIT_X4_CHAR_CLASS(lowercase)
BOOST_SPIRIT_X4_CHAR_CLASS(white_space)
BOOST_SPIRIT_X4_CHAR_CLASS(hex_digit)
BOOST_SPIRIT_X4_CHAR_CLASS(noncharacter_code_point)
BOOST_SPIRIT_X4_CHAR_CLASS(default_ignorable_code_point)

// Unicode Scripts

BOOST_SPIRIT_X4_CHAR_CLASS(adlam)
BOOST_SPIRIT_X4_CHAR_CLASS(caucasian_albanian)
BOOST_SPIRIT_X4_CHAR_CLASS(ahom)
BOOST_SPIRIT_X4_CHAR_CLASS(arabic)
BOOST_SPIRIT_X4_CHAR_CLASS(imperial_aramaic)
BOOST_SPIRIT_X4_CHAR_CLASS(armenian)
BOOST_SPIRIT_X4_CHAR_CLASS(avestan)
BOOST_SPIRIT_X4_CHAR_CLASS(balinese)
BOOST_SPIRIT_X4_CHAR_CLASS(bamum)
BOOST_SPIRIT_X4_CHAR_CLASS(bassa_vah)
BOOST_SPIRIT_X4_CHAR_CLASS(batak)
BOOST_SPIRIT_X4_CHAR_CLASS(bengali)
BOOST_SPIRIT_X4_CHAR_CLASS(bhaiksuki)
BOOST_SPIRIT_X4_CHAR_CLASS(bopomofo)
BOOST_SPIRIT_X4_CHAR_CLASS(brahmi)
BOOST_SPIRIT_X4_CHAR_CLASS(braille)
BOOST_SPIRIT_X4_CHAR_CLASS(buginese)
BOOST_SPIRIT_X4_CHAR_CLASS(buhid)
BOOST_SPIRIT_X4_CHAR_CLASS(chakma)
BOOST_SPIRIT_X4_CHAR_CLASS(canadian_aboriginal)
BOOST_SPIRIT_X4_CHAR_CLASS(carian)
BOOST_SPIRIT_X4_CHAR_CLASS(cham)
BOOST_SPIRIT_X4_CHAR_CLASS(cherokee)
BOOST_SPIRIT_X4_CHAR_CLASS(chorasmian)
BOOST_SPIRIT_X4_CHAR_CLASS(coptic)
BOOST_SPIRIT_X4_CHAR_CLASS(cypro_minoan)
BOOST_SPIRIT_X4_CHAR_CLASS(cypriot)
BOOST_SPIRIT_X4_CHAR_CLASS(cyrillic)
BOOST_SPIRIT_X4_CHAR_CLASS(devanagari)
BOOST_SPIRIT_X4_CHAR_CLASS(dives_akuru)
BOOST_SPIRIT_X4_CHAR_CLASS(dogra)
BOOST_SPIRIT_X4_CHAR_CLASS(deseret)
BOOST_SPIRIT_X4_CHAR_CLASS(duployan)
BOOST_SPIRIT_X4_CHAR_CLASS(egyptian_hieroglyphs)
BOOST_SPIRIT_X4_CHAR_CLASS(elbasan)
BOOST_SPIRIT_X4_CHAR_CLASS(elymaic)
BOOST_SPIRIT_X4_CHAR_CLASS(ethiopic)
BOOST_SPIRIT_X4_CHAR_CLASS(georgian)
BOOST_SPIRIT_X4_CHAR_CLASS(glagolitic)
BOOST_SPIRIT_X4_CHAR_CLASS(gunjala_gondi)
BOOST_SPIRIT_X4_CHAR_CLASS(masaram_gondi)
BOOST_SPIRIT_X4_CHAR_CLASS(gothic)
BOOST_SPIRIT_X4_CHAR_CLASS(grantha)
BOOST_SPIRIT_X4_CHAR_CLASS(greek)
BOOST_SPIRIT_X4_CHAR_CLASS(gujarati)
BOOST_SPIRIT_X4_CHAR_CLASS(gurmukhi)
BOOST_SPIRIT_X4_CHAR_CLASS(hangul)
BOOST_SPIRIT_X4_CHAR_CLASS(han)
BOOST_SPIRIT_X4_CHAR_CLASS(hanunoo)
BOOST_SPIRIT_X4_CHAR_CLASS(hatran)
BOOST_SPIRIT_X4_CHAR_CLASS(hebrew)
BOOST_SPIRIT_X4_CHAR_CLASS(hiragana)
BOOST_SPIRIT_X4_CHAR_CLASS(anatolian_hieroglyphs)
BOOST_SPIRIT_X4_CHAR_CLASS(pahawh_hmong)
BOOST_SPIRIT_X4_CHAR_CLASS(nyiakeng_puachue_hmong)
BOOST_SPIRIT_X4_CHAR_CLASS(katakana_or_hiragana)
BOOST_SPIRIT_X4_CHAR_CLASS(old_hungarian)
BOOST_SPIRIT_X4_CHAR_CLASS(old_italic)
BOOST_SPIRIT_X4_CHAR_CLASS(javanese)
BOOST_SPIRIT_X4_CHAR_CLASS(kayah_li)
BOOST_SPIRIT_X4_CHAR_CLASS(katakana)
BOOST_SPIRIT_X4_CHAR_CLASS(kawi)
BOOST_SPIRIT_X4_CHAR_CLASS(kharoshthi)
BOOST_SPIRIT_X4_CHAR_CLASS(khmer)
BOOST_SPIRIT_X4_CHAR_CLASS(khojki)
BOOST_SPIRIT_X4_CHAR_CLASS(khitan_small_script)
BOOST_SPIRIT_X4_CHAR_CLASS(kannada)
BOOST_SPIRIT_X4_CHAR_CLASS(kaithi)
BOOST_SPIRIT_X4_CHAR_CLASS(tai_tham)
BOOST_SPIRIT_X4_CHAR_CLASS(lao)
BOOST_SPIRIT_X4_CHAR_CLASS(latin)
BOOST_SPIRIT_X4_CHAR_CLASS(lepcha)
BOOST_SPIRIT_X4_CHAR_CLASS(limbu)
BOOST_SPIRIT_X4_CHAR_CLASS(linear_a)
BOOST_SPIRIT_X4_CHAR_CLASS(linear_b)
BOOST_SPIRIT_X4_CHAR_CLASS(lisu)
BOOST_SPIRIT_X4_CHAR_CLASS(lycian)
BOOST_SPIRIT_X4_CHAR_CLASS(lydian)
BOOST_SPIRIT_X4_CHAR_CLASS(mahajani)
BOOST_SPIRIT_X4_CHAR_CLASS(makasar)
BOOST_SPIRIT_X4_CHAR_CLASS(mandaic)
BOOST_SPIRIT_X4_CHAR_CLASS(manichaean)
BOOST_SPIRIT_X4_CHAR_CLASS(marchen)
BOOST_SPIRIT_X4_CHAR_CLASS(medefaidrin)
BOOST_SPIRIT_X4_CHAR_CLASS(mende_kikakui)
BOOST_SPIRIT_X4_CHAR_CLASS(meroitic_cursive)
BOOST_SPIRIT_X4_CHAR_CLASS(meroitic_hieroglyphs)
BOOST_SPIRIT_X4_CHAR_CLASS(malayalam)
BOOST_SPIRIT_X4_CHAR_CLASS(modi)
BOOST_SPIRIT_X4_CHAR_CLASS(mongolian)
BOOST_SPIRIT_X4_CHAR_CLASS(mro)
BOOST_SPIRIT_X4_CHAR_CLASS(meetei_mayek)
BOOST_SPIRIT_X4_CHAR_CLASS(multani)
BOOST_SPIRIT_X4_CHAR_CLASS(myanmar)
BOOST_SPIRIT_X4_CHAR_CLASS(nag_mundari)
BOOST_SPIRIT_X4_CHAR_CLASS(nandinagari)
BOOST_SPIRIT_X4_CHAR_CLASS(old_north_arabian)
BOOST_SPIRIT_X4_CHAR_CLASS(nabataean)
BOOST_SPIRIT_X4_CHAR_CLASS(newa)
BOOST_SPIRIT_X4_CHAR_CLASS(nko)
BOOST_SPIRIT_X4_CHAR_CLASS(nushu)
BOOST_SPIRIT_X4_CHAR_CLASS(ogham)
BOOST_SPIRIT_X4_CHAR_CLASS(ol_chiki)
BOOST_SPIRIT_X4_CHAR_CLASS(old_turkic)
BOOST_SPIRIT_X4_CHAR_CLASS(oriya)
BOOST_SPIRIT_X4_CHAR_CLASS(osage)
BOOST_SPIRIT_X4_CHAR_CLASS(osmanya)
BOOST_SPIRIT_X4_CHAR_CLASS(old_uyghur)
BOOST_SPIRIT_X4_CHAR_CLASS(palmyrene)
BOOST_SPIRIT_X4_CHAR_CLASS(pau_cin_hau)
BOOST_SPIRIT_X4_CHAR_CLASS(old_permic)
BOOST_SPIRIT_X4_CHAR_CLASS(phags_pa)
BOOST_SPIRIT_X4_CHAR_CLASS(inscriptional_pahlavi)
BOOST_SPIRIT_X4_CHAR_CLASS(psalter_pahlavi)
BOOST_SPIRIT_X4_CHAR_CLASS(phoenician)
BOOST_SPIRIT_X4_CHAR_CLASS(miao)
BOOST_SPIRIT_X4_CHAR_CLASS(inscriptional_parthian)
BOOST_SPIRIT_X4_CHAR_CLASS(rejang)
BOOST_SPIRIT_X4_CHAR_CLASS(hanifi_rohingya)
BOOST_SPIRIT_X4_CHAR_CLASS(runic)
BOOST_SPIRIT_X4_CHAR_CLASS(samaritan)
BOOST_SPIRIT_X4_CHAR_CLASS(old_south_arabian)
BOOST_SPIRIT_X4_CHAR_CLASS(saurashtra)
BOOST_SPIRIT_X4_CHAR_CLASS(signwriting)
BOOST_SPIRIT_X4_CHAR_CLASS(shavian)
BOOST_SPIRIT_X4_CHAR_CLASS(sharada)
BOOST_SPIRIT_X4_CHAR_CLASS(siddham)
BOOST_SPIRIT_X4_CHAR_CLASS(khudawadi)
BOOST_SPIRIT_X4_CHAR_CLASS(sinhala)
BOOST_SPIRIT_X4_CHAR_CLASS(sogdian)
BOOST_SPIRIT_X4_CHAR_CLASS(old_sogdian)
BOOST_SPIRIT_X4_CHAR_CLASS(sora_sompeng)
BOOST_SPIRIT_X4_CHAR_CLASS(soyombo)
BOOST_SPIRIT_X4_CHAR_CLASS(sundanese)
BOOST_SPIRIT_X4_CHAR_CLASS(syloti_nagri)
BOOST_SPIRIT_X4_CHAR_CLASS(syriac)
BOOST_SPIRIT_X4_CHAR_CLASS(tagbanwa)
BOOST_SPIRIT_X4_CHAR_CLASS(takri)
BOOST_SPIRIT_X4_CHAR_CLASS(tai_le)
BOOST_SPIRIT_X4_CHAR_CLASS(new_tai_lue)
BOOST_SPIRIT_X4_CHAR_CLASS(tamil)
BOOST_SPIRIT_X4_CHAR_CLASS(tangut)
BOOST_SPIRIT_X4_CHAR_CLASS(tai_viet)
BOOST_SPIRIT_X4_CHAR_CLASS(telugu)
BOOST_SPIRIT_X4_CHAR_CLASS(tifinagh)
BOOST_SPIRIT_X4_CHAR_CLASS(tagalog)
BOOST_SPIRIT_X4_CHAR_CLASS(thaana)
BOOST_SPIRIT_X4_CHAR_CLASS(thai)
BOOST_SPIRIT_X4_CHAR_CLASS(tibetan)
BOOST_SPIRIT_X4_CHAR_CLASS(tirhuta)
BOOST_SPIRIT_X4_CHAR_CLASS(tangsa)
BOOST_SPIRIT_X4_CHAR_CLASS(toto)
BOOST_SPIRIT_X4_CHAR_CLASS(ugaritic)
BOOST_SPIRIT_X4_CHAR_CLASS(vai)
BOOST_SPIRIT_X4_CHAR_CLASS(vithkuqi)
BOOST_SPIRIT_X4_CHAR_CLASS(warang_citi)
BOOST_SPIRIT_X4_CHAR_CLASS(wancho)
BOOST_SPIRIT_X4_CHAR_CLASS(old_persian)
BOOST_SPIRIT_X4_CHAR_CLASS(cuneiform)
BOOST_SPIRIT_X4_CHAR_CLASS(yezidi)
BOOST_SPIRIT_X4_CHAR_CLASS(yi)
BOOST_SPIRIT_X4_CHAR_CLASS(zanabazar_square)
BOOST_SPIRIT_X4_CHAR_CLASS(inherited)
BOOST_SPIRIT_X4_CHAR_CLASS(common)
BOOST_SPIRIT_X4_CHAR_CLASS(unknown)

#undef BOOST_SPIRIT_X4_CHAR_CLASS

} // boost::spirit::x4

#endif
