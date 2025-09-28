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

// Don't place these in anonymous namespace; they make debug harder

template<class T>
struct ref_holder
{
    T& ref;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

struct existent_tag;
struct non_existent_tag;
struct next_tag;

TEST_CASE("context")
{
    using x4::context;

    {
        using NextContext = context<next_tag, double>;
        using Context = context<existent_tag, int, NextContext>;
        STATIC_CHECK(sizeof(NextContext) == sizeof(ref_holder<double>));
        STATIC_CHECK(sizeof(Context) == sizeof(ref_holder<int>) + sizeof(NextContext));
    }
    {
        using NextContext = context<next_tag, double>;
        using Context = context<existent_tag, int, NextContext const&>;
        STATIC_CHECK(sizeof(NextContext) == sizeof(ref_holder<double>));
        STATIC_CHECK(sizeof(Context) == sizeof(ref_holder<int>) + sizeof(ref_holder<NextContext>));
    }

    {
        struct IntRef { int& ref; };  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        STATIC_CHECK(sizeof(context<struct i_, int>) == sizeof(IntRef));
        STATIC_CHECK(sizeof(context<struct i_, int, context<struct d_, double>>) == sizeof(IntRef) * 2);
        STATIC_CHECK(sizeof(context<struct i_, int, context<struct d_, double> const&>) == sizeof(IntRef) * 2);
    }

    {
        // Replace the first context of `context<..., non-owning>`
        {
            double j = 3.14;
            auto next_ctx = x4::make_context<next_tag>(j);

            int i = 42;
            auto ctx = x4::make_context<existent_tag>(i, next_ctx);
            STATIC_CHECK(std::same_as<decltype(ctx), context<existent_tag, int, context<next_tag, double> const&>>);

            int replaced_i = 43;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, replaced_i);

            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, int, context<next_tag, double> const&>&&>);
        }
        // Replace the first context of `context<..., owning>`
        {
            double j = 3.14;

            int i = 42;
            auto ctx = x4::make_context<existent_tag>(i, x4::make_context<next_tag>(j));
            STATIC_CHECK(std::same_as<decltype(ctx), context<existent_tag, int, context<next_tag, double>>>);

            int replaced_i = 43;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, replaced_i); // Should not create copy of `Next`

            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, int, context<next_tag, double> /* not reference */>&&>);
        }
    }

    {
        int i = 42;
        auto ctx = x4::make_context<existent_tag>(i);

        using Context = decltype(ctx);
        STATIC_CHECK(std::same_as<Context, context<existent_tag, int>>);
        STATIC_CHECK(std::move_constructible<Context>);
        STATIC_CHECK(std::copy_constructible<Context>);
        STATIC_CHECK(!std::assignable_from<Context&, Context>);

        CHECK(x4::get<existent_tag>(ctx) == 42);

        {
            auto&& removed_ctx = x4::remove_first_context<existent_tag>(ctx);
            STATIC_CHECK(std::same_as<decltype(removed_ctx), x4::detail::monostate_context&&>);

            // monostate tests
            {
                auto&& mono_removed_ctx = x4::remove_first_context<non_existent_tag>(removed_ctx);
                STATIC_CHECK(std::same_as<decltype(mono_removed_ctx), x4::detail::monostate_context const&>);
                (void)mono_removed_ctx;
            }
            {
                double d = 3.14;
                auto&& mono_replaced_ctx = x4::replace_first_context<non_existent_tag>(removed_ctx, d);
                STATIC_CHECK(std::same_as<decltype(mono_replaced_ctx), context<non_existent_tag, double>&&>);
                CHECK(x4::get<existent_tag>(ctx) == 42);
                CHECK(x4::get<non_existent_tag>(mono_replaced_ctx) == 3.14);
            }
        }
        {
            auto&& removed_ctx = x4::remove_first_context<non_existent_tag>(ctx);
            STATIC_CHECK(std::same_as<decltype(removed_ctx), context<existent_tag, int> const&>);
            CHECK(x4::get<existent_tag>(ctx) == 42);
            CHECK(x4::get<existent_tag>(removed_ctx) == 42);
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, d);
            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, double>&&>);
            CHECK(x4::get<existent_tag>(ctx) == 42);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 3.14);
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<non_existent_tag>(ctx, d);
            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, int, context<non_existent_tag, double>>&&>);
            CHECK(x4::get<existent_tag>(ctx) == 42);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 42);
            CHECK(x4::get<non_existent_tag>(replaced_ctx) == 3.14);

            i = 43;
            CHECK(x4::get<existent_tag>(ctx) == 43);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 43);
            i = 42;
        }

        {
            auto ctx_ctx = x4::make_context<non_existent_tag>(i, ctx);
            using ContextContext = decltype(ctx_ctx);
            STATIC_CHECK(std::same_as<ContextContext, context<non_existent_tag, int, context<existent_tag, int> const&>>);
            STATIC_CHECK(std::move_constructible<ContextContext>);
            STATIC_CHECK(std::copy_constructible<ContextContext>);
            STATIC_CHECK(!std::assignable_from<ContextContext&, ContextContext>);

            CHECK(x4::get<existent_tag>(ctx) == 42);
            CHECK(x4::get<existent_tag>(ctx_ctx) == 42);
            i = 43;
            CHECK(x4::get<existent_tag>(ctx) == 43);
            CHECK(x4::get<existent_tag>(ctx_ctx) == 43);
            i = 42;

            STATIC_CHECK(std::move_constructible<Context>);
            STATIC_CHECK(std::copy_constructible<Context>);
            STATIC_CHECK(!std::assignable_from<Context&, Context>);
        }
    }

    // ---------------------------------------

    {
        int i = 42;
        auto ctx = x4::make_context<existent_tag>(std::as_const(i));
        CHECK(x4::get<existent_tag>(ctx) == 42);
        STATIC_CHECK(std::same_as<decltype(x4::get<existent_tag>(ctx)), int const&>);
        STATIC_CHECK(std::same_as<decltype(x4::get<existent_tag>(std::as_const(ctx))), int const&>);
    }

    // Start with plain `context<tag, int>`
    {
        int i = 42;
        auto ctx = x4::make_context<existent_tag>(i);
        STATIC_CHECK(std::same_as<decltype(ctx), context<existent_tag, int>>);
        CHECK(x4::get<existent_tag>(ctx) == 42);
        STATIC_CHECK(std::same_as<decltype(x4::get<existent_tag>(ctx)), int&>);
        STATIC_CHECK(std::same_as<decltype(x4::get<existent_tag>(std::as_const(ctx))), int&>);
        CHECK(std::addressof(ctx.val) == std::addressof(i));

        {
            int j = 999;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, j);
            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, int>&&>);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 999);
            CHECK(std::addressof(ctx.val) == std::addressof(i));
            CHECK(std::addressof(replaced_ctx.val) == std::addressof(j));

            {
                auto&& removed_ctx = x4::remove_first_context<non_existent_tag>(replaced_ctx);
                STATIC_CHECK(std::same_as<decltype(removed_ctx), context<existent_tag, int> const&>);
                CHECK(std::addressof(removed_ctx) == std::addressof(replaced_ctx));
            }
            {
                auto&& removed_ctx = x4::remove_first_context<existent_tag>(replaced_ctx);
                STATIC_CHECK(std::same_as<decltype(removed_ctx), x4::detail::monostate_context&&>);

                {
                    [[maybe_unused]] auto&& removed_removed_ctx = x4::remove_first_context<existent_tag>(removed_ctx);
                    STATIC_CHECK(std::same_as<decltype(removed_removed_ctx), x4::detail::monostate_context const&>);
                }
                {
                    [[maybe_unused]] auto&& new_ctx = x4::make_context<existent_tag>(i, removed_ctx);
                    STATIC_CHECK(std::same_as<decltype(new_ctx), context<existent_tag, int>&&>);
                }
                {
                    auto&& new_ctx = x4::replace_first_context<existent_tag>(removed_ctx, i);
                    STATIC_CHECK(std::same_as<decltype(new_ctx), context<existent_tag, int>&&>);
                    CHECK(std::addressof(new_ctx.val) == std::addressof(i));
                }
            }
        }
        {
            double d = 3.14;
            auto replaced_ctx = x4::replace_first_context<existent_tag>(ctx, d);
            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, double>>);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 3.14);
            CHECK(std::addressof(ctx.val) == std::addressof(i));
            CHECK(std::addressof(replaced_ctx.val) == std::addressof(d));
        }
        {
            double d = 3.14;
            auto replaced_ctx = x4::replace_first_context<non_existent_tag>(ctx, d);
            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, int, context<non_existent_tag, double>>>);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 42);
            CHECK(std::addressof(ctx.val) == std::addressof(i));
            CHECK(std::addressof(x4::get<non_existent_tag>(replaced_ctx)) == std::addressof(d));
        }
    }

    // Start with nested; `context<dummy_tag, dummy_t const, context<tag, int>>`
    {
        struct dummy_tag;
        struct dummy_t {};
        constexpr dummy_t dummy;

        auto const dummy_ctx = x4::make_context<dummy_tag>(dummy);
        using dummy_ctx_t = context<dummy_tag, dummy_t const>;
        STATIC_CHECK(std::same_as<decltype(dummy_ctx), dummy_ctx_t const>);

        int i = 42;
        auto ctx = x4::make_context<existent_tag>(i, dummy_ctx);
        STATIC_CHECK(std::same_as<decltype(ctx), context<existent_tag, int, dummy_ctx_t const&>>);
        CHECK(x4::get<existent_tag>(ctx) == 42);
        STATIC_CHECK(std::same_as<decltype(x4::get<existent_tag>(ctx)), int&>);
        STATIC_CHECK(std::same_as<decltype(x4::get<existent_tag>(std::as_const(ctx))), int&>);
        CHECK(std::addressof(ctx.val) == std::addressof(i));

        {
            int j = 999;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, j);
            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, int, dummy_ctx_t const&>&&>);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 999);
            CHECK(std::addressof(ctx.val) == std::addressof(i));
            CHECK(std::addressof(replaced_ctx.val) == std::addressof(j));

            {
                auto&& removed_ctx = x4::remove_first_context<non_existent_tag>(replaced_ctx);
                STATIC_CHECK(std::same_as<decltype(removed_ctx), context<existent_tag, int, dummy_ctx_t const&> const&>);
                CHECK(std::addressof(removed_ctx) == std::addressof(replaced_ctx));

                {
                    [[maybe_unused]] auto&& removed_removed_ctx = x4::remove_first_context<existent_tag>(removed_ctx);
                    STATIC_CHECK(std::same_as<decltype(removed_removed_ctx), dummy_ctx_t const&>);
                }
                {
                    [[maybe_unused]] auto new_ctx = x4::make_context<non_existent_tag>(i, removed_ctx);
                    STATIC_CHECK(std::same_as<decltype(new_ctx), context<non_existent_tag, int, context<existent_tag, int, dummy_ctx_t const&> const&>>);
                }
                {
                    auto&& new_ctx = x4::replace_first_context<existent_tag>(removed_ctx, i);
                    STATIC_CHECK(std::same_as<decltype(new_ctx), context<existent_tag, int, dummy_ctx_t const&>&&>);
                    CHECK(std::addressof(new_ctx.val) == std::addressof(i));
                }
            }
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<existent_tag>(ctx, d);
            STATIC_CHECK(std::same_as<decltype(replaced_ctx), context<existent_tag, double, dummy_ctx_t const&>&&>);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 3.14);
            CHECK(std::addressof(ctx.val) == std::addressof(i));
            CHECK(std::addressof(replaced_ctx.val) == std::addressof(d));
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<dummy_tag>(ctx, d);
            STATIC_CHECK(std::same_as<
                decltype(replaced_ctx),
                context<
                    existent_tag, int,
                    context<
                        dummy_tag, double
                    >
                >&&
            >);
            CHECK(x4::get<existent_tag>(replaced_ctx) == 42);
            CHECK(std::addressof(ctx.val) == std::addressof(i));
            CHECK(std::addressof(x4::get<dummy_tag>(replaced_ctx)) == std::addressof(d));
        }
        {
            double d = 3.14;
            auto&& replaced_ctx = x4::replace_first_context<non_existent_tag>(ctx, d);
            STATIC_CHECK(std::same_as<
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
            CHECK(x4::get<existent_tag>(replaced_ctx) == 42);
            CHECK(std::addressof(ctx.val) == std::addressof(i));
            CHECK(std::addressof(x4::get<non_existent_tag>(replaced_ctx)) == std::addressof(d));
        }
    }
}
