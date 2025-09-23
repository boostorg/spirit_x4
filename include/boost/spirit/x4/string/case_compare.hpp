#ifndef BOOST_SPIRIT_X4_STRING_CASE_COMPARE_HPP
#define BOOST_SPIRIT_X4_STRING_CASE_COMPARE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/core/context.hpp>
#include <boost/spirit/x4/char/char_class_tags.hpp>

#include <boost/spirit/x4/char_encoding/standard.hpp>

#ifndef BOOST_SPIRIT_X4_NO_STANDARD_WIDE
# include <boost/spirit/x4/char_encoding/standard_wide.hpp>
#endif

#ifdef BOOST_SPIRIT_X4_UNICODE
# include <boost/spirit/x4/char_encoding/unicode.hpp>
#endif

#include <concepts>

namespace boost::spirit::x4 {

template<class Encoding>
struct case_compare
{
    using encoding_type = Encoding;
    using char_type = typename Encoding::char_type;
    using classify_type = typename Encoding::classify_type;

    template<class Char, class CharSet>
    [[nodiscard]] static constexpr bool in_set(Char ch, CharSet const& set) noexcept
    {
        static_assert(std::same_as<Char, char_type> || std::same_as<Char, classify_type>);
        static_assert(noexcept(set.test(static_cast<classify_type>(ch))));
        return set.test(static_cast<classify_type>(ch));
    }

    template<class LChar, class RChar>
    [[nodiscard]] static constexpr int
    operator()(LChar lc, RChar rc) noexcept
    {
        static_assert(std::same_as<LChar, char_type> || std::same_as<LChar, classify_type>);
        static_assert(std::same_as<RChar, char_type> || std::same_as<RChar, classify_type>);
        return lc - rc;
    }

    template<class CharClassTag>
    [[nodiscard]] static constexpr CharClassTag get_char_class_tag(CharClassTag tag) noexcept
    {
        return tag;
    }
};

template<class Encoding>
struct no_case_compare
{
    using encoding_type = Encoding;
    using char_type = typename Encoding::char_type;
    using classify_type = typename Encoding::classify_type;

    template<class Char, class CharSet>
    [[nodiscard]] static constexpr bool in_set(Char ch, CharSet const& set) noexcept
    {
        static_assert(std::same_as<Char, char_type> || std::same_as<Char, classify_type>);
        auto const classify_ch = static_cast<classify_type>(ch);
        static_assert(noexcept(set.test(classify_ch)));
        return set.test(classify_ch)
            || set.test(static_cast<classify_type>(
                Encoding::islower(classify_ch) ? Encoding::toupper(classify_ch) : Encoding::tolower(classify_ch))
            );
    }

    template<class LChar, class RChar>
    [[nodiscard]] static constexpr int
    operator()(LChar lc, RChar rc) noexcept
    {
        static_assert(std::same_as<LChar, char_type> || std::same_as<LChar, classify_type>);
        static_assert(std::same_as<RChar, char_type> || std::same_as<RChar, classify_type>);

        auto const classify_lc = static_cast<classify_type>(lc);
        auto const classify_rc = static_cast<classify_type>(rc);
        return Encoding::islower(classify_rc)
            ? Encoding::tolower(classify_lc) - classify_rc : Encoding::toupper(classify_lc) - classify_rc;
    }

    template<class CharClassTag>
    [[nodiscard]] static constexpr CharClassTag get_char_class_tag(CharClassTag tag) noexcept
    {
        return tag;
    }

    [[nodiscard]] static constexpr alpha_tag get_char_class_tag(lower_tag) noexcept
    {
        return {};
    }

    [[nodiscard]] static constexpr alpha_tag get_char_class_tag(upper_tag) noexcept
    {
        return {};
    }
};

namespace detail {

struct case_compare_tag
{
    static constexpr bool is_unique = false;
};

struct case_compare_no_case_t {};
inline constexpr case_compare_no_case_t case_compare_no_case{};

} // detail

template<class Encoding, class Context>
[[nodiscard]] constexpr auto
get_case_compare(Context const&) noexcept
{
    if constexpr (has_context_of_v<Context, detail::case_compare_tag, detail::case_compare_no_case_t>) {
        return no_case_compare<Encoding>{};
    } else {
        return case_compare<Encoding>{};
    }
}

} // boost::spirit::x4

#endif
