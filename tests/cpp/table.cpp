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
    // noexcept ==, potentially-throwing <: forces all_nothrow_equality_comparable != all_nothrow_less_than_comparable
    struct noexcept_eq_throwing_lt
    {
        int value = 0;

        [[maybe_unused]]
        bool operator==(const noexcept_eq_throwing_lt& o) const noexcept
        {
            return value == o.value;
        }

        [[maybe_unused]]
        bool operator<(const noexcept_eq_throwing_lt& o) const
        {
            return value < o.value;
        }
    };

    using throwing_lt_table = soagen::table<soagen::table_traits<soagen::column_traits<noexcept_eq_throwing_lt>>>;
}

//----------------------------------------------------------------------------------------------------------------------
// construction / capacity invariants (all fixtures)
//----------------------------------------------------------------------------------------------------------------------

TEMPLATE_TEST_CASE("table - default construction", "[table]", rich, trivial, move_only)
{
    TestType soa;
    CHECK(soa.empty());
    CHECK(soa.size() == 0u);
    CHECK(soa.capacity() == 0u);
    CHECK(soa.allocation_size() == 0u);
    CHECK(soa.max_size() > 0u);
    soa.for_each_column([](auto* ptr) { CHECK(ptr == nullptr); });
}

TEMPLATE_TEST_CASE("table - reserve", "[table]", rich, trivial, move_only)
{
    TestType soa;
    soa.reserve(16);
    CHECK(soa.empty());
    CHECK(soa.size() == 0u);
    CHECK(soa.capacity() >= 16u);
    CHECK(soa.allocation_size() > 0u);
    soa.for_each_column([](auto* ptr) { CHECK(ptr != nullptr); });

    const auto cap = soa.capacity();
    soa.reserve(8); // no-op: already big enough
    CHECK(soa.capacity() == cap);
}

TEMPLATE_TEST_CASE("table - grows on append", "[table]", rich, trivial, move_only)
{
    auto soa = make_soa<TestType>(5);
    CHECK(!soa.empty());
    CHECK(soa.size() == 5u);
    CHECK(soa.capacity() >= 5u);
    CHECK(soa.size() <= soa.max_size());
    soa.for_each_column(
        [&](auto* ptr, std::size_t idx)
        {
            REQUIRE(ptr != nullptr);
            CHECK(reinterpret_cast<std::uintptr_t>(ptr) % TestType::table_traits::column_alignments[idx] == 0u);
        });
}

TEMPLATE_TEST_CASE("table - clear keeps capacity", "[table]", rich, trivial, move_only)
{
    auto soa       = make_soa<TestType>(5);
    const auto cap = soa.capacity();
    soa.clear();
    CHECK(soa.empty());
    CHECK(soa.size() == 0u);
    CHECK(soa.capacity() == cap);
    CHECK(soa.allocation_size() > 0u);
}

TEMPLATE_TEST_CASE("table - shrink_to_fit", "[table]", rich, trivial, move_only)
{
    auto soa = make_soa<TestType>(5);
    soa.reserve(64);
    REQUIRE(soa.capacity() >= 64u);
    soa.shrink_to_fit();
    CHECK(soa.size() == 5u);
    CHECK(soa.capacity() >= 5u);
    CHECK(soa.capacity() < 64u);

    soa.clear();
    soa.shrink_to_fit(); // empty: frees the allocation
    CHECK(soa.capacity() == 0u);
    CHECK(soa.allocation_size() == 0u);
    soa.for_each_column([](auto* ptr) { CHECK(ptr == nullptr); });
}

TEMPLATE_TEST_CASE("table - resize grow and shrink", "[table]", rich, trivial, move_only)
{
    TestType soa;
    soa.resize(8);
    CHECK(soa.size() == 8u);
    soa.resize(3);
    CHECK(soa.size() == 3u);
    soa.resize(3); // no-op
    CHECK(soa.size() == 3u);
    soa.resize(0);
    CHECK(soa.empty());
}

TEMPLATE_TEST_CASE("table - pop_back", "[table]", rich, trivial, move_only)
{
    auto soa = make_soa<TestType>(4);
    soa.pop_back();
    CHECK(soa.size() == 3u);
    soa.pop_back(2u);
    CHECK(soa.size() == 1u);
    soa.pop_back(0u); // no-op
    CHECK(soa.size() == 1u);
}

