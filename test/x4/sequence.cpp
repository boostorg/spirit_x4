/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/negated_char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/numeric/real.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/optional.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/fusion/include/comparison.hpp>

#include <optional>
#include <string>

int main()
{
    namespace x4 = boost::spirit::x4;
    namespace traits = x4::traits;

    using x4::unused_type;

    using x4::standard::char_;
    using x4::standard::space;
    using x4::standard::string;
    using x4::standard::lit;
    using x4::standard::alnum;
    using x4::attr;
    using x4::omit;
    using x4::unused;
    using x4::int_;
    using x4::float_;
    using x4::no_case;
    using x4::rule;

    using traits::attribute_of_t;

    using boost::fusion::vector;
    using boost::fusion::deque;
    using boost::fusion::at_c;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ >> char_);

    {
        BOOST_TEST(parse("aa", char_ >> char_));
        BOOST_TEST(parse("aa", char_ >> 'a'));
        BOOST_TEST(parse("aaa", char_ >> char_ >> char_('a')));
        BOOST_TEST(parse("xi", char_('x') >> char_('i')));
        BOOST_TEST(!parse("xi", char_('x') >> char_('o')));
        BOOST_TEST(parse("xin", char_('x') >> char_('i') >> char_('n')));
    }

#ifdef BOOST_SPIRIT_COMPILE_ERROR_CHECK
    {
        // Compile check only
        struct x {};
        char_ >> x(); // this should give a reasonable error message
    }
#endif

    {
        BOOST_TEST(parse(" a a", char_ >> char_, space));
        BOOST_TEST(parse(" x i", char_('x') >> char_('i'), space));
        BOOST_TEST(!parse(" x i", char_('x') >> char_('o'), space));
    }


    {
        BOOST_TEST(parse(" Hello, World", lit("Hello") >> ',' >> "World", space));
    }


    {
        vector<char, char> vec;
        BOOST_TEST(parse("ab", char_ >> char_, vec));
        BOOST_TEST((at_c<0>(vec) == 'a'));
        BOOST_TEST((at_c<1>(vec) == 'b'));
    }

#ifdef BOOST_SPIRIT_COMPILE_ERROR_CHECK
    {
        // Compile check only
        vector<char, char> attr;

        // error: attr does not have enough elements
        parse("abc", char_ >> char_ >> char_, attr);
    }
