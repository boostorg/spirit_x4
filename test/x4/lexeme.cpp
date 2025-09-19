/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/operator/plus.hpp>

int main()
{
    using x4::standard::space;
    using x4::standard::space_type;
    using x4::standard::digit;
    using x4::lexeme;
    using x4::rule;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(lexeme['x']);

    {
        BOOST_TEST(parse(" 1 2 3 4 5", +digit, space));
        BOOST_TEST(!parse(" 1 2 3 4 5", lexeme[+digit], space));
        BOOST_TEST(parse(" 12345", lexeme[+digit], space));
        BOOST_TEST(parse(" 12345  ", lexeme[+digit], space));

        // lexeme collapsing
        BOOST_TEST(!parse(" 1 2 3 4 5", lexeme[lexeme[+digit]], space));
        BOOST_TEST(parse(" 12345", lexeme[lexeme[+digit]], space));
        BOOST_TEST(parse(" 12345  ", lexeme[lexeme[+digit]], space));

        auto r = +digit;
        auto rr = lexeme[r];

        BOOST_TEST(!parse(" 1 2 3 4 5", rr, space));
        BOOST_TEST(parse(" 12345", rr, space));
    }

    return boost::report_errors();
}
