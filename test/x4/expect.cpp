/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 6262) // Function uses 'XXXXX' bytes of stack
#endif

#include "test.hpp"

#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/symbols.hpp>
#include <boost/spirit/x4/binary.hpp>
#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eoi.hpp>
#include <boost/spirit/x4/auxiliary/eol.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char_class.hpp>
#include <boost/spirit/x4/directive/with.hpp>
#include <boost/spirit/x4/core/expectation.hpp>
#include <boost/spirit/x4/directive/confix.hpp>
#include <boost/spirit/x4/directive/expect.hpp>
#include <boost/spirit/x4/directive/lexeme.hpp>
#include <boost/spirit/x4/directive/matches.hpp>
#include <boost/spirit/x4/directive/no_case.hpp>
#include <boost/spirit/x4/directive/no_skip.hpp>
#include <boost/spirit/x4/directive/omit.hpp>
#include <boost/spirit/x4/directive/raw.hpp>
#include <boost/spirit/x4/directive/repeat.hpp>
#include <boost/spirit/x4/directive/seek.hpp>
#include <boost/spirit/x4/directive/skip.hpp>
#include <boost/spirit/x4/numeric/int.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/kleene.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>
#include <boost/spirit/x4/operator/and_predicate.hpp>
#include <boost/spirit/x4/operator/difference.hpp>
#include <boost/spirit/x4/operator/not_predicate.hpp>
#include <boost/spirit/x4/operator/optional.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/facilities/expand.hpp>

#include <string>
#include <string_view>
#include <utility>
#include <type_traits>

// NOLINTBEGIN(bugprone-chained-comparison)

#define TEST_SUCCESS_IMPL(tester, input, parser, ...) \
    do { \
        auto const res = parse(input, parser __VA_OPT__(,) __VA_ARGS__); \
        CHECK(tester res.completed()); \
        CHECK(!res.expect_failure.has_value()); \
    } while (false)

#define TEST_FAILURE_IMPL_4(tester, input, parser, catch_stmt) \
    do { \
        auto const res = parse(input, parser); \
        REQUIRE(tester res.completed()); \
        REQUIRE(res.expect_failure.has_value()); \
        auto const& x = res.expect_failure; \
        [[maybe_unused]] auto const& which = x.which(); \
        [[maybe_unused]] auto const& where = std::string_view(x.where(), res.remainder.end()); \
        catch_stmt \
    } while (false)

#define TEST_FAILURE_IMPL_5(tester, input, parser, arg0, catch_stmt) \
    do { \
        auto const res = parse(input, parser, arg0); \
        REQUIRE(tester res.completed()); \
        REQUIRE(res.expect_failure.has_value()); \
        auto const& x = res.expect_failure; \
        [[maybe_unused]] auto const& which = x.which(); \
        [[maybe_unused]] auto const& where = std::string_view(x.where(), res.remainder.end()); \
        catch_stmt \
    } while (false)

#define TEST_FAILURE_IMPL_6(tester, input, parser, arg0, arg1, catch_stmt) \
    do { \
        auto const res = parse(input, parser, arg0, arg1); \
        REQUIRE(tester res.completed()); \
        REQUIRE(res.expect_failure.has_value()); \
        auto const& x = res.expect_failure; \
        [[maybe_unused]] auto const& which = x.which(); \
        [[maybe_unused]] auto const& where = std::string_view(x.where(), res.remainder.end()); \
        catch_stmt \
    } while (false)

#define TEST_FAILURE_IMPL(...) BOOST_PP_EXPAND(BOOST_PP_OVERLOAD(TEST_FAILURE_IMPL_, __VA_ARGS__) (__VA_ARGS__))

// Comments below are intentionally written verbosely
// to provide human-friendly intellisense tooltip for testers

// parser = ok, exception = none, expectation_failure = none
#define TEST_SUCCESS_PASS(...)      TEST_SUCCESS_IMPL(, __VA_ARGS__)

// parser = ok, exception = none, expectation_failure = none
#define TEST_ATTR_SUCCESS_PASS(...) TEST_SUCCESS_IMPL(, __VA_ARGS__)

// parser = fail, exception = none, expectation_failure = none
#define TEST_SUCCESS_FAIL(...)      TEST_SUCCESS_IMPL(!, __VA_ARGS__)

