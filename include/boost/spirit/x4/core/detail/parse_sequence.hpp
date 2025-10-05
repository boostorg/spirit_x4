#ifndef BOOST_SPIRIT_X4_CORE_DETAIL_PARSE_SEQUENCE_HPP
#define BOOST_SPIRIT_X4_CORE_DETAIL_PARSE_SEQUENCE_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

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

namespace boost::spirit::x4 {

template<class Left, class Right>
struct sequence;

} // boost::spirit::x4

namespace boost::spirit::x4::detail {

template<class Parser, class Context>
struct sequence_size
{
    static constexpr std::size_t value = traits::has_attribute_v<Parser, Context>;
};

template<class Parser, class Context>
    requires Parser::is_pass_through_unary
struct sequence_size<Parser, Context>
    : sequence_size<typename Parser::subject_type, Context>
{};

template<class L, class R, class Context>
struct sequence_size<sequence<L, R>, Context>
{
    static constexpr std::size_t value =
        sequence_size<L, Context>::value +
        sequence_size<R, Context>::value;
};

template<class Parser, class Context>
constexpr bool is_sequence_size_more_than_1 = sequence_size<Parser, Context>::value > 1;


struct pass_sequence_attribute_unused
{
    using type = unused_type;

    template<class T>
    [[nodiscard]] static constexpr unused_type
    call(T&) noexcept
    {
        return unused_type{};
    }
};

template<class Attr>
struct pass_sequence_attribute_size_one_view
{
    using type = typename fusion::result_of::deref<
        typename fusion::result_of::begin<Attr>::type
    >::type;

    [[nodiscard]] static constexpr type
    call(Attr& attribute)
        noexcept(noexcept(fusion::deref(fusion::begin(attribute))))
    {
        return fusion::deref(fusion::begin(attribute));
    }
};

template<class Attr>
struct pass_through_sequence_attribute
{
    using type = Attr&;

    template<class Attr_>
    [[nodiscard]] static constexpr Attr_&
    call(Attr_& attribute) noexcept
    {
        return attribute;
    }
};

template<class Parser, class Attr>
struct pass_sequence_attribute : std::conditional_t<
    traits::is_size_one_view_v<Attr>,
    pass_sequence_attribute_size_one_view<Attr>,
    pass_through_sequence_attribute<Attr>
>
{};

template<class L, class R, class Attr>
struct pass_sequence_attribute<sequence<L, R>, Attr>
    : pass_through_sequence_attribute<Attr>
{};

template<class Parser, class Attr>
    requires Parser::is_pass_through_unary
struct pass_sequence_attribute<Parser, Attr>
    : pass_sequence_attribute<typename Parser::subject_type, Attr>
{};

template<class L, class R, class Attr, class Context>
struct partition_attribute
{
    using attr_category = traits::attribute_category_t<Attr>;

    static_assert(
        std::same_as<traits::tuple_attr, attr_category>,
        "The parser expects tuple-like attribute type"
    );

    static constexpr std::size_t l_size = sequence_size<L, Context>::value;
    static constexpr std::size_t r_size = sequence_size<R, Context>::value;

    static constexpr std::size_t actual_size = static_cast<std::size_t>(fusion::result_of::size<Attr>::value);
    static constexpr std::size_t expected_size = l_size + r_size;

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

    using l_begin = typename fusion::result_of::begin<Attr>::type;
    using l_end = typename fusion::result_of::advance_c<l_begin, l_size>::type;
    using r_end = typename fusion::result_of::end<Attr>::type;
    using l_part = fusion::iterator_range<l_begin, l_end>;
    using r_part = fusion::iterator_range<l_end, r_end>;
    using l_pass = pass_sequence_attribute<L, l_part>;
    using r_pass = pass_sequence_attribute<R, r_part>;

    [[nodiscard]] static constexpr l_part left(Attr& s)
        // TODO: noexcept
    {
        auto i = fusion::begin(s);
        return l_part(i, fusion::advance_c<l_size>(i));
    }

    [[nodiscard]] static constexpr r_part right(Attr& s)
        // TODO: noexcept
    {
        return r_part(
            fusion::advance_c<l_size>(fusion::begin(s)),
            fusion::end(s)
        );
    }
};

template<class L, class R, class Attr, class Context>
    requires
        (!traits::has_attribute_v<L, Context>) &&
        traits::has_attribute_v<R, Context>
struct partition_attribute<L, R, Attr, Context>
{
    using l_pass = pass_sequence_attribute_unused;
    using r_pass = pass_sequence_attribute<R, Attr>;

    [[nodiscard]] static constexpr unused_type left(Attr&) noexcept
    {
        return unused;
    }

    [[nodiscard]] static constexpr Attr& right(Attr& s) noexcept
    {
        return s;
    }
};

template<class L, class R, class Attr, class Context>
    requires
        traits::has_attribute_v<L, Context> &&
        (!traits::has_attribute_v<R, Context>)
struct partition_attribute<L, R, Attr, Context>
{
    using l_pass = pass_sequence_attribute<L, Attr>;
    using r_pass = pass_sequence_attribute_unused;

    [[nodiscard]] static constexpr Attr& left(Attr& s) noexcept
    {
        return s;
    }

