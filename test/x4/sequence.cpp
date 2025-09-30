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

#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/fusion/include/comparison.hpp>

#include <concepts>
#include <optional>
#include <string>

TEST_CASE("sequence")
{
    namespace traits = x4::traits;

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
    using x4::_attr;
    using x4::eps;

    using traits::attribute_of_t;

    using boost::fusion::vector;
    using boost::fusion::deque;
    using boost::fusion::at_c;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ >> char_);

    CHECK(parse("aa", char_ >> char_));
    CHECK(parse("aa", char_ >> 'a'));
    CHECK(parse("aaa", char_ >> char_ >> char_('a')));
    CHECK(parse("xi", char_('x') >> char_('i')));
    CHECK(!parse("xi", char_('x') >> char_('o')));
    CHECK(parse("xin", char_('x') >> char_('i') >> char_('n')));

    CHECK(parse(" a a", char_ >> char_, space));
    CHECK(parse(" x i", char_('x') >> char_('i'), space));
    CHECK(!parse(" x i", char_('x') >> char_('o'), space));

    CHECK(parse(" Hello, World", lit("Hello") >> ',' >> "World", space));

    {
        vector<char, char> vec;
        REQUIRE(parse("ab", char_ >> char_, vec));
        CHECK(at_c<0>(vec) == 'a');
        CHECK(at_c<1>(vec) == 'b');
    }

    {
        vector<char, char, char> vec;
        REQUIRE(parse(" a\n  b\n  c", char_ >> char_ >> char_, space, vec));
        CHECK(at_c<0>(vec) == 'a');
        CHECK(at_c<1>(vec) == 'b');
        CHECK(at_c<2>(vec) == 'c');
    }

    {
        // 'b' has an unused_type. unused attributes are not part of the sequence
        vector<char, char> vec;
        REQUIRE(parse("abc", char_ >> 'b' >> char_, vec));
        CHECK(at_c<0>(vec) == 'a');
        CHECK(at_c<1>(vec) == 'c');
    }

    {
        // 'b' has an unused_type. unused attributes are not part of the sequence
        vector<char, char> vec;
        REQUIRE(parse("acb", char_ >> char_ >> 'b', vec));
        CHECK(at_c<0>(vec) == 'a');
        CHECK(at_c<1>(vec) == 'c');
    }

    {
        // "hello" has an unused_type. unused attributes are not part of the sequence
        vector<char, char> vec;
        REQUIRE(parse("a hello c", char_ >> "hello" >> char_, space, vec));
        CHECK(at_c<0>(vec) == 'a');
        CHECK(at_c<1>(vec) == 'c');
    }

    {
        // a single element
        char c{};
        REQUIRE(parse("ab", char_ >> 'b', c));
        CHECK(c == 'a');
    }

    {
        // a single element fusion sequence
        vector<char> vec;
        REQUIRE(parse("ab", char_ >> 'b', vec));
        CHECK(at_c<0>(vec) == 'a');
    }

    {
        // Make sure single element tuples get passed through if the rhs
        // has a single element tuple as its attribute. Edit JDG 2014:
        // actually the issue here is that if the rhs in this case a rule
        // (r), it should get it (i.e. the sequence parser should not
        // unwrap it). It's odd that the RHS (r) does not really have a
        // single element tuple (it's a deque<char, int>), so the original
        // comment is not accurate.

        using attr_type = deque<char, int>;
        attr_type fv;

        auto r = rule<class r_id, attr_type>{} = char_ >> ',' >> int_;

        REQUIRE(parse("test:x,1", "test:" >> r, fv));
        CHECK((fv == attr_type('x', 1)));
    }

    {
        // make sure single element tuples get passed through if the rhs
        // has a single element tuple as its attribute. This is a correction
        // of the test above.

        using attr_type = deque<int>;
        attr_type fv;

        auto r = rule<class r_id, attr_type>{} = int_;

        REQUIRE(parse("test:1", "test:" >> r, fv));
        CHECK((fv == attr_type(1)));
    }

    // unused means we don't care about the attribute
    CHECK(parse("abc", char_ >> 'b' >> char_, unused));

    CHECK(parse("aA", no_case[char_('a') >> 'a']));
    CHECK(parse("BEGIN END", no_case[lit("begin") >> "end"], space));
    CHECK(!parse("BEGIN END", no_case[lit("begin") >> "nend"], space));

    // check attribute is passed through unary to another sequence
    {
        std::string s;
        REQUIRE(parse("ab", eps >> no_case[char_ >> char_], s));
        CHECK("ab" == s);
    }
    {
        std::string s;
        REQUIRE(parse("ab", no_case[char_ >> char_] >> eps, s));
        CHECK("ab" == s);
    }
    {
        std::string s;
        REQUIRE(parse("abc", char_ >> no_case[char_ >> char_], s));
        CHECK("abc" == s);
    }
    {
        std::string s;
        REQUIRE(parse("abc", no_case[char_ >> char_] >> char_, s));
        CHECK("abc" == s);
    }

    // Alternative forms of attributes. Allow sequences to take in
    // STL containers.
    {

        std::vector<char> v;
        REQUIRE(parse("abc", char_ >> char_ >> char_, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'c');
    }
    {
        std::vector<char> v;
        REQUIRE(parse("a,b,c", char_ >> *(',' >> char_), v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'c');
    }
    {
        std::vector<char> v;
        REQUIRE(parse("abc", char_ >> *char_, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'c');
    }
    {
        std::vector<char> v;
        REQUIRE(parse("abc", char_ >> *(char_ >> char_), v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'c');
    }
    {
        std::vector<char> v;
        CHECK(!parse("abcd", char_ >> *(char_ >> char_), v));
    }
    {
        std::vector<char> v;
        REQUIRE(parse("abc", char_ >> +(char_ >> char_), v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'c');
    }
    {
        std::vector<char> v;
        REQUIRE(parse("abc", char_ >> -(+char_), v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
        CHECK(v[2] == 'c');
    }
    {
        std::string s;
        REQUIRE(parse("foobar", string("foo") >> string("bar"), s));
        CHECK(s == "foobar");
    }

    // Make sure get_sequence_types works for sequences of sequences.
    {
        std::vector<char> v;
        REQUIRE(parse(" a b", (' ' >> char_) >> (' ' >> char_), v));
        REQUIRE(v.size() == 2);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
    }

    // alternative forms of attributes. Allow sequences to take in
    // stl containers of stl containers.
    {
        std::vector<std::string> v;
        REQUIRE(parse("abc1,abc2", *~char_(',') >> *(',' >> *~char_(',')), v));
        REQUIRE(v.size() == 2);
        CHECK(v[0] == "abc1");
        CHECK(v[1] == "abc2");
    }

    {
        std::vector<std::string> v;

        auto e = rule<class e_id, std::string>{} = *~char_(',');
        auto l = rule<class l_id, std::vector<std::string>>{} = e >> *(',' >> e);

        REQUIRE(parse("abc1,abc2,abc3", l, v));
        REQUIRE(v.size() == 3);
        CHECK(v[0] == "abc1");
        CHECK(v[1] == "abc2");
        CHECK(v[2] == "abc3");
    }

    // do the same with a plain string object
    {
        std::string s;
        REQUIRE(parse("abc1,abc2", *~char_(',') >> *(',' >> *~char_(',')), s));
        CHECK(s == "abc1abc2");
    }

    {
        std::string s;
        auto e = rule<class e_id, std::string>{} = *~char_(',');
        auto l = rule<class l_id, std::string>{} = e >> *(',' >> e);

        REQUIRE(parse("abc1,abc2,abc3", l, s));
        CHECK(s == "abc1abc2abc3");
    }

    {
        std::vector<char> v;
        REQUIRE(parse("ab", char_ >> -char_, v));
        REQUIRE(v.size() == 2);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
    }
    {
        std::vector<char> v;
        REQUIRE(parse("a", char_ >> -char_, v));
        REQUIRE(v.size() == 1);
        CHECK(v[0] == 'a');
    }
    {
        // We don't think this should be allowed
        // std::vector<char> v;
        // REQUIRE(parse("a", char_, v));
        // REQUIRE(v.size() == 1);
        // CHECK(v[0] == 'a');
    }

    {
        std::vector<std::optional<char>> v;
        REQUIRE(parse("ab", char_ >> -char_, v));
        REQUIRE(v.size() == 2);
        CHECK(v[0] == 'a');
        CHECK(v[1] == 'b');
    }
    {
        std::vector<std::optional<char>> v;
        REQUIRE(parse("a", char_ >> -char_, v));
        REQUIRE(v.size() == 2);
        CHECK(v[0] == 'a');
        CHECK(!v[1].has_value());
    }
    {
        // We don't think this should be allowed
        //std::vector<std::optional<char>> v;
        // REQUIRE(parse("a", char_, v));
        // REQUIRE(v.size() == 1);
        // CHECK(v[0] == 'a');
    }

    // Test from spirit mailing list
    // "Error with container within sequence"
    {
        using attr_type = vector<std::string>;
        attr_type vec;

        constexpr auto r = *alnum;

        REQUIRE(parse("abcdef", r, vec));
        CHECK(at_c<0>(vec) == "abcdef");
    }
    {
        using attr_type = vector<std::vector<int>>;
        attr_type vec;

        constexpr auto r = *int_;

        REQUIRE(parse("123 456", r, space, vec));
        REQUIRE(at_c<0>(vec).size() == 2);
        CHECK(at_c<0>(vec)[0] == 123);
        CHECK(at_c<0>(vec)[1] == 456);
    }

    {
        // Non-flat optional
        vector<int, std::optional<vector<int, int>>> v;
        constexpr auto p = int_ >> -(':' >> int_ >> '-' >> int_);
        REQUIRE(parse("1:2-3", p, v));
        REQUIRE(at_c<1>(v).has_value());
        CHECK(at_c<0>(*at_c<1>(v)) == 2);
    }

    // optional with container attribute
    {
        constexpr auto p = char_ >> -(':' >> +char_);

        {
            vector<char, std::optional<std::string>> v;
            REQUIRE(parse("x", p, v));
            CHECK(!at_c<1>(v).has_value());
        }
        {
            vector<char, std::optional<std::string>> v;
            REQUIRE(parse("x:abc", p, v));
            REQUIRE(at_c<1>(v).has_value());
            CHECK(*at_c<1>(v) == "abc");
        }
    }

    {
        using Attr = boost::variant<int, float>;
        constexpr auto term = rule<class term_id, Attr>("term") = int_ | float_;
        constexpr auto expr = rule<class expr_id, Attr>("expr") = term | ('(' > term > ')');
        Attr var;
        CHECK(parse("(1)", expr, space, var));
    }

    // Test that failing sequence leaves attribute consistent
    {
        std::string str;
        //no need to use omit[], but lit() is buggy ATM
        REQUIRE(parse("A\nB\nC", *(char_ >> omit[lit("\n")]), str).is_partial_match());
        CHECK(str == "AB");
    }

    // Test that sequence with only one parser producing attribute makes it unwrapped
    STATIC_CHECK(std::same_as<
        attribute_of_t<decltype(lit("abc") >> attr(long())), unused_type>,
        long
    >);

    {
        // test action

        char c = 0;
        int n = 0;
        auto f = [&](auto& ctx) {
            c = at_c<0>(_attr(ctx));
            n = at_c<1>(_attr(ctx));
        };

        REQUIRE(parse("x123\"a string\"", (char_ >> int_ >> "\"a string\"")[f]));
        CHECK(c == 'x');
        CHECK(n == 123);
    }

    {
        // test action
        char c = 0;
        int n = 0;
        auto f = [&](auto& ctx) {
            c = at_c<0>(_attr(ctx));
            n = at_c<1>(_attr(ctx));
        };

        REQUIRE(parse("x 123 \"a string\"", (char_ >> int_ >> "\"a string\"")[f], space));
        CHECK(c == 'x');
        CHECK(n == 123);
    }

    {
        // Test move only type

        using T = std::vector<spirit_test::move_only>;
        STATIC_CHECK(requires(T& c) {
            typename T::value_type;
        });
        STATIC_CHECK(requires(T& c) {
            traits::begin(c);
        });
        STATIC_CHECK(requires(T& c) {
            requires std::forward_iterator<decltype(traits::begin(c))>;
        });
        STATIC_CHECK(requires(T& c) {
            traits::end(c);
        });
        STATIC_CHECK(requires(T& c) {
            requires std::sentinel_for<decltype(traits::end(c)), decltype(traits::begin(c))>;
        });
        STATIC_CHECK(requires(T& c) {
            traits::is_empty(c);
        });
        STATIC_CHECK(requires(T& c) {
            traits::push_back(c, std::declval<typename T::value_type>());
        });
        STATIC_CHECK(requires(T& c) {
            traits::append(
                c,
                std::declval<decltype(std::make_move_iterator(traits::begin(c)))>(),
                std::declval<decltype(std::make_move_iterator(traits::end(c)))>()
            );
        });
        STATIC_CHECK(requires(T& c) {
            traits::clear(c);
        });

        STATIC_CHECK(traits::is_container_v<std::vector<spirit_test::move_only>>);
        STATIC_CHECK(traits::CategorizedAttr<std::vector<spirit_test::move_only>, x4::traits::container_attr>);

        std::vector<spirit_test::move_only> v;
        REQUIRE(parse("ssszs", *spirit_test::synth_move_only >> 'z' >> spirit_test::synth_move_only, v));
        CHECK(v.size() == 4);
    }
}
