#ifndef BOOST_SPIRIT_X4_CHAR_CHAR_PARSER_HPP
#define BOOST_SPIRIT_X4_CHAR_CHAR_PARSER_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>
#include <boost/spirit/x4/core/move_to.hpp>

#include <boost/spirit/x4/traits/string_traits.hpp>

#include <concepts>
#include <iterator>
#include <utility>

namespace boost::spirit::x4 {

template<class Encoding, class Derived>
struct char_parser : parser<Derived>
{
    using encoding_type = Encoding;
    using char_type = typename Encoding::char_type;
    using classify_type = typename Encoding::classify_type;

private:
    template<class Context>
    static constexpr bool has_static_test = requires(classify_type classify_ch, Context const& ctx) {
        { Derived::test(classify_ch, ctx) } -> std::same_as<bool>;
    };

public:
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires has_static_test<Context>
    [[nodiscard]] static constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr)
        noexcept(
            noexcept(x4::skip_over(first, last, ctx)) &&
            noexcept(first != last) &&
            noexcept(Derived::test(static_cast<classify_type>(*first), ctx)) &&
            noexcept(x4::move_to(std::iter_value_t<It>{*first}, attr)) &&
            noexcept(++first)
        )
    {
        static_assert(std::same_as<std::iter_value_t<It>, char_type>, "Mixing incompatible char types is not allowed");
        static_assert(!traits::CharLike<Attr> || std::same_as<Attr, char_type>, "Mixing incompatible char types is not allowed");

        x4::skip_over(first, last, ctx);

        if (first != last && Derived::test(static_cast<classify_type>(*first), ctx)) {
            x4::move_to(std::iter_value_t<It>{*first}, attr);
            ++first;
            return true;
        }
        return false;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
        requires (!has_static_test<Context>)
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr) const
        noexcept(
            noexcept(x4::skip_over(first, last, ctx)) &&
            noexcept(first != last) &&
            noexcept(this->derived().test(static_cast<classify_type>(*first), ctx)) &&
            noexcept(x4::move_to(std::iter_value_t<It>{*first}, attr)) &&
            noexcept(++first)
        )
    {
        static_assert(std::same_as<std::iter_value_t<It>, char_type>, "Mixing incompatible char types is not allowed");
        static_assert(!traits::CharLike<Attr> || std::same_as<Attr, char_type>, "Mixing incompatible char types is not allowed");

        x4::skip_over(first, last, ctx);

        if (first != last && this->derived().test(static_cast<classify_type>(*first), ctx)) {
            x4::move_to(std::iter_value_t<It>{*first}, attr);
            ++first;
            return true;
        }
        return false;
    }
};

} // boost::spirit::x4

#endif
