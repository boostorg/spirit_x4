#ifndef BOOST_SPIRIT_X4_CHAR_NEGATED_CHAR_HPP
#define BOOST_SPIRIT_X4_CHAR_NEGATED_CHAR_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/traits/attribute.hpp>
#include <boost/spirit/x4/char/char_parser.hpp>

#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

// `negated_char_parser` handles `~cp`, where `cp` is a `char_parser`
template<class Positive>
struct negated_char_parser : char_parser<typename Positive::encoding_type, negated_char_parser<Positive>>
{
    using encoding_type = typename Positive::encoding_type;

    template<class PositiveT>
        requires
            (!std::is_same_v<std::remove_cvref_t<PositiveT>, negated_char_parser>) &&
            std::is_constructible_v<Positive, PositiveT>
    constexpr explicit negated_char_parser(PositiveT&& positive)
        noexcept(std::is_nothrow_constructible_v<Positive, PositiveT>)
        : positive_(std::forward<PositiveT>(positive))
    {}

    template<class CharT, class Context>
    [[nodiscard]] constexpr bool
    test(CharT ch, Context const& ctx) const noexcept
    {
        static_assert(noexcept(!positive_.test(ch, ctx)));
        return !positive_.test(ch, ctx);
    }

    [[nodiscard]] constexpr Positive const& positive() const noexcept
    {
        return positive_;
    }

private:
    Positive positive_;
};

template<class Encoding, class Positive>
[[nodiscard]] constexpr negated_char_parser<Positive>
operator~(char_parser<Encoding, Positive> const& cp)
    noexcept(std::is_nothrow_constructible_v<negated_char_parser<Positive>, Positive const&>)
{
    return negated_char_parser<Positive>{cp.derived()};
}

template<class Positive>
[[nodiscard]] constexpr Positive const&
operator~(negated_char_parser<Positive> const& cp) noexcept
{
    return cp.positive();
}

} // boost::spirit::x4

namespace boost::spirit::x4::traits {

template<class Positive, class Context>
struct attribute_of<negated_char_parser<Positive>, Context>
    : attribute_of<Positive, Context>
{};

template<class Positive, class Context>
struct has_attribute<negated_char_parser<Positive>, Context>
    : has_attribute<Positive, Context>
{};

} // boost::spirit::x4::traits

#endif
