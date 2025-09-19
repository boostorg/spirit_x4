/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/raw.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/variant.hpp>

#include <ranges>
#include <string>

using x4::rule;

rule<class direct_rule, int> direct_rule = "direct_rule";
rule<class indirect_rule, int> indirect_rule = "indirect_rule";

auto const direct_rule_def = x4::int_;
auto const indirect_rule_def = direct_rule;

BOOST_SPIRIT_X4_DEFINE(direct_rule)
BOOST_SPIRIT_X4_DEFINE(indirect_rule)

int main()
{
    using namespace x4::standard;
    using x4::raw;
    using x4::eps;
    using x4::lit;
    using x4::_attr;
    using x4::int_;
    using x4::char_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(raw['x']);

    {
        std::ranges::subrange<std::string_view::const_iterator> range;
        std::string str;
        BOOST_TEST(parse("spirit_test_123", raw[alpha >> *(alnum | '_')], range));
        BOOST_TEST((std::string(range.begin(), range.end()) == "spirit_test_123"));
        BOOST_TEST(parse("  spirit", raw[*alpha], range, space));
        BOOST_TEST((range.size() == 6));
    }

    {
        std::string str;
        BOOST_TEST(parse("spirit_test_123", raw[alpha >> *(alnum | '_')], str));
        BOOST_TEST((str == "spirit_test_123"));

        str.clear();
        BOOST_TEST((parse("x123", alpha >> raw[+alnum], str)))
          && BOOST_TEST_EQ(str, "x123");
    }

    {
        std::ranges::subrange<std::string_view::const_iterator> range;
        BOOST_TEST(parse("x", raw[alpha]));
        BOOST_TEST(parse("x", raw[alpha], range));
        BOOST_TEST(parse("x", raw[alpha] >> eps, range));
    }

    {
        std::ranges::subrange<std::string_view::const_iterator> range;
        BOOST_TEST(parse("x", raw[alpha][ ([&](auto& ctx){ range = _attr(ctx); }) ]));
        BOOST_TEST(range.size() == 1 && *range.begin() == 'x');
    }

    {
        std::ranges::subrange<std::string_view::const_iterator> range;
        BOOST_TEST(parse("x123x", lit('x') >> raw[+digit] >> lit('x')));
        BOOST_TEST(parse("x123x", lit('x') >> raw[+digit] >> lit('x'), range));
        BOOST_TEST((std::string(range.begin(), range.end()) == "123"));
    }

    {
        using range = std::ranges::subrange<std::string::iterator>;
        boost::variant<int, range> attr;

        std::string str("test");
        (void)parse(str.begin(), str.end(), (int_ | raw[*char_]), attr);

        auto rng = boost::get<range>(attr);
        BOOST_TEST(std::string(rng.begin(), rng.end()) == "test");
    }

    {
        std::vector<std::ranges::subrange<std::string::iterator>> attr;
        std::string str("123abcd");
        (void)parse(str.begin(), str.end()
          , (raw[int_] >> raw[*char_])
          , attr
        );
        BOOST_TEST(attr.size() == 2);
        BOOST_TEST(std::string(attr[0].begin(), attr[0].end()) == "123");
        BOOST_TEST(std::string(attr[1].begin(), attr[1].end()) == "abcd");
    }

    {
        std::pair<int, std::ranges::subrange<std::string::iterator>> attr;
        std::string str("123abcd");
        (void)parse(str.begin(), str.end()
          , (int_ >> raw[*char_])
          , attr
        );
        BOOST_TEST(attr.first == 123);
        BOOST_TEST(std::string(attr.second.begin(), attr.second.end()) == "abcd");
    }

    {
        // test with simple rule
        std::ranges::subrange<std::string_view::const_iterator> range;
        BOOST_TEST(parse("123", raw[direct_rule], range));
        BOOST_TEST((std::string(range.begin(), range.end()) == "123"));
    }

    {
        // test with complex rule
        std::ranges::subrange<std::string_view::const_iterator> range;
        BOOST_TEST(parse("123", raw[indirect_rule], range));
        BOOST_TEST((std::string(range.begin(), range.end()) == "123"));
    }

    return boost::report_errors();
}
