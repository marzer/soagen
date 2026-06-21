// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"

using namespace tests;

TEST_CASE("rows - equality", "[rows]")
{
    auto r = make_rich(2); // ids 0, 1

    CHECK(r[0] == r[0]);
    CHECK(r[0] != r[1]);
    CHECK_FALSE(r[0] == r[1]);

    // cross-cv comparisons
    CHECK(r[1] == std::as_const(r)[1]);
    CHECK(r[1] == static_cast<rich::rvalue_row_type>(r[1]));
    CHECK(r[1] == std::move(std::as_const(r))[1]);
    CHECK(r[1] != std::as_const(r)[0]);
}

TEST_CASE("rows - ordering is lexicographic over columns", "[rows]")
{
    auto r = make_rich(2); // row 0 < row 1 (name "name 0" < "name 1")

    CHECK(r[0] < r[1]);
    CHECK(r[0] <= r[1]);
    CHECK(r[1] > r[0]);
    CHECK(r[1] >= r[0]);
    CHECK(r[0] <= r[0]);
    CHECK(r[0] >= r[0]);
    CHECK_FALSE(r[0] < r[0]);
    CHECK_FALSE(r[0] > r[0]);
}

TEST_CASE("rows - implicit conversion to const / rvalue", "[rows]")
{
    auto r = make_rich(1);

    rich::row_type mutable_row       = r[0];
    rich::const_row_type const_row   = mutable_row; // lvalue -> const
    rich::rvalue_row_type rvalue_row = std::move(r)[0];
    [[maybe_unused]]
    rich::const_row_type const_from_rvalue = rvalue_row; // rvalue -> const

    CHECK(const_row.name == "name 0");
    CHECK(rvalue_row.column<rich::columns::id>() == 0u);

    static_assert(is_implicitly_convertible<rich::row_type, rich::const_row_type>);
    static_assert(is_implicitly_convertible<rich::rvalue_row_type, rich::const_row_type>);
}

TEST_CASE("rows - for_each_column", "[rows]")
{
    auto r   = make_rich(1);
    auto row = r[0];

    std::size_t visited = 0;
    row.for_each_column([&](auto&&, std::size_t) { visited++; });
    CHECK(visited == rich::column_count);

    // index passed as integral_constant overload
    std::size_t visited_ic = 0;
    row.for_each_column([&](auto&&, auto ic) { visited_ic += decltype(ic)::value < rich::column_count ? 1 : 0; });
    CHECK(visited_ic == rich::column_count);
}
