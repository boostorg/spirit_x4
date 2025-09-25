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
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/debug/annotate_on_success.hpp>
#include <boost/spirit/x4/debug/error_reporting.hpp>
#include <boost/spirit/x4/directive/with.hpp>

#include <iterator>
#include <string>
#include <sstream>

namespace {

struct error_handler_base
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Failure>
    void on_error(It const&, Se const&, Context const& ctx, Failure const& x) const
    {
        std::string message = "Error! Expecting: " + x.which() + " here:";
        auto& error_handler = x4::get<x4::contexts::error_handler>(ctx).get();
        error_handler(x.where(), message);
    }
};

struct test_inner_rule_class;
struct test_rule_class : x4::annotate_on_success, error_handler_base {};

x4::rule<test_inner_rule_class> const test_inner_rule = "\"bar\"";
x4::rule<test_rule_class> const test_rule;
auto const test_inner_rule_def = x4::lit("bar");
auto const test_rule_def = x4::lit("foo") > test_inner_rule > x4::lit("git");

BOOST_SPIRIT_X4_DEFINE(test_inner_rule)
BOOST_SPIRIT_X4_DEFINE(test_rule)

void do_parse(std::string const& line_break)
{
    std::string const input("foo" + line_break + "  foo" + line_break + "git");
    auto const begin = std::begin(input);
    auto const end = std::end(input);

    {
        std::stringstream stream;
        x4::error_handler<std::string::const_iterator> error_handler{begin, end, stream};

        auto const parser = x4::with<x4::contexts::error_handler>(std::ref(error_handler))[test_rule];
        (void)parse(begin, end, parser, x4::standard::space);

        CHECK(stream.str() == "In line 2:\nError! Expecting: \"bar\" here:\n  foo\n__^_\n");
    }

    {
        // TODO: cleanup when error_handler is reenterable
        std::stringstream stream;
        x4::error_handler<std::string::const_iterator> error_handler{ begin, end, stream };

        auto const parser = x4::with<x4::contexts::error_handler>(std::ref(error_handler))[test_rule];
        (void)parse(begin, end, parser);

        CHECK(stream.str() == "In line 1:\nError! Expecting: \"bar\" here:\nfoo\n___^_\n");
    }
}

void test_line_break_first(std::string const& line_break)
{
    std::string const input(line_break + "foo  foo" + line_break + "git");
    auto const begin = std::begin(input);
    auto const end = std::end(input);

    std::stringstream stream;
    x4::error_handler<std::string::const_iterator> error_handler{begin, end, stream};

    auto const parser = x4::with<x4::contexts::error_handler>(std::ref(error_handler))[test_rule];
    (void)parse(begin, end, parser, x4::space);

    CHECK(stream.str() == "In line 2:\nError! Expecting: \"bar\" here:\nfoo  foo\n_____^_\n");
}

} // anonymous

TEST_CASE("error_handler")
{
    do_parse("\n");
    do_parse("\r");
    do_parse("\r\n");

    test_line_break_first("\n");
    test_line_break_first("\r");
    test_line_break_first("\r\n");
}
