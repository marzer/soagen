// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include <cstdlib>
#include <stdexcept>

namespace tests
{
    // a value type whose copy ops throw on demand; not nothrow-movable, so soagen takes the copy-based rollback paths.
    struct throwing
    {
        // -1 == disarmed; otherwise throw once the countdown reaches zero during a copy
        static inline int countdown = -1;
        // number of live instances, for leak detection across a thrown operation
        static inline int live = 0;

        static void arm(int after_n_copies) noexcept
        {
            countdown = after_n_copies;
        }
        static void disarm() noexcept
        {
            countdown = -1;
        }
        static void maybe_throw()
        {
            if (countdown < 0)
                return;
            if (countdown-- == 0)
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
                throw std::runtime_error("throwing: boom");
#else
                std::abort();
#endif
        }

        int value = 0;

        throwing() noexcept
        {
            live++;
        }
        throwing(int v) noexcept : value{ v }
        {
            live++;
        }
        throwing(const throwing& other) : value{ other.value }
        {
            maybe_throw(); // before live++, so a thrown copy doesn't register a live instance
            live++;
        }
        throwing& operator=(const throwing& other)
        {
            maybe_throw();
            value = other.value;
            return *this;
        }
        ~throwing() noexcept
        {
            live--;
        }

        bool operator==(const throwing& other) const noexcept
        {
            return value == other.value;
        }
        bool operator<(const throwing& other) const noexcept
        {
            return value < other.value;
        }
    };
}
