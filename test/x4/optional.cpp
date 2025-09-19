/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/optional.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>

#include <boost/fusion/adapted/struct.hpp>
#include <boost/fusion/include/vector.hpp>

#include <optional>

#ifdef _MSC_VER
// bogus https://developercommunity.visualstudio.com/t/buggy-warning-c4709/471956
# pragma warning(disable: 4709) // comma operator within array index expression
#endif

struct adata
{
    int a;
    std::optional<int> b;
};

BOOST_FUSION_ADAPT_STRUCT(adata,
    a, b
)

struct test_attribute_type
{
    template <typename Context>
    void operator()(Context& ctx) const
    {
        BOOST_TEST(typeid(decltype(_attr(ctx))).name() == typeid(std::optional<int>).name());
    }
};

int main()
{
    static_assert(x4::traits::is_optional_v<std::optional<int>>);

    using x4::int_;
    using x4::omit;
    using x4::standard::char_;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(-int_);

    {
        BOOST_TEST(parse("1234", -int_));
        BOOST_TEST(parse("abcd", -int_).is_partial_match());

        std::optional<int> n;
        BOOST_TEST(parse("", -int_, n)) && BOOST_TEST(!n);
        BOOST_TEST(parse("123", -int_, n)) && BOOST_TEST(n) && BOOST_TEST_EQ(*n, 123);

        std::optional<std::string> s;
        BOOST_TEST(parse("", -+char_, s)) && BOOST_TEST(!s);
        BOOST_TEST(parse("abc", -+char_, s)) && BOOST_TEST(s) && BOOST_TEST_EQ(*s, "abc");
    }

    {
        // test propagation of unused
        using boost::fusion::at_c;
        using boost::fusion::vector;

        vector<char, char> v;
        BOOST_TEST(parse("a1234c", char_ >> -omit[int_] >> char_, v));
        BOOST_TEST((at_c<0>(v) == 'a'));
        BOOST_TEST((at_c<1>(v) == 'c'));

        v = boost::fusion::vector<char, char>();
        BOOST_TEST(parse("a1234c", char_ >> omit[-int_] >> char_, v));
        BOOST_TEST((at_c<0>(v) == 'a'));
        BOOST_TEST((at_c<1>(v) == 'c'));

        char ch;
        BOOST_TEST(parse(",c", -(',' >> char_), ch));
        BOOST_TEST((ch == 'c'));
    }

    {
        // test action
        std::optional<int> n = 0;
        BOOST_TEST(parse("1234", (-int_)[test_attribute_type()], n));
        BOOST_TEST((*n == 1234));
    }

    {
        std::string s;
        BOOST_TEST(parse("abc", char_ >> -(char_ >> char_), s));
        BOOST_TEST(s == "abc");
    }

    {
        std::optional<int> n = 0;
        auto f = [&](auto& ctx){ n = _attr(ctx); };

        BOOST_TEST(parse("1234", (-int_)[f]));
        BOOST_TEST(*n == 1234);

        n = std::optional<int>();
        BOOST_TEST(parse("abcd", (-int_)[f]).is_partial_match());
        BOOST_TEST(!n);
    }

    {
        std::vector<adata> v;
        BOOST_TEST(parse("a 1 2 a 2", *('a' >> int_ >> -int_), v, char_(' ')));
        BOOST_TEST(
            2 == v.size() &&
            1 == v[0].a && v[0].b && 2 == *(v[0].b) &&
            2 == v[1].a && !v[1].b
        );
    }

    {
        // test move only types
        std::optional<spirit_test::move_only> o;
        BOOST_TEST(parse("s", -spirit_test::synth_move_only, o));
        BOOST_TEST(o);
    }

    return boost::report_errors();
}
