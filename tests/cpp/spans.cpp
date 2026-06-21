// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"

using namespace tests;

TEST_CASE("spans - subspan over whole table", "[spans]")
{
    auto r    = make_rich(3);
    auto span = r.subspan(0u);
    CHECK(!span.empty());
    CHECK(span.size() == 3u);
    CHECK_RICH_ROW(span.front(), "name 0", 0, (1970, 1, 1), 0, nullptr);
    CHECK_RICH_ROW(span[1], "name 1", 1, (1971, 1, 1), 1000, nullptr);
    CHECK_RICH_ROW(span.back(), "name 2", 2, (1972, 1, 1), 2000, nullptr);
    CHECK_RICH_ROW(span.row(2), "name 2", 2, (1972, 1, 1), 2000, nullptr);
    CHECK_RICH_ROW(span.at(0), "name 0", 0, (1970, 1, 1), 0, nullptr);
    CHECK(span.name()[0] == "name 0");
}

TEST_CASE("spans - subspan offset + count", "[spans]")
{
    auto r    = make_rich(4);
    auto span = r.subspan(1u, 2u);
    CHECK(span.size() == 2u);
    CHECK_RICH_ROW(span.front(), "name 1", 1, (1971, 1, 1), 1000, nullptr);
    CHECK_RICH_ROW(span.back(), "name 2", 2, (1972, 1, 1), 2000, nullptr);

    // span of a span
    auto inner = span.subspan(1u);
    CHECK(inner.size() == 1u);
    CHECK_RICH_ROW(inner.front(), "name 2", 2, (1972, 1, 1), 2000, nullptr);
}

TEST_CASE("spans - const_subspan", "[spans]")
{
    auto r    = make_rich(2);
    auto span = r.const_subspan(0u);
    static_assert(soagen::is_span<decltype(span)>);
    CHECK(span.size() == 2u);
    CHECK_RICH_ROW(span.back(), "name 1", 1, (1971, 1, 1), 1000, nullptr);
}

TEST_CASE("spans - iteration + swizzle", "[spans]")
{
    auto r    = make_rich(3);
    auto span = r.subspan(0u);

    std::vector<std::string> names;
    for (auto&& row : span)
        names.push_back(row.column<rich::columns::name>());
    REQUIRE(names.size() == 3u);
    CHECK(names[0] == "name 0");
    CHECK(names[2] == "name 2");

    CHECK(span.row<3, 1, 0>(1).salary == 1000);
    CHECK(span.front<3, 1, 0>().name == "name 0");
    CHECK(span.back<3, 1, 0>().name == "name 2");
}

TEST_CASE("spans - equality and ordering (span vs span)", "[spans]")
{
    auto a = make_rich(3);
    auto b = make_rich(3);
    auto c = make_rich(2);

    CHECK(a.subspan(0u) == b.subspan(0u));
    CHECK(a.subspan(0u) != c.subspan(0u)); // different length
    CHECK(c.subspan(0u) < a.subspan(0u));  // shared prefix, shorter is less
    CHECK(c.subspan(0u) <= a.subspan(0u));
    CHECK(a.subspan(0u) > c.subspan(0u));
    CHECK(a.subspan(0u) >= c.subspan(0u));
    CHECK(a.subspan(0u, 2u) == c.subspan(0u)); // equal content over the shared prefix
    CHECK_FALSE(a.subspan(0u) < b.subspan(0u));

    // mutable span vs const span over the same data
    CHECK(a.subspan(0u) == a.const_subspan(0u));
}

TEST_CASE("spans - heterogeneous comparison (table vs span)", "[spans]")
{
    auto a = make_rich(3);
    auto c = make_rich(2);

    CHECK(a == a.subspan(0u)); // whole-table span compares equal to the table
    CHECK(a.subspan(0u) == a);
    CHECK(a != a.subspan(0u, 2u)); // table has more rows than the 2-row span
    CHECK(a.subspan(0u, 2u) < a);
    CHECK(a > c.subspan(0u)); // table vs shorter span
    CHECK(c.subspan(0u) == a.subspan(0u, 2u));
}

TEST_CASE("spans - empty", "[spans]")
{
    // default-constructed span is empty
    rich::span_type def{};
    CHECK(def.empty());
    CHECK(def.size() == 0u);

    // a zero-count subspan of a non-empty table is also empty (start must be < size; see span.hpp precondition)
    auto r    = make_rich(3);
    auto span = r.subspan(1u, 0u);
    CHECK(span.empty());
    CHECK(span.size() == 0u);
    CHECK(span.begin() == span.end());
}

TEST_CASE("spans - rvalue subspan instantiates and source() round-trips", "[spans]")
{
    auto r          = make_rich(3);
    const auto* src = &r;
    auto span       = std::move(r).subspan(0u, 2u);
    CHECK(span.size() == 2u);
    CHECK(span.source() == src);
    static_assert(std::is_same_v<decltype(span.source()), rich*>);
    static_assert(std::is_same_v<decltype(std::declval<soagen::const_span_type<rich>>().source()), const rich*>);
}