#endif

    {
        vector<char, char, char> vec;
        BOOST_TEST(parse(" a\n  b\n  c", char_ >> char_ >> char_, vec, space));
        BOOST_TEST((at_c<0>(vec) == 'a'));
        BOOST_TEST((at_c<1>(vec) == 'b'));
        BOOST_TEST((at_c<2>(vec) == 'c'));
    }

    {
        // 'b' has an unused_type. unused attributes are not part of the sequence
        vector<char, char> vec;
        BOOST_TEST(parse("abc", char_ >> 'b' >> char_, vec));
        BOOST_TEST((at_c<0>(vec) == 'a'));
        BOOST_TEST((at_c<1>(vec) == 'c'));
    }

    {
        // 'b' has an unused_type. unused attributes are not part of the sequence
        vector<char, char> vec;
        BOOST_TEST(parse("acb", char_ >> char_ >> 'b', vec));
        BOOST_TEST((at_c<0>(vec) == 'a'));
        BOOST_TEST((at_c<1>(vec) == 'c'));
    }

    {
        // "hello" has an unused_type. unused attributes are not part of the sequence
        vector<char, char> vec;
        BOOST_TEST(parse("a hello c", char_ >> "hello" >> char_, vec, space));
        BOOST_TEST((at_c<0>(vec) == 'a'));
        BOOST_TEST((at_c<1>(vec) == 'c'));
    }

    {
        // a single element
        char c;
        BOOST_TEST(parse("ab", char_ >> 'b', c));
        BOOST_TEST((c == 'a'));
    }

    {
        // a single element fusion sequence
        vector<char> vec;
        BOOST_TEST(parse("ab", char_ >> 'b', vec));
        BOOST_TEST((at_c<0>(vec) == 'a'));
    }

    {
        // make sure single element tuples get passed through if the rhs
        // has a single element tuple as its attribute. Edit JDG 2014:
        // actually he issue here is that if the rhs in this case a rule
        // (r), it should get it (i.e. the sequence parser should not
        // unwrap it). It's odd that the RHS (r) does not really have a
        // single element tuple (it's a deque<char, int>), so the original
        // comment is not accurate.

        using attr_type = deque<char, int>;
        attr_type fv;

        auto r = rule<class r_id, attr_type>()
            = char_ >> ',' >> int_;

        BOOST_TEST((parse("test:x,1", "test:" >> r, fv) &&
            fv == attr_type('x', 1)));
    }

    {
        // make sure single element tuples get passed through if the rhs
        // has a single element tuple as its attribute. This is a correction
        // of the test above.

        using attr_type = deque<int>;
        attr_type fv;

        auto r = rule<class r_id, attr_type>()
            = int_;

        BOOST_TEST((parse("test:1", "test:" >> r, fv) &&
            fv == attr_type(1)));
    }

    {
        // unused means we don't care about the attribute
        BOOST_TEST(parse("abc", char_ >> 'b' >> char_, unused));
    }

    {
        BOOST_TEST(parse("aA", no_case[char_('a') >> 'a']));
        BOOST_TEST(parse("BEGIN END", no_case[lit("begin") >> "end"], space));
        BOOST_TEST(!parse("BEGIN END", no_case[lit("begin") >> "nend"], space));
    }

    { // check attribute is passed through unary to another sequence
        using x4::eps;
        std::string s;
        BOOST_TEST(parse("ab", eps >> no_case[char_ >> char_], s));
        BOOST_TEST("ab" == s);
        s.clear();
        BOOST_TEST(parse("ab", no_case[char_ >> char_] >> eps, s));
        BOOST_TEST("ab" == s);
        s.clear();
        BOOST_TEST(parse("abc", char_ >> no_case[char_ >> char_], s));
        BOOST_TEST("abc" == s);
        s.clear();
        BOOST_TEST(parse("abc", no_case[char_ >> char_] >> char_, s));
        BOOST_TEST("abc" == s);
    }

    {
#ifdef SPIRIT_NO_COMPILE_CHECK
        char_ >> char_ = char_ >> char_; // disallow this!
#endif
    }

    { // alternative forms of attributes. Allow sequences to take in
      // stl containers.

        std::vector<char> v;
        BOOST_TEST(parse("abc", char_ >> char_ >> char_, v));
        BOOST_TEST(v.size() == 3);
        BOOST_TEST(v[0] == 'a');
        BOOST_TEST(v[1] == 'b');
        BOOST_TEST(v[2] == 'c');
    }

    { // alternative forms of attributes. Allow sequences to take in
      // stl containers.

        std::vector<char> v;
        BOOST_TEST(parse("a,b,c", char_ >> *(',' >> char_), v));
        BOOST_TEST(v.size() == 3);
        BOOST_TEST(v[0] == 'a');
        BOOST_TEST(v[1] == 'b');
        BOOST_TEST(v[2] == 'c');
    }

    { // alternative forms of attributes. Allow sequences to take in
      // stl containers.

        std::vector<char> v;
        BOOST_TEST(parse("abc", char_ >> *char_, v));
        BOOST_TEST(v.size() == 3);
        BOOST_TEST(v[0] == 'a');
        BOOST_TEST(v[1] == 'b');
        BOOST_TEST(v[2] == 'c');
    }

    { // alternative forms of attributes. Allow sequences to take in
      // stl containers.
        //~ using x4::hold;

        std::vector<char> v;
        BOOST_TEST(parse("abc", char_ >> *(char_ >> char_), v));
        BOOST_TEST(v.size() == 3);
        BOOST_TEST(v[0] == 'a');
        BOOST_TEST(v[1] == 'b');
        BOOST_TEST(v[2] == 'c');

        v.clear();
        BOOST_TEST(!parse("abcd", char_ >> *(char_ >> char_), v));

        // $$$ hold not yet implemented $$$
        //~ v.clear();
        //~ BOOST_TEST(parse("abcdef", char_ >> *hold[char_ >> char_] >> char_, v));
        //~ BOOST_TEST(v.size() == 6);
        //~ BOOST_TEST(v[0] == 'a');
        //~ BOOST_TEST(v[1] == 'b');
        //~ BOOST_TEST(v[2] == 'c');
        //~ BOOST_TEST(v[3] == 'd');
        //~ BOOST_TEST(v[4] == 'e');
        //~ BOOST_TEST(v[5] == 'f');

        v.clear();
        BOOST_TEST(parse("abc", char_ >> +(char_ >> char_), v));
        BOOST_TEST(v.size() == 3);
        BOOST_TEST(v[0] == 'a');
        BOOST_TEST(v[1] == 'b');
        BOOST_TEST(v[2] == 'c');
    }

    { // alternative forms of attributes. Allow sequences to take in
      // stl containers.

        std::vector<char> v;
        BOOST_TEST(parse("abc", char_ >> -(+char_), v));
        BOOST_TEST(v.size() == 3);
        BOOST_TEST(v[0] == 'a');
        BOOST_TEST(v[1] == 'b');
        BOOST_TEST(v[2] == 'c');
    }

    { // alternative forms of attributes. Allow sequences to take in
      // stl containers.

        std::string s;
        BOOST_TEST(parse("foobar", string("foo") >> string("bar"), s));
        BOOST_TEST(s == "foobar");

        s.clear();

        // $$$ hold not yet implemented $$$
        //~ using x4::hold;

        //~ rule<char const*, std::string()> word = +char_("abc");
        //~ BOOST_TEST(parse("ab.bc.ca", *hold[word >> string(".")] >> word, s));
        //~ BOOST_TEST(s == "ab.bc.ca");
    }

    // Make sure get_sequence_types works for sequences of sequences.
    {
        std::vector<char> v;
        BOOST_TEST(parse(" a b", (' ' >> char_) >> (' ' >> char_), v));
        BOOST_TEST(v.size() == 2);
        BOOST_TEST(v[0] == 'a');
        BOOST_TEST(v[1] == 'b');
    }

    // alternative forms of attributes. Allow sequences to take in
    // stl containers of stl containers.
    {
        std::vector<std::string> v;
        BOOST_TEST(parse("abc1,abc2",
            *~char_(',') >> *(',' >> *~char_(',')), v));
        BOOST_TEST(v.size() == 2 && v[0] == "abc1" && v[1] == "abc2");
    }

    {
        std::vector<std::string> v;

        auto e = rule<class e_id, std::string>()
            = *~char_(',');

        auto l = rule<class l_id, std::vector<std::string>>()
            = e >> *(',' >> e);

        BOOST_TEST(parse("abc1,abc2,abc3", l, v));
        BOOST_TEST(v.size() == 3);
        BOOST_TEST(v[0] == "abc1");
        BOOST_TEST(v[1] == "abc2");
        BOOST_TEST(v[2] == "abc3");
    }

    // do the same with a plain string object
    {
        std::string s;
        BOOST_TEST(parse("abc1,abc2",
            *~char_(',') >> *(',' >> *~char_(',')), s));
        BOOST_TEST(s == "abc1abc2");
    }

    {
        std::string s;
        auto e = rule<class e_id, std::string>()
            = *~char_(',');

        auto l = rule<class l_id, std::string>()
            = e >> *(',' >> e);

        BOOST_TEST(parse("abc1,abc2,abc3", l, s));
        BOOST_TEST(s == "abc1abc2abc3");
    }

    {
        std::vector<char> v;
        BOOST_TEST(parse("ab", char_ >> -char_, v));
        BOOST_TEST(v.size() == 2 && v[0] == 'a' && v[1] == 'b');

        v.clear();
        BOOST_TEST(parse("a", char_ >> -char_, v));
        BOOST_TEST(v.size() == 1 && v[0] == 'a');

        // $$$ should this be allowed? I don't think so... $$$
        //~ v.clear();
        //~ BOOST_TEST(parse("a", char_, v));
        //~ BOOST_TEST(v.size() == 1 && v[0] == 'a');
    }

    {
        std::vector<std::optional<char>> v;
        BOOST_TEST(parse("ab", char_ >> -char_, v));
        BOOST_TEST(v.size() == 2 && v[0] == 'a' && v[1] == 'b');

        v.clear();
        BOOST_TEST(parse("a", char_ >> -char_, v));
        BOOST_TEST(v.size() == 2 && v[0] == 'a' && !v[1]);

        // $$$ should this be allowed? I don't think so... $$$
        //~ v.clear();
        //~ BOOST_TEST(parse("a", char_, v));
        //~ BOOST_TEST(v.size() == 1 && v[0] == 'a');
    }

    // test from spirit mailing list
    // "Error with container within sequence"
    {
        using attr_type = vector<std::string>;
        attr_type vec;

        auto r = *alnum;

        BOOST_TEST(parse("abcdef", r, vec));
        BOOST_TEST(at_c<0>(vec) == "abcdef");
    }

    // test from spirit mailing list (variation of above)
    // "Error with container within sequence"
    {
        using attr_type = vector<std::vector<int>>;
        attr_type vec;

        auto r = *int_;

        BOOST_TEST(parse("123 456", r, vec, space));
        BOOST_TEST(at_c<0>(vec).size() == 2);
        BOOST_TEST(at_c<0>(vec)[0] == 123);
        BOOST_TEST(at_c<0>(vec)[1] == 456);
    }

    { // non-flat optional
        vector<int, std::optional<vector<int, int>>> v;
        auto const p = int_ >> -(':' >> int_ >> '-' >> int_);
        BOOST_TEST(parse("1:2-3", p, v))
            && BOOST_TEST(at_c<1>(v)) && BOOST_TEST_EQ(at_c<0>(*at_c<1>(v)), 2);
    }

    { // optional with container attribute
        vector<char, std::optional<std::string>> v;
        auto const p = char_ >> -(':' >> +char_);
        BOOST_TEST(parse("x", p, v))
            && BOOST_TEST(!at_c<1>(v));
        v = {};
        BOOST_TEST(parse("x:abc", p, v))
            && BOOST_TEST(at_c<1>(v)) && BOOST_TEST(*at_c<1>(v) == "abc");
    }

    {
        using Attr = boost::variant<int, float>;
        auto const term = rule<class term_id, Attr>("term") = int_ | float_;
        auto const expr = rule<class expr_id, Attr>("expr") = term | ('(' > term > ')');
        Attr var;
        BOOST_TEST(parse("(1)", expr, var, space));
    }

    // test that failing sequence leaves attribute consistent
    {
	    std::string str;
	    //no need to use omit[], but lit() is buggy ATM
	    BOOST_TEST(parse("A\nB\nC", *(char_ >> omit[lit("\n")]), str).is_partial_match());
	    BOOST_TEST(str == "AB");
    }

    // test that sequence with only one parser producing attribute
    // makes it unwrapped
    {
	    BOOST_TEST((std::is_same_v<
		    attribute_of_t<decltype(lit("abc") >> attr(long())), unused_type>,
		    long
        >));
    }

    {
        // test action

        char c = 0;
        int n = 0;
        auto f = [&](auto& ctx)
            {
                c = at_c<0>(_attr(ctx));
                n = at_c<1>(_attr(ctx));
            };

        BOOST_TEST(parse("x123\"a string\"", (char_ >> int_ >> "\"a string\"")[f]));
        BOOST_TEST(c == 'x');
        BOOST_TEST(n == 123);
    }

    {
        // test action
        char c = 0;
        int n = 0;
        auto f = [&](auto& ctx)
            {
                c = at_c<0>(_attr(ctx));
                n = at_c<1>(_attr(ctx));
            };

        BOOST_TEST(parse("x 123 \"a string\"", (char_ >> int_ >> "\"a string\"")[f], space));
        BOOST_TEST(c == 'x');
        BOOST_TEST(n == 123);
    }

    {
#ifdef SPIRIT_NO_COMPILE_CHECK
        char const* const s = "";
        int i;
        parse(s, s, int_ >> int_, i);
#endif
    }

    {
        // test move only types
        using x4::eps;
        std::vector<spirit_test::move_only> v;

        using T = std::vector<spirit_test::move_only>;
        static_assert(requires(T& c) {
            typename T::value_type;
        });
        static_assert(requires(T& c) {
            traits::begin(c);
        });
        static_assert(requires(T& c) {
            requires std::forward_iterator<decltype(traits::begin(c))>;
        });
        static_assert(requires(T& c) {
            traits::end(c);
        });
        static_assert(requires(T& c) {
            requires std::sentinel_for<decltype(traits::end(c)), decltype(traits::begin(c))>;
        });
        static_assert(requires(T& c) {
            traits::is_empty(c);
        });
        static_assert(requires(T& c) {
            traits::push_back(c, std::declval<typename T::value_type>());
        });
        static_assert(requires(T& c) {
            traits::append(
                c,
                std::declval<decltype(std::make_move_iterator(traits::begin(c)))>(),
                std::declval<decltype(std::make_move_iterator(traits::end(c)))>()
            );
        });

        static_assert(traits::is_container_v<std::vector<spirit_test::move_only>>);
        // static_assert(traits::CategorizedAttr<std::vector<move_only>, traits::container_attribute>);
        BOOST_TEST(parse("ssszs", *spirit_test::synth_move_only >> 'z' >> spirit_test::synth_move_only, v));
        BOOST_TEST_EQ(v.size(), 4);
    }

    return boost::report_errors();
}
