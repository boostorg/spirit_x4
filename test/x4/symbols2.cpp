/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/symbols.hpp>

TEST_CASE("symbols2")
{
    using x4::shared_symbols;
    using x4::rule;

    {
        // construction from symbol array
        char const* syms[] = {"Joel", "Ruby", "Tenji", "Tutit", "Kim", "Joey"};
        shared_symbols<int> sym(syms);

        CHECK(parse("Joel", sym));
        CHECK(parse("Ruby", sym));
        CHECK(parse("Tenji", sym));
        CHECK(parse("Tutit", sym));
        CHECK(parse("Kim", sym));
        CHECK(parse("Joey", sym));
        CHECK(!parse("XXX", sym));
    }

    {
        // construction from 2 arrays

        char const* syms[] = {"Joel", "Ruby", "Tenji", "Tutit", "Kim", "Joey"};
        int data[] = {1, 2, 3, 4, 5, 6};
        shared_symbols<int> sym(syms, data);

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
    }

    {
        // allow std::string and other string types
        shared_symbols<> sym;

        // const and non-const std::string
        std::string a("abc");
        std::string const b("def");
        sym += a;
        sym += b;
        CHECK(parse("abc", sym));
        CHECK(parse("def", sym));
        sym = a;
        CHECK(parse("abc", sym));
        CHECK(!parse("def", sym));

        // non-const C-style string
        char arr[2]; arr[0] = 'a'; arr[1] = '\0';
        sym = arr;
        CHECK(parse("a", sym));
        CHECK(!parse("b", sym));
    }

    {
        // find

        shared_symbols<int> sym;
        sym.add("a", 1)("b", 2);

        CHECK(!sym.find("c"));

        REQUIRE(sym.find("a"));
        CHECK(*sym.find("a") == 1);

        REQUIRE(sym.find("b"));
        CHECK(*sym.find("b") == 2);

        CHECK(sym.at("a") == 1);
        CHECK(sym.at("b") == 2);
        CHECK(sym.at("c") == 0);

        REQUIRE(sym.find("a"));
        CHECK(*sym.find("a") == 1);
        REQUIRE(sym.find("b"));
        CHECK(*sym.find("b") == 2);
        REQUIRE(sym.find("c"));
        CHECK(*sym.find("c") == 0);

        shared_symbols<int> const_sym(sym);

        REQUIRE(const_sym.find("a"));
        CHECK(*const_sym.find("a") == 1);
        REQUIRE(const_sym.find("b"));
        CHECK(*const_sym.find("b") == 2);
        REQUIRE(const_sym.find("c"));
        CHECK(*const_sym.find("c") == 0);
        CHECK(!const_sym.find("d"));

        char const *str1 = "all";
        char const *first = str1, *last = str1 + 3;
        REQUIRE(*sym.prefix_find(first, last) == 1);
        CHECK(first == str1 + 1);

        char const *str2 = "dart";
        first = str2; last = str2 + 4;
        REQUIRE(!sym.prefix_find(first, last));
        CHECK(first == str2);
    }

    {
        // name
        shared_symbols<int> sym,sym2;
        sym.name("test");
        CHECK(sym.name()=="test");
        sym2 = sym;
        CHECK(sym2.name()=="test");

        shared_symbols<int> sym3(sym);
        CHECK(sym3.name()=="test");
    }

    {
        // Substrings

        shared_symbols<int> sym;
        CHECK(sym.at("foo") == 0);
        sym.at("foo") = 1;
        CHECK(sym.at("foo") == 1);
        CHECK(sym.at("fool") == 0);
        sym.at("fool") = 2;
        REQUIRE(sym.find("foo"));
        CHECK(*sym.find("foo") == 1);
        REQUIRE(sym.find("fool"));
        CHECK(*sym.find("fool") == 2);
        CHECK(!sym.find("foolish"));
        CHECK(!sym.find("foot"));
        CHECK(!sym.find("afoot"));

        char const *str, *first, *last;
        str = "foolish"; first = str; last = str + 7;
        REQUIRE(*sym.prefix_find(first, last) == 2);
        CHECK(first == str + 4);

        first = str; last = str + 4;
        REQUIRE(*sym.prefix_find(first, last) == 2);
        CHECK(first == str + 4);

        str = "food"; first = str; last = str + 4;
        REQUIRE(*sym.prefix_find(first, last) == 1);
        CHECK(first == str + 3);

        first = str; last = str + 3;
        REQUIRE(*sym.prefix_find(first, last) == 1);
        CHECK(first == str + 3);

        first = str; last = str + 2;
        REQUIRE(!sym.prefix_find(first, last));
        CHECK(first == str);
    }

    {
        std::string s;
        shared_symbols<double> vars;

        vars.add("l1", 12.0);
        vars.add("l2", 0.0);
        vars.remove("l2");
        (void)vars.find("l1");
        double* d = vars.find("l1");
        CHECK(d != nullptr);
    }

    {
        // test for proto problem with rvalue references (10-11-2011)
        shared_symbols<int> sym;
        sym += std::string("Joel");
        sym += std::string("Ruby");

        CHECK(parse("Joel", sym));
        CHECK(parse("Ruby", sym));
    }
}
