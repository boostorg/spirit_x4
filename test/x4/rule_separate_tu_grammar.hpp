/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_SPIRIT_X4_TEST_X4_RULE_SEPARATE_TU_GRAMMAR_HPP
#define BOOST_SPIRIT_X4_TEST_X4_RULE_SEPARATE_TU_GRAMMAR_HPP

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>

// Check that `BOOST_SPIRIT_X4_INSTANTIATE` instantiates `parse_rule` with proper
// types when a rule has no attribute.

namespace unused_attr {

namespace x4 = boost::spirit::x4;

// skipper must has no attribute, checks `parse` and `skip_over`
using skipper_type = x4::rule<class skipper_r>;
const skipper_type skipper;
BOOST_SPIRIT_X4_DECLARE(skipper_type)

// the `unused_type const` must have the same effect as no attribute
using skipper2_type = x4::rule<class skipper2_r, x4::unused_type const>;
const skipper2_type skipper2;
BOOST_SPIRIT_X4_DECLARE(skipper2_type)

// grammar must has no attribute, checks `parse` and `phrase_parse`
using grammar_type = x4::rule<class grammar_r>;
const grammar_type grammar;
BOOST_SPIRIT_X4_DECLARE(grammar_type)

}

// Check instantiation when rule has an attribute.

namespace used_attr {

namespace x4 = boost::spirit::x4;

using skipper_type = x4::rule<class skipper_r>;
const skipper_type skipper;
BOOST_SPIRIT_X4_DECLARE(skipper_type)

using grammar_type = x4::rule<class grammar_r, int, true>;
const grammar_type grammar;
BOOST_SPIRIT_X4_DECLARE(grammar_type)

}

#endif
