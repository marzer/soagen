// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "employees.hpp"

using namespace soagen::examples;

int main()
{
    employees db;
    db.reserve(4);

    db.push_back("Alice", 1001, { 1990, 1, 2 }, 95000, nullptr);
    db.emplace_back("Bob", 1002, std::tuple{ 1985, 6, 15 }, 120000, nullptr);
    db.insert(0, "Carol", 1003, { 1979, 11, 30 }, 150000, nullptr);

    int total = 0;
    for (auto&& row : db)
        total += row.salary;

    db.pop_back();
    db.resize(1);

    [[maybe_unused]]
    auto first = db[0];

    return total != 0 ? 0 : 1;
}
