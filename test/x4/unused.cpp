/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/core/unused.hpp>
#include <boost/spirit/x4/traits/transform_attribute.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/rule.hpp>

#include <type_traits>
#include <concepts>

int main()
{
    namespace x4 = boost::spirit::x4;
    namespace traits = x4::traits;
    using traits::Transformable;
    using traits::transform_attribute;
    using x4::unused_type;
    using x4::unused;
    using x4::unused_container_type;
    using x4::unused_container;

    static_assert(std::is_trivially_copyable_v<unused_type>);
    static_assert(std::is_trivially_default_constructible_v<unused_type>);

    static_assert(std::assignable_from<unused_type&, unused_type>);
    static_assert(std::assignable_from<unused_type&, unused_type const>);
    static_assert(std::assignable_from<unused_type&, unused_type&>);
    static_assert(std::assignable_from<unused_type&, unused_type const&>);

    // static_assert(std::assignable_from<unused_type const&, unused_type>);
    // static_assert(std::assignable_from<unused_type const&, unused_type const>);
    // static_assert(std::assignable_from<unused_type const&, unused_type&>);
    // static_assert(std::assignable_from<unused_type const&, unused_type const&>);

    unused_type unused_mut;
    static_assert(std::is_same_v<decltype((unused)), unused_type const&>);
    static_assert(std::is_same_v<decltype((unused_mut)), unused_type&>);
    // static_assert(std::is_same_v<decltype(unused = 123), unused_type const&>);
    // static_assert(std::is_same_v<decltype(unused = unused), unused_type const&>);
    // static_assert(std::is_same_v<decltype(unused = unused_mut), unused_type const&>);
    // static_assert(std::is_same_v<decltype(unused_mut = 123), unused_type&>);
    static_assert(std::is_same_v<decltype(unused_mut = unused), unused_type&>);
    static_assert(std::is_same_v<decltype(unused_mut = unused_mut), unused_type&>);

    {
        constexpr auto test_use = [](x4::unused_type) { return true; };
        // static_assert(test_use(0));
        static_assert(test_use(unused));
        (void)test_use(unused_mut);
        static_assert(test_use(unused_type{}));
    }

    static_assert(x4::X4Attribute<unused_type>);
    static_assert(x4::X4Attribute<unused_type const>);

    static_assert(x4::X4Attribute<unused_container_type>);
    static_assert(x4::X4Attribute<unused_container_type const>);

    // unused => unused
    {
        static_assert(Transformable<unused_type, unused_type>);
        static_assert(std::same_as<typename transform_attribute<unused_type, unused_type>::type, unused_type>);

        static_assert(Transformable<unused_type const, unused_type>);
        static_assert(std::same_as<typename transform_attribute<unused_type const, unused_type>::type, unused_type>);

        static_assert(Transformable<unused_type, unused_type const>);
        static_assert(std::same_as<typename transform_attribute<unused_type, unused_type const>::type, unused_type>);

        static_assert(Transformable<unused_type const, unused_type const>);
        static_assert(std::same_as<typename transform_attribute<unused_type const, unused_type const>::type, unused_type>);
    }

    // unused_container => unused_container
    {
        static_assert(Transformable<unused_container_type, unused_container_type>);
        static_assert(std::same_as<typename transform_attribute<unused_container_type, unused_container_type>::type, unused_container_type>);

        static_assert(Transformable<unused_container_type const, unused_container_type>);
        static_assert(std::same_as<typename transform_attribute<unused_container_type const, unused_container_type>::type, unused_container_type>);

        static_assert(Transformable<unused_container_type, unused_container_type const>);
        static_assert(std::same_as<typename transform_attribute<unused_container_type, unused_container_type const>::type, unused_container_type>);

        static_assert(Transformable<unused_container_type const, unused_container_type const>);
        static_assert(std::same_as<typename transform_attribute<unused_container_type const, unused_container_type const>::type, unused_container_type>);
    }

    return boost::report_errors();
}
