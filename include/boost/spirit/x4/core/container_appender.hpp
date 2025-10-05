#ifndef BOOST_SPIRIT_X4_CORE_CONTAINER_APPENDER_HPP
#define BOOST_SPIRIT_X4_CORE_CONTAINER_APPENDER_HPP

#include <boost/spirit/core/type_traits.hpp>
#include <boost/spirit/x4/traits/container_traits.hpp>
#include <boost/spirit/x4/traits/transform_attribute.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x4 {

namespace detail {

template<class ContainerAttr>
struct container_appender_base {};

template<class ContainerAttr>
    requires traits::is_associative_v<ContainerAttr>
struct container_appender_base<ContainerAttr>
{
    using key_type = typename ContainerAttr::key_type;
};

} // detail

template<class ContainerAttr>
struct container_appender : detail::container_appender_base<ContainerAttr>
{
    static_assert(!is_ttp_specialization_of_v<std::remove_cvref_t<ContainerAttr>, container_appender>);
    static_assert(traits::X4Container<ContainerAttr>);

    using container_type = ContainerAttr;
    using value_type = typename ContainerAttr::value_type;

    ContainerAttr& container;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    constexpr explicit container_appender(ContainerAttr& container) noexcept
        : container(container)
    {}

    constexpr auto begin() const
        noexcept(noexcept(traits::begin(this->container)))
    {
        return traits::begin(this->container);
    }

    constexpr auto begin()
        noexcept(noexcept(traits::begin(this->container)))
    {
        return traits::begin(this->container);
    }

    constexpr auto end() const
        noexcept(noexcept(traits::end(this->container)))
    {
        return traits::end(this->container);
    }

    constexpr auto end()
        noexcept(noexcept(traits::end(this->container)))
    {
        return traits::end(this->container);
    }

    [[nodiscard]] constexpr bool empty() const
        noexcept(noexcept(traits::is_empty(this->container)))
    {
        return traits::is_empty(this->container);
    }

    constexpr void clear()
        noexcept(noexcept(traits::clear(this->container)))
    {
        traits::clear(this->container);
    }

    template<class T>
        requires requires(ContainerAttr& container) {
            traits::push_back(container, std::declval<T>());
        }
    constexpr void push_back(T&& value)
        noexcept(noexcept(traits::push_back(this->container, std::forward<T>(value))))
    {
        traits::push_back(this->container, std::forward<T>(value));
    }
};

template<class T>
struct unwrap_container_appender
{
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);
    using type = T;
};

template<class T>
using unwrap_container_appender_t = typename unwrap_container_appender<T>::type;

template<class ContainerAttr>
struct unwrap_container_appender<container_appender<ContainerAttr>>
{
    using type = ContainerAttr;
};


template<class Attr>
[[nodiscard]] constexpr Attr&
make_container_appender(Attr& other_attr) noexcept
{
    return other_attr;  // NOLINT(bugprone-return-const-ref-from-parameter)
}

template<class Attr>
constexpr void
make_container_appender(Attr const&&) = delete; // dangling

template<traits::X4Container ContainerAttr>
[[nodiscard]] constexpr container_appender<ContainerAttr>
make_container_appender(ContainerAttr& container_attr) noexcept
{
    return container_appender<ContainerAttr>{container_attr};
}

template<traits::X4Container ContainerAttr>
    requires is_ttp_specialization_of_v<std::remove_cvref_t<ContainerAttr>, container_appender>
[[nodiscard]] constexpr ContainerAttr&
make_container_appender(ContainerAttr& appender) noexcept
{
    return appender;
}

namespace traits {

template<class ContainerAttr>
struct append_container<container_appender<ContainerAttr>>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se>
    static constexpr void call(container_appender<ContainerAttr>& appender, It first, Se last)
        noexcept(noexcept(traits::append(appender.container, std::move(first), std::move(last))))
    {
        traits::append(appender.container, std::move(first), std::move(last));
    }
};

template<class Transformed>
struct transform_attribute<Transformed, container_appender<Transformed>>
{
    using type = Transformed&;

    [[nodiscard]] static constexpr Transformed& pre(container_appender<Transformed>& appender)
    {
        return appender.container;
    }

    template<class TransformedT>
    static constexpr void post(container_appender<Transformed>&, TransformedT&&) noexcept
    {
    }
};

} // traits

} // boost::spirit::x4

#endif
