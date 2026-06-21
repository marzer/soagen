// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "soa.hpp"

int main()
{
    tests::rich r;
    r.reserve(4);
    r.push_back("Alice", 1, { 1990, 1, 1 }, 100000, nullptr);

    tests::trivial t;
    t.resize(4);

    tests::move_only m;
    m.emplace_back(std::make_unique<int>(1), std::size_t{ 0 });

    return (r.size() + t.size() + m.size()) == 6u ? 0 : 1;
}
