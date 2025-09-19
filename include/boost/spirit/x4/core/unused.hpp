/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X4_UNUSED_APRIL_16_2006_0616PM
#define BOOST_SPIRIT_X4_UNUSED_APRIL_16_2006_0616PM

#include <boost/spirit/x4/core/config.hpp>

#include <type_traits>

namespace boost::spirit::x4
{
    struct unused_type
    {
        constexpr explicit unused_type() noexcept = default;

        // unused_type can masquerade as an empty context (see context.hpp)

        template <typename ID>
        [[nodiscard]] static constexpr unused_type get(ID&&) noexcept
        {
            return unused_type{};
        }
    };

    // The attribute category type for `unused_container_type` is
    // `container_attribute`, but it does not satisfy `is_container`.
    struct unused_container_type
    {
        constexpr explicit unused_container_type() noexcept = default;
    };

    inline namespace cpos
    {
        inline constexpr unused_type unused{};
        inline constexpr unused_container_type unused_container{};
    } // cpos

    template <typename T>
        requires (!std::is_same_v<std::remove_const_t<T>, unused_type>)
    [[nodiscard]] constexpr T&
    assume_container(T& attr) noexcept
    {
        return attr;
    }

    [[nodiscard]] constexpr unused_container_type const&
    assume_container(unused_type const&) noexcept
    {
        return unused_container;
    }

} // boost::spirit::x4

#endif
