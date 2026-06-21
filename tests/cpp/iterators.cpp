// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"

using namespace tests;

TEST_CASE("iterators - ranged for + dereference + arrow", "[iterators]")
{
    auto r = make_rich(4);

    std::vector<std::string> names;
    for (auto&& row : r)
        names.push_back(row.name);
    REQUIRE(names.size() == 4u);
    CHECK(names[0] == "name 0");
    CHECK(names[3] == "name 3");

    auto it = r.begin();
    CHECK((*it).name == "name 0");
    CHECK(it->name == "name 0");
}

TEST_CASE("iterators - increment / decrement", "[iterators]")
{
    auto r  = make_rich(3);
    auto it = r.begin();
    CHECK(it->id == 0u);
    ++it;
    CHECK(it->id == 1u);
    it++;
    CHECK(it->id == 2u);
    ++it;
    CHECK(it == r.end());
    --it;
    CHECK(it->id == 2u);
    it--;
    CHECK(it->id == 1u);
    CHECK(it == r.begin() + 1);
}

TEST_CASE("iterators - random access arithmetic", "[iterators]")
{
    auto r  = make_rich(5);
    auto it = r.begin();
    it += 3;
    CHECK(it->id == 3u);
    it -= 2;
    CHECK(it->id == 1u);
    CHECK((it + 2)->id == 3u);
    CHECK((it - 1)->id == 0u);
    CHECK((r.end() - r.begin()) == 5);
    CHECK(it - r.begin() == 1);
    CHECK(it[2].id == 3u); // subscript relative to iterator
}

TEST_CASE("iterators - comparison", "[iterators]")
{
    auto r = make_rich(3);
    auto a = r.begin();
    auto b = r.begin() + 1;
    CHECK(a == r.begin());
    CHECK(a != b);
    CHECK(a < b);
    CHECK(a <= b);
    CHECK(b > a);
    CHECK(b >= a);
    CHECK_FALSE(a > b);
}

TEST_CASE("iterators - const + cbegin/cend", "[iterators]")
{
    auto r  = make_rich(2);
    auto it = r.cbegin();
    static_assert(soagen::is_iterator<decltype(it)>);
    CHECK(it->name == "name 0");
    CHECK(r.cend() - r.cbegin() == 2);

    std::size_t n = 0;
    for (auto it2 = std::as_const(r).begin(); it2 != std::as_const(r).end(); ++it2)
        n++;
    CHECK(n == 2u);
}

TEST_CASE("iterators - underlying table iteration", "[iterators]")
{
    auto r = make_rich(2);
    std::vector<unsigned long long> ids;
    for (auto&& row : r.table())
        ids.push_back(row.column<rich::columns::id>());
    REQUIRE(ids.size() == 2u);
    CHECK(ids[0] == 0u);
    CHECK(ids[1] == 1u);
}
