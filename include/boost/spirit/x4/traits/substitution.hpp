/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_IS_SUBSTITUTE_JAN_9_2012_1049PM
#define BOOST_SPIRIT_X4_IS_SUBSTITUTE_JAN_9_2012_1049PM

#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/tuple_traits.hpp>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/value_at_key.hpp>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>

#include <optional>
#include <type_traits>

namespace boost::spirit::x4::traits
{
    template <typename T>
    struct is_variant;

    // Find out if T can be a (strong) substitute for Attribute
    template <typename T, typename Attribute>
    struct is_substitute;

    template <typename T, typename Attribute>
    constexpr bool is_substitute_v = is_substitute<T, Attribute>::value;

    template <typename Variant, typename Attribute>
    struct variant_has_substitute;

    // TODO: reduce MPL usage

    namespace detail
    {
        template <typename T, typename Attribute>
        struct value_type_is_substitute
            : is_substitute<container_value_t<T>, container_value_t<Attribute>>
        {};

        template <typename T, typename Attribute>
        struct is_substitute_impl : std::false_type {};

        template <typename T, typename Attribute>
            requires std::conjunction_v<
                fusion::traits::is_sequence<T>,
                fusion::traits::is_sequence<Attribute>
            >
        struct is_substitute_impl<T, Attribute>
            : mpl::equal<T, Attribute, is_substitute<mpl::_1, mpl::_2>>
        {};

        template <typename T, typename Attribute>
            requires
                is_container_v<std::remove_const_t<T>> &&
                is_container_v<std::remove_const_t<Attribute>>
        struct is_substitute_impl<T, Attribute>
            : value_type_is_substitute<T, Attribute>
        {};

        template <typename T, typename Attribute>
            requires is_variant<std::remove_const_t<Attribute>>::value
        struct is_substitute_impl<T, Attribute>
            : variant_has_substitute<Attribute, T>
        {};
    }

    template <typename T, typename Attribute>
    struct is_substitute
        : std::disjunction<
              std::is_same<T, Attribute>,
              detail::is_substitute_impl<T, Attribute>
        >
    {};

    // for reference T
    template <typename T, typename Attribute>
    struct is_substitute<T&, Attribute>
        : is_substitute<T, Attribute>
    {};

    // for reference Attribute
    template <typename T, typename Attribute>
    struct is_substitute<T, Attribute&>
        : is_substitute<T, Attribute>
    {};

    namespace detail
    {
        template <typename Key, typename Value, typename Map>
        struct has_fusion_kv_in_map : std::false_type {};

        template <typename Key, typename Value, typename Map>
            requires fusion::result_of::has_key<Map, Key>::value
        struct has_fusion_kv_in_map<Key, Value, Map> : is_substitute<
            typename fusion::result_of::value_at_key<Map, Key>::type,
            Value
        > {};
    } // detail

    // 2 element mpl tuple is compatible with fusion::map if:
    // - it's first element type is existing key in map
    // - it second element type is compatible to type stored at the key in map
    template <typename T, typename Attribute>
        requires std::conjunction_v<
            fusion::traits::is_sequence<T>,
            fusion::traits::is_sequence<Attribute>,
            traits::has_size<T, 2>,
            fusion::traits::is_associative<Attribute>
        >
    struct is_substitute<T, Attribute>
    {
        // Checking that "p_key >> p_value" parser can
        // store its result in fusion::map attribute
        using p_key = typename mpl::at_c<T, 0>::type;
        using p_value = typename mpl::at_c<T, 1>::type;

        // For simple `p_key` type we just check that
        // such key can be found in attr and that value under that key
        // matches `p_value`.

        // Otherwise, if p_key is variant over multiple types (as a result of
        // "(key1|key2|key3) >> p_value" parser), check that all
        // keys are found in `fusion::map` attribute and that values
        // under these keys match `p_value`.
        template <typename Variant>
        struct variant_kv
            : mpl::equal_to<
                mpl::size<typename Variant::types>,
                mpl::size<
                    mpl::filter_view<
                        typename Variant::types,
                        detail::has_fusion_kv_in_map<mpl::_1, p_value, Attribute>
                    >
                >
            >
        {};

        static constexpr bool value = std::conditional_t<
            is_variant<p_key>::value,
            variant_kv<p_key>,
            detail::has_fusion_kv_in_map<p_key, p_value, Attribute>
        >::value;
    };

    template <typename T, typename Attribute>
    struct is_substitute<std::optional<T>, std::optional<Attribute>>
        : is_substitute<T, Attribute>
    {};

} // boost::spirit::x4::traits

#endif
