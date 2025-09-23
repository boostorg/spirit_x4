#ifndef BOOST_SPIRIT_X4_CHAR_CHAR_CLASS_HPP
#define BOOST_SPIRIT_X4_CHAR_CHAR_CLASS_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/char/char_parser.hpp>
#include <boost/spirit/x4/char/char_class_tags.hpp>
#include <boost/spirit/x4/char/detail/cast_char.hpp>

#include <boost/spirit/x4/string/case_compare.hpp>

#include <boost/spirit/x4/traits/string_traits.hpp>

#include <boost/spirit/x4/char_encoding/standard.hpp>

#ifndef BOOST_SPIRIT_X4_NO_STANDARD_WIDE
# include <boost/spirit/x4/char_encoding/standard_wide.hpp>
#endif

#include <concepts>

namespace boost::spirit::x4 {

namespace detail {

template<class Encoding>
struct char_class_base
{
    using classify_type = typename Encoding::classify_type;

#define BOOST_SPIRIT_X4_CLASSIFY(name) \
    template<class Char> \
    [[nodiscard]] static constexpr bool \
    is(name##_tag, Char ch) noexcept \
    { \
        static_assert(std::same_as<Char, classify_type>); \
        return (Encoding::is##name)(detail::cast_char<classify_type>(ch)); \
    }

    BOOST_SPIRIT_X4_CLASSIFY(char)
    BOOST_SPIRIT_X4_CLASSIFY(alnum)
    BOOST_SPIRIT_X4_CLASSIFY(alpha)
    BOOST_SPIRIT_X4_CLASSIFY(digit)
    BOOST_SPIRIT_X4_CLASSIFY(xdigit)
    BOOST_SPIRIT_X4_CLASSIFY(cntrl)
    BOOST_SPIRIT_X4_CLASSIFY(graph)
    BOOST_SPIRIT_X4_CLASSIFY(lower)
    BOOST_SPIRIT_X4_CLASSIFY(print)
    BOOST_SPIRIT_X4_CLASSIFY(punct)
    BOOST_SPIRIT_X4_CLASSIFY(space)
    BOOST_SPIRIT_X4_CLASSIFY(blank)
    BOOST_SPIRIT_X4_CLASSIFY(upper)

#undef BOOST_SPIRIT_X4_CLASSIFY
};

} // detail

template<class Encoding, class Tag>
struct char_class_parser : char_parser<Encoding, char_class_parser<Encoding, Tag>>
{
    using encoding_type = Encoding;
    using tag = Tag;
    using char_type = typename Encoding::char_type;
    using attribute_type = char_type;
    static constexpr bool has_attribute = true;

    [[nodiscard]] static constexpr bool
    test(typename Encoding::classify_type const classify_ch, auto const& ctx) noexcept
    {
        //static_assert(std::same_as<ClassifyCharT, typename Encoding::classify_type>);
        return encoding_type::ischar(classify_ch)
            && detail::char_class_base<Encoding>::is(
                x4::get_case_compare<Encoding>(ctx).get_char_class_tag(tag{}),
                static_cast<typename Encoding::classify_type>(classify_ch)
            );
    }

    static void
    test(auto const, auto const&) = delete; // Mixing incompatible char types is not allowed. Did you forget `static_cast<typename Encoding::classify_type>(ch)`?
};

#define BOOST_SPIRIT_X4_CHAR_CLASS(encoding, name) \
    using name##_type = char_class_parser<char_encoding::encoding, name##_tag>; \
    inline constexpr name##_type name{};

#define BOOST_SPIRIT_X4_CHAR_CLASSES(encoding) \
    namespace encoding \
    { \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, alnum) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, alpha) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, digit) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, xdigit) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, cntrl) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, graph) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, lower) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, print) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, punct) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, space) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, blank) \
        BOOST_SPIRIT_X4_CHAR_CLASS(encoding, upper) \
    } /* encoding */

BOOST_SPIRIT_X4_CHAR_CLASSES(standard)

#ifndef BOOST_SPIRIT_X4_NO_STANDARD_WIDE
BOOST_SPIRIT_X4_CHAR_CLASSES(standard_wide)
#endif

#undef BOOST_SPIRIT_X4_CHAR_CLASS
#undef BOOST_SPIRIT_X4_CHAR_CLASSES

using alnum_type  = standard::alnum_type;
using alpha_type  = standard::alpha_type;
using digit_type  = standard::digit_type;
using xdigit_type = standard::xdigit_type;
using cntrl_type  = standard::cntrl_type;
using graph_type  = standard::graph_type;
using lower_type  = standard::lower_type;
using print_type  = standard::print_type;
using punct_type  = standard::punct_type;
using space_type  = standard::space_type;
using blank_type  = standard::blank_type;
using upper_type  = standard::upper_type;

inline constexpr auto const& alnum  = standard::alnum;
inline constexpr auto const& alpha  = standard::alpha;
inline constexpr auto const& digit  = standard::digit;
inline constexpr auto const& xdigit = standard::xdigit;
inline constexpr auto const& cntrl  = standard::cntrl;
inline constexpr auto const& graph  = standard::graph;
inline constexpr auto const& lower  = standard::lower;
inline constexpr auto const& print  = standard::print;
inline constexpr auto const& punct  = standard::punct;
inline constexpr auto const& space  = standard::space;
inline constexpr auto const& blank  = standard::blank;
inline constexpr auto const& upper  = standard::upper;

} // boost::spirit::x4

#endif
