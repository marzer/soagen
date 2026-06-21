// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

// exercises the strong-exception-guarantee rollback machinery with exceptions disabled (see the -Dcpp_eh=none
// CI job). the non-trivially-copyable columns force the copy/move-based construction paths that wrap their
// forward progress in try/catch when exceptions are available; those must still compile when they aren't.

#include "soa.hpp"
#include <cassert>
#include <memory>

int main()
{
    tests::rich r;
    r.reserve(4);
    r.push_back("Alice", 1, { 1990, 1, 1 }, 100000, nullptr);
    r.push_back("Bob", 2, { 1980, 2, 2 }, 90000, nullptr);
    r.emplace(0u, "Carol", 3, std::tuple{ 1970, 3, 3 }, 80000, nullptr);
    r.resize(8);
    r.resize(2);
    r.unordered_erase(0u);

    tests::move_only m;
    m.emplace_back(std::make_unique<int>(1), std::size_t{ 0 });
    m.emplace_back(std::make_unique<int>(2), std::size_t{ 1 });

    assert(r.size() == 1u);
    assert(m.size() == 2u);
    return (r.size() + m.size()) == 3u ? 0 : 1;
}