    [[nodiscard]] static constexpr unused_type right(Attr&) noexcept
    {
        return unused;
    }
};

template<class L, class R, class Attr, class Context>
    requires
        (!traits::has_attribute_v<L, Context>) &&
        (!traits::has_attribute_v<R, Context>)
struct partition_attribute<L, R, Attr, Context>
{
    using l_pass = pass_sequence_attribute_unused;
    using r_pass = pass_sequence_attribute_unused;

    [[nodiscard]] static constexpr unused_type left(Attr&) noexcept
    {
        return unused;
    }

    [[nodiscard]] static constexpr unused_type right(Attr&) noexcept
    {
        return unused;
    }
};

// Default overload, no constraints on attribute category
template<
    class Parser,
    std::forward_iterator It, std::sentinel_for<It> Se,
    class Context,
    class Attr // unconstrained
>
[[nodiscard]] constexpr bool
parse_sequence(
    Parser const& parser,
    It& first, Se const& last,
    Context const& ctx,
    Attr& attr
) // TODO: noexcept
{
    static_assert(X4Attribute<Attr>);

    using partition = partition_attribute<
        typename Parser::left_type,
        typename Parser::right_type,
        Attr, Context
    >;
    using l_pass = typename partition::l_pass;
    using r_pass = typename partition::r_pass;

    auto&& l_part = partition::left(attr);
    auto&& r_part = partition::right(attr);
    auto&& l_attr = l_pass::call(l_part);
    auto&& r_attr = r_pass::call(r_part);

    auto&& l_attr_appender = x4::make_container_appender(l_attr);
    auto&& r_attr_appender = x4::make_container_appender(r_attr);

    It local_it = first;
    if (parser.left.parse(local_it, last, ctx, l_attr_appender) &&
        parser.right.parse(local_it, last, ctx, r_attr_appender)
    ) {
        first = std::move(local_it);
        return true;
    }

    return false;
}

template<
    class Parser,
    std::forward_iterator It, std::sentinel_for<It> Se,
    class Context,
    X4Attribute Attr
>
    requires is_sequence_size_more_than_1<Parser, Context>
[[nodiscard]] constexpr bool
parse_sequence_impl(
    Parser const& parser,
    It& first, Se const& last,
    Context const& ctx,
    Attr& attr
)
    noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, Attr>)
{
    static_assert(Parsable<Parser, It, Se, Context, Attr>);
    return parser.parse(first, last, ctx, attr);
}

template<class Parser, std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    requires (!is_sequence_size_more_than_1<Parser, Context>)
[[nodiscard]] constexpr bool
parse_sequence_impl(
    Parser const& parser,
    It& first, Se const& last,
    Context const& ctx,
    X4Attribute auto& attr
)
    noexcept(noexcept(detail::parse_into_container(parser, first, last, ctx, attr)))
{
    return detail::parse_into_container(parser, first, last, ctx, attr);
}

template<class Parser, std::forward_iterator It, std::sentinel_for<It> Se, class Context>
[[nodiscard]] constexpr bool
parse_sequence(
    Parser const& parser,
    It& first, Se const& last,
    Context const& ctx,
    traits::CategorizedAttr<traits::container_attr> auto& attr
)
    noexcept(
        std::is_nothrow_copy_assignable_v<It> &&
        noexcept(detail::parse_sequence_impl(parser.left, first, last, ctx, attr)) &&
        noexcept(detail::parse_sequence_impl(parser.right, first, last, ctx, attr))
    )
{
    It local_it = first;
    if (detail::parse_sequence_impl(parser.left, local_it, last, ctx, attr) &&
        detail::parse_sequence_impl(parser.right, local_it, last, ctx, attr)
    ) {
        first = std::move(local_it);
        return true;
    }
    return false;
}

template<class Left, class Right, class Context>
struct parse_into_container_impl<sequence<Left, Right>, Context>
{
    using parser_type = sequence<Left, Right>;

    template<X4Attribute Attr>
    static constexpr bool is_container_substitute = traits::is_substitute_v<
        traits::attribute_of_t<parser_type, Context>,
        traits::container_value_t<Attr>
    >;

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
        requires is_container_substitute<Attr>
    [[nodiscard]] static constexpr bool
    call(
        parser_type const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) noexcept(noexcept(parse_into_container_base_impl<parser_type>::call(
        parser, first, last, ctx, attr
    )))
    {
        return parse_into_container_base_impl<parser_type>::call(
            parser, first, last, ctx, attr
        );
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
        requires (!is_container_substitute<Attr>)
    [[nodiscard]] static constexpr bool
    call(
        parser_type const& parser, It& first, Se const& last,
        Context const& ctx, Attr& attr
    ) // never noexcept (requires container insertion)
    {
        static_assert(
            std::same_as<traits::attribute_category_t<Attr>, traits::container_attr> ||
            std::same_as<traits::attribute_category_t<Attr>, traits::unused_attr>
        );

        if constexpr (
            std::same_as<std::remove_const_t<Attr>, unused_type> ||
            std::same_as<std::remove_const_t<Attr>, unused_container_type>
        ) {
            return detail::parse_sequence(parser, first, last, ctx, x4::assume_container(attr));

        } else {
            auto&& appender = x4::make_container_appender(x4::assume_container(attr));
            return detail::parse_sequence(parser, first, last, ctx, appender);
        }
    }
};

} // boost::spirit::x4::detail

#endif
