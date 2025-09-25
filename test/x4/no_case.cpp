/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/string/string.hpp>

TEST_CASE("no_case")
{
    using x4::no_case;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(no_case['x']);

    {
        using namespace x4::standard;
        CHECK(parse("x", no_case[char_]));
        CHECK(parse("X", no_case[char_('x')]));
        CHECK(parse("X", no_case[char_('X')]));
        CHECK(parse("x", no_case[char_('X')]));
        CHECK(parse("x", no_case[char_('x')]));
        CHECK(!parse("z", no_case[char_('X')]));
        CHECK(!parse("z", no_case[char_('x')]));
        CHECK(parse("x", no_case[char_('a', 'z')]));
        CHECK(parse("X", no_case[char_('a', 'z')]));
        CHECK(!parse("a", no_case[char_('b', 'z')]));
        CHECK(!parse("z", no_case[char_('a', 'y')]));
    }
    {
        using namespace x4::standard;
        CHECK(parse("X", no_case['x']));
        CHECK(parse("X", no_case['X']));
        CHECK(parse("x", no_case['X']));
        CHECK(parse("x", no_case['x']));
        CHECK(!parse("z", no_case['X']));
        CHECK(!parse("z", no_case['x']));
    }

    {
        using namespace x4::standard;
        CHECK(parse("X", no_case[char_("a-z")]));
        CHECK(!parse("1", no_case[char_("a-z")]));
    }

    {
        using namespace x4::standard;
        CHECK(parse("Bochi Bochi", no_case[lit("bochi bochi")]));
        CHECK(parse("BOCHI BOCHI", no_case[lit("bochi bochi")]));
        CHECK(!parse("Vavoo", no_case[lit("bochi bochi")]));
    }

    {
        // should work!
        using namespace x4::standard;
        CHECK(parse("x", no_case[no_case[char_]]));
        CHECK(parse("x", no_case[no_case[char_('x')]]));
        CHECK(parse("yabadabadoo", no_case[no_case[lit("Yabadabadoo")]]));
    }

    {
        using namespace x4::standard;
        CHECK(parse("X", no_case[alnum]));
        CHECK(parse("6", no_case[alnum]));
        CHECK(!parse(":", no_case[alnum]));

        CHECK(parse("X", no_case[lower]));
        CHECK(parse("x", no_case[lower]));
        CHECK(parse("X", no_case[upper]));
        CHECK(parse("x", no_case[upper]));
        CHECK(!parse(":", no_case[lower]));
        CHECK(!parse(":", no_case[upper]));
    }

    {
        // chsets
        namespace standard = x4::standard;
        namespace standard_wide = x4::standard_wide;

        CHECK(parse("x", no_case[standard::char_("a-z")]));
        CHECK(parse("X", no_case[standard::char_("a-z")]));
        CHECK(parse(L"X", no_case[standard_wide::char_(L"a-z")]));
        CHECK(parse(L"X", no_case[standard_wide::char_(L"X")]));
    }

    {
        using namespace x4::standard;
        std::string s("bochi bochi");
        CHECK(parse("Bochi Bochi", no_case[lit(s.c_str())]));
        CHECK(parse("Bochi Bochi", no_case[lit(s)]));
        CHECK(parse("Bochi Bochi", no_case[s.c_str()]));
        CHECK(parse("Bochi Bochi", no_case[s]));
    }

    {
        using namespace x4::standard;
        CHECK(!parse("ą", no_case['a']));
    }
}
