/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>

#include <functional>

#include <cstring>

int main()
{
    using x4::int_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(x4::int_type{}[std::true_type{}]);

    {
        int x = 0;
        auto const fun_action = [&](auto& ctx) { x += x4::_attr(ctx); };
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        BOOST_TEST(parse(s1, e1, '{' >> int_[fun_action] >> '}'));
    }
    {
        auto const fail = [](auto& ctx) { x4::_pass(ctx) = false; };
        std::string input("1234 6543");
        char next = '\0';

        auto const setnext = [&](auto& ctx) {
            next = x4::_attr(ctx);
        };

        BOOST_TEST(parse(input, x4::int_[fail] | x4::digit[setnext], x4::space).is_partial_match());
        BOOST_TEST(next == '1');
    }

    {
        // ensure no unneeded synthesization, copying and moving occurred
        auto p = '{' >> int_ >> '}';

        spirit_test::stationary st { 0 };
        static_assert(x4::X4Attribute<spirit_test::stationary>);

        BOOST_TEST(parse("{42}", p[([]{})], st));
        BOOST_TEST_EQ(st.val, 42);
    }

    return boost::report_errors();
}
