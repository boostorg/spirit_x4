/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "test.hpp"

#include <boost/spirit/home/x3.hpp>

#include <functional>

#include <cstring>


#ifdef _MSC_VER
// bogus https://developercommunity.visualstudio.com/t/buggy-warning-c4709/471956
# pragma warning(disable: 4709) // comma operator within array index expression
#endif

namespace x3 = boost::spirit::x3;

struct setnext
{
    setnext(char& next) : next(next) {}

    template <typename Context>
    void operator()(Context const& ctx) const
    {
        next = x3::_attr(ctx);
    }

    char& next;
};

int main()
{
    using x3::int_;

    BOOST_SPIRIT_X3_ASSERT_CONSTEXPR_CTORS(x3::int_type{}[std::true_type{}]);

    {
        int x = 0;
        auto const fun_action = [&](auto& ctx) { x += x3::_attr(ctx); };
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        BOOST_TEST(parse(s1, e1, '{' >> int_[fun_action] >> '}'));
    }
    {
        auto const fail = [](auto& ctx) { x3::_pass(ctx) = false; };
        std::string input("1234 6543");
        char next = '\0';
        BOOST_TEST(parse(input, x3::int_[fail] | x3::digit[setnext(next)], x3::space).is_partial_match());
        BOOST_TEST(next == '1');
    }

    {
        // ensure no unneeded synthesization, copying and moving occurred
        auto p = '{' >> int_ >> '}';

        spirit_test::stationary st { 0 };
        static_assert(x3::X3Attribute<spirit_test::stationary>);

        BOOST_TEST(parse("{42}", p[([]{})], st));
        BOOST_TEST_EQ(st.val, 42);
    }

    return boost::report_errors();
}
