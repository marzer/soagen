// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"

using namespace tests;

TEST_CASE("access - column pointers", "[access]")
{
    auto r = make_rich(3);
    REQUIRE(r.name() != nullptr);
    CHECK(r.name()[0] == "name 0");
    CHECK(r.id()[1] == 1u);
    CHECK(r.date_of_birth()[2] == std::tuple{ 1972, 1, 1 });

    // by enum and by index, const and non-const
    CHECK(r.column<rich::columns::name>()[0] == "name 0");
    CHECK(r.column<0>()[0] == "name 0");
    CHECK(std::as_const(r).column<0>()[0] == "name 0");
    CHECK(std::as_const(r).name()[0] == "name 0");
}

TEST_CASE("access - front / back / operator[] / row / at", "[access]")
{
    auto r = make_rich(3);

    CHECK_RICH_ROW(r.front(), "name 0", 0, (1970, 1, 1), 0, nullptr);
    CHECK_RICH_ROW(r.back(), "name 2", 2, (1972, 1, 1), 2000, nullptr);
    CHECK_RICH_ROW(r[1], "name 1", 1, (1971, 1, 1), 1000, nullptr);
    CHECK_RICH_ROW(r.row(1), "name 1", 1, (1971, 1, 1), 1000, nullptr);
    CHECK_RICH_ROW(r.at(1), "name 1", 1, (1971, 1, 1), 1000, nullptr);

    // const variants
    CHECK_RICH_ROW(std::as_const(r).front(), "name 0", 0, (1970, 1, 1), 0, nullptr);
    CHECK_RICH_ROW(std::as_const(r)[2], "name 2", 2, (1972, 1, 1), 2000, nullptr);

    // rvalue variant
    CHECK(std::move(r)[0].column<rich::columns::name>() == "name 0");
}

TEST_CASE("access - at() bounds checking throws", "[access]")
{
    auto r = make_rich(2);
    CHECK_THROWS_AS(r.at(2), std::out_of_range);
    CHECK_NOTHROW(r.at(1));
}

TEST_CASE("access - row member names and tuple protocol", "[access]")
{
    auto r   = make_rich(1);
    auto row = r[0];
    CHECK(row.name == "name 0");
    CHECK(row.id == 0u);
    CHECK(row.salary == 0);
    CHECK(row.get<0>() == "name 0");
    CHECK(row.get<1>() == 0u);
    CHECK(row.get<rich::columns::name>() == "name 0"); // enum index also accepted
    CHECK(row.get<rich::columns::id>() == 0u);
}

TEST_CASE("access - column swizzles (3,1,0 == salary,id,name)", "[access]")
{
    auto r = make_rich(3); // ids 0,1,2

    CHECK(r.row<3, 1, 0>(1).salary == 1000);
    CHECK(r.row<3, 1, 0>(1).get<0>() == 1000);
    CHECK(r.row<3, 1, 0>(1).column<3>() == 1000);
    CHECK(r.row<3, 1, 0>(1).id == 1u);
    CHECK(r.row<3, 1, 0>(1).name == "name 1");

    CHECK(r.at<3, 1, 0>(2).salary == 2000);
    CHECK(r.front<3, 1, 0>().name == "name 0");
    CHECK(r.back<3, 1, 0>().name == "name 2");
}

TEST_CASE("access - trivial data() byte pointer", "[access]")
{
    auto t          = make_trivial(4);
    std::byte* data = t.data();
    CHECK(data != nullptr);
    CHECK(std::as_const(t).data() != nullptr);
    CHECK(reinterpret_cast<std::byte*>(t.x()) == data); // first column lives at the start of the allocation
}
