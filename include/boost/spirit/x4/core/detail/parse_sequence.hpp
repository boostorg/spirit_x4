/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X4_SEQUENCE_DETAIL_JAN_06_2013_1015AM
#define BOOST_SPIRIT_X4_SEQUENCE_DETAIL_JAN_06_2013_1015AM

#include <boost/spirit/config.hpp>
#include <boost/spirit/x4/core/detail/parse_into_container.hpp>

#include <boost/spirit/x4/traits/attribute.hpp>
#include <boost/spirit/x4/traits/attribute_category.hpp>
#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/tuple_traits.hpp>
#include <boost/spirit/x4/traits/substitution.hpp>

#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/advance.hpp>
#include <boost/fusion/include/deref.hpp>
#include <boost/fusion/include/iterator_range.hpp>

#include <iterator>
#include <type_traits>
#include <concepts>
#include <utility>

namespace boost::spirit::x4
{
    template <typename Left, typename Right>
    struct sequence;
} // boost::spirit::x4

namespace boost::spirit::x4::detail
{
    template <typename Parser, typename Context, typename Enable = void>
    struct sequence_size
    {
        static constexpr int value = traits::has_attribute_v<Parser, Context>;
    };

    template <typename Parser, typename Context>
        requires Parser::is_pass_through_unary
    struct sequence_size<Parser, Context>
        : sequence_size<typename Parser::subject_type, Context>
    {};

    template <typename L, typename R, typename Context>
    struct sequence_size<sequence<L, R>, Context>
    {
        static constexpr int value =
            sequence_size<L, Context>::value +
            sequence_size<R, Context>::value;
    };

    struct pass_sequence_attribute_unused
    {
        using type = unused_type;

        template <typename T>
        [[nodiscard]] static constexpr unused_type
        call(T&) noexcept
        {
            return unused_type{};
        }
    };

    template <typename Attribute>
    struct pass_sequence_attribute_size_one_view
    {
        using type = typename fusion::result_of::deref<
            typename fusion::result_of::begin<Attribute>::type
        >::type;

        [[nodiscard]] static constexpr type
        call(Attribute& attribute)
            noexcept(noexcept(fusion::deref(fusion::begin(attribute))))
        {
            return fusion::deref(fusion::begin(attribute));
        }
    };

    template <typename Attribute>
    struct pass_through_sequence_attribute
    {
        using type = Attribute&;

        template <typename Attribute_>
        [[nodiscard]] static constexpr Attribute_&
        call(Attribute_& attribute) noexcept
        {
            return attribute;
        }
    };

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute : std::conditional_t<
        traits::is_size_one_view_v<Attribute>,
        pass_sequence_attribute_size_one_view<Attribute>,
        pass_through_sequence_attribute<Attribute>
    >
    {};

    template <typename L, typename R, typename Attribute>
    struct pass_sequence_attribute<sequence<L, R>, Attribute>
        : pass_through_sequence_attribute<Attribute>
    {};

    template <typename Parser, typename Attribute>
        requires Parser::is_pass_through_unary
    struct pass_sequence_attribute<Parser, Attribute>
        : pass_sequence_attribute<typename Parser::subject_type, Attribute>
    {};

    template <typename L, typename R, typename Attribute, typename Context>
    struct partition_attribute
    {
        using attr_category = traits::attribute_category_t<Attribute>;

        static_assert(
            std::same_as<traits::tuple_attribute, attr_category>,
            "The parser expects tuple-like attribute type"
        );

        static constexpr int l_size = sequence_size<L, Context>::value;
        static constexpr int r_size = sequence_size<R, Context>::value;

        static constexpr int actual_size = fusion::result_of::size<Attribute>::value;
        static constexpr int expected_size = l_size + r_size;

        // If you got an error here, then you are trying to pass
        // a fusion sequence with the wrong number of elements
        // as that expected by the (sequence) parser.
        static_assert(
            actual_size >= expected_size,
            "Sequence size of the passed attribute is less than expected."
        );
        static_assert(
            actual_size <= expected_size,
            "Sequence size of the passed attribute is greater than expected."
        );

        using l_begin = typename fusion::result_of::begin<Attribute>::type;
        using l_end = typename fusion::result_of::advance_c<l_begin, l_size>::type;
        using r_end = typename fusion::result_of::end<Attribute>::type;
        using l_part = fusion::iterator_range<l_begin, l_end>;
        using r_part = fusion::iterator_range<l_end, r_end>;
        using l_pass = pass_sequence_attribute<L, l_part>;
        using r_pass = pass_sequence_attribute<R, r_part>;

        [[nodiscard]] static constexpr l_part left(Attribute& s)
            // TODO: noexcept
        {
            auto i = fusion::begin(s);
            return l_part(i, fusion::advance_c<l_size>(i));
        }

