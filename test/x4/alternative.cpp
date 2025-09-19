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

int main()
{
    using x4::standard::char_;
    using x4::standard::lit;
    using x4::attr;
    using x4::int_;
    using x4::unused_type;
    using x4::unused;
    using x4::omit;
    using x4::eps;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ | char_);

    {
        BOOST_TEST(parse("a", char_ | char_));
        BOOST_TEST(parse("x", lit('x') | lit('i')));
        BOOST_TEST(parse("i", lit('x') | lit('i')));
        BOOST_TEST(!parse("z", lit('x') | lit('o')));
        BOOST_TEST(parse("rock", lit("rock") | lit("roll")));
        BOOST_TEST(parse("roll", lit("rock") | lit("roll")));
        BOOST_TEST(parse("rock", lit("rock") | int_));
        BOOST_TEST(parse("12345", lit("rock") | int_));
    }

    {
        using attr_type = boost::variant<undefined, int, char>;
        attr_type v;

        BOOST_TEST(parse("12345", int_ | char_, v));
        BOOST_TEST(boost::get<int>(v) == 12345);

        BOOST_TEST(parse("12345", lit("rock") | int_ | char_, v));
        BOOST_TEST(boost::get<int>(v) == 12345);

        v = attr_type();
        BOOST_TEST(parse("rock", lit("rock") | int_ | char_, v));
        BOOST_TEST(v.which() == 0);

        BOOST_TEST(parse("x", lit("rock") | int_ | char_, v));
        BOOST_TEST(boost::get<char>(v) == 'x');
    }

    {   // Make sure that we are using the actual supplied attribute types
        // from the variant and not the expected type.
        boost::variant<int, std::string> v;
        BOOST_TEST(parse("12345", int_ | +char_, v));
        BOOST_TEST(boost::get<int>(v) == 12345);

        BOOST_TEST(parse("abc", int_ | +char_, v));
        BOOST_TEST(boost::get<std::string>(v) == "abc");

        BOOST_TEST(parse("12345", +char_ | int_, v));
        BOOST_TEST(boost::get<std::string>(v) == "12345");
    }

    {
        unused_type x;
        BOOST_TEST(parse("rock", lit("rock") | lit('x'), x));
    }

    {
        // test if alternatives with all components having unused
        // attributes have an unused attribute

        using boost::fusion::vector;
        using boost::fusion::at_c;

        vector<char, char> v;
        BOOST_TEST((parse("abc",
            char_ >> (omit[char_] | omit[char_]) >> char_, v)));
        BOOST_TEST((at_c<0>(v) == 'a'));
        BOOST_TEST((at_c<1>(v) == 'c'));
    }

    {
        // Test that we can still pass a "compatible" attribute to
        // an alternate even if its "expected" attribute is unused type.

        std::string s;
        BOOST_TEST(parse("...", *(char_('.') | char_(',')), s));
        BOOST_TEST(s == "...");
    }

    {   // make sure collapsing eps works as expected
        // (compile check only)

        using x4::rule;
        using x4::_attr;
        using x4::_val;

        rule<class r1, wchar_t> r1;
        rule<class r2, wchar_t> r2;
        rule<class r3, wchar_t> r3;

        auto f = [&](auto& ctx){ _val(ctx) = _attr(ctx); };

        (void)(r3 = ((eps >> r1))[f]);
        (void)(r3 = ((r1) | r2)[f]);
        (void)(r3 = ((eps >> r1) | r2));
        (void)r3;
    }

    {
        std::string s;

        // test having a variant<container, ...>
        BOOST_TEST( (parse("a,b", (char_ % ',') | eps, s )) );
        BOOST_TEST(s == "ab");
    }

    {
        // testing a sequence taking a container as attribute
        std::string s;
        BOOST_TEST( (parse("abc,a,b,c",
            char_ >> char_ >> (char_ % ','), s )) );
        BOOST_TEST(s == "abcabc");

        // test having an optional<container> inside a sequence
        s.erase();
        BOOST_TEST( (parse("ab",
            char_ >> char_ >> -(char_ % ','), s )) );
        BOOST_TEST(s == "ab");

        // test having a variant<container, ...> inside a sequence
        s.erase();
        BOOST_TEST( (parse("ab",
            char_ >> char_ >> ((char_ % ',') | eps), s )) );
        BOOST_TEST(s == "ab");
        s.erase();
        BOOST_TEST( (parse("abc",
            char_ >> char_ >> ((char_ % ',') | eps), s )) );
        BOOST_TEST(s == "abc");
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
        BOOST_TEST(parse("12345", int_ | +char_, fv));
        BOOST_TEST(boost::get<int>(boost::fusion::at_c<0>(fv)) == 12345);

        boost::fusion::vector<boost::variant<int, std::string>> fvi;
        BOOST_TEST(parse("12345", int_ | int_, fvi));
        BOOST_TEST(boost::get<int>(boost::fusion::at_c<0>(fvi)) == 12345);
    }

    // alternative over single element sequences as part of another sequence
    {
        auto key1 = lit("long") >> attr(long());
        auto key2 = lit("char") >> attr(char());
        auto keys = key1 | key2;
        auto pair = keys >> lit("=") >> +char_;

        boost::fusion::deque<boost::variant<long, char>, std::string> attr_;

        BOOST_TEST(parse("long=ABC", pair, attr_));
        BOOST_TEST(boost::get<long>(&boost::fusion::front(attr_)) != nullptr);
        BOOST_TEST(boost::get<char>(&boost::fusion::front(attr_)) == nullptr);
    }

    {
        // ensure no unneeded synthesization, copying and moving occurred
        auto p = '{' >> int_ >> '}';

        spirit_test::stationary st { 0 };
        BOOST_TEST(parse("{42}", p | eps | p, st));
        BOOST_TEST_EQ(st.val, 42);
    }

    {
        // attributeless parsers must not insert values
        std::vector<int> v;
        BOOST_TEST(parse("1 2 3 - 5 - - 7 -", (int_ | '-') % ' ', v));
        BOOST_TEST_EQ(v.size(), 5)
            && BOOST_TEST_EQ(v[0], 1)
            && BOOST_TEST_EQ(v[1], 2)
            && BOOST_TEST_EQ(v[2], 3)
            && BOOST_TEST_EQ(v[3], 5)
            && BOOST_TEST_EQ(v[4], 7)
            ;
    }

    {
        // regressing test for #603
        struct X {};
        std::vector<boost::variant<std::string, int, X>> v;
        BOOST_TEST(parse("xx42x9y", *(int_ | +char_('x') | 'y' >> attr(X{})), v));
        BOOST_TEST_EQ(v.size(), 5);
    }

    {
        // sequence parser in alternative into container
        std::string s;
        BOOST_TEST(parse("abcbbcd",
            *(char_('a') >> *(*char_('b') >> char_('c')) | char_('d')), s));
        BOOST_TEST_EQ(s, "abcbbcd");
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
        BOOST_TEST(parse("z", p, v));
        BOOST_TEST(boost::get<Z>(&v) != nullptr);
        BOOST_TEST(parse("x", p, v));
        BOOST_TEST(boost::get<X>(&v) != nullptr);
    }

    { // regression test for #679
        using Qaz = std::vector<boost::variant<int>>;
        using Foo = std::vector<boost::variant<Qaz, int>>;
        using Bar = std::vector<boost::variant<Foo, int>>;
        Bar x;
        BOOST_TEST(parse("abaabb", +('a' >> attr(Foo{}) | 'b' >> attr(int{})), x));
    }

    return boost::report_errors();
}
