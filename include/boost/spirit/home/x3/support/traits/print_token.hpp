/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
================================================_==============================*/
#ifndef BOOST_SPIRIT_X3_PRINT_TOKEN_JANUARY_20_2013_0814AM
#define BOOST_SPIRIT_X3_PRINT_TOKEN_JANUARY_20_2013_0814AM

#include <type_traits>
#include <ios>

#include <cctype>

namespace boost::spirit::x3::traits
{
    namespace detail
    {
        // generate debug output for lookahead token (character) stream
        struct token_printer_debug_for_chars
        {
            template<typename Out, typename Char>
            static void print(Out& o, Char c)
            {
                using namespace std;    // allow for ADL to find the proper iscntrl

                switch (c)
                {
                    case '\a': o << "\\a"; break;
                    case '\b': o << "\\b"; break;
                    case '\f': o << "\\f"; break;
                    case '\n': o << "\\n"; break;
                    case '\r': o << "\\r"; break;
                    case '\t': o << "\\t"; break;
                    case '\v': o << "\\v"; break;
                    default:
                        if (c >= 0 && c < 127)
                        {
                          if (iscntrl(c))
                            o << "\\" << std::oct << int(c);
                          else if (isprint(c))
                            o << char(c);
                          else
                            o << "\\x" << std::hex << int(c);
                        }
                        else
                          o << "\\x" << std::hex << int(c);
                }
            }
        };

        // for token types where the comparison with char constants wouldn't work
        struct token_printer_debug
        {
            template<typename Out, typename T>
            static void print(Out& o, T const& val)
            {
                o << val;
            }
        };

    } // detail

    template <typename T, typename Enable = void>
    struct token_printer_debug
        : std::conditional_t<
            std::is_convertible_v<T, char> && std::is_convertible_v<char, T>,
            detail::token_printer_debug_for_chars,
            detail::token_printer_debug
        >
    {};

    template <typename Out, typename T>
    void print_token(Out& out, T const& val)
    {
        // allow to customize the token printer routine
        token_printer_debug<T>::print(out, val);
    }

} // boost::spirit::x3::traits

#endif
