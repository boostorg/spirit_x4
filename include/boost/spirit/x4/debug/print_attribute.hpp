/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
================================================_==============================*/
#ifndef BOOST_SPIRIT_X4_PRINT_ATTRIBUTE_JANUARY_20_2013_0814AM
#define BOOST_SPIRIT_X4_PRINT_ATTRIBUTE_JANUARY_20_2013_0814AM

#include <boost/spirit/x4/traits/attribute_category.hpp>
#include <boost/spirit/x4/traits/variant_traits.hpp>

#include <boost/fusion/include/for_each.hpp>

#include <boost/variant.hpp>
#include <boost/variant/apply_visitor.hpp> // TODO: remove this

#ifdef BOOST_SPIRIT_X4_UNICODE
# include <boost/spirit/x4/char_encoding/unicode.hpp>
#endif

namespace boost::spirit::x4::traits
{
    template <typename Out, typename T>
    void print_attribute(Out& out, T const& val);

    namespace detail
    {
        template <typename Out>
        struct print_fusion_sequence
        {
            print_fusion_sequence(Out& out)
                : out(out)
                , is_first(true)
            {}

            using result_type = void;

            template <typename T>
            void operator()(T const& val) const
            {
                if (is_first)
                {
                    is_first = false;
                }
                else
                {
                    out << ", ";
                }
                x4::traits::print_attribute(out, val);
            }

            Out& out;
            mutable bool is_first;
        };

        // print elements in a variant
        template <typename Out>
        struct print_visitor : static_visitor<>
        {
            print_visitor(Out& out)
                : out(out)
            {}

            template <typename T>
            void operator()(T const& val) const
            {
                x4::traits::print_attribute(out, val);
            }

            Out& out;
        };
    }

    template <typename Out, typename T>
    struct print_attribute_debug
    {
        static void call(Out& out, unused_type const&)
        {
            out << "unused";
        }

        static void call(Out& out, unused_container_type const&)
        {
            out << "unused_container";
        }

        static void call(Out& out, CategorizedAttr<plain_attribute> auto const& val)
        {
            out << val;
        }

#ifdef BOOST_SPIRIT_X4_UNICODE
        static void call(Out& out, char_encoding::unicode::char_type const& val)
        {
            if (val >= 0 && val < 127)
            {
              if (iscntrl(val)) // TODO
                out << "\\" << std::oct << int(val) << std::dec;
              else if (isprint(val))
                out << char(val);
              else
                out << "\\x" << std::hex << int(val) << std::dec;
            }
            else
              out << "\\x" << std::hex << int(val) << std::dec;
        }

        static void call(Out& out, char const& val)
        {
            print_attribute_debug::call(out, static_cast<char_encoding::unicode::char_type>(val));
        }
#endif

        // for fusion data types
        static void call(Out& out, CategorizedAttr<tuple_attribute> auto const& val)
        {
            out << '[';
            fusion::for_each(val, detail::print_fusion_sequence<Out>(out));
            out << ']';
        }

        template <CategorizedAttr<container_attribute> T_>
            requires (!std::is_same_v<T_, unused_container_type>)
        static void call(Out& out, T_ const& val)
        {
            out << '[';
            bool is_first = true;
            auto last = traits::end(val);
            for (auto it = traits::begin(val); it != last; ++it)
            {
                if (is_first)
                {
                    is_first = false;
                }
                else
                {
                    out << ", ";
                }
                x4::traits::print_attribute(out, *it);
            }
            out << ']';
        }

        // for variant types
        static void call(Out& out, CategorizedAttr<variant_attribute> auto const& val)
        {
            boost::apply_visitor(detail::print_visitor<Out>(out), val);
        }

        static void call(Out& out, CategorizedAttr<optional_attribute> auto const& val)
        {
            if (val)
            {
                x4::traits::print_attribute(out, *val);
            }
            else
            {
                out << "[empty]";
            }
        }
    };

    template <typename Out, typename T>
    void print_attribute(Out& out, T const& val)
    {
        print_attribute_debug<Out, T>::call(out, val);
    }

} // boost::spirit::x4::traits

#endif
