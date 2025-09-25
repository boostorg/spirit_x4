/*=============================================================================
    Copyright (c) 2001-2017 Joel de Guzman
    Copyright (c) 2017 think-cell GmbH
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/eoi.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/raw.hpp>
#include <boost/spirit/x4/directive/repeat.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>

#include <ranges>
#include <algorithm>
#include <string>

TEST_CASE("iterator_check")
{
    using x4::raw;
    using x4::eps;
    using x4::eoi;
    using x4::standard::upper;
    using x4::repeat;

    std::string input = "abcde";
    auto const rng = input | std::views::transform([](char c) {
        return c < 'a' || 'z' < c ? c : static_cast<char>(c - 'a' + 'A');
    });
    using range_type = decltype(rng);
    static_assert(std::ranges::forward_range<range_type>);

    {
        std::string str;
        REQUIRE(parse(std::ranges::begin(rng), std::ranges::end(rng), +upper >> eoi, str));
        CHECK("ABCDE" == str);
    }

    {
        std::ranges::subrange<std::ranges::iterator_t<range_type>> str;

        REQUIRE(parse(std::ranges::begin(rng), std::ranges::end(rng), raw[+upper >> eoi], str));
        CHECK(std::ranges::equal(std::string("ABCDE"), str));
    }

    CHECK(parse(std::ranges::begin(rng), std::ranges::end(rng), (repeat(6)[upper] | repeat(5)[upper]) >> eoi));
}