// parser = fail, exception = none, expectation_failure = none
#define TEST_ATTR_SUCCESS_FAIL(...) TEST_SUCCESS_IMPL(!, __VA_ARGS__)

// parser = fail, exception = none, expectation_failure = yes
#define TEST_FAILURE(...)           TEST_FAILURE_IMPL(!, __VA_ARGS__)

// parser = fail, exception = none, expectation_failure = yes
#define TEST_ATTR_FAILURE(...)      TEST_FAILURE_IMPL(!, __VA_ARGS__)


// For testers; development QOL purpose only.

#define DEBUG_PRINT(x) \
    do { \
        std::cout << "----------------------------------\n"; \
        std::cout << "which: " << x.which() << "\n"; \
        std::cout << "where: " << x.where() << "\n"; \
        std::cout << "----------------------------------\n"; \
        CHECK(!"remove DEBUG_PRINT before commit!"); \
    } while (false)


TEST_CASE("expect")
{
    using namespace std::string_view_literals;

    using x4::standard::alpha;
    using x4::standard::digit;
    using x4::standard::space;
    using x4::standard::blank;
    using x4::standard::char_;
    using x4::standard::string;
    using x4::standard::lit;

    // using x4::lit;
    using x4::expect;
    using x4::lexeme;
    using x4::no_case;
    using x4::no_skip;
    using x4::omit;
    using x4::raw;
    using x4::skip;
    using x4::reskip;
    using x4::seek;
    using x4::repeat;
    using x4::matches;
    using x4::eps;
    using x4::eoi;
    using x4::eol;
    //using x4::attr;
    using x4::dword;
    using x4::int_;
    using x4::shared_symbols;
    using x4::confix;
    using x4::with;

    using boost::fusion::vector;
    using boost::fusion::at_c;

    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(expect['x']);
    BOOST_SPIRIT_X4_ASSERT_CONSTEXPR_CTORS(char_ > char_);

    {
        TEST_SUCCESS_PASS("aa", char_ >> expect[char_]);
        TEST_SUCCESS_PASS("aaa", char_ >> expect[char_ >> char_('a')]);
        TEST_SUCCESS_PASS("xi", char_('x') >> expect[char_('i')]);
        TEST_SUCCESS_FAIL("xi", char_('y') >> expect[char_('o')]); // should not throw!
        TEST_SUCCESS_PASS("xin", char_('x') >> expect[char_('i') >> char_('n')]);

        TEST_FAILURE("xi", char_('x') >> expect[char_('o')], {
            CHECK(which == "'o'"sv);
            CHECK(where == "i"sv);
        });
    }

    {
        TEST_SUCCESS_PASS("aa", char_ > char_);
        TEST_SUCCESS_PASS("aaa", char_ > char_ > char_('a'));
        TEST_SUCCESS_PASS("xi", char_('x') > char_('i'));
        TEST_SUCCESS_FAIL("xi", char_('y') > char_('o')); // should not throw!
        TEST_SUCCESS_PASS("xin", char_('x') > char_('i') > char_('n'));

        TEST_FAILURE("xi", char_('x') > char_('o'),
        {
            CHECK(which == "'o'"sv);
            CHECK(where == "i"sv);
        });
    }

    {
    #ifndef BOOST_SPIRIT_X4_NO_RTTI
        TEST_FAILURE("ay:a", char_ > char_('x') >> ':' > 'a',
        {
            CHECK(x.which().find("sequence") != std::string::npos);
            CHECK(where == "y:a"sv);
        });
    #else
        TEST_FAILURE("ay:a", char_ > char_('x') >> ':' > 'a',
        {
            CHECK(which == "undefined"sv);
            CHECK(where == "y:a"sv);
        });
    #endif
    }

#if defined(BOOST_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
#endif

    // Test that attributes with > (sequences) work just like >> (sequences)
    {
        vector<char, char, char> attr;
        TEST_ATTR_SUCCESS_PASS(" a\n  b\n  c", char_ > char_ > char_, space, attr);
        CHECK((at_c<0>(attr) == 'a'));
        CHECK((at_c<1>(attr) == 'b'));
        CHECK((at_c<2>(attr) == 'c'));
    }

    {
        vector<char, char, char> attr;
        TEST_ATTR_SUCCESS_PASS(" a\n  b\n  c", char_ > char_ >> char_, space, attr);
        CHECK((at_c<0>(attr) == 'a'));
        CHECK((at_c<1>(attr) == 'b'));
        CHECK((at_c<2>(attr) == 'c'));
    }

    {
        vector<char, char, char> attr;
        TEST_ATTR_SUCCESS_PASS(" a, b, c", char_ >> ',' > char_ >> ',' > char_, space, attr);
        CHECK((at_c<0>(attr) == 'a'));
        CHECK((at_c<1>(attr) == 'b'));
        CHECK((at_c<2>(attr) == 'c'));
    }

    {
        std::string attr;
        TEST_ATTR_SUCCESS_PASS("'azaaz'", "'" > *(char_("a") | char_("z")) > "'", space, attr);
        CHECK(attr == "azaaz");
    }

#if defined(BOOST_CLANG)
#pragma clang diagnostic pop
#endif

    {
        TEST_SUCCESS_PASS(" a a", char_ > char_, space);
        TEST_SUCCESS_PASS(" x i", char_('x') > char_('i'), space);

        TEST_FAILURE(" x i", char_('x') > char_('o'), space, {
            CHECK(which == "'o'"sv);
            CHECK(where == "i"sv);
        });
    }

    {
        TEST_FAILURE("bar", expect[lit("foo")],
        {
            CHECK(which == "\"foo\""sv);
            CHECK(where == "bar"sv);
        });
    }


    // skipper
    {
        TEST_FAILURE("accb", repeat(7)[alpha], (lit('a') > 'b') | (lit('c') > 'd'), {
            CHECK(which == "'b'"sv);
            CHECK(where == "ccb"sv);
        });
    }

    //
    // ********* Developers note **********
    //
    // As of now (see `git blame`), get_info<T> is still not
    // specialized for many of the X4 parsers so that the
    // value of `expectation_failure<...>::which()` will be
    // implementation-defined demangled string.
    // Therefore, it's essentially impossible to test them
    // right now; further work must be done.
    //
    // Some specific situations are already been reported
    // (e.g. https://github.com/boostorg/spirit/issues/777)
    // but we really need to implement all specializations for
    // X4's predefined parsers, not just the one reported above.
    //


    // sanity check: test expectation_failure propagation
    // on custom skippers
    {
        TEST_SUCCESS_PASS("a..b", lit('a') >> 'b', lit('.') >> '.');
        TEST_SUCCESS_FAIL("a..b", lit('a') >> 'b', lit('.') >> 'z');

        TEST_SUCCESS_PASS("a  b", lit('a') >> 'b', blank);
        TEST_SUCCESS_PASS("a..b", lit('a') >> 'b', +lit('.'));
        TEST_SUCCESS_PASS("a..b", lit('a') >> 'b', lit('.') >> '.');

        // if this test fails, there might be a problem in x4::skip_over
        TEST_FAILURE     ("a..b", lit('a') >> 'b', lit('.') >> expect[lit('z')], {
            CHECK(which == "'z'"sv);
            CHECK(where == ".b"sv);
        });

        // ---------------------------------------------------------
        // skip(...) version of the code above
        // we must test against semantically identical cases!

        TEST_SUCCESS_PASS("a..b", skip(lit('.') >> '.')[lit('a') >> 'b']);
        TEST_SUCCESS_FAIL("a..b", skip(lit('.') >> 'z')[lit('a') >> 'b']);

        TEST_SUCCESS_PASS("a  b", skip(blank)[lit('a') >> 'b']);
        TEST_SUCCESS_PASS("a..b", skip(+lit('.'))[lit('a') >> 'b']);
        TEST_SUCCESS_PASS("a..b", skip(lit('.') >> '.')[lit('a') >> 'b']);

        // if this test fails, there might be a problem in x4::skip_over and/or x4::skip_directive
        TEST_FAILURE     ("a..b", skip(lit('.') >> expect[lit('z')])[lit('a') >> 'b'], {
            CHECK(which == "'z'"sv);
            CHECK(where == ".b"sv);
        });
    }

    // sanity check; test `post_skip` in `x4::phrase_parse(...)`
    {
        TEST_SUCCESS_PASS("a..b..", lit('a') >> 'b', lit('.') >> '.');
        TEST_SUCCESS_FAIL("a..b..", lit('a') >> 'z', lit('.') >> '.');
        TEST_SUCCESS_FAIL("a..b..", lit('a') >> 'b', lit('.') >> 'z');

        // should fail in `post_skip`
        TEST_SUCCESS_FAIL("a..b.z", lit('a') >> 'b', lit('.') >> '.');

        // if this test fails, x4::skip_over is BUGGED when `post_skip` is run
        TEST_FAILURE("a..b.z", lit('a') >> 'b', lit('.') > '.', {
            CHECK(which == "'.'"sv);
            CHECK(where == "z"sv);
        });
    }

    // sequence
    {
        TEST_SUCCESS_PASS("ab", lit('a') >> 'b');
        TEST_SUCCESS_FAIL("ac", lit('a') >> 'b');

        TEST_FAILURE("ac", lit('a') >> expect[lit('b')], {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
        TEST_FAILURE("ac", lit('a') > lit('b'), {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
    }

    // auxilary parsers
    {
        TEST_SUCCESS_PASS("a12", lit('a') > eps > +digit);
        TEST_SUCCESS_PASS("a12", lit('a') > +digit > eoi);
        TEST_SUCCESS_PASS("a12\n", lit('a') > +digit > eol);

        TEST_FAILURE("a12", lit('a') > eps(false) > +digit, {
            CHECK(where == "12"sv);
        });
        TEST_FAILURE("a12", lit('a') > eoi > +digit, {
            CHECK(where == "12"sv);
        });
        TEST_FAILURE("a12\n", lit('a') > eol > +digit, {
            CHECK(where == "12\n"sv);
        });

        int n = 0;
        TEST_ATTR_SUCCESS_PASS("abc", lit("abc") > x4::attr(12) > eoi, n);
        CHECK(n == 12);
    }

    // binary, numeric, char, string parsers
    {
        TEST_SUCCESS_PASS("12abcd", +digit > dword);
        TEST_SUCCESS_PASS("abc12", +alpha > int_);
        TEST_SUCCESS_PASS("12a", +digit > lit('a'));

        TEST_FAILURE("12abc", +digit > dword, {
            CHECK(where == "abc"sv);
        });
        TEST_FAILURE("abc", +alpha > int_, {
            CHECK(where == ""sv);
        });
        TEST_FAILURE("12a", +digit > lit('b'), {
            CHECK(which == "'b'"sv);
            CHECK(where == "a"sv);
        });

        shared_symbols<> s;
        s.add("cat");
        TEST_SUCCESS_PASS("12cat", +digit > s);
        TEST_FAILURE("12dog", +digit > s, {
            CHECK(where == "dog"sv);
        });
    }

    // confix
    {
        TEST_SUCCESS_PASS("[12cat]", confix('[', ']')[+digit > lit("cat")]);
        TEST_FAILURE("[12dog]", confix('[', ']')[+digit > lit("cat")], {
            CHECK(which == "\"cat\""sv);
            CHECK(where == "dog]"sv);
        });
    }

    // expect
    {
        TEST_SUCCESS_PASS("abc", lit('a') >> expect[lit('b') >> 'c']);
        TEST_FAILURE("abc", lit('a') >> expect[lit('b') >> 'd'], {
            CHECK(where == "bc"sv);
        });
        TEST_FAILURE("abc", lit('a') >> expect[lit('b') > 'd'], {
            CHECK(which == "'d'"sv);
            CHECK(where == "c"sv);
        });
    }

    // lexeme
    {
        TEST_SUCCESS_PASS("12 ab", int_ >> lexeme[lit('a') > 'b'], space);
        TEST_FAILURE("12 a b", int_ >> lexeme[lit('a') > 'b'], space, {
            CHECK(which == "'b'"sv);
            CHECK(where == " b"sv);
        });
    }

    // matches
    {
        TEST_SUCCESS_PASS("ab", matches[lit('a') >> 'b']);
        TEST_SUCCESS_PASS("ac", matches[lit('a') >> 'b'] >> "ac");
        TEST_SUCCESS_PASS("ab", matches[lit('a') > 'b']);
        TEST_FAILURE("ac", matches[lit('a') > 'b'] >> "ac", {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });

        bool attr = false;
        TEST_ATTR_SUCCESS_PASS("ab", matches[lit('a') > 'b'], attr);
        CHECK(attr == true);
    }

    // no_case
    {
        TEST_SUCCESS_PASS("12 aB", int_ >> no_case[lit('a') > 'b'], space);
        TEST_FAILURE("12 aB", int_ >> no_case[lit('a') > 'c'], space, {
            CHECK(which == "'c'"sv);
            CHECK(where == "B"sv);
        });
    }

    // no_skip
    {
        TEST_SUCCESS_PASS("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'b'], space);
        TEST_FAILURE("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'c'], space, {
            CHECK(which == "'c'"sv);
            CHECK(where == "b"sv);
        });
    }

    // skip
    {
        TEST_SUCCESS_PASS("ab[]c[]d", skip(lit('[') > ']')[+alpha]);
        TEST_FAILURE("ab[]c[5]d", skip(lit('[') > ']')[+alpha], {
            CHECK(which == "']'"sv);
            CHECK(where == "5]d"sv);
        });

        TEST_SUCCESS_PASS("a1[]b2c3[]d4", skip(lit('[') > ']')[+(alpha > digit)]);
        TEST_FAILURE("a1[]b2c3[]d", skip(lit('[') > ']')[+(alpha > digit)], {
            CHECK(where == ""sv);
        });

        TEST_FAILURE("a b c", lit('a') > 'c', space, {
            CHECK(which == "'c'"sv);
            CHECK(where == "b c"sv);
        });


        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d", skip(space)[*char_ > lit('z')], s, {
                CHECK(which == "'z'"sv);
                CHECK(where == ""sv);
            });
        }

        {
            std::string s;
            TEST_ATTR_SUCCESS_PASS("a b\n c\n d", char_('a') > char_('b') > skip(space)[char_('c') > char_('d')], blank, s);
            CHECK(s == "abcd");
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b\n c\n d", char_('a') > char_('z') > skip(space)[char_('c') > char_('d')], blank, s, {
                CHECK(which == "'z'"sv);
                CHECK(where == "b\n c\n d"sv);
            });
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b\n c\n d", char_('a') > char_('b') > skip(space)[char_('z') > char_('d')], blank, s, {
                CHECK(where == "\n c\n d"sv);
            });
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b\n c\n d", char_('a') > char_('b') > skip(space)[char_('c') > char_('z')], blank, s, {
                CHECK(which == "'z'"sv);
                CHECK(where == "d"sv);
            });
        }

        // reskip
        {
            std::string s;
            TEST_ATTR_SUCCESS_PASS("a b c d", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > reskip[char_('d')]], blank, s);
            CHECK(s == "abcd");
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > reskip[char_('z')]], blank, s, {
                CHECK(where == "d"sv);
            });
        }

        // reskip with expectation failure context propagation
        {
            std::string s;
            TEST_ATTR_SUCCESS_PASS("a b c d e", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > reskip[char_('d') > char_('e')]], blank, s);
            CHECK(s == "abcde");
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d e", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > reskip[char_('z') > char_('e')]], blank, s, {
                CHECK(where == " d e"sv);
            });
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d e", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > reskip[char_('d') > char_('z')]], blank, s, {
                CHECK(which == "'z'"sv);
                CHECK(where == "e"sv);
            });
        }
    }

    // omit
    {
        TEST_SUCCESS_PASS("ab", omit[lit('a') > 'b']);
        TEST_FAILURE("ab", omit[lit('a') > 'c'], {
            CHECK(which == "'c'"sv);
            CHECK(where == "b"sv);
        });
    }

    // raw
    {
        TEST_SUCCESS_PASS("ab", raw[lit('a') > 'b']);
        TEST_FAILURE("ab", raw[lit('a') > 'c'], {
            CHECK(which == "'c'"sv);
            CHECK(where == "b"sv);
        });
    }

    // repeat
    {
        TEST_SUCCESS_PASS("ababac", repeat(1, 3)[lit('a') >> 'b'] >> "ac" | +alpha);

        TEST_FAILURE("ababac", repeat(1, 3)[lit('a') > 'b'] | +alpha, {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
        TEST_FAILURE("acab", repeat(2, 3)[lit('a') > 'b'] | +alpha, {
            CHECK(which == "'b'"sv);
            CHECK(where == "cab"sv);
        });

        TEST_SUCCESS_PASS("bcab", repeat(2, 3)[lit('a') > 'b'] | +alpha);
    }

    // seek
    {
        TEST_SUCCESS_PASS("a1b1c1", seek[lit('c') > '1']);
        TEST_FAILURE("a1b1c2c1", seek[lit('c') > '1'], {
            CHECK(which == "'1'"sv);
            CHECK(where == "2c1"sv);
        });
    }

    // alternative
    {
        TEST_SUCCESS_PASS("ac", lit('a') >> 'b' | "ac");
        TEST_SUCCESS_PASS("ac", lit('a') >> 'b' | lit('a') >> 'd' | "ac");

        TEST_FAILURE("ac", (lit('a') > 'b') | "ac", {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
        TEST_FAILURE("ac", lit('a') >> 'b' | (lit('a') > 'd') | "ac", {
            CHECK(which == "'d'"sv);
            CHECK(where == "c"sv);
        });
    }

    // predicate
    {
        TEST_SUCCESS_PASS("abc", lit('a') >> &(lit('b') > 'c') >> "bc");
        TEST_FAILURE("abc", lit('a') >> &(lit('b') > 'd') >> "bc", {
            CHECK(which == "'d'"sv);
            CHECK(where == "c"sv);
        });
    }

    // difference
    {
        TEST_SUCCESS_PASS("bcac", *(char_ - (lit('a') >> 'b')));
        TEST_SUCCESS_PASS("bcab", *(char_ - (lit('a') > 'b')) >> "ab");
        TEST_FAILURE("bcac", *(char_ - (lit('a') > 'b')) >> "ab", {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
    }

    // kleene
    {
        TEST_SUCCESS_PASS("abac", *(lit('a') >> 'b') >> "ac");
        TEST_SUCCESS_PASS("abbc", *(lit('a') > 'b') >> "bc");
        TEST_FAILURE("abac", *(lit('a') > 'b') >> "ac", {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
    }

    // list
    {
        TEST_SUCCESS_PASS("ab::ab::ac", (lit('a') >> 'b') % (lit(':') >> ':') >> "::ac");
        TEST_SUCCESS_PASS("ab::ab:ac", (lit('a') > 'b') % (lit(':') >> ':') >> ":ac");

        TEST_FAILURE("ab::ab::ac", (lit('a') > 'b') % (lit(':') >> ':') >> "::ac", {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
        TEST_FAILURE("ab::ab:ab", (lit('a') >> 'b') % (lit(':') > ':') >> ":ab", {
            CHECK(which == "':'"sv);
            CHECK(where == "ab"sv);
        });
    }

    // not
    {
        TEST_SUCCESS_PASS("[ac]", lit('[') >> !(lit('a') >> 'b') >> +alpha >> ']');
        TEST_SUCCESS_PASS("[bc]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']');
        TEST_FAILURE("[ac]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']', {
            CHECK(which == "'b'"sv);
            CHECK(where == "c]"sv);
        });
    }

    // optional
    {
        TEST_SUCCESS_PASS("ac", -(lit('a') >> 'b') >> "ac");
        TEST_SUCCESS_PASS("ab", -(lit('a') > 'b'));
        TEST_FAILURE("ac", -(lit('a') > 'b') >> "ac", {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
    }

    // plus
    {
        TEST_SUCCESS_PASS("abac", +(lit('a') >> 'b') >> "ac");
        TEST_SUCCESS_PASS("abbc", +(lit('a') > 'b') >> "bc");
        TEST_FAILURE("abac", +(lit('a') > 'b') >> "ac", {
            CHECK(which == "'b'"sv);
            CHECK(where == "c"sv);
        });
    }
}

// NOLINTEND(bugprone-chained-comparison)

#ifdef _MSC_VER
# pragma warning(pop)
#endif
