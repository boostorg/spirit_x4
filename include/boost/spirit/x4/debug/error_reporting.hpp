/*=============================================================================
    Copyright (c) 2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X4_ERROR_REPORTING_MAY_19_2014_00405PM
#define BOOST_SPIRIT_X4_ERROR_REPORTING_MAY_19_2014_00405PM

#include <boost/spirit/x4/ast/position_tagged.hpp>
#include <boost/spirit/x4/string/utf8.hpp>

#include <string>
#include <iterator>

// Clang-style error handling utilities

namespace boost::spirit::x4
{
    // tag used to get our error handler from the context
    struct error_handler_tag;

    template <std::forward_iterator It>
    class error_handler
    {
    public:
        using iterator_type = It;

        error_handler(
            It first, It last, std::ostream& err_out
          , std::string file = "", int tabs = 4)
          : err_out(err_out)
          , file(file)
          , tabs(tabs)
          , pos_cache(first, last) {}

        using result_type = void;

        void operator()(It err_pos, std::string const& error_message) const;
        void operator()(It err_first, It err_last, std::string const& error_message) const;
        void operator()(position_tagged pos, std::string const& message) const
        {
            auto where = pos_cache.position_of(pos);
            (*this)(where.begin(), where.end(), message);
        }

        template <typename AST>
        void tag(AST& ast, It first, It last)
        {
            return pos_cache.annotate(ast, first, last);
        }

        [[nodiscard]] std::ranges::subrange<It>
        position_of(position_tagged pos) const
        {
            return pos_cache.position_of(pos);
        }

        [[nodiscard]] position_cache<std::vector<It>> const&
        get_position_cache() const noexcept
        {
            return pos_cache;
        }

    private:

        void print_file_line(std::size_t line) const;
        void print_line(It line_start, It last) const;
        void print_indicator(It& line_start, It last, char ind) const;
        It get_line_start(It first, It pos) const;
        std::size_t position(It i) const;

        std::ostream& err_out;
        std::string file;
        int tabs;
        position_cache<std::vector<It>> pos_cache;
    };

    template <std::forward_iterator It>
    void error_handler<It>::print_file_line(std::size_t line) const
    {
        if (file != "")
        {
            err_out << "In file " << file << ", ";
        }
        else
        {
            err_out << "In ";
        }

        err_out << "line " << line << ':' << '\n';
    }

    template <std::forward_iterator It>
    void error_handler<It>::print_line(It start, It last) const
    {
        auto end = start;
        while (end != last)
        {
            auto c = *end;
            if (c == '\r' || c == '\n')
                break;
            else
                ++end;
        }
        typedef typename std::iterator_traits<It>::value_type char_type;
        std::basic_string<char_type> line{start, end};
        err_out << x4::to_utf8(line) << '\n';
    }

    template <std::forward_iterator It>
    void error_handler<It>::print_indicator(It& start, It last, char ind) const
    {
        for (; start != last; ++start)
        {
            auto c = *start;
            if (c == '\r' || c == '\n')
                break;
            else if (c == '\t')
                for (int i = 0; i < tabs; ++i)
                    err_out << ind;
            else
                err_out << ind;
        }
    }

    template <std::forward_iterator It>
    It error_handler<It>::get_line_start(It first, It pos) const
    {
        It latest = first;
        for (It i = first; i != pos;)
            if (*i == '\r' || *i == '\n')
                latest = ++i;
            else
                ++i;
        return latest;
    }

    template <std::forward_iterator It>
    std::size_t error_handler<It>::position(It i) const
    {
        std::size_t line { 1 };
        typename std::iterator_traits<It>::value_type prev { 0 };

        for (It pos = pos_cache.first(); pos != i; ++pos) {
            auto c = *pos;
            switch (c) {
            case '\n':
                if (prev != '\r') ++line;
                break;
            case '\r':
                ++line;
                break;
            default:
                break;
            }
            prev = c;
        }

        return line;
    }

    template <std::forward_iterator It>
    void error_handler<It>::operator()(
        It err_pos, std::string const& error_message) const
    {
        It first = pos_cache.first();
        It last = pos_cache.last();

        print_file_line(position(err_pos));
        err_out << error_message << '\n';

        It start = get_line_start(first, err_pos);
        print_line(start, last);
        print_indicator(start, err_pos, '_');
        err_out << "^_" << '\n';
    }

    template <std::forward_iterator It>
    void error_handler<It>::operator()(
        It err_first, It err_last, std::string const& error_message) const
    {
        It first = pos_cache.first();
        It last = pos_cache.last();

        print_file_line(position(err_first));
        err_out << error_message << '\n';

        It start = get_line_start(first, err_first);
        print_line(start, last);
        print_indicator(start, err_first, ' ');
        print_indicator(start, err_last, '~');
        err_out << " <<-- Here" << '\n';
    }

} // boost::spirit::x4

#endif
