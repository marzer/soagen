// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "tests.hpp"
#include "soa.hpp"

SOAGEN_DISABLE_ARITHMETIC_WARNINGS;

namespace tests
{
    inline rich make_rich(std::size_t count)
    {
        rich r;
        for (std::size_t i = 0; i < count; i++)
            r.push_back("name " + std::to_string(i),
                        static_cast<unsigned long long>(i),
                        { 1970 + static_cast<int>(i), 1, 1 },
                        static_cast<int>(i) * 1000,
                        nullptr);
        return r;
    }

    inline trivial make_trivial(std::size_t count)
    {
        trivial t;
        for (std::size_t i = 0; i < count; i++)
        {
            const auto f = static_cast<float>(i);
            t.push_back(f, f + 1.0f, f + 2.0f, static_cast<unsigned>(i));
        }
        return t;
    }

    inline move_only make_move_only(std::size_t count)
    {
        move_only m;
        for (std::size_t i = 0; i < count; i++)
            m.emplace_back(std::make_unique<int>(static_cast<int>(i)), i);
        return m;
    }

    inline fragile make_fragile(std::size_t count)
    {
        fragile f;
        f.reserve(count);
        for (std::size_t i = 0; i < count; i++)
            f.push_back(throwing{ static_cast<int>(i) }, static_cast<int>(i) * 10);
        return f;
    }

    template <typename Soa>
    Soa make_soa(std::size_t count);
    template <>
    inline rich make_soa<rich>(std::size_t count)
    {
        return make_rich(count);
    }
    template <>
    inline trivial make_soa<trivial>(std::size_t count)
    {
        return make_trivial(count);
    }
    template <>
    inline move_only make_soa<move_only>(std::size_t count)
    {
        return make_move_only(count);
    }
}

#define CHECK_RICH_ROW(row, Name, Id, Date, Salary, Tag)                                                               \
    CHECK((row).template column<rich::columns::name>() == (Name));                                                     \
    CHECK((row).template column<rich::columns::id>() == static_cast<unsigned long long>(Id));                          \
    CHECK((row).template column<rich::columns::date_of_birth>() == std::tuple Date);                                   \
    CHECK((row).template column<rich::columns::salary>() == static_cast<int>(Salary));                                 \
    CHECK((row).template column<rich::columns::tag>() == (Tag))
