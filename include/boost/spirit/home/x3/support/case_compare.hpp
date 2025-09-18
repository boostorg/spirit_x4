/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_SUPPORT_NO_CASE_SEPT_24_2014_1125PM
#define BOOST_SPIRIT_X3_SUPPORT_NO_CASE_SEPT_24_2014_1125PM

#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/char/char_class_tags.hpp>

#include <cstdint>

namespace boost::spirit::x3
{
    template <typename Encoding>
    struct case_compare
    {
        template <typename Char, typename CharSet>
        [[nodiscard]] constexpr bool in_set(Char ch, CharSet const& set) noexcept
        {
            static_assert(noexcept(set.test(ch)));
            return set.test(ch);
        }

        template <typename Char>
        [[nodiscard]] constexpr std::int32_t operator()(Char lc, Char rc) const noexcept
        {
            return lc - rc;
        }

        template <typename CharClassTag>
        [[nodiscard]] constexpr CharClassTag get_char_class_tag(CharClassTag tag) const noexcept
        {
            return tag;
        }
    };

    template <typename Encoding>
    struct no_case_compare
    {
        template <typename Char, typename CharSet>
        [[nodiscard]] bool in_set(Char ch_, CharSet const& set) noexcept // TODO: constexpr
        {
            using classify_type = typename Encoding::classify_type;
            auto ch = classify_type(ch_);
            static_assert(noexcept(set.test(ch)));
            return set.test(ch)
                || set.test(static_cast<classify_type>(
                    Encoding::islower(ch) ? Encoding::toupper(ch) : Encoding::tolower(ch))
                );
        }

        template <typename Char>
        [[nodiscard]] std::int32_t operator()(Char lc_, Char const rc_) const noexcept // TODO: constexpr
        {
            using classify_type = typename Encoding::classify_type;
            auto lc = classify_type(lc_);
            auto rc = classify_type(rc_);
            return Encoding::islower(rc)
                ? Encoding::tolower(lc) - rc : Encoding::toupper(lc) - rc;
        }

        template <typename CharClassTag>
        [[nodiscard]] constexpr CharClassTag get_char_class_tag(CharClassTag tag) const noexcept
        {
            return tag;
        }

        [[nodiscard]] constexpr alpha_tag get_char_class_tag(lower_tag) const noexcept
        {
            return {};
        }

        [[nodiscard]] constexpr alpha_tag get_char_class_tag(upper_tag) const noexcept
        {
            return {};
        }
    };

    namespace detail
    {
        struct no_case_tag_t {};
        inline constexpr no_case_tag_t no_case_tag{};

        template <typename Encoding>
        [[nodiscard]] constexpr case_compare<Encoding> get_case_compare_impl(unused_type const&) noexcept
        {
            return {};
        }

        template <typename Encoding>
        [[nodiscard]] constexpr no_case_compare<Encoding> get_case_compare_impl(no_case_tag_t const&) noexcept
        {
            return {};
        }

    } // detail

    template <typename Encoding, typename Context>
    [[nodiscard]] constexpr decltype(auto)
    get_case_compare(Context const& context) noexcept
    {
        return detail::get_case_compare_impl<Encoding>(x3::get<detail::no_case_tag_t>(context));
    }

} // boost::spirit::x3

#endif
