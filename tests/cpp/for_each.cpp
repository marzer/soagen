// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"

using namespace tests;

// soagen::for_each_column resolves to one of five callable signatures (see for_each.hpp), tried in order.

TEST_CASE("for_each - (ptr, integral_constant)", "[for_each]")
{
    auto r           = make_rich(1);
    std::size_t sum  = 0;
    bool index_is_ic = false;
    soagen::for_each_column(r,
                            [&](auto* ptr, auto ic)
                            {
                                index_is_ic = !std::is_same_v<decltype(ic), std::size_t>;
                                sum += decltype(ic)::value;
                                CHECK(ptr != nullptr);
                            });
    CHECK(index_is_ic);
    CHECK(sum == (0u + 1u + 2u + 3u + 4u));
}

TEST_CASE("for_each - (ptr, size_t)", "[for_each]")
{
    auto r          = make_rich(1);
    std::size_t sum = 0;
    soagen::for_each_column(r,
                            [&](auto* ptr, std::size_t idx)
                            {
                                sum += idx;
                                CHECK(ptr != nullptr);
                            });
    CHECK(sum == (0u + 1u + 2u + 3u + 4u));
}

TEST_CASE("for_each - (size_t, ptr)", "[for_each]")
{
    auto r          = make_rich(1);
    std::size_t sum = 0;
    soagen::for_each_column(r,
                            [&](std::size_t idx, auto* ptr)
                            {
                                sum += idx;
                                CHECK(ptr != nullptr);
                            });
    CHECK(sum == (0u + 1u + 2u + 3u + 4u));
}

TEST_CASE("for_each - (integral_constant, ptr)", "[for_each]")
{
    auto r          = make_rich(1);
    std::size_t sum = 0;
    soagen::for_each_column(r,
                            [&](auto ic, auto* ptr)
                            {
                                sum += decltype(ic)::value;
                                CHECK(ptr != nullptr);
                            });
    CHECK(sum == (0u + 1u + 2u + 3u + 4u));
}

TEST_CASE("for_each - (ptr) alone", "[for_each]")
{
    auto r            = make_rich(1);
    std::size_t count = 0;
    soagen::for_each_column(r, [&](auto* ptr) { count += (ptr != nullptr); });
    CHECK(count == rich::column_count);
}

TEST_CASE("for_each - member form matches free function", "[for_each]")
{
    auto r            = make_rich(1);
    std::size_t count = 0;
    r.for_each_column([&](auto*) { count++; });
    CHECK(count == rich::column_count);
}
