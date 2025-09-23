/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Bryce Lelbach
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/numeric/bool.hpp>

#include <string_view>
#include <iterator>

namespace {

struct backwards_bool_policies : x4::bool_policies<>
{
    // we want to interpret a 'true' spelled backwards as 'false'
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Attr, class CaseCompare>
    [[nodiscard]] static constexpr bool
    parse_false(It& first, Se const& last, Attr& attr, CaseCompare const& case_compare)
    {
        using namespace std::string_view_literals;
        namespace x4 = boost::spirit::x4;
        if (x4::detail::string_parse("eurt"sv, first, last, unused_container, case_compare)) {
            x4::move_to(false, attr);   // result is false
            return true;
        }
        return false;
    }
};

} // anonymous

int main()
{
    using x4::bool_;
    using x4::true_;
    using x4::false_;
    using x4::no_case;

    {
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(bool_);

        BOOST_TEST(parse("true", bool_));
        BOOST_TEST(parse("false", bool_));
        BOOST_TEST(!parse("fasle", bool_));
    }

    {
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(true_);
        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(false_);

        BOOST_TEST(parse("true", true_));
        BOOST_TEST(!parse("true", false_));
        BOOST_TEST(parse("false", false_));
        BOOST_TEST(!parse("false", true_));
    }

    {
        BOOST_TEST(parse("True", no_case[bool_]));
        BOOST_TEST(parse("False", no_case[bool_]));
        BOOST_TEST(parse("True", no_case[true_]));
        BOOST_TEST(parse("False", no_case[false_]));
    }

    {
        bool b = false;
        BOOST_TEST(parse("true", bool_, b) && b);
        BOOST_TEST(parse("false", bool_, b) && !b);
        BOOST_TEST(!parse("fasle", bool_, b));
    }

    {
        using backwards_bool_type = x4::bool_parser<bool, x4::char_encoding::standard, backwards_bool_policies>;
        constexpr backwards_bool_type backwards_bool{};

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(backwards_bool);

        BOOST_TEST(parse("true", backwards_bool));
        BOOST_TEST(parse("eurt", backwards_bool));
        BOOST_TEST(!parse("false", backwards_bool));
        BOOST_TEST(!parse("fasle", backwards_bool));

        bool b = false;
        BOOST_TEST(parse("true", backwards_bool, b) && b);
        BOOST_TEST(parse("eurt", backwards_bool, b) && !b);
        BOOST_TEST(!parse("false", backwards_bool, b));
        BOOST_TEST(!parse("fasle", backwards_bool, b));
    }

    {
        struct test_bool_type
        {
            test_bool_type(bool b = false) // provide conversion
                : b(b)
            {}
            bool b;
        };

        using bool_test_type = x4::bool_parser<test_bool_type, x4::char_encoding::standard>;
        constexpr bool_test_type test_bool{};

        BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(test_bool);

        BOOST_TEST(parse("true", test_bool));
        BOOST_TEST(parse("false", test_bool));
        BOOST_TEST(!parse("fasle", test_bool));

        test_bool_type b = false;
        BOOST_TEST(parse("true", test_bool, b) && b.b);
        BOOST_TEST(parse("false", test_bool, b) && !b.b);
        BOOST_TEST(!parse("fasle", test_bool, b));
    }

    return boost::report_errors();
}
