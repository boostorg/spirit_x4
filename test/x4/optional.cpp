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
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/vector.hpp>

#include <optional>

#ifdef _MSC_VER
// bogus https://developercommunity.visualstudio.com/t/buggy-warning-c4709/471956
# pragma warning(disable: 4709) // comma operator within array index expression
#endif

struct adata
{
    int a = 0;
    std::optional<int> b;
};

BOOST_FUSION_ADAPT_STRUCT(adata,
    a, b
)

namespace {

struct test_attribute_type
{
    template<class Context>
    void operator()(Context& ctx) const
    {
        CHECK(typeid(decltype(x4::_attr(ctx))).name() == typeid(std::optional<int>).name());
    }
};

} // anonymous

TEST_CASE("optional")
{
    static_assert(x4::traits::is_optional_v<std::optional<int>>);

    using x4::int_;
    using x4::omit;
    using x4::standard::char_;
    using x4::_attr;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(-int_);

    CHECK(parse("1234", -int_));
    CHECK(parse("abcd", -int_).is_partial_match());

    {
        std::optional<int> n;
        REQUIRE(parse("", -int_, n));
        CHECK(!n.has_value());
    }
    {
        std::optional<int> n;
        REQUIRE(parse("123", -int_, n));
        REQUIRE(n.has_value());
        CHECK(*n == 123);
    }
    {
        std::optional<std::string> s;
        REQUIRE(parse("", -+char_, s));
        REQUIRE(!s.has_value());
    }
    {
        std::optional<std::string> s;
        REQUIRE(parse("abc", -+char_, s));
        REQUIRE(s.has_value());
        CHECK(*s == "abc");
    }

    {
        // test propagation of unused
        using boost::fusion::at_c;
        using boost::fusion::vector;

        {
            vector<char, char> v;
            REQUIRE(parse("a1234c", char_ >> -omit[int_] >> char_, v));
            CHECK(at_c<0>(v) == 'a');
            CHECK(at_c<1>(v) == 'c');
        }
        {
            vector<char, char> v;
            REQUIRE(parse("a1234c", char_ >> omit[-int_] >> char_, v));
            CHECK(at_c<0>(v) == 'a');
            CHECK(at_c<1>(v) == 'c');
        }

        {
            char ch{};
            REQUIRE(parse(",c", -(',' >> char_), ch));
            CHECK(ch == 'c');
        }
    }

    {
        // test action
        std::optional<int> n = 0;
        REQUIRE(parse("1234", (-int_)[test_attribute_type()], n));
        CHECK(*n == 1234);
    }

    {
        std::string s;
        REQUIRE(parse("abc", char_ >> -(char_ >> char_), s));
        CHECK(s == "abc");
    }

    {
        std::optional<int> n;
        auto f = [&](auto& ctx) { n = _attr(ctx); };
        CHECK(parse("abcd", (-int_)[f]).is_partial_match());
        CHECK(!n.has_value());
    }
    {
        std::optional<int> n = 0;
        auto f = [&](auto& ctx){ n = _attr(ctx); };
        REQUIRE(parse("1234", (-int_)[f]));
        CHECK(*n == 1234);
    }

    {
        std::vector<adata> v;
        REQUIRE(parse("a 1 2 a 2", *('a' >> int_ >> -int_), char_(' '), v));
        REQUIRE(v.size() == 2);
        CHECK(v[0].a == 1);
        REQUIRE(v[0].b.has_value());
        CHECK(*v[0].b == 2);
        CHECK(v[1].a == 2);
        CHECK(!v[1].b.has_value());
    }

    {
        // test move only types
        std::optional<spirit_test::move_only> o;
        REQUIRE(parse("s", -spirit_test::synth_move_only, o));
        CHECK(o.has_value());
    }
}
