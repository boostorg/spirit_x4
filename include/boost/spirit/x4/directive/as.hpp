#ifndef BOOST_SPIRIT_X4_DIRECTIVE_AS_HPP
#define BOOST_SPIRIT_X4_DIRECTIVE_AS_HPP

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/move_to.hpp>
#include <boost/spirit/x4/core/unused.hpp>

#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

namespace detail {

template<bool NeedAsVal, class Context, X4Attribute Attr>
struct as_directive_ctx_impl // false
{
    using type = Context;
};
template<class Context, X4Attribute Attr>
struct as_directive_ctx_impl<true, Context, Attr>
{
    using type = std::remove_cvref_t<decltype(x4::replace_first_context<contexts::as_val>(
        std::declval<Context const&>(),
        std::declval<Attr&>()
    ))>;
};

} // detail

// `as_directive` forces the attribute of subject parser
// to be `T`. When `T` is `unused_type`, this is equivalent to
// `omit_directive`.
template<X4Attribute T, class Subject>
struct as_directive : unary_parser<Subject, as_directive<T, Subject>>
{
    static_assert(!std::is_const_v<T>); // Forbid const `unused_type`
    static_assert(!std::same_as<T, unused_container_type>); // Unknown use case, not supported for now

    static_assert(std::default_initializable<T>);

    using base_type = unary_parser<Subject, as_directive>;
    using attribute_type = T;
    using subject_type = Subject;

    static constexpr bool has_attribute = !std::same_as<T, unused_type>;
    static constexpr bool has_action = false; // Important: explicitly re-enable attribute detection in `x4::rule`

private:
    static constexpr bool need_as_val = Subject::has_action;

public:
    template<class SubjectT>
        requires
            (!std::is_same_v<std::remove_cvref_t<SubjectT>, as_directive>) &&
            std::is_constructible_v<base_type, SubjectT>
    constexpr as_directive(SubjectT&& subject)
        noexcept(std::is_nothrow_constructible_v<base_type, SubjectT>)
        : base_type(std::forward<SubjectT>(subject))
    {}

    // `as<T>(as<T>(subject))` forwards the outer `T&` for `subject`
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute OuterAttr>
        requires std::same_as<std::remove_const_t<OuterAttr>, T>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, OuterAttr& outer_attr) const
        noexcept(is_nothrow_parsable_v<Subject, It, Se, typename detail::as_directive_ctx_impl<need_as_val, Context, OuterAttr>::type, OuterAttr>)
    {
        if constexpr (need_as_val) {
            return this->subject.parse(first, last, x4::replace_first_context<contexts::as_val>(ctx, outer_attr), outer_attr);
        } else {
            return this->subject.parse(first, last, ctx, outer_attr);
        }
    }

    // `as<unused_type>(as<T>(subject))` gives `unused_type` for `subject`
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4UnusedAttribute OuterAttr>
        requires
            (!std::same_as<std::remove_const_t<OuterAttr>, T>)
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, OuterAttr&) const
        noexcept(is_nothrow_parsable_v<Subject, It, Se, typename detail::as_directive_ctx_impl<need_as_val, Context, unused_type>::type, unused_type>)
    {
        if constexpr (need_as_val) {
            return this->subject.parse(first, last, x4::replace_first_context<contexts::as_val>(ctx, unused), unused);
        } else {
            return this->subject.parse(first, last, ctx, unused);
        }
    }

    // `as<U>(as<T>(subject))` gives `T` for `subject`, then move `T&&` to `U&`
    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4NonUnusedAttribute OuterAttr>
        requires
            (!std::same_as<std::remove_const_t<OuterAttr>, T>) &&
            X4Movable<T, OuterAttr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, OuterAttr& outer_attr) const
        noexcept(
            is_nothrow_parsable_v<Subject, It, Se, typename detail::as_directive_ctx_impl<need_as_val, Context, T>::type, T> &&
            noexcept(x4::move_to(std::declval<T>(), outer_attr))
        )
    {
        // Ideally we should default to default-initialization and avoid value-initialization.
        // However, there is currently no way to determine whether the attribute is ever touched
        // by the underlying parser (for example: semantic action).
        //
        // Note that this behavior is our implementation details. The underlying parser should
        // not rely on this behavior; they should never assume the given attribute is defaulted
        // to some arbitrary initial value.

        T attr_{}; // value-initialize

        if constexpr (need_as_val) {
            if (!this->subject.parse(first, last, x4::replace_first_context<contexts::as_val>(ctx, attr_), attr_)) return false;
        } else {
            if (!this->subject.parse(first, last, ctx, attr_)) return false;
        }

        x4::move_to(std::move(attr_), outer_attr);
        return true;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4NonUnusedAttribute OuterAttr>
        requires
            (!std::same_as<std::remove_const_t<OuterAttr>, T>) &&
            (!X4Movable<T, OuterAttr>)
    constexpr void
    parse(It&, Se const&, Context const&, OuterAttr&) const = delete; // `T` is not movable to the exposed attribute
};

namespace detail {

template<X4Attribute T>
struct as_fn
{
    template<X4Subject Subject>
    [[nodiscard]] static constexpr as_directive<T, as_parser_plain_t<Subject>>
    operator()(Subject&& subject)
        noexcept(is_parser_nothrow_constructible_v<as_directive<T, as_parser_plain_t<Subject>>, Subject>)
    {
        return {std::forward<Subject>(subject)};
    }
};

} // detail

namespace parsers::directive {

template<X4Attribute T>
inline constexpr detail::as_fn<T> as{};

} // parsers::directive

using parsers::directive::as;

} // boost::spirit::x4

#endif
