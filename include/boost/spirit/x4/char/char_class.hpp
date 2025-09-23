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
    is(char_classes::name##_tag, Char ch) noexcept \
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
    namespace encoding { \
    [[maybe_unused]] inline constexpr char_class_parser<char_encoding::encoding, char_classes::name##_tag> name{}; \
    } /* encoding */ \
    namespace parsers::encoding { \
    using x4::encoding::name; \
    } /* parsers::encoding */

#define BOOST_SPIRIT_X4_CHAR_CLASSES(encoding) \
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
    BOOST_SPIRIT_X4_CHAR_CLASS(encoding, upper)

BOOST_SPIRIT_X4_CHAR_CLASSES(standard)

#ifndef BOOST_SPIRIT_X4_NO_STANDARD_WIDE
BOOST_SPIRIT_X4_CHAR_CLASSES(standard_wide)
#endif

#undef BOOST_SPIRIT_X4_CHAR_CLASS
#undef BOOST_SPIRIT_X4_CHAR_CLASSES

// Don't put these in namespace `parsers`, these are too much

using x4::standard::alnum;
using x4::standard::alpha;
using x4::standard::digit;
using x4::standard::xdigit;
using x4::standard::cntrl;
using x4::standard::graph;
using x4::standard::lower;
using x4::standard::print;
using x4::standard::punct;
using x4::standard::space;
using x4::standard::blank;
using x4::standard::upper;

} // boost::spirit::x4

#endif
