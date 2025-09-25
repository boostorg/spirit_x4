/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2019 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"
#include "grammar.hpp"

TEST_CASE("grammar")
{
    CHECK(parse("123", grammar));
}
