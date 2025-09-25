/*=============================================================================
    Copyright (c) 2017-2021 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/equal_to.hpp>

#include <iterator>
#include <optional>
#include <string>
#include <type_traits>

namespace {

// just an `attr` with added type checker
template<class Value, class Expected>
struct checked_attr_parser : x4::attr_parser<Value>
{
    using base_type = x4::attr_parser<Value>;

    checked_attr_parser(Value const& value) : base_type(value) {}
    checked_attr_parser(Value&& value) : base_type(std::move(value)) {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, class Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr_) const
    {
        static_assert(std::is_same_v<Expected, Attr>, "attribute type check failed");
        return base_type::parse(first, last, ctx, attr_);
    }
};

template<class Expected, class Value>
checked_attr_parser<std::decay_t<Value>, Expected>
checked_attr(Value&& value) { return { std::forward<Value>(value) }; }

// instantiate our type checker
// (checks attribute value just to be sure we are ok)
template<class Value, class Expr>
void test_expr(Value const& v, Expr&& expr)
{
    Value r;
    REQUIRE(x4::parse("", std::forward<Expr>(expr), r));
    CHECK((r == v));
}

template<class Expr, class Attr>
void gen_sequence(Attr const& attribute, Expr&& expr)
{
    test_expr(attribute, expr);
    test_expr(attribute, expr >> x4::eps);
}

template<class Expected, class... ExpectedTail, class Attr, class Expr, class Value, class... Tail>
void gen_sequence(Attr const& attribute, Expr&& expr, Value const& v, Tail const&... tail)
{
    gen_sequence<ExpectedTail...>(attribute, expr >> checked_attr<Expected>(v), tail...);
    gen_sequence<ExpectedTail...>(attribute, expr >> x4::eps >> checked_attr<Expected>(v), tail...);
    gen_sequence<ExpectedTail...>(attribute, expr >> (x4::eps >> checked_attr<Expected>(v)), tail...);
}

template<class Expected, class... ExpectedTail, class Attr, class Value, class... Tail>
void gen_sequence_tests(Attr const& attribute, Value const& v, Tail const&... tail)
{
    gen_sequence<ExpectedTail...>(attribute, checked_attr<Expected>(v), tail...);
    gen_sequence<ExpectedTail...>(attribute, x4::eps >> checked_attr<Expected>(v), tail...);
}

template<class Expected, class Value>
void gen_single_item_tests(Value const& v)
{
    Expected attribute(v);
    gen_sequence(attribute, checked_attr<Expected>(v));
    gen_sequence(attribute, x4::eps >> checked_attr<Expected>(v));
}

template<class Expected, class... ExpectedTail, class Value, class... Tail>
void gen_single_item_tests(Value const& v, Tail const&... tail)
{
    gen_single_item_tests<Expected>(v);
    gen_single_item_tests<ExpectedTail...>(tail...);
}

template<class... Expected, class... Values>
void gen_tests(Values const&... values)
{
    gen_single_item_tests<Expected...>(values...);

    boost::fusion::vector<Expected...> attribute = boost::fusion::make_vector(values...);
    gen_sequence_tests<Expected...>(attribute, values...);
}

template<class... Attributes>
void make_test(Attributes const&... attrs)
{
    // I would like to place all of this in a single call
    // but it requires tremendous amount of heap to compile
    gen_tests<Attributes...>(attrs...);
    gen_tests<
        std::optional<Attributes>...,
        boost::fusion::vector<Attributes>...
    >(attrs..., attrs...);

    gen_tests<
        std::optional<boost::fusion::vector<Attributes>>...,
        boost::fusion::vector<std::optional<Attributes>>...
    >(boost::fusion::vector<Attributes>(attrs)..., attrs...);
}

} // anonymous

TEST_CASE("attribute_type_check")
{
    make_test<int, std::string>(123, "hello");
}
