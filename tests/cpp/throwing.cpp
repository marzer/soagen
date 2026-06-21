// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"

using namespace tests;

// drives the table's exception-rollback paths; leak checks are baseline-relative because throwing::live is global
// across tests.

namespace
{
    struct disarm_guard
    {
        disarm_guard() noexcept
        {
            throwing::disarm();
        }
        ~disarm_guard() noexcept
        {
            throwing::disarm();
        }
    };
}

TEST_CASE("throwing - strong guarantee: reallocating push_back", "[throwing]")
{
    disarm_guard g;
    const int base = throwing::live;
    {
        auto f = make_fragile(4); // size 4, capacity 4 -> next push_back reallocates
        REQUIRE(f.size() == 4u);
        REQUIRE(throwing::live == base + 4);

        throwing::arm(2); // throw partway through copying the existing rows into the new buffer
        CHECK_THROWS_AS(f.push_back(throwing{ 99 }, 990), std::runtime_error);
        throwing::disarm();

        CHECK(f.size() == 4u);             // unchanged
        CHECK(throwing::live == base + 4); // partial new buffer rolled back, no leak
        for (int i = 0; i < 4; i++)
        {
            CHECK(f.v()[i].value == i); // original rows intact
            CHECK(f.tag()[i] == i * 10);
        }
    }
    CHECK(throwing::live == base);
}

TEST_CASE("throwing - strong guarantee: reallocating resize", "[throwing]")
{
    disarm_guard g;
    const int base = throwing::live;
    {
        auto f = make_fragile(3); // capacity 3 -> resize(20) reallocates and copies the existing rows

        throwing::arm(2);
        CHECK_THROWS_AS(f.resize(20), std::runtime_error);
        throwing::disarm();

        CHECK(f.size() == 3u);
        CHECK(throwing::live == base + 3);
        for (int i = 0; i < 3; i++)
            CHECK(f.v()[i].value == i);
    }
    CHECK(throwing::live == base);
}

TEST_CASE("throwing - copy construction unwinds cleanly", "[throwing]")
{
    disarm_guard g;
    const int base = throwing::live;
    {
        auto src = make_fragile(5);

        throwing::arm(3);
        CHECK_THROWS_AS(fragile{ src }, std::runtime_error);
        throwing::disarm();

        CHECK(throwing::live == base + 5); // the partially-built copy is fully destroyed
        CHECK(src.size() == 5u);           // source untouched
        for (int i = 0; i < 5; i++)
            CHECK(src.v()[i].value == i);
    }
    CHECK(throwing::live == base);
}

TEST_CASE("throwing - copy assignment leaves a valid container", "[throwing]")
{
    disarm_guard g;
    const int base = throwing::live;
    {
        auto src    = make_fragile(5);
        fragile dst = make_fragile(2);

        throwing::arm(3);
        CHECK_THROWS_AS(dst = src, std::runtime_error);
        throwing::disarm();

        CHECK(src.size() == 5u); // source untouched
        // dst is left in a valid (if unspecified) state; it must still be usable
        dst.clear();
        dst.push_back(throwing{ 1 }, 1);
        CHECK(dst.size() == 1u);
    }
    CHECK(throwing::live == base); // no leak regardless of where the throw landed
}

TEST_CASE("throwing - insert with throwing shift leaves a valid container", "[throwing]")
{
    disarm_guard g;
    const int base = throwing::live;
    {
        auto f = make_fragile(5);
        f.reserve(32); // avoid reallocation so the in-place shift path is exercised

        throwing::arm(1);
        CHECK_THROWS_AS(f.insert(2u, throwing{ 99 }, 990), std::runtime_error);
        throwing::disarm();

        // basic guarantee: no leak/hole; the element may or may not have been added, but the container stays usable.
        const auto sz = f.size();
        CHECK((sz == 5u || sz == 6u));
        f.clear();
        f.push_back(throwing{ 7 }, 7);
        CHECK(f.size() == 1u);
    }
    CHECK(throwing::live == base); // no leak
}

TEST_CASE("throwing - copy construction rolls back a partially-built multi-column row", "[throwing]")
{
    disarm_guard g;
    const int base = throwing::live;
    {
        fragile2 src;
        src.push_back(throwing{ 1 }, throwing{ 2 });
        src.push_back(throwing{ 3 }, throwing{ 4 });
        REQUIRE(src.size() == 2u);
        REQUIRE(throwing::live == base + 4);

        throwing::arm(3); // copies a0,b0,a1 then b1 throws: the already-built column a of the row must be destroyed
        CHECK_THROWS_AS(fragile2{ src }, std::runtime_error);
        throwing::disarm();

        CHECK(src.size() == 2u);
        CHECK(src.a()[1].value == 3); // source untouched
        CHECK(throwing::live == base + 4);
    }
    CHECK(throwing::live == base);
}

TEST_CASE("throwing - reallocating push_back rolls back a partially-built multi-column row", "[throwing]")
{
    disarm_guard g;
    const int base = throwing::live;
    {
        fragile2 f;
        f.reserve(2);
        f.push_back(throwing{ 1 }, throwing{ 2 });
        f.push_back(throwing{ 3 }, throwing{ 4 }); // size 2 == capacity -> next push_back reallocates + relocates
        REQUIRE(f.size() == 2u);

        throwing::arm(3); // relocate a0,b0,a1 then b1 throws mid-row
        CHECK_THROWS_AS(f.push_back(throwing{ 5 }, throwing{ 6 }), std::runtime_error);
        throwing::disarm();

        CHECK(f.size() == 2u); // strong guarantee
        CHECK(f.a()[0].value == 1);
        CHECK(f.b()[1].value == 4);
        CHECK(throwing::live == base + 4); // 2 rows x 2 throwing columns; the partial new buffer rolled back
    }
    CHECK(throwing::live == base);
}
