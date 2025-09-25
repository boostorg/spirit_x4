/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/optional.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/vector.hpp>

#include <boost/variant.hpp>

#include <string>
#include <vector>

struct di_ignore
{
    std::string text;
};

struct di_include
{
    std::string FileName;
};

BOOST_FUSION_ADAPT_STRUCT(di_ignore,
    text
)

BOOST_FUSION_ADAPT_STRUCT(di_include,
    FileName
)

struct undefined {};

TEST_CASE("alternative")
{
    using x4::standard::char_;
    using x4::standard::lit;
    using x4::attr;
    using x4::int_;
    using x4::unused;
    using x4::omit;
    using x4::eps;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ | char_);

    {
        CHECK(parse("a", char_ | char_));
        CHECK(parse("x", lit('x') | lit('i')));
        CHECK(parse("i", lit('x') | lit('i')));
        CHECK(!parse("z", lit('x') | lit('o')));
        CHECK(parse("rock", lit("rock") | lit("roll")));
        CHECK(parse("roll", lit("rock") | lit("roll")));
        CHECK(parse("rock", lit("rock") | int_));
        CHECK(parse("12345", lit("rock") | int_));
    }

    {
        using attr_type = boost::variant<undefined, int, char>;
        {
            attr_type v;
            REQUIRE(parse("12345", int_ | char_, v));
            CHECK(boost::get<int>(v) == 12345);
        }
        {
            attr_type v;
            REQUIRE(parse("12345", lit("rock") | int_ | char_, v));
            CHECK(boost::get<int>(v) == 12345);
        }
        {
            attr_type v;
            REQUIRE(parse("rock", lit("rock") | int_ | char_, v));
            CHECK(v.which() == 0);
        }
        {
            attr_type v;
            REQUIRE(parse("x", lit("rock") | int_ | char_, v));
            CHECK(boost::get<char>(v) == 'x');
        }
    }

    {
        // Make sure that we are using the actual supplied attribute types
        // from the variant and not the expected type.
        using attr_type = boost::variant<int, std::string>;
        {
            attr_type v;
            REQUIRE(parse("12345", int_ | +char_, v));
            CHECK(boost::get<int>(v) == 12345);
        }
        {
            attr_type v;
            REQUIRE(parse("abc", int_ | +char_, v));
            CHECK(boost::get<std::string>(v) == "abc");
        }
        {
            attr_type v;
            REQUIRE(parse("12345", +char_ | int_, v));
            CHECK(boost::get<std::string>(v) == "12345");
        }
    }

    {
        unused_type x;
        CHECK(parse("rock", lit("rock") | lit('x'), x));
    }

    {
        // test if alternatives with all components having unused
        // attributes have an unused attribute

        using boost::fusion::vector;
        using boost::fusion::at_c;

        vector<char, char> v;
        REQUIRE((parse("abc", char_ >> (omit[char_] | omit[char_]) >> char_, v)));
        CHECK((at_c<0>(v) == 'a'));
        CHECK((at_c<1>(v) == 'c'));
    }

    {
        // Test that we can still pass a "compatible" attribute to
        // an alternate even if its "expected" attribute is unused type.

        std::string s;
        REQUIRE(parse("...", *(char_('.') | char_(',')), s));
        CHECK(s == "...");
    }

    {   // make sure collapsing eps works as expected
        // (compile check only)

        using x4::rule;
        using x4::_attr;
        using x4::_val;

        rule<class r1, wchar_t> r1;
        rule<class r2, wchar_t> r2;
        rule<class r3, wchar_t> r3;

        constexpr auto f = [&](auto& ctx){ _val(ctx) = _attr(ctx); };

        (void)(r3 = (eps >> r1)[f]);
        (void)(r3 = (r1 | r2)[f]);
        (void)(r3 = eps >> r1 | r2);
        (void)r3;
    }

    {
        // test having a variant<container, ...>
        std::string s;
        REQUIRE(parse("a,b", char_ % ',' | eps, s));
        CHECK(s == "ab");
    }

    {
        {
            // testing a sequence taking a container as attribute
            std::string s;
            REQUIRE(parse("abc,a,b,c", char_ >> char_ >> (char_ % ','), s));
            CHECK(s == "abcabc");
        }
        {
            // test having an optional<container> inside a sequence
            std::string s;
            REQUIRE(parse("ab", char_ >> char_ >> -(char_ % ','), s));
            CHECK(s == "ab");
        }
        {
            // test having a variant<container, ...> inside a sequence
            std::string s;
            CHECK(parse("ab", char_ >> char_ >> ((char_ % ',') | eps), s));
            CHECK(s == "ab");
        }
        {
            std::string s;
            CHECK(parse("abc", char_ >> char_ >> ((char_ % ',') | eps), s));
            CHECK(s == "abc");
        }
    }

    {
        //compile test only (bug_march_10_2011_8_35_am)
        using value_type = boost::variant<double, std::string>;

        using x4::rule;

        rule<class r1, value_type> r1;
        [[maybe_unused]] auto r1_ = r1 = r1 | eps; // left recursive!
    }

    {
        using x4::rule;
        using d_line = boost::variant<di_ignore, di_include>;

        rule<class ignore, di_ignore> ignore;
        rule<class include, di_include> include;
        rule<class line, d_line> line;

        [[maybe_unused]] auto start = line = include | ignore;
        (void)line;
    }

    // single-element fusion vector tests
    {
        boost::fusion::vector<boost::variant<int, std::string>> fv;
        REQUIRE(parse("12345", int_ | +char_, fv));
        CHECK(boost::get<int>(boost::fusion::at_c<0>(fv)) == 12345);
    }
    {
        boost::fusion::vector<boost::variant<int, std::string>> fvi;
        REQUIRE(parse("12345", int_ | int_, fvi));
        CHECK(boost::get<int>(boost::fusion::at_c<0>(fvi)) == 12345);
    }

    // alternative over single element sequences as part of another sequence
    {
        constexpr auto key1 = lit("long") >> attr(long());
        constexpr auto key2 = lit("char") >> attr(char());
        constexpr auto keys = key1 | key2;
        constexpr auto pair = keys >> lit("=") >> +char_;

        boost::fusion::deque<boost::variant<long, char>, std::string> attr_;

        REQUIRE(parse("long=ABC", pair, attr_));
        CHECK(boost::get<long>(&boost::fusion::front(attr_)) != nullptr);
        CHECK(boost::get<char>(&boost::fusion::front(attr_)) == nullptr);
    }

    {
        // ensure no unneeded synthesization, copying and moving occurred
        constexpr auto p = '{' >> int_ >> '}';

        spirit_test::stationary st { 0 };
        REQUIRE(parse("{42}", p | eps | p, st));
        CHECK(st.val == 42);
    }

    {
        // attributeless parsers must not insert values
        std::vector<int> v;
        REQUIRE(parse("1 2 3 - 5 - - 7 -", (int_ | '-') % ' ', v));
        REQUIRE(v.size() == 5);
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
        CHECK(v[3] == 5);
        CHECK(v[4] == 7);
    }

    {
        // regressing test for #603
        struct X {};
        std::vector<boost::variant<std::string, int, X>> v;
        REQUIRE(parse("xx42x9y", *(int_ | +char_('x') | 'y' >> attr(X{})), v));
        CHECK(v.size() == 5);
    }

    {
        // sequence parser in alternative into container
        std::string s;
        REQUIRE(parse("abcbbcd", *(char_('a') >> *(*char_('b') >> char_('c')) | char_('d')), s));
        CHECK(s == "abcbbcd");
    }

    {
        // conversion between alternatives
        struct X {};
        struct Y {};
        struct Z {};
        boost::variant<X, Y, Z> v;
        boost::variant<Y, X> x{X{}};
        v = x; // boost::variant supports that convertion
        auto const p = 'x' >> attr(x) | 'z' >> attr(Z{});
        REQUIRE(parse("z", p, v));
        CHECK(boost::get<Z>(&v) != nullptr);
        REQUIRE(parse("x", p, v));
        CHECK(boost::get<X>(&v) != nullptr);
    }

    {
        // regression test for #679
        using Qaz = std::vector<boost::variant<int>>;
        using Foo = std::vector<boost::variant<Qaz, int>>;
        using Bar = std::vector<boost::variant<Foo, int>>;
        Bar x;
        CHECK(parse("abaabb", +('a' >> attr(Foo{}) | 'b' >> attr(int{})), x));
    }
}
