#ifndef BOOST_SPIRIT_X4_NUMERIC_REAL_HPP
#define BOOST_SPIRIT_X4_NUMERIC_REAL_HPP

/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/x4/core/parser.hpp>
#include <boost/spirit/x4/core/skip_over.hpp>

#include <boost/spirit/x4/numeric/utils/extract_int.hpp>
#include <boost/spirit/x4/numeric/utils/extract_real.hpp>

#include <boost/spirit/x4/string/detail/string_parse.hpp>

#include <string_view>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

// TODO: use `std::from_chars`

namespace boost::spirit::x4 {

// Default unsigned real number policies
template<class T>
struct ureal_policies
{
    // trailing dot policy suggested by Gustavo Guerra
    static constexpr bool allow_leading_dot = true;
    static constexpr bool allow_trailing_dot = true;
    static constexpr bool expect_dot = false;

    template<std::forward_iterator It, std::sentinel_for<It> Se>
    [[nodiscard]] static constexpr bool
    parse_sign(It& /*first*/, Se const& /*last*/) noexcept
    {
        return false;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
    [[nodiscard]] static constexpr bool
    parse_n(It& first, Se const& last, Attr& attr_)
        noexcept(noexcept(extract_uint<T, 10, 1, -1>::call(first, last, attr_)))
    {
        return extract_uint<T, 10, 1, -1>::call(first, last, attr_);
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se>
    [[nodiscard]] static constexpr bool
    parse_dot(It& first, Se const& last)
        noexcept(noexcept(*first) && noexcept(++first))
    {
        if (first == last || *first != '.') {
            return false;
        }
        ++first;
        return true;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
    [[nodiscard]] static constexpr bool
    parse_frac_n(It& first, Se const& last, Attr& attr_)
        noexcept(noexcept(extract_uint<T, 10, 1, -1, true>::call(first, last, attr_)))
    {
        return extract_uint<T, 10, 1, -1, true>::call(first, last, attr_);
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se>
    [[nodiscard]] static constexpr bool
    parse_exp(It& first, Se const& last)
        noexcept(noexcept(*first) && noexcept(++first))
    {
        if (first == last || (*first != 'e' && *first != 'E')) {
            return false;
        }
        ++first;
        return true;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se>
    [[nodiscard]] static constexpr bool
    parse_exp_n(It& first, Se const& last, int& attr_)
        noexcept(noexcept(extract_int<int, 10, 1, -1>::call(first, last, attr_)))
    {
        return extract_int<int, 10, 1, -1>::call(first, last, attr_);
    }

    // The parse_nan() and parse_inf() functions get called whenever
    // a number to parse does not start with a digit (after having
    // successfully parsed an optional sign).
    //
    // The functions should return true if a Nan or Inf has been found. In
    // this case the attr should be set to the matched value (NaN or
    // Inf). The optional sign will be automatically applied afterwards.
    //
    // The default implementation below recognizes representations of NaN
    // and Inf as mandated by the C99 Standard and as proposed for
    // inclusion into the C++0x Standard: nan, nan(...), inf and infinity
    // (the matching is performed case-insensitively).
    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
    [[nodiscard]] static constexpr bool
    parse_nan(It& first, Se const& last, Attr& attr_)
    {
        using namespace std::string_view_literals;

        if (first == last) return false;   // end of input reached
        if (*first != 'n' && *first != 'N') return false;   // not "nan"

        // nan[(...)] ?
        if (detail::string_parse("nan"sv, "NAN"sv, first, last, unused_container)) {
            if (first != last && *first == '(') {
                // skip trailing (...) part
                It i = first;

                while (++i != last && *i != ')')
                    /* loop */;

                if (i == last) return false;     // no trailing ')' found, give up

                first = ++i;
            }
            attr_ = std::numeric_limits<T>::quiet_NaN();
            return true;
        }
        return false;
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, X4Attribute Attr>
    [[nodiscard]] static constexpr bool
    parse_inf(It& first, Se const& last, Attr& attr_)
    {
        using namespace std::string_view_literals;

        if (first == last) return false;   // end of input reached
        if (*first != 'i' && *first != 'I') return false;   // not "inf"

        // inf or infinity ?
        if (detail::string_parse("inf"sv, "INF"sv, first, last, unused_container)) {
            // skip allowed 'inity' part of infinity
            (void)detail::string_parse("inity"sv, "INITY"sv, first, last, unused_container);
            attr_ = std::numeric_limits<T>::infinity();
            return true;
        }
        return false;
    }
};

// Default signed real number policies
template<class T>
struct real_policies : ureal_policies<T>
{
    template<std::forward_iterator It, std::sentinel_for<It> Se>
    [[nodiscard]] static constexpr bool
    parse_sign(It& first, Se const& last)
        noexcept(noexcept(detail::extract_sign(first, last)))
    {
        return detail::extract_sign(first, last);
    }
};

template<class T>
struct strict_ureal_policies : ureal_policies<T>
{
    static constexpr bool expect_dot = true;
};

template<class T>
struct strict_real_policies : real_policies<T>
{
    static constexpr bool expect_dot = true;
};

template<class T, class RealPolicies = real_policies<T>>
struct real_parser : parser<real_parser<T, RealPolicies>>
{
    using attribute_type = T;
    using policies_type = RealPolicies;

    static constexpr bool has_attribute = true;

    constexpr real_parser() = default;

    template<class RealPoliciesT>
        requires
            (!std::is_same_v<std::remove_cvref_t<RealPoliciesT>, real_parser>) &&
            std::is_constructible_v<RealPolicies, RealPoliciesT>
    constexpr real_parser(RealPoliciesT&& policies)
        noexcept(std::is_nothrow_constructible_v<RealPolicies, RealPoliciesT>)
        : policies_(std::forward<RealPoliciesT>(policies))
    {}

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, T& attr_) const
        noexcept(
            noexcept(x4::skip_over(first, last, ctx)) &&
            noexcept(extract_real<T, RealPolicies>::parse(first, last, attr_, policies_))
        )
    {
        x4::skip_over(first, last, ctx);
        return extract_real<T, RealPolicies>::parse(first, last, attr_, policies_);
    }

    template<std::forward_iterator It, std::sentinel_for<It> Se, class Context, X4Attribute Attr>
    [[nodiscard]] constexpr bool
    parse(It& first, Se const& last, Context const& ctx, Attr& attr_param) const
        noexcept(
            std::is_nothrow_default_constructible_v<T> &&
            noexcept(this->parse(first, last, ctx, std::declval<T&>())) &&
            noexcept(x4::move_to(std::declval<T&&>(), attr_param))
        )
    {
        // this case is called when Attribute is not T
        T attr_;
        if (this->parse(first, last, ctx, attr_)) {
            x4::move_to(std::move(attr_), attr_param);
            return true;
        }
        return false;
    }

private:
    BOOST_SPIRIT_NO_UNIQUE_ADDRESS RealPolicies policies_{};
};

inline namespace cpos {

using float_type = real_parser<float>;
inline constexpr float_type float_{};

using double_type = real_parser<double>;
inline constexpr double_type double_{};

using long_double_type = real_parser<long double>;
inline constexpr long_double_type long_double{};

} // cpos

} // boost::spirit::x4

#endif
