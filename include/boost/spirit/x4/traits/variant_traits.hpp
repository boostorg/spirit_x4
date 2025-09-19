/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_IS_VARIANT_JAN_10_2012_0823AM
#define BOOST_SPIRIT_X4_IS_VARIANT_JAN_10_2012_0823AM

#include <boost/spirit/x4/core/unused.hpp>

#include <boost/spirit/x4/traits/substitution.hpp>

#include <boost/variant/variant_fwd.hpp> // TODO: remove this

#include <boost/mpl/find.hpp> // TODO: remove this
#include <boost/mpl/deref.hpp> // TODO: remove this
#include <boost/mpl/find_if.hpp> // TODO: remove this
#include <boost/mpl/eval_if.hpp> // TODO: remove this
#include <boost/mpl/begin_end.hpp> // TODO: remove this

#include <type_traits>

namespace boost::spirit::x4::traits
{
    // TODO: define a legit concept for determining variant-like types

    template <typename T>
    struct is_variant : std::false_type {};

    template <typename T>
    constexpr bool is_variant_v = is_variant<T>::value;

    // By declaring a nested struct named `adapted_variant_tag` in
    // your class, you tell spirit that it is regarded as a variant type.
    // The minimum required interface for such a variant is that it has
    // constructors for various types supported by your variant and a
    // typedef 'types' which is an mpl sequence of the contained types.
    // Note (2025): The above spec is obsolete and will change in the near future.
    //
    // This is an intrusive interface. For a non-intrusive interface,
    // specialize the is_variant trait.
    template <typename T>
        requires requires {
            typename T::adapted_variant_tag;
        }
    struct is_variant<T> : std::true_type
    {};

    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct is_variant<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>>
        : std::true_type
    {};


    template <typename Variant, typename T>
    struct variant_find_substitute
    {
        // Get the type from the Variant that can be a substitute for T.
        // If none is found, just return T

        using variant_type = Variant;
        using types = typename variant_type::types;
        using end = typename mpl::end<types>::type;

        using iter_1 = typename mpl::find<types, T>::type;

        using iter = typename mpl::eval_if<
            std::is_same<iter_1, end>,
            mpl::find_if<types, is_substitute<T, mpl::_1> >,
            std::type_identity<iter_1>
        >::type;

        using type = typename mpl::eval_if<
            std::is_same<iter, end>,
            std::type_identity<T>,
            mpl::deref<iter>
        >::type;
    };

    template <typename Variant, typename T>
    using variant_find_substitute_t = typename variant_find_substitute<Variant, T>::type;

    template <typename Variant>
    struct variant_find_substitute<Variant, Variant>
    {
        using type = Variant;
    };


    namespace detail
    {
        template <typename Variant, typename T>
        struct variant_has_substitute_impl
        {
            // Find a type from the Variant that can be a substitute for T.
            // return true_ if one is found, else false_

            using variant_type = Variant;
            using types = typename variant_type::types;
            using end = typename mpl::end<types>::type;
            using iter_1 = typename mpl::find<types, T>::type;

            using iter = typename mpl::eval_if<
                std::is_same<iter_1, end>,
                mpl::find_if<types, is_substitute<T, mpl::_1>>,
                std::type_identity<iter_1>
            >::type;

            using type = std::bool_constant<!std::is_same_v<iter, end>>;
        };
    } // detail

    template <typename Variant, typename T>
    struct variant_has_substitute
        : detail::variant_has_substitute_impl<Variant, T>::type
    {};

    template <typename Variant, typename T>
    constexpr bool variant_has_substitute_v = variant_has_substitute<Variant, T>::value;

    template <typename T>
    struct variant_has_substitute<unused_type, T> : std::true_type {};

    template <typename T>
    struct variant_has_substitute<unused_type const, T> : std::true_type {};

} // boost::spirit::x4::traits

#endif