TEMPLATE_TEST_CASE("table - move construction", "[table]", rich, trivial, move_only)
{
    auto src = make_soa<TestType>(4);
    TestType dst{ std::move(src) };
    CHECK(dst.size() == 4u);
    CHECK(src.empty());
    CHECK(src.capacity() == 0u);
}

TEMPLATE_TEST_CASE("table - move assignment", "[table]", rich, trivial, move_only)
{
    auto src     = make_soa<TestType>(4);
    TestType dst = make_soa<TestType>(1);
    dst          = std::move(src);
    CHECK(dst.size() == 4u);
    CHECK(src.empty());
}

TEMPLATE_TEST_CASE("table - swap", "[table]", rich, trivial, move_only)
{
    auto a = make_soa<TestType>(2);
    auto b = make_soa<TestType>(5);
    a.swap(b);
    CHECK(a.size() == 5u);
    CHECK(b.size() == 2u);
    using std::swap;
    swap(a, b);
    CHECK(a.size() == 2u);
    CHECK(b.size() == 5u);
}

TEMPLATE_TEST_CASE("table - erase shifts", "[table]", rich, trivial, move_only)
{
    auto soa = make_soa<TestType>(5);
    soa.erase(0u);
    CHECK(soa.size() == 4u);
    soa.erase(soa.begin());
    CHECK(soa.size() == 3u);
}

TEMPLATE_TEST_CASE("table - unordered_erase", "[table]", rich, trivial, move_only)
{
    auto soa   = make_soa<TestType>(5);
    auto moved = soa.unordered_erase(0u); // not last: reports the row swapped into the gap
    CHECK(soa.size() == 4u);
    CHECK(moved.has_value());
    moved = soa.unordered_erase(soa.size() - 1u); // last: nothing moved
    CHECK(soa.size() == 3u);
    CHECK(!moved.has_value());
}

TEST_CASE("table - erase removes the right rows and shifts the rest down", "[table]")
{
    auto soa = make_rich(5); // ids 0,1,2,3,4
    soa.erase(0u);
    REQUIRE(soa.size() == 4u);
    CHECK(soa.id()[0] == 1u);
    CHECK(soa.id()[3] == 4u);
    soa.erase(soa.begin()); // removes id 1
    REQUIRE(soa.size() == 3u);
    CHECK(soa.id()[0] == 2u);
    CHECK(soa.id()[1] == 3u);
    CHECK(soa.id()[2] == 4u);
}

TEST_CASE("table - unordered_erase swaps the last row into the gap", "[table]")
{
    auto soa   = make_rich(5); // ids 0,1,2,3,4
    auto moved = soa.unordered_erase(1u);
    REQUIRE(soa.size() == 4u);
    REQUIRE(moved.has_value());
    CHECK(moved.value() == 4u); // old index of the row relocated into the gap
    CHECK(soa.id()[0] == 0u);
    CHECK(soa.id()[1] == 4u); // last row swapped into slot 1
    CHECK(soa.id()[2] == 2u);
    CHECK(soa.id()[3] == 3u);

    moved = soa.unordered_erase(soa.size() - 1u); // erase last: nothing relocated
    REQUIRE(soa.size() == 3u);
    CHECK(!moved.has_value());
    CHECK(soa.id()[0] == 0u);
    CHECK(soa.id()[1] == 4u);
    CHECK(soa.id()[2] == 2u);
}

TEMPLATE_TEST_CASE("table - swap_columns", "[table]", rich, trivial, move_only)
{
    auto soa = make_soa<TestType>(3);
    soa.template swap_columns<0, 0>(); // self-swap is a no-op but must compile
    CHECK(soa.size() == 3u);
}

TEMPLATE_TEST_CASE("table - reserve beyond max_size throws instead of overflowing", "[table]", rich, trivial)
{
    TestType soa;
    REQUIRE_THROWS_AS(soa.reserve(static_cast<std::size_t>(-1)), std::bad_alloc);
    CHECK(soa.empty());
    CHECK(soa.capacity() == 0u);
    REQUIRE_THROWS_AS(soa.resize(static_cast<std::size_t>(-1)), std::bad_alloc);
    CHECK(soa.empty());
}

