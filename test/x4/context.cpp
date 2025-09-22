/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/core/context.hpp>

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

int main()
{
    using x4::context;
    struct existent_tag;
    struct non_existent_tag;

    {
        struct IntRef { int& ref; };  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        static_assert(sizeof(context<struct i_, int>) == sizeof(IntRef));
        static_assert(sizeof(context<struct i_, int, context<struct d_, double>>) == sizeof(IntRef) * 2);
        static_assert(sizeof(context<struct i_, int, context<struct d_, double> const&>) == sizeof(IntRef) * 2);
    }

    {
        int i = 42;
        auto ctx = x4::make_context<existent_tag>(i);

        using Context = decltype(ctx);
        static_assert(std::same_as<Context, context<existent_tag, int>>);
        static_assert(std::move_constructible<Context>);
        static_assert(std::copy_constructible<Context>);
        static_assert(!std::assignable_from<Context&, Context>);

        BOOST_TEST(x4::get<existent_tag>(ctx) == 42);

        {
            auto&& removed_ctx = x4::remove_first_context<existent_tag>(ctx);
            static_assert(std::same_as<decltype(removed_ctx), x4::detail::monostate_context&&>);

            // monostate tests
            {
                auto&& mono_removed_ctx = x4::remove_first_context<non_existent_tag>(removed_ctx);
                static_assert(std::same_as<decltype(mono_removed_ctx), x4::detail::monostate_context const&>);
                (void)mono_removed_ctx;
            }
            {
                double d = 3.14;
                auto&& mono_replaced_ctx = x4::replace_first_context<non_existent_tag>(removed_ctx, d);
                static_assert(std::same_as<decltype(mono_replaced_ctx), context<non_existent_tag, double>&&>);
                BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
                BOOST_TEST(x4::get<non_existent_tag>(mono_replaced_ctx) == 3.14);
            }
        }
        {
            auto&& removed_ctx = x4::remove_first_context<non_existent_tag>(ctx);
            static_assert(std::same_as<decltype(removed_ctx), context<existent_tag, int> const&>);
            BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
            BOOST_TEST(x4::get<existent_tag>(removed_ctx) == 42);
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, d);
            static_assert(std::same_as<decltype(replaced_ctx), context<existent_tag, double>&&>);
            BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 3.14);
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<non_existent_tag>(ctx, d);
            static_assert(std::same_as<decltype(replaced_ctx), context<existent_tag, int, context<non_existent_tag, double>>&&>);
            BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 42);
            BOOST_TEST(x4::get<non_existent_tag>(replaced_ctx) == 3.14);

            i = 43;
            BOOST_TEST(x4::get<existent_tag>(ctx) == 43);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 43);
            i = 42;
        }

        {
            auto ctx_ctx = x4::make_context<non_existent_tag>(i, ctx);
            using ContextContext = decltype(ctx_ctx);
            static_assert(std::same_as<ContextContext, context<non_existent_tag, int, context<existent_tag, int> const&>>);
            static_assert(std::move_constructible<ContextContext>);
            static_assert(std::copy_constructible<ContextContext>);
            static_assert(!std::assignable_from<ContextContext&, ContextContext>);

            BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
            BOOST_TEST(x4::get<existent_tag>(ctx_ctx) == 42);
            i = 43;
            BOOST_TEST(x4::get<existent_tag>(ctx) == 43);
            BOOST_TEST(x4::get<existent_tag>(ctx_ctx) == 43);
            i = 42;

            static_assert(std::move_constructible<Context>);
            static_assert(std::copy_constructible<Context>);
            static_assert(!std::assignable_from<Context&, Context>);
        }
    }

    // ---------------------------------------

    {
        int i = 42;
        auto ctx = x4::make_context<existent_tag>(std::as_const(i));
        BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
        static_assert(std::same_as<decltype(x4::get<existent_tag>(ctx)), int const&>);
        static_assert(std::same_as<decltype(x4::get<existent_tag>(std::as_const(ctx))), int const&>);
    }

    // Start with plain `context<tag, int>`
    {
        int i = 42;
        auto ctx = x4::make_context<existent_tag>(i);
        static_assert(std::same_as<decltype(ctx), context<existent_tag, int>>);
        BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
        static_assert(std::same_as<decltype(x4::get<existent_tag>(ctx)), int&>);
        static_assert(std::same_as<decltype(x4::get<existent_tag>(std::as_const(ctx))), int&>);
        BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));

        {
            int j = 999;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, j);
            static_assert(std::same_as<decltype(replaced_ctx), context<existent_tag, int>&&>);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 999);
            BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));
            BOOST_TEST(std::addressof(replaced_ctx.val) == std::addressof(j));

            {
                auto&& removed_ctx = x4::remove_first_context<non_existent_tag>(replaced_ctx);
                static_assert(std::same_as<decltype(removed_ctx), context<existent_tag, int> const&>);
                BOOST_TEST(std::addressof(removed_ctx) == std::addressof(replaced_ctx));
            }
            {
                auto&& removed_ctx = x4::remove_first_context<existent_tag>(replaced_ctx);
                static_assert(std::same_as<decltype(removed_ctx), x4::detail::monostate_context&&>);

                {
                    [[maybe_unused]] auto&& removed_removed_ctx = x4::remove_first_context<existent_tag>(removed_ctx);
                    static_assert(std::same_as<decltype(removed_removed_ctx), x4::detail::monostate_context const&>);
                }
                {
                    [[maybe_unused]] auto&& new_ctx = x4::make_context<existent_tag>(i, removed_ctx);
                    static_assert(std::same_as<decltype(new_ctx), context<existent_tag, int>&&>);
                }
                {
                    auto&& new_ctx = x4::replace_first_context<existent_tag>(removed_ctx, i);
                    static_assert(std::same_as<decltype(new_ctx), context<existent_tag, int>&&>);
                    BOOST_TEST(std::addressof(new_ctx.val) == std::addressof(i));
                }
            }
        }
        {
            double d = 3.14;
            auto replaced_ctx = x4::replace_first_context<existent_tag>(ctx, d);
            static_assert(std::same_as<decltype(replaced_ctx), context<existent_tag, double>>);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 3.14);
            BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));
            BOOST_TEST(std::addressof(replaced_ctx.val) == std::addressof(d));
        }
        {
            double d = 3.14;
            auto replaced_ctx = x4::replace_first_context<non_existent_tag>(ctx, d);
            static_assert(std::same_as<decltype(replaced_ctx), context<existent_tag, int, context<non_existent_tag, double>>>);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 42);
            BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));
            BOOST_TEST(std::addressof(x4::get<non_existent_tag>(replaced_ctx)) == std::addressof(d));
        }
    }

    // Start with nested; `context<dummy_tag, dummy_t const, context<tag, int>>`
    {
        struct dummy_tag;
        struct dummy_t {};
        constexpr dummy_t dummy;

        auto const dummy_ctx = x4::make_context<dummy_tag>(dummy);
        using dummy_ctx_t = context<dummy_tag, dummy_t const>;
        static_assert(std::same_as<decltype(dummy_ctx), dummy_ctx_t const>);

        int i = 42;
        auto ctx = x4::make_context<existent_tag>(i, dummy_ctx);
        static_assert(std::same_as<decltype(ctx), context<existent_tag, int, dummy_ctx_t const&>>);
        BOOST_TEST(x4::get<existent_tag>(ctx) == 42);
        static_assert(std::same_as<decltype(x4::get<existent_tag>(ctx)), int&>);
        static_assert(std::same_as<decltype(x4::get<existent_tag>(std::as_const(ctx))), int&>);
        BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));

        {
            int j = 999;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, j);
            static_assert(std::same_as<decltype(replaced_ctx), context<existent_tag, int, dummy_ctx_t const&>&&>);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 999);
            BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));
            BOOST_TEST(std::addressof(replaced_ctx.val) == std::addressof(j));

            {
                auto&& removed_ctx = x4::remove_first_context<non_existent_tag>(replaced_ctx);
                static_assert(std::same_as<decltype(removed_ctx), context<existent_tag, int, dummy_ctx_t const&> const&>);
                BOOST_TEST(std::addressof(removed_ctx) == std::addressof(replaced_ctx));

                {
                    [[maybe_unused]] auto&& removed_removed_ctx = x4::remove_first_context<existent_tag>(removed_ctx);
                    static_assert(std::same_as<decltype(removed_removed_ctx), dummy_ctx_t const&>);
                }
                {
                    [[maybe_unused]] auto new_ctx = x4::make_context<non_existent_tag>(i, removed_ctx);
                    static_assert(std::same_as<decltype(new_ctx), context<non_existent_tag, int, context<existent_tag, int, dummy_ctx_t const&> const&>>);
                }
                {
                    auto&& new_ctx = x4::replace_first_context<existent_tag>(removed_ctx, i);
                    static_assert(std::same_as<decltype(new_ctx), context<existent_tag, int, dummy_ctx_t const&>&&>);
                    BOOST_TEST(std::addressof(new_ctx.val) == std::addressof(i));
                }
            }
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, d);
            static_assert(std::same_as<decltype(replaced_ctx), context<existent_tag, double, dummy_ctx_t const&>&&>);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 3.14);
            BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));
            BOOST_TEST(std::addressof(replaced_ctx.val) == std::addressof(d));
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<dummy_tag>(ctx, d);
            static_assert(std::same_as<
                decltype(replaced_ctx),
                context<
                    existent_tag, int,
                    context<
                        dummy_tag, double
                    >
                >&&
            >);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 42);
            BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));
            BOOST_TEST(std::addressof(x4::get<dummy_tag>(replaced_ctx)) == std::addressof(d));
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<non_existent_tag>(ctx, d);
            static_assert(std::same_as<
                decltype(replaced_ctx),
                context<
                    existent_tag, int,
                    context<
                        dummy_tag, dummy_t const,
                        context<
                            non_existent_tag, double
                        >
                    >
                >&&
            >);
            BOOST_TEST(x4::get<existent_tag>(replaced_ctx) == 42);
            BOOST_TEST(std::addressof(ctx.val) == std::addressof(i));
            BOOST_TEST(std::addressof(x4::get<non_existent_tag>(replaced_ctx)) == std::addressof(d));
        }
    }

    return boost::report_errors();
}
