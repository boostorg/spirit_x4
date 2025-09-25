/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>

#include <boost/fusion/include/std_pair.hpp>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <list>
#include <string>
#include <type_traits>

namespace x4 = boost::spirit::x4;

// check if we did not break user defined specializations
namespace check_substitute {

template<class T> struct foo {};
template<class T> struct bar { using type = T; };
template<class T> struct is_bar : std::false_type {};
template<class T> struct is_bar<bar<T>> : std::true_type {};

} // check_substitute

namespace boost::spirit::x4::traits {

using namespace check_substitute;

template<class T, X4Attribute U>
struct is_substitute<foo<T>, foo<U>>
    : is_substitute<T, U>
{};

template<class T, X4Attribute U>
    requires is_bar<T>::value && is_bar<U>::value
struct is_substitute<T, U>
    : is_substitute<typename T::type, typename U::type>
{};

} // boost::spirit::x4::traits

namespace check_substitute {

using x4::traits::is_substitute_v;
static_assert( is_substitute_v<foo<int>, foo<int>>,  "is_substitute problem");
static_assert(!is_substitute_v<foo<int>, foo<long>>, "is_substitute problem");
static_assert( is_substitute_v<bar<int>, bar<int>>,  "is_substitute problem");
static_assert(!is_substitute_v<bar<int>, bar<long>>, "is_substitute problem");

} // check_substitute

namespace {

constexpr x4::rule<class pair_rule, std::pair<std::string, std::string>> pair_rule("pair");
constexpr x4::rule<class string_rule, std::string> string_rule("string");

constexpr auto pair_rule_def = string_rule > x4::lit('=') > string_rule;
constexpr auto string_rule_def = x4::lexeme[*x4::standard::alnum];

BOOST_SPIRIT_X4_DEFINE(pair_rule)
BOOST_SPIRIT_X4_DEFINE(string_rule)

template<class Container>
void test_map_support()
{
    {
        constexpr auto rule = pair_rule % x4::lit(',');
        Container actual;
        REQUIRE(parse("k1=v1,k2=v2,k2=v3", rule, actual));
        CHECK(actual.size() == 2);
        CHECK(actual == Container{{"k1", "v1"}, {"k2", "v2"}});
    }
    {
        // test sequences parsing into containers
        constexpr auto seq_rule = pair_rule >> ',' >> pair_rule >> ',' >> pair_rule;
        Container container;
        CHECK(parse("k1=v1,k2=v2,k2=v3", seq_rule, container));
    }
    {
        // test parsing container into container
        constexpr auto cic_rule = pair_rule >> +(',' >> pair_rule);
        Container container;
        CHECK(parse("k1=v1,k2=v2,k2=v3", cic_rule, container));
    }
}

template<class Container>
void test_multimap_support()
{
    {
        constexpr auto rule = pair_rule % x4::lit(',');
        Container actual;
        REQUIRE(parse("k1=v1,k2=v2,k2=v3", rule, actual));
        CHECK(actual.size() == 3);
        CHECK(actual == Container{{"k1", "v1"}, {"k2", "v2"}, {"k2", "v3"}});
    }
    {
        // test sequences parsing into containers
        constexpr auto seq_rule = pair_rule >> ',' >> pair_rule >> ',' >> pair_rule;
        Container container;
        CHECK(parse("k1=v1,k2=v2,k2=v3", seq_rule, container));
    }
    {
        // test parsing container into container
        constexpr auto cic_rule = pair_rule >> +(',' >> pair_rule);
        Container container;
        CHECK(parse("k1=v1,k2=v2,k2=v3", cic_rule, container));
    }
}

template<class Container>
void test_sequence_support()
{
    {
        constexpr auto rule = string_rule % x4::lit(',');
        Container actual;
        REQUIRE(parse("e1,e2,e2", rule, actual));
        CHECK(actual.size() == 3);
        CHECK(actual == Container{"e1", "e2", "e2"});
    }
    {
        // test sequences parsing into containers
        constexpr auto seq_rule = string_rule >> ',' >> string_rule >> ',' >> string_rule;
        Container container;
        CHECK(parse("e1,e2,e2", seq_rule, container));
    }
    {
        // test parsing container into container
        constexpr auto cic_rule = string_rule >> +(',' >> string_rule);
        Container container;
        CHECK(parse("e1,e2,e2", cic_rule, container));
    }
}

template<class Container>
void test_set_support()
{
    {
        constexpr auto rule = string_rule % x4::lit(',');
        Container actual;
        REQUIRE(parse("e1,e2,e2", rule, actual));
        CHECK(actual.size() == 2);
        CHECK(actual == Container{"e1", "e2"});
    }
    {
        // test sequences parsing into containers
        constexpr auto seq_rule = string_rule >> ',' >> string_rule >> ',' >> string_rule;
        Container container;
        CHECK(parse("e1,e2,e2", seq_rule, container));
    }
    {
        // test parsing container into container
        constexpr auto cic_rule = string_rule >> +(',' >> string_rule);
        Container container;
        CHECK(parse("e1,e2,e2", cic_rule, container));
    }
}

template<class Container>
void test_multiset_support()
{
    {
        constexpr auto rule = string_rule % x4::lit(',');
        Container actual;
        REQUIRE(parse("e1,e2,e2", rule, actual));
        CHECK(actual.size() == 3);
        CHECK(actual == Container{"e1", "e2", "e2"});
    }
    {
        // test sequences parsing into containers
        constexpr auto seq_rule = string_rule >> ',' >> string_rule >> ',' >> string_rule;
        Container container;
        CHECK(parse("e1,e2,e2", seq_rule, container));
    }
    {
        // test parsing container into container
        constexpr auto cic_rule = string_rule >> +(',' >> string_rule);
        Container container;
        CHECK(parse("e1,e2,e2", cic_rule, container));
    }
}

template<class Container>
void test_string_support()
{
    {
        constexpr auto rule = string_rule % x4::lit(',');
        Container container;
        REQUIRE(parse("e1,e2,e2", rule, container));
        CHECK(container.size() == 6);
        CHECK(container == Container{"e1e2e2"});
    }
    {
        // test sequences parsing into containers
        constexpr auto seq_rule = string_rule >> ',' >> string_rule >> ',' >> string_rule;
        Container container;
        CHECK(parse("e1,e2,e2", seq_rule, container));
    }
    {
        // test parsing container into container
        constexpr auto cic_rule = string_rule >> +(',' >> string_rule);
        Container container;
        CHECK(parse("e1,e2,e2", cic_rule, container));
    }
}

} // anonymous

