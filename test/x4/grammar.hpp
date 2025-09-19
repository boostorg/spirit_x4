/*=============================================================================
    Copyright (c) 2019 Joel de Guzman
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_SPIRIT_TEST_X4_GRAMMAR_HPP
#define BOOST_SPIRIT_TEST_X4_GRAMMAR_HPP

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>

namespace x4 = boost::spirit::x4;

x4::rule<struct grammar_r, int> const grammar;
using grammar_type = decltype(grammar);
BOOST_SPIRIT_X4_DECLARE(grammar_type)

#endif