        [[nodiscard]] static constexpr r_part right(Attribute& s)
            // TODO: noexcept
        {
            return r_part(
                fusion::advance_c<l_size>(fusion::begin(s)),
                fusion::end(s)
            );
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
        requires
            (!traits::has_attribute_v<L, Context>) &&
            traits::has_attribute_v<R, Context>
    struct partition_attribute<L, R, Attribute, Context>
    {
        using l_pass = pass_sequence_attribute_unused;
        using r_pass = pass_sequence_attribute<R, Attribute>;

        [[nodiscard]] static constexpr unused_type left(Attribute&) noexcept
        {
            return unused;
        }

        [[nodiscard]] static constexpr Attribute& right(Attribute& s) noexcept
        {
            return s;
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
        requires
            traits::has_attribute_v<L, Context> &&
            (!traits::has_attribute_v<R, Context>)
    struct partition_attribute<L, R, Attribute, Context>
    {
        using l_pass = pass_sequence_attribute<L, Attribute>;
        using r_pass = pass_sequence_attribute_unused;

        [[nodiscard]] static constexpr Attribute& left(Attribute& s) noexcept
        {
            return s;
        }

        [[nodiscard]] static constexpr unused_type right(Attribute&) noexcept
        {
            return unused;
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
        requires
            (!traits::has_attribute_v<L, Context>) &&
            (!traits::has_attribute_v<R, Context>)
    struct partition_attribute<L, R, Attribute, Context>
    {
        using l_pass = pass_sequence_attribute_unused;
        using r_pass = pass_sequence_attribute_unused;

        [[nodiscard]] static constexpr unused_type left(Attribute&) noexcept
        {
            return unused;
        }

        [[nodiscard]] static constexpr unused_type right(Attribute&) noexcept
        {
            return unused;
        }
    };

    // Default overload, no constraints on attribute category
    template <
        typename Parser,
        std::forward_iterator It, std::sentinel_for<It> Se,
        typename Context, typename RContext, typename Attribute
    >
    [[nodiscard]] constexpr bool
    parse_sequence(
        Parser const& parser,
        It& first, Se const& last,
        Context const& context, RContext& rcontext,
        Attribute& attr
    )
    {
        using partition = partition_attribute<
            typename Parser::left_type,
            typename Parser::right_type,
            Attribute, Context
        >;
        using l_pass = typename partition::l_pass;
        using r_pass = typename partition::r_pass;

        auto&& l_part = partition::left(attr);
        auto&& r_part = partition::right(attr);
        auto&& l_attr = l_pass::call(l_part);
        auto&& r_attr = r_pass::call(r_part);

        It const first_saved = first;

        if (parser.left.parse(first, last, context, rcontext, l_attr) &&
            parser.right.parse(first, last, context, rcontext, r_attr)
        )
        {
            return true;
        }
        first = first_saved;
        return false;
    }

    template <typename Parser, typename Context>
    constexpr bool pass_sequence_container_attribute = sequence_size<Parser, Context>::value > 1;

    template <
        typename Parser,
        std::forward_iterator It, std::sentinel_for<It> Se,
        typename Context, typename RContext, typename Attribute
    >
        requires pass_sequence_container_attribute<Parser, Context>
    [[nodiscard]] constexpr bool
    parse_sequence_container(
        Parser const& parser,
        It& first, Se const& last,
        Context const& context, RContext& rcontext,
        Attribute& attr
    )
        noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, RContext, Attribute>)
    {
        static_assert(Parsable<Parser, It, Se, Context, RContext, Attribute>);
        return parser.parse(first, last, context, rcontext, attr);
    }

    template <
        typename Parser,
        std::forward_iterator It, std::sentinel_for<It> Se,
        typename Context, typename RContext,
        typename Attribute
    >
        requires (!pass_sequence_container_attribute<Parser, Context>)
    [[nodiscard]] constexpr bool
    parse_sequence_container(
        Parser const& parser,
        It& first, Se const& last,
        Context const& context, RContext& rcontext,
        Attribute& attr
    )
        noexcept(noexcept(detail::parse_into_container(parser, first, last, context, rcontext, attr)))
    {
        return detail::parse_into_container(parser, first, last, context, rcontext, attr);
    }

    template <
        typename Parser,
        std::forward_iterator It, std::sentinel_for<It> Se,
        typename Context, typename RContext
    >
    [[nodiscard]] constexpr bool
    parse_sequence(
        Parser const& parser,
        It& first, Se const& last,
        Context const& context, RContext& rcontext,
        traits::CategorizedAttr<traits::container_attribute> auto& attr
    )
        noexcept(
            std::is_nothrow_copy_assignable_v<It> &&
            noexcept(detail::parse_sequence_container(parser.left, first, last, context, rcontext, attr)) &&
            noexcept(detail::parse_sequence_container(parser.right, first, last, context, rcontext, attr))
        )
    {
        It const first_saved = first;
        if (detail::parse_sequence_container(parser.left, first, last, context, rcontext, attr) &&
            detail::parse_sequence_container(parser.right, first, last, context, rcontext, attr)
        )
        {
            return true;
        }
        first = first_saved;
        return false;
    }

    // We can come here in 2 cases:
    //
    // 1) When sequence is key >> value and therefore must
    //    be parsed with tuple synthesized attribute and then
    //    that tuple is used to save into associative attribute provided here.
    //    Example: key >> value;
    //
    // 2) When either this->left or this->right provides full key-value
    //    pair (like in case 1) and another one provides nothing.
    //    Example: eps >> rule<class x; fusion::map<...>>
    //
    // The first case must be parsed as whole.
    // The second one should be parsed separately for left and right.
    template <typename Parser, typename Context>
    constexpr bool assoc_sequence_should_split =
        std::is_same_v<traits::attribute_of_t<decltype(std::declval<Parser>().left), Context>, unused_type> ||
        std::is_same_v<traits::attribute_of_t<decltype(std::declval<Parser>().right), Context>, unused_type>;

    template <
        typename Parser,
        std::forward_iterator It, std::sentinel_for<It> Se,
        typename Context, typename RContext
    >
        requires (!assoc_sequence_should_split<Parser, Context>)
    [[nodiscard]] constexpr bool
    parse_sequence(
        Parser const& parser,
        It& first, Se const& last,
        Context const& context, RContext& rcontext,
        traits::CategorizedAttr<traits::associative_attribute> auto& attr
    ) noexcept(noexcept(detail::parse_into_container(parser, first, last, context, rcontext, attr)))
    {
	    return detail::parse_into_container(parser, first, last, context, rcontext, attr);
    }

    template <
        typename Parser,
        std::forward_iterator It, std::sentinel_for<It> Se,
        typename Context, typename RContext
    >
        requires assoc_sequence_should_split<Parser, Context>
    [[nodiscard]] constexpr bool
    parse_sequence(
        Parser const& parser,
        It& first, Se const& last,
        Context const& context, RContext& rcontext,
        traits::CategorizedAttr<traits::associative_attribute> auto& attr
    ) noexcept(
        std::is_nothrow_copy_assignable_v<It> &&
        noexcept(parser.left.parse(first, last, context, rcontext, attr)) &&
        noexcept(parser.right.parse(first, last, context, rcontext, attr))
    )
    {
        It const first_saved = first;
        if (parser.left.parse(first, last, context, rcontext, attr) &&
            parser.right.parse(first, last, context, rcontext, attr)
        )
        {
            return true;
        }
        first = first_saved;
        return false;
    }

    template <typename Left, typename Right, typename Context, typename RContext>
    struct parse_into_container_impl<sequence<Left, Right>, Context, RContext>
    {
        using parser_type = sequence<Left, Right>;

        template <typename Attribute>
        static constexpr bool is_container_substitute = traits::is_substitute_v<
            traits::attribute_of_t<parser_type, Context>,
            traits::container_value_t<Attribute>
        >;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
            requires is_container_substitute<Attribute>
        [[nodiscard]] static constexpr bool
        call(
            parser_type const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext,
            Attribute& attr
        ) noexcept(noexcept(parse_into_container_base_impl<parser_type>::call(
            parser, first, last, context, rcontext, attr
        )))
        {
            return parse_into_container_base_impl<parser_type>::call(
                parser, first, last, context, rcontext, attr
            );
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
            requires (!is_container_substitute<Attribute>)
        [[nodiscard]] static constexpr bool
        call(
            parser_type const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext,
            Attribute& attr
        ) // never noexcept (requires container insertion)
        {
            // inform user what went wrong if we jumped here in attempt to
            // parse incompatible sequence into fusion::map
            static_assert(
                !std::is_same_v<traits::attribute_category_t<Attribute>, traits::associative_attribute>,
                "To parse directly into fusion::map sequence must produce tuple attribute "
                "where type of first element is existing key in fusion::map and second element "
                "is value to be stored under that key"
            );

            static_assert(
                std::same_as<traits::attribute_category_t<Attribute>, traits::container_attribute> ||
                std::same_as<traits::attribute_category_t<Attribute>, traits::unused_attribute>
            );

            if constexpr (
                std::is_same_v<std::remove_const_t<Attribute>, unused_type> ||
                std::is_same_v<std::remove_const_t<Attribute>, unused_container_type>
            )
            {
                return detail::parse_sequence(parser, first, last, context, rcontext, x4::assume_container(attr));
            }
            else
            {
                Attribute attr_;
                if (!detail::parse_sequence(parser, first, last, context, rcontext, attr_))
                {
                    return false;
                }
                traits::append(
                    attr,
                    std::make_move_iterator(traits::begin(attr_)),
                    std::make_move_iterator(traits::end(attr_))
                );
                return true;
            }
        }
    };

} // boost::spirit::x4::detail

#endif