TEST_CASE("container_support")
{
    using x4::traits::is_associative_v;

    // ------------------------------------------------------------------

    static_assert(is_associative_v<std::set<int>>, "is_associative problem");
    static_assert(is_associative_v<std::unordered_set<int>>, "is_associative problem");
    static_assert(is_associative_v<std::multiset<int>>, "is_associative problem");
    static_assert(is_associative_v<std::unordered_multiset<int>>, "is_associative problem");
    static_assert(is_associative_v<std::map<int,int>>, "is_associative problem");
    static_assert(is_associative_v<std::unordered_map<int,int>>, "is_associative problem");
    static_assert(is_associative_v<std::multimap<int,int>>, "is_associative problem");
    static_assert(is_associative_v<std::unordered_multimap<int,int>>, "is_associative problem");

    static_assert(!is_associative_v<std::vector<int>>, "is_associative problem");
    static_assert(!is_associative_v<std::string>, "is_associative problem");
    static_assert(!is_associative_v<std::deque<int>>, "is_associative problem");
    static_assert(!is_associative_v<std::list<int>>, "is_associative problem");

    // ------------------------------------------------------------------

    test_string_support<std::string>();

    test_sequence_support<std::vector<std::string>>();
    test_sequence_support<std::list<std::string>>();
    test_sequence_support<std::deque<std::string>>();

    test_set_support<std::set<std::string>>();
    test_set_support<std::unordered_set<std::string>>();

    test_multiset_support<std::multiset<std::string>>();
    test_multiset_support<std::unordered_multiset<std::string>>();

    test_map_support<std::map<std::string,std::string>>();
    test_map_support<std::unordered_map<std::string,std::string>>();

    test_multimap_support<std::multimap<std::string,std::string>>();
    test_multimap_support<std::unordered_multimap<std::string,std::string>>();
}
