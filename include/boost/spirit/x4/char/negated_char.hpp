/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X4_NEGATED_CHAR_PARSER_APR_16_2006_0906AM
#define BOOST_SPIRIT_X4_NEGATED_CHAR_PARSER_APR_16_2006_0906AM

#include <boost/spirit/x4/traits/attribute.hpp>
#include <boost/spirit/x4/char/char_parser.hpp>

#include <type_traits>
#include <utility>

namespace boost::spirit::x4
{
    // `negated_char_parser` handles `~cp`, where `cp` is a `char_parser`
    template <typename Positive>
    struct negated_char_parser : char_parser<negated_char_parser<Positive>>
    {
        template <typename PositiveT>
            requires
                (!std::is_same_v<std::remove_cvref_t<PositiveT>, negated_char_parser>) &&
                std::is_constructible_v<Positive, PositiveT>
        constexpr negated_char_parser(PositiveT&& positive)
            noexcept(std::is_nothrow_constructible_v<Positive, PositiveT>)
            : positive_(std::forward<PositiveT>(positive))
        {}

        template <typename CharParam, typename Context>
        [[nodiscard]] constexpr bool
        test(CharParam ch, Context const& context) const noexcept
        {
            static_assert(noexcept(!positive_.test(ch, context)));
            return !positive_.test(ch, context);
        }

        [[nodiscard]] constexpr Positive const& positive() const noexcept
        {
            return positive_;
        }

    private:
        Positive positive_;
    };

    template <typename Positive>
    [[nodiscard]] constexpr negated_char_parser<Positive>
    operator~(char_parser<Positive> const& cp)
        noexcept(std::is_nothrow_constructible_v<negated_char_parser<Positive>, Positive const&>)
    {
        return { cp.derived() };
    }

    template <typename Positive>
    [[nodiscard]] constexpr Positive const&
    operator~(negated_char_parser<Positive> const& cp) noexcept
    {
        return cp.positive();
    }

} // boost::spirit::x4

namespace boost::spirit::x4::traits
{
    template <typename Positive, typename Context>
    struct attribute_of<negated_char_parser<Positive>, Context>
        : attribute_of<Positive, Context>
    {};

    template <typename Positive, typename Context>
    struct has_attribute<negated_char_parser<Positive>, Context>
        : has_attribute<Positive, Context>
    {};

} // boost::spirit::x4::traits

#endif
