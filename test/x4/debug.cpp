/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_SPIRIT_X4_DEBUG

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/debug/error_reporting.hpp>
#include <boost/spirit/x4/symbols.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/directive/with.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>

#include <boost/fusion/include/vector.hpp>

#include <iterator>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

namespace {

struct my_error_handler
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Exception, class Context>
    void operator()(It const&, Se const& last, Exception const& x, Context const&) const
    {
        std::cout
            << "Error! Expecting: "
            << x.which()
            << ", got: \""
            << std::string(x.where(), last)
            << "\""
            << std::endl;
    }
};

struct my_attribute
{
    bool alive = true;

    void access() const
    {
        CHECK(alive);
    }
    ~my_attribute()
    {
        alive = false;
    }

    friend std::ostream& operator<<(std::ostream & os, my_attribute const & attr)
    {
        attr.access();
        return os << "my_attribute";
    }
};

} // anonymous

TEST_CASE("debug")
{
    using namespace x4::standard;
    using x4::rule;
    using x4::int_;

    {
        // basic tests

        constexpr auto a = rule<class a_id>("a") = 'a';
        constexpr auto b = rule<class b_id>("b") = 'b';
        constexpr auto c = rule<class c_id>("c") = 'c';

        {
            auto start = *(a | b | c);
            CHECK(parse("abcabcacb", start));
        }

        {
            rule<class start> start("start");
            auto start_def = start = (a | b) >> (start | b);

            CHECK(parse("aaaabababaaabbb", start_def));
            CHECK(parse("aaaabababaaabba", start_def).is_partial_match());
        }
    }

    {
        // basic tests w/ skipper

        auto a = rule<class a_id>("a") = 'a';
        auto b = rule<class b_id>("b") = 'b';
        auto c = rule<class c_id>("c") = 'c';

        {
            auto start = *(a | b | c);
            CHECK(parse(" a b c a b c a c b ", start, space));
        }

        {
            rule<class start> start("start");
            auto start_def = start = (a | b) >> (start | b);

            CHECK(parse(" a a a a b a b a b a a a b b b ", start_def, space));
            CHECK(parse(" a a a a b a b a b a a a b b a ", start_def, space).is_partial_match());
        }
    }

    {
        // std::container attributes

        using fs = boost::fusion::vector<int, char>;
        rule<class start, std::vector<fs>> start("start");
        auto start_def = start = *(int_ >> alpha);

        CHECK(parse("1 a 2 b 3 c", start_def, space));
    }

    {
        // error handling

        auto r_def = '(' > int_ > ',' > int_ > ')';
        my_error_handler error_handler;

        auto parser = x4::with<x4::contexts::error_handler>(error_handler)[r_def];

        CHECK( parse("(123,456)", parser));
        CHECK(!parse("(abc,def)", parser));
        CHECK(!parse("(123,456]", parser));
        CHECK(!parse("(123;456)", parser));
        CHECK(!parse("[123,456]", parser));
    }

    {
        x4::shared_symbols<my_attribute> a{{{ "a", my_attribute{} }}};

        auto b = rule<struct b_id, my_attribute>("b") = a;

        my_attribute attr;

        CHECK(parse("a", b, attr));
    }
}