TEST_CASE("table - relational operators are not noexcept when element ordering can throw", "[table][ordering]")
{
    static_assert(noexcept(std::declval<const throwing_lt_table&>() == std::declval<const throwing_lt_table&>()));
    static_assert(!noexcept(std::declval<const throwing_lt_table&>() < std::declval<const throwing_lt_table&>()));
    static_assert(!noexcept(std::declval<const throwing_lt_table&>() <= std::declval<const throwing_lt_table&>()));
    static_assert(!noexcept(std::declval<const throwing_lt_table&>() > std::declval<const throwing_lt_table&>()));
    static_assert(!noexcept(std::declval<const throwing_lt_table&>() >= std::declval<const throwing_lt_table&>()));
    SUCCEED();
}

TEST_CASE("table - colliding pascal-case column names emplace", "[table]")
{
    collide c;
    c.emplace_back(1, 2.0f);
    c.push_back(3, 4.0f);
    CHECK(c.size() == 2u);
    CHECK(c.column<collide::columns::foo_bar>()[0] == 1);
    CHECK(c.column<collide::columns::fooBar>()[1] == 4.0f);
}

TEMPLATE_TEST_CASE("table - resize_for_overwrite", "[table]", rich, trivial, move_only)
{
    TestType soa;
    soa.resize_for_overwrite(8);
    CHECK(soa.size() == 8u);
    soa.resize_for_overwrite(2);
    CHECK(soa.size() == 2u);
}

TEST_CASE("table - swap_columns swaps distinct same-type columns", "[table]")
{
    // trivial columns x and y are both float; swapping them exchanges their data
    auto t = make_trivial(3); // x = i, y = i+1
    REQUIRE(t.x()[0] == 0.0f);
    REQUIRE(t.y()[0] == 1.0f);
    t.swap_columns<0, 1>();
    CHECK(t.x()[0] == 1.0f);
    CHECK(t.y()[0] == 0.0f);
    CHECK(t.x()[2] == 3.0f);
}

//----------------------------------------------------------------------------------------------------------------------
// copyable fixtures only
//----------------------------------------------------------------------------------------------------------------------

TEMPLATE_TEST_CASE("table - copy construction", "[table]", rich, trivial)
{
    auto src = make_soa<TestType>(4);
    TestType dst{ src };
    CHECK(dst.size() == 4u);
    CHECK(src.size() == 4u);
    CHECK(dst == src);
    CHECK(dst.template column<0>() != src.template column<0>()); // distinct storage
}

TEMPLATE_TEST_CASE("table - copy assignment", "[table]", rich, trivial)
{
    auto src     = make_soa<TestType>(4);
    TestType dst = make_soa<TestType>(1);
    dst          = src;
    CHECK(dst.size() == 4u);
    CHECK(dst == src);
}

//----------------------------------------------------------------------------------------------------------------------
// rich: exact-value modifiers
//----------------------------------------------------------------------------------------------------------------------

TEST_CASE("table - rich push_back values", "[table]")
{
    rich r;
    r.push_back("mark", 0, { 1987, 3, 16 }, 999999, nullptr);
    r.push_back("joe", 1, { 1970, 1, 1 }, 50000, nullptr);
    REQUIRE(r.size() == 2u);
    CHECK_RICH_ROW(r.front(), "mark", 0, (1987, 3, 16), 999999, nullptr);
    CHECK_RICH_ROW(r[1], "joe", 1, (1970, 1, 1), 50000, nullptr);
    CHECK_RICH_ROW(r.back(), "joe", 1, (1970, 1, 1), 50000, nullptr);
    CHECK(r.name()[0] == "mark");
    CHECK(r.id()[1] == 1u);
    CHECK(r.date_of_birth()[0] == std::tuple{ 1987, 3, 16 });
}

TEST_CASE("table - rich emplace_back via emplacer/tuple", "[table]")
{
    int someval = 7;
    rich r;
    r.emplace_back(emplacer{ std::size_t{ 10 }, 'A' }, 2u, std::tuple{ 1980, 2, 2 }, 40000, &someval);
    REQUIRE(r.size() == 1u);
    CHECK_RICH_ROW(r.back(), "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
}

TEST_CASE("table - rich insert shifts", "[table]")
{
    auto r = make_rich(2); // ids 0, 1
    r.insert(1u, "hot diggity", 3, { 1955, 3, 3 }, 70000, nullptr);
    REQUIRE(r.size() == 3u);
    CHECK(r.id()[0] == 0u);
    CHECK(r.name()[1] == "hot diggity");
    CHECK(r.id()[2] == 1u);

    r.insert(r.begin(), "first", 9, { 2000, 1, 1 }, 1, nullptr); // insert by iterator
    CHECK(r.size() == 4u);
    CHECK(r.name()[0] == "first");
}
