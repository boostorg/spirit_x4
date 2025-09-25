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

namespace {

// Custom string type with a C-style string conversion.
struct custom_string_c
{
    custom_string_c(char c) { str[0] = c; str[1] = '\0'; }

    operator char*() { return str; }
    operator char const*() const { return str; }

private:
    char str[2];
};

} // anonymous

TEST_CASE("symbols1")
{
    using x4::shared_symbols;
    using x4::no_case;

    {
        // basics
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

        CHECK(parse("Joel", sym));
        CHECK(parse("Ruby", sym));
        CHECK(parse("Tenji", sym));
        CHECK(parse("Tutit", sym));
        CHECK(parse("Kim", sym));
        CHECK(parse("Joey", sym));
        CHECK(parse("Joeyboy", sym));
        CHECK(!parse("XXX", sym));

        // test copy
        shared_symbols<int> sym2;
        sym2 = sym;
        CHECK(parse("Joel", sym2));
        CHECK(parse("Ruby", sym2));
        CHECK(parse("Tenji", sym2));
        CHECK(parse("Tutit", sym2));
        CHECK(parse("Kim", sym2));
        CHECK(parse("Joey", sym2));
        CHECK(!parse("XXX", sym2));

        // make sure it plays well with other parsers
        CHECK(parse("Joelyo", sym >> "yo"));

        sym.remove
            ("Joel")
            ("Ruby")
        ;

        CHECK(!parse("Joel", sym));
        CHECK(!parse("Ruby", sym));
    }


    {
        // no-case handling
        using namespace x4::standard;

        // NOTE: make sure all entries are in lower-case!!!
        shared_symbols<int> sym{"joel", "ruby", "tenji", "tutit", "kim", "joey"};

        CHECK(parse("joel", no_case[sym]));
        CHECK(parse("ruby", no_case[sym]));
        CHECK(parse("tenji", no_case[sym]));
        CHECK(parse("tutit", no_case[sym]));
        CHECK(parse("kim", no_case[sym]));
        CHECK(parse("joey", no_case[sym]));

        CHECK(parse("JOEL", no_case[sym]));
        CHECK(parse("RUBY", no_case[sym]));
        CHECK(parse("TENJI", no_case[sym]));
        CHECK(parse("TUTIT", no_case[sym]));
        CHECK(parse("KIM", no_case[sym]));
        CHECK(parse("JOEY", no_case[sym]));

        // make sure it plays well with other parsers
        CHECK(parse("Joelyo", no_case[sym] >> "yo"));
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

        int i = 0;
        CHECK(parse("Joel", sym, i));
        CHECK(i == 1);
        CHECK(parse("Ruby", sym, i));
        CHECK(i == 2);
        CHECK(parse("Tenji", sym, i));
        CHECK(i == 3);
        CHECK(parse("Tutit", sym, i));
        CHECK(i == 4);
        CHECK(parse("Kim", sym, i));
        CHECK(i == 5);
        CHECK(parse("Joey", sym, i));
        CHECK(i == 6);
        CHECK(!parse("XXX", sym, i));

        // double add:

        sym.add("Joel", 265);
        CHECK(parse("Joel", sym, i));
        CHECK(i == 1);
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

        int i = 0;
        auto f = [&](auto& ctx){ i = _attr(ctx); }; // lambda with capture (important for subsequent checks)

        using Parser = std::remove_cvref_t<decltype(sym[f])>;
        STATIC_CHECK(x4::X4ExplicitSubject<Parser>);

        CHECK(parse("Joel", sym[f]));
        CHECK(i == 1);
        CHECK(parse("Ruby", sym[f]));
        CHECK(i == 2);
        CHECK(parse("Tenji", sym[f]));
        CHECK(i == 3);
        CHECK(parse("Tutit", sym[f]));
        CHECK(i == 4);
        CHECK(parse("Kim", sym[f]));
        CHECK(i == 5);
        CHECK(parse("Joey", sym[f]));
        CHECK(i == 6);
        CHECK(!parse("XXX", sym[f]));
    }
}
