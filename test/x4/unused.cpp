/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/traits/transform_attribute.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/rule.hpp>

#include <type_traits>
#include <concepts>

TEST_CASE("unused")
{
    namespace traits = x4::traits;
    using traits::Transformable;
    using traits::transform_attribute;
    using x4::unused_container_type;
    using x4::unused_container;

    STATIC_CHECK(std::is_trivially_copyable_v<unused_type>);
    STATIC_CHECK(std::is_trivially_default_constructible_v<unused_type>);

    STATIC_CHECK(std::assignable_from<unused_type&, unused_type>);
    STATIC_CHECK(std::assignable_from<unused_type&, unused_type const>);
    STATIC_CHECK(std::assignable_from<unused_type&, unused_type&>);
    STATIC_CHECK(std::assignable_from<unused_type&, unused_type const&>);

    // STATIC_CHECK(std::assignable_from<unused_type const&, unused_type>);
    // STATIC_CHECK(std::assignable_from<unused_type const&, unused_type const>);
    // STATIC_CHECK(std::assignable_from<unused_type const&, unused_type&>);
    // STATIC_CHECK(std::assignable_from<unused_type const&, unused_type const&>);

    unused_type unused_mut;
    STATIC_CHECK(std::same_as<decltype((unused)), unused_type const&>);
    STATIC_CHECK(std::same_as<decltype((unused_mut)), unused_type&>);
    // STATIC_CHECK(std::same_as<decltype(unused = 123), unused_type const&>);
    // STATIC_CHECK(std::same_as<decltype(unused = unused), unused_type const&>);
    // STATIC_CHECK(std::same_as<decltype(unused = unused_mut), unused_type const&>);
    // STATIC_CHECK(std::same_as<decltype(unused_mut = 123), unused_type&>);
    STATIC_CHECK(std::same_as<decltype(unused_mut = unused), unused_type&>);
    STATIC_CHECK(std::same_as<decltype(unused_mut = unused_mut), unused_type&>);

    {
        constexpr auto test_use = [](x4::unused_type) { return true; };
        // STATIC_CHECK(test_use(0));
        STATIC_CHECK(test_use(unused));
        (void)test_use(unused_mut);
        STATIC_CHECK(test_use(unused_type{}));
    }

    STATIC_CHECK(x4::X4Attribute<unused_type>);
    STATIC_CHECK(x4::X4Attribute<unused_type const>);

    STATIC_CHECK(x4::X4Attribute<unused_container_type>);
    STATIC_CHECK(x4::X4Attribute<unused_container_type const>);

    // unused => unused
    {
        STATIC_CHECK(Transformable<unused_type, unused_type>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_type, unused_type>::type, unused_type>);

        STATIC_CHECK(Transformable<unused_type const, unused_type>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_type const, unused_type>::type, unused_type>);

        STATIC_CHECK(Transformable<unused_type, unused_type const>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_type, unused_type const>::type, unused_type>);

        STATIC_CHECK(Transformable<unused_type const, unused_type const>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_type const, unused_type const>::type, unused_type>);
    }

    // unused_container => unused_container
    {
        STATIC_CHECK(Transformable<unused_container_type, unused_container_type>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_container_type, unused_container_type>::type, unused_container_type>);

        STATIC_CHECK(Transformable<unused_container_type const, unused_container_type>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_container_type const, unused_container_type>::type, unused_container_type>);

        STATIC_CHECK(Transformable<unused_container_type, unused_container_type const>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_container_type, unused_container_type const>::type, unused_container_type>);

        STATIC_CHECK(Transformable<unused_container_type const, unused_container_type const>);
        STATIC_CHECK(std::same_as<typename transform_attribute<unused_container_type const, unused_container_type const>::type, unused_container_type>);
    }
}
