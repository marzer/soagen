// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"
#include <new>
#include <utility>

using namespace tests;

namespace
{
    // a stateful, propagating, not-always-equal allocator, to exercise the table's allocator-propagation branches.
    struct tracking_allocator
    {
        using value_type                             = std::byte;
        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::true_type;
        using propagate_on_container_swap            = std::true_type;
        using is_always_equal                        = std::false_type;

        static constexpr std::size_t min_alignment = 64;

        int id = 0;

        [[maybe_unused]]
        explicit tracking_allocator(int i = 0) noexcept //
            : id{ i }
        {
        }

        std::byte* allocate(std::size_t n, std::align_val_t = std::align_val_t{ min_alignment })
        {
            return static_cast<std::byte*>(::operator new(n, std::align_val_t{ min_alignment }));
        }

        void deallocate(std::byte* p, std::size_t) noexcept
        {
            ::operator delete(p, std::align_val_t{ min_alignment });
        }

        bool operator==(const tracking_allocator& o) const noexcept
        {
            return id == o.id;
        }
        bool operator!=(const tracking_allocator& o) const noexcept
        {
            return id != o.id;
        }
    };

    using table_t = soagen::table<rich::table_traits, tracking_allocator>;

    table_t make_one(int alloc_id)
    {
        table_t t{ tracking_allocator{ alloc_id } };
        t.emplace_back(std::string("x"), 1ull, std::tuple{ 1, 2, 3 }, 10, nullptr);
        return t;
    }
}

TEST_CASE("allocator - stateful equality", "[allocator]")
{
    tracking_allocator a{ 1 }, b{ 1 }, c{ 2 };
    CHECK(a == b);
    CHECK(a != c);
}

TEST_CASE("allocator - get_allocator round-trips state", "[allocator]")
{
    table_t t{ tracking_allocator{ 42 } };
    CHECK(t.get_allocator().id == 42);
}

TEST_CASE("allocator - propagate on copy assignment", "[allocator]")
{
    auto src = make_one(1);
    table_t dst{ tracking_allocator{ 2 } };
    dst = src; // POCCA == true: dst adopts src's allocator
    CHECK(dst.size() == 1u);
    CHECK(dst.get_allocator().id == 1);
    CHECK(dst.get_allocator() == src.get_allocator());
}

TEST_CASE("allocator - propagate on move assignment", "[allocator]")
{
    auto src = make_one(5);
    table_t dst{ tracking_allocator{ 6 } };
    dst = std::move(src); // POCMA == true: dst adopts source's allocator
    CHECK(dst.size() == 1u);
    CHECK(dst.get_allocator().id == 5);
}

TEST_CASE("allocator - propagate on swap", "[allocator]")
{
    auto a = make_one(7);
    auto b = make_one(8);
    a.swap(b); // POCS == true: allocators are exchanged with the contents
    CHECK(a.get_allocator().id == 8);
    CHECK(b.get_allocator().id == 7);
}

TEST_CASE("allocator - copy construction copies via select_on_container_copy", "[allocator]")
{
    auto src = make_one(3);
    table_t dst{ src };
    CHECK(dst.size() == 1u);
    // the copy's allocator must derive from the SOURCE's allocator (default select_on_container_copy_construction
    // copies its argument), not from the destination's own uninitialised allocator.
    CHECK(dst.get_allocator().id == 3);
    CHECK(dst.get_allocator() == src.get_allocator());
}

namespace
{
    // a stateful, NON-propagating, not-always-equal allocator, to exercise the elementwise copy/move-assign fallbacks.
    struct stateful_allocator
    {
        using value_type                             = std::byte;
        using propagate_on_container_copy_assignment = std::false_type;
        using propagate_on_container_move_assignment = std::false_type;
        using propagate_on_container_swap            = std::false_type;
        using is_always_equal                        = std::false_type;

        static constexpr std::size_t min_alignment = 64;

        int id = 0;

        explicit stateful_allocator(int i = 0) noexcept //
            : id{ i }
        {
        }

        std::byte* allocate(std::size_t n, std::align_val_t = std::align_val_t{ min_alignment })
        {
            return static_cast<std::byte*>(::operator new(n, std::align_val_t{ min_alignment }));
        }

        void deallocate(std::byte* p, std::size_t) noexcept
        {
            ::operator delete(p, std::align_val_t{ min_alignment });
        }

        [[maybe_unused]]
        bool operator==(const stateful_allocator& o) const noexcept
        {
            return id == o.id;
        }

        [[maybe_unused]]
        bool operator!=(const stateful_allocator& o) const noexcept
        {
            return id != o.id;
        }
    };

    using ne_table = soagen::table<rich::table_traits, stateful_allocator>;

    ne_table make_ne(int alloc_id, int n)
    {
        ne_table t{ stateful_allocator{ alloc_id } };
        for (int i = 0; i < n; i++)
            t.emplace_back(std::string("x"),
                           static_cast<unsigned long long>(i),
                           std::tuple{ i, i, i },
                           i * 10,
                           nullptr);
        return t;
    }
}

TEST_CASE("allocator - non-propagating copy assignment keeps own allocator and copies elements", "[allocator]")
{
    auto src = make_ne(1, 3);
    auto dst = make_ne(2, 1);
    dst      = src; // POCCA == false + unequal allocators: dst keeps its own allocator, copies elements
    CHECK(dst.size() == 3u);
    CHECK(dst.get_allocator().id == 2);
    CHECK(src.get_allocator().id == 1);
    for (int i = 0; i < 3; i++)
        CHECK(dst.column<1>()[i] == static_cast<unsigned long long>(i));
}

TEST_CASE("allocator - non-propagating move assignment with unequal allocators moves elementwise", "[allocator]")
{
    auto src = make_ne(1, 3);
    auto dst = make_ne(2, 1);
    dst      = std::move(src); // POCMA == false + unequal allocators: dst keeps its own allocator, moves elementwise
    CHECK(dst.size() == 3u);
    CHECK(dst.get_allocator().id == 2);
    for (int i = 0; i < 3; i++)
        CHECK(dst.column<1>()[i] == static_cast<unsigned long long>(i));
}
