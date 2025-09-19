/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/directive/skip.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

int main()
{
    using x4::standard::space;
    using x4::standard::space_type;
    using x4::standard::char_;
    using x4::standard::alpha;
    using x4::lexeme;
    using x4::skip;
    using x4::reskip;
    using x4::lit;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(skip('x')['y']);

    {
        BOOST_TEST(parse("a b c d", skip(space)[*char_]));
    }

    {
        // test attribute
        std::string s;
        BOOST_TEST(parse("a b c d", skip(space)[*char_], s));
        BOOST_TEST(s == "abcd");
    }

    {
        // reskip
        BOOST_TEST(parse("ab c d", lexeme[lit('a') >> 'b' >> reskip[lit('c') >> 'd']], space));
        BOOST_TEST(parse("abcd", lexeme[lit('a') >> 'b' >> reskip[lit('c') >> 'd']], space));
        BOOST_TEST(!(parse("a bcd", lexeme[lit('a') >> 'b' >> reskip[lit('c') >> 'd']], space)));

        BOOST_TEST(parse("ab c d", lexeme[lexeme[lit('a') >> 'b' >> reskip[lit('c') >> 'd']]], space));
        BOOST_TEST(parse("abcd", lexeme[lexeme[lit('a') >> 'b' >> reskip[lit('c') >> 'd']]], space));
        BOOST_TEST(!(parse("a bcd", lexeme[lexeme[lit('a') >> 'b' >> reskip[lit('c') >> 'd']]], space)));
    }

    return boost::report_errors();
}
