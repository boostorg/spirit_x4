/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "rule_separate_tu_grammar.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>

namespace unused_attr {

using iterator_type = std::string_view::const_iterator;

constexpr auto skipper_def = x4::standard::lit('*');
BOOST_SPIRIT_X4_DEFINE(skipper)
BOOST_SPIRIT_X4_INSTANTIATE(skipper_type, iterator_type, x4::parse_context_for<iterator_type>)

constexpr auto skipper2_def = x4::standard::lit('#');
BOOST_SPIRIT_X4_DEFINE(skipper2)
BOOST_SPIRIT_X4_INSTANTIATE(skipper2_type, iterator_type, x4::parse_context_for<iterator_type>)

constexpr auto grammar_def = *x4::standard::lit('=');
BOOST_SPIRIT_X4_DEFINE(grammar)

BOOST_SPIRIT_X4_INSTANTIATE(grammar_type, iterator_type, x4::parse_context_for<iterator_type>)
BOOST_SPIRIT_X4_INSTANTIATE(grammar_type, iterator_type, x4::phrase_parse_context_for<skipper_type, iterator_type>)
BOOST_SPIRIT_X4_INSTANTIATE(grammar_type, iterator_type, x4::phrase_parse_context_for<skipper2_type, iterator_type>)

} // unused_attr

namespace used_attr {

using iterator_type = std::string_view::const_iterator;

constexpr auto skipper_def = x4::standard::space;
BOOST_SPIRIT_X4_DEFINE(skipper)
BOOST_SPIRIT_X4_INSTANTIATE(skipper_type, iterator_type, x4::parse_context_for<iterator_type>)

constexpr auto grammar_def = x4::int_;
BOOST_SPIRIT_X4_DEFINE(grammar)
BOOST_SPIRIT_X4_INSTANTIATE(grammar_type, iterator_type, x4::parse_context_for<iterator_type>)

BOOST_SPIRIT_X4_INSTANTIATE(grammar_type, iterator_type, x4::phrase_parse_context_for<skipper_type, iterator_type>)

} // used_attr
