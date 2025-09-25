/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/variant.hpp>

#include <iterator>
#include <string>
#include <cstring>
#include <iostream>

namespace {

int got_it = 0;

struct my_rule_class
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Failure>
    static void on_error(It const&, Se const& last, Context const&, Failure const& x)
    {
        std::cout
            << "Error! Expecting: "
            << x.which()
            << ", got: \""
            << std::string(x.where(), last)
            << "\""
            << std::endl;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, x4::X4Attribute Attr>
    static void on_success(It const&, Se const&, Context const&, Attr&)
    {
        ++got_it;
    }
};

struct on_success_gets_preskipped_iterator
{
    inline static bool ok = false;

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, x4::X4Attribute Attr>
    static void on_success(It before, Se const& after, Context const&, Attr&)
    {
        bool const before_was_b = 'b' == *before;
        ok = before_was_b && (++before == after);
    }
};

} // anonymous

TEST_CASE("rule4")
{
    using namespace x4::standard;
    using x4::rule;
    using x4::int_;
    using x4::lit;

    // show that ra = rb and ra %= rb works as expected
    {
        rule<class a, int> ra;
        rule<class b, int> rb;

        auto ra_def = ra %= int_;
        auto rb_def = rb %= ra_def;

        {
            int attr = 0;
            REQUIRE(parse("123", ra_def, attr));
            CHECK(attr == 123);
        }
        {
            int attr = 0;
            REQUIRE(parse("123", rb_def, attr));
            CHECK(attr == 123);
        }
        {
            int attr = 0;
            auto rb_def2 = rb = ra_def;
            REQUIRE(parse("123", rb_def2, attr));
            CHECK(attr == 123);
        }
    }

    // show that ra %= rb works as expected with semantic actions
    {
        rule<class a, int> ra;
        rule<class b, int> rb;
        (void)rb;
        auto f = [](auto&) {};
        auto ra_def = ra %= int_[f];
        auto ra_def2 = (rb = (ra %= int_[f]));

        {
            int attr = 0;
            REQUIRE(parse("123", ra_def, attr));
            CHECK(attr == 123);
        }
        {
            int attr = 0;
            REQUIRE(parse("123", ra_def2, attr));
            CHECK(attr == 123);
        }
    }

    // std::string as container attribute with auto rules
    {
        std::string attr;

        // test deduced auto rule behavior

        auto text = rule<class text_id, std::string>{}
            = +(!char_(')') >> !char_('>') >> char_);

        REQUIRE(parse("x", text, attr));
        CHECK(attr == "x");
    }

    // error handling
    {
        auto r = rule<my_rule_class, char const*>{}
            = '(' > int_ > ',' > int_ > ')';

        CHECK(parse("(123,456)", r));
        CHECK(!parse("(abc,def)", r));
        CHECK(!parse("(123,456]", r));
        CHECK(!parse("(123;456)", r));
        CHECK(!parse("[123,456]", r));

        CHECK(got_it == 1);
    }

    // on_success gets pre-skipped iterator
    {
        auto r = rule<on_success_gets_preskipped_iterator, char const*>{} = lit("b");
        REQUIRE(parse("a b", 'a' >> r, lit(' ')));
        CHECK(on_success_gets_preskipped_iterator::ok);
    }

    {
        using v_type = boost::variant<double, int>;
        auto r1 = rule<class r1_id, v_type>{} = int_;
        v_type v;
        REQUIRE(parse("1", r1, v));
        REQUIRE(v.which() == 1);
        CHECK(boost::get<int>(v) == 1);

        using ov_type = std::optional<int>;
        auto r2 = rule<class r2_id, ov_type>{} = int_;
        ov_type ov;
        REQUIRE(parse("1", r2, ov));
        REQUIRE(ov.has_value());
        CHECK(*ov == 1);
    }

    // test handling of single element fusion sequences
    {
        using boost::fusion::vector;
        using boost::fusion::at_c;
        auto r = rule<class r_id, vector<int>>{} = int_;

        vector<int> v(0);
        REQUIRE(parse("1", r, v));
        CHECK(at_c<0>(v) == 1);
    }

    // attribute compatibility test
    {
        constexpr auto expr = int_;
        long long i = 0;
        REQUIRE(parse("1", expr, i));
        CHECK(i == 1);
    }
    {
        constexpr rule<class int_rule, int> int_rule("int_rule");
        (void)int_rule;
        constexpr auto int_rule_def = int_;
        constexpr auto start = int_rule = int_rule_def;

        long long j = 0;
        REQUIRE(parse("1", start, j));
        CHECK(j == 1);
    }
}
