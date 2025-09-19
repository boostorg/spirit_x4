/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>

#include <string>
#include <cstring>

int main()
{
    using namespace x4::standard;
    using x4::rule;
    using x4::lit;
    using x4::int_;
    using x4::unused_type;
    using x4::phrase_parse;
    using x4::root_skipper_flag;
    using x4::traits::has_attribute_v;

#ifdef BOOST_SPIRIT_X4_NO_RTTI
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(rule<class r_>{});
#endif
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(rule<class r_>{"r"});
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(rule<class r_>{"r"} = 'x');

    // check attribute advertising
    static_assert( has_attribute_v<rule<class r_, int>, /*Context=*/unused_type>);
    static_assert(!has_attribute_v<rule<class r_     >, /*Context=*/unused_type>);
    static_assert( has_attribute_v<decltype(rule<class r_, int>{} = int_), /*Context=*/unused_type>);
    static_assert(!has_attribute_v<decltype(rule<class r_     >{} = int_), /*Context=*/unused_type>);

    {
        // basic tests

        constexpr auto a = lit('a');
        constexpr auto b = lit('b');
        constexpr auto c = lit('c');
        constexpr rule<class r_> r("rule");

        {
            constexpr auto start = r = *(a | b | c);
            BOOST_TEST(parse("abcabcacb", start));
        }

        {
            constexpr auto start = r = (a | b) >> (r | b);
            BOOST_TEST(parse("aaaabababaaabbb", start));
            BOOST_TEST(parse("aaaabababaaabba", start).is_partial_match());

            // ignore the skipper!
            BOOST_TEST(parse("aaaabababaaabba", start, space).is_partial_match());
        }
    }

    {
        // basic tests w/ skipper

        constexpr auto a = lit('a');
        constexpr auto b = lit('b');
        constexpr auto c = lit('c');
        constexpr rule<class r_> r("rule");

        {
            constexpr auto start = r = *(a | b | c);
            BOOST_TEST(parse(" a b c a b c a c b ", start, space));
        }

        {
            constexpr auto start = r = (a | b) >> (r | b);
            BOOST_TEST(parse(" a a a a b a b a b a a a b b b ", start, space));
            BOOST_TEST(parse(" a a a a b a b a b a a a b b a ", start, space).is_partial_match());
        }
    }

    {
        // basic tests w/ skipper but no final post-skip

        constexpr auto a = rule<class a_id>("a") = lit('a');
        constexpr auto b = rule<class b_id>("b") = lit('b');
        constexpr auto c = rule<class c_id>("c") = lit('c');

        {
            constexpr auto start = rule<class start_>("start") = *(a | b) >> c;

            auto const res = parse(" a b a a b b a c ... ", start, space, root_skipper_flag::dont_post_skip);
            BOOST_TEST(res.ok && res.remainder.size() == 5);
        }

        {
            constexpr rule<class start_> start("start");
            constexpr auto p = start = (a | b) >> (start | c);

            BOOST_TEST(parse(" a a a a b a b a b a a a b b b c ", p, space, root_skipper_flag::do_post_skip));

            {
                auto const res = parse(" a a a a b a b a b a a a b b b c ", p, space, root_skipper_flag::dont_post_skip);
                BOOST_TEST(res.ok && res.remainder.size() == 1);
            }
        }
    }

    return boost::report_errors();
}
