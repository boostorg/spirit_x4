/*=============================================================================
    Copyright (c) 2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/rule.hpp>
#include <boost/spirit/x4/directive/with.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/list.hpp>

#include <concepts>
#include <iterator>
#include <utility>
#include <type_traits>

namespace {

struct my_tag;

struct my_rule_class
{
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Failure, class Context>
    void on_error(It const&, Se const&, Context const& ctx, Failure const&)
    {
        ++x4::get<my_tag>(ctx);
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, x4::X4Attribute Attr>
    void on_success(It const&, Se const&, Context const& ctx, Attr&)
    {
        ++x4::get<my_tag>(ctx);
    }
};

using x4::rule;
using x4::int_;
using x4::with;
using x4::_pass;
using x4::_attr;

template<class T>
constexpr auto value_equals = int_[([](auto& ctx) {
    auto&& with_val = x4::get<my_tag>(ctx);
    static_assert(std::same_as<decltype(with_val), T>);
    _pass(ctx) = with_val == _attr(ctx);
})];

} // anonymous

int main()
{
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(with<my_tag>(0)['x']);

    {
        constexpr int i = 0;
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(with<my_tag>(i)['x']);
    }

    // check various value categories
    {
        {
            int i = 42;
            BOOST_TEST(parse("42", with<my_tag>(i)[value_equals<int&>]));
        }
        {
            int const i = 42;
            BOOST_TEST(parse("42", with<my_tag>(i)[value_equals<int const&>]));
        }
        {
            int i = 42;
            BOOST_TEST(parse("42", with<my_tag>(std::move(i))[value_equals<int&>]));
        }
        {
            int const i = 42;
            BOOST_TEST(parse("42", with<my_tag>(std::move(i))[value_equals<int const&>]));
        }

        {
            int i = 42;
            auto with_gen = with<my_tag>(i);
            BOOST_TEST(parse("42", with_gen[value_equals<int&>]));
        }
        {
            int const i = 42;
            auto with_gen = with<my_tag>(i);
            BOOST_TEST(parse("42", with_gen[value_equals<int const&>]));
        }
        {
            int i = 42;
            auto with_gen = with<my_tag>(std::move(i));
            BOOST_TEST(parse("42", with_gen[value_equals<int&>]));
        }
        {
            int const i = 42;
            auto with_gen = with<my_tag>(std::move(i));
            BOOST_TEST(parse("42", with_gen[value_equals<int const&>]));
        }

        // lvalue `move_only`
        {
            spirit_test::move_only mo;
            (void)with<my_tag>(mo)[int_];
        }
        {
            spirit_test::move_only mo;
            auto with_gen = with<my_tag>(mo); // passed-by-reference
            (void)with_gen[int_]; // permitted, never copies
            (void)std::move(with_gen)[int_];
        }

        // rvalue `move_only`
        {
            (void)with<my_tag>(spirit_test::move_only{})[int_];
        }
        {
            auto with_gen = with<my_tag>(spirit_test::move_only{});
            // (void)with_gen[int_]; // requires copy-constructible
            (void)std::move(with_gen)[int_];
        }
    }

    {
        // injecting data into the context in the grammar
        int val = 0;
        auto r = rule<my_rule_class, char const*>{} =
            '(' > int_ > ',' > int_ > ')';

        auto start = with<my_tag>(std::ref(val))[r];

        BOOST_TEST(parse("(123,456)", start));
        BOOST_TEST(!parse("(abc,def)", start));
        BOOST_TEST(val == 2);
    }

    {
        // injecting non-const lvalue into the context
        int val = 0;
        auto const r = int_[([](auto& ctx){
            x4::get<my_tag>(ctx) += x4::_attr(ctx);
        })];
        BOOST_TEST(parse("123,456", with<my_tag>(val)[r % ',']));
        BOOST_TEST(579 == val);
    }

    {
        // injecting rvalue into the context
        auto const r1 = int_[([](auto& ctx){
            x4::get<my_tag>(ctx) += x4::_attr(ctx);
        })];
        auto const r2 = rule<struct my_rvalue_rule_class, int>() =
            x4::lit('(') >> (r1 % ',') >> x4::lit(')')[([](auto& ctx){
                x4::_val(ctx) = x4::get<my_tag>(ctx);
            })];
        int attr = 0;
        BOOST_TEST(parse("(1,2,3)", with<my_tag>(100)[r2], attr));
        BOOST_TEST(106 == attr);
    }

    {
        // injecting const/non-const lvalue and rvalue into the context
        struct functor
        {
            int operator()(int& val)
            {
                return val * 10; // non-const ref returns 10 * injected val
            }
            int operator()(int const& val)
            {
                return val; // const ref returns injected val
            }
        };

        auto f = [](auto& ctx){
            x4::_val(ctx) = x4::_attr(ctx) + functor()(x4::get<my_tag>(ctx));
        };
        auto const r = rule<struct my_rule_class2, int>() = int_[f];

        int attr = 0;
        int const cval = 10;
        BOOST_TEST(parse("5", with<my_tag>(cval)[r], attr));
        BOOST_TEST(15 == attr); // x4::get returns const ref to cval

        attr = 0;
        int val = 10;
        BOOST_TEST(parse("5", with<my_tag>(val)[r], attr));
        BOOST_TEST(105 == attr); // x4::get returns ref to val

        attr = 0;

        BOOST_TEST(parse("5", with<my_tag>(10)[r], attr));
        // x4::get returns ref to member variable of with_directive
        BOOST_TEST(105 == attr);
    }

    return boost::report_errors();
}
