#ifndef BOOST_SPIRIT_X4_TEST_X4_RULE_SEPARATE_TU_GRAMMAR_HPP
#define BOOST_SPIRIT_X4_TEST_X4_RULE_SEPARATE_TU_GRAMMAR_HPP

/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>

// Check that `BOOST_SPIRIT_X4_INSTANTIATE` instantiates `parse_rule` with proper
// types when the rule has no attribute.

namespace unused_attr {

// skipper must have no attribute, check `parse` and `skip_over`
using skipper_type = x4::rule<class skipper_r>;
constexpr skipper_type skipper = "skipper";
BOOST_SPIRIT_X4_DECLARE(skipper_type)

// the `unused_type const` must have the same effect as no attribute
using skipper2_type = x4::rule<class skipper2_r, unused_type const>;
constexpr skipper2_type skipper2 = "skipper2";
BOOST_SPIRIT_X4_DECLARE(skipper2_type)

// grammar must have no attribute, check `parse` and `phrase_parse`
using grammar_type = x4::rule<class grammar_r>;
constexpr grammar_type grammar = "grammar";
BOOST_SPIRIT_X4_DECLARE(grammar_type)

} // unused_attr

// Check instantiation when the rule has an attribute.

namespace used_attr {

using skipper_type = x4::rule<class skipper_r>;
constexpr skipper_type skipper = "skipper";
BOOST_SPIRIT_X4_DECLARE(skipper_type)

using grammar_type = x4::rule<class grammar_r, int, true>;
constexpr grammar_type grammar = "grammar";
BOOST_SPIRIT_X4_DECLARE(grammar_type)

} // used_attr

#endif
