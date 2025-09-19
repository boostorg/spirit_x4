/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/symbols.hpp>
#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

// Custom string type with a C-style string conversion.
struct custom_string_c
{
    custom_string_c(char c) { str[0] = c; str[1] = '\0'; }

    operator char*() { return str; }
    operator char const*() const { return str; }

private:
    char str[2];
};

int main()
{
    using x4::shared_symbols;
    using x4::no_case;

    { // basics
        shared_symbols<int> sym;

        sym.add
            ("Joel")
            ("Ruby")
            ("Tenji")
            ("Tutit")
            ("Kim")
            ("Joey")
            ("Joeyboy")
        ;

        BOOST_TEST(parse("Joel", sym));
        BOOST_TEST(parse("Ruby", sym));
        BOOST_TEST(parse("Tenji", sym));
        BOOST_TEST(parse("Tutit", sym));
        BOOST_TEST(parse("Kim", sym));
        BOOST_TEST(parse("Joey", sym));
        BOOST_TEST(parse("Joeyboy", sym));
        BOOST_TEST(!parse("XXX", sym));

        // test copy
        shared_symbols<int> sym2;
        sym2 = sym;
        BOOST_TEST(parse("Joel", sym2));
        BOOST_TEST(parse("Ruby", sym2));
        BOOST_TEST(parse("Tenji", sym2));
        BOOST_TEST(parse("Tutit", sym2));
        BOOST_TEST(parse("Kim", sym2));
        BOOST_TEST(parse("Joey", sym2));
        BOOST_TEST(!parse("XXX", sym2));

        // make sure it plays well with other parsers
        BOOST_TEST(parse("Joelyo", sym >> "yo"));

        sym.remove
            ("Joel")
            ("Ruby")
        ;

        BOOST_TEST(!parse("Joel", sym));
        BOOST_TEST(!parse("Ruby", sym));
    }


    { // no-case handling
        using namespace x4::standard;

        // NOTE: make sure all entries are in lower-case!!!
        shared_symbols<int> sym{"joel", "ruby", "tenji", "tutit", "kim", "joey"};

        BOOST_TEST(parse("joel", no_case[sym]));
        BOOST_TEST(parse("ruby", no_case[sym]));
        BOOST_TEST(parse("tenji", no_case[sym]));
        BOOST_TEST(parse("tutit", no_case[sym]));
        BOOST_TEST(parse("kim", no_case[sym]));
        BOOST_TEST(parse("joey", no_case[sym]));

        BOOST_TEST(parse("JOEL", no_case[sym]));
        BOOST_TEST(parse("RUBY", no_case[sym]));
        BOOST_TEST(parse("TENJI", no_case[sym]));
        BOOST_TEST(parse("TUTIT", no_case[sym]));
        BOOST_TEST(parse("KIM", no_case[sym]));
        BOOST_TEST(parse("JOEY", no_case[sym]));

        // make sure it plays well with other parsers
        BOOST_TEST(parse("Joelyo", no_case[sym] >> "yo"));
    }

    {
         // attributes
        shared_symbols<int> sym;

        sym.add
            ("Joel", 1)
            ("Ruby", 2)
            ("Tenji", 3)
            ("Tutit", 4)
            ("Kim", 5)
            ("Joey", 6)
        ;

        int i;
        BOOST_TEST(parse("Joel", sym, i));
        BOOST_TEST(i == 1);
        BOOST_TEST(parse("Ruby", sym, i));
        BOOST_TEST(i == 2);
        BOOST_TEST(parse("Tenji", sym, i));
        BOOST_TEST(i == 3);
        BOOST_TEST(parse("Tutit", sym, i));
        BOOST_TEST(i == 4);
        BOOST_TEST(parse("Kim", sym, i));
        BOOST_TEST(i == 5);
        BOOST_TEST(parse("Joey", sym, i));
        BOOST_TEST(i == 6);
        BOOST_TEST(!parse("XXX", sym, i));

        // double add:

        sym.add("Joel", 265);
        BOOST_TEST(parse("Joel", sym, i));
        BOOST_TEST(i == 1);
    }

    {
        // actions
        using x4::_attr;

        shared_symbols<int> sym;
        sym.add
            ("Joel", 1)
            ("Ruby", 2)
            ("Tenji", 3)
            ("Tutit", 4)
            ("Kim", 5)
            ("Joey", 6)
        ;

        int i;
        auto f = [&](auto& ctx){ i = _attr(ctx); };

        BOOST_TEST(parse("Joel", sym[f]));
        BOOST_TEST(i == 1);
        BOOST_TEST(parse("Ruby", sym[f]));
        BOOST_TEST(i == 2);
        BOOST_TEST(parse("Tenji", sym[f]));
        BOOST_TEST(i == 3);
        BOOST_TEST(parse("Tutit", sym[f]));
        BOOST_TEST(i == 4);
        BOOST_TEST(parse("Kim", sym[f]));
        BOOST_TEST(i == 5);
        BOOST_TEST(parse("Joey", sym[f]));
        BOOST_TEST(i == 6);
        BOOST_TEST(!parse("XXX", sym[f]));
    }

    return boost::report_errors();
}
