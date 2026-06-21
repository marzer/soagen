//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "warnings.hpp"
SOAGEN_DISABLE_WARNINGS;

// clang-tidier defines __clang_tidy__ so we can enforce harder lifetime analysis on assertion failure

#ifndef SOAGEN_ASSERT
    #ifdef __clang_tidy__
        #include <cstdlib>
        #define SOAGEN_ASSERT(...)                                                                                     \
            do                                                                                                         \
            {                                                                                                          \
                SOAGEN_DISABLE_WARNINGS;                                                                               \
                if (!(__VA_ARGS__)) /* NOLINT */                                                                       \
                    std::abort();   /* NOLINT */                                                                       \
                SOAGEN_ENABLE_WARNINGS;                                                                                \
            }                                                                                                          \
            while (false)
    #elif defined(NDEBUG)
        #define SOAGEN_ASSERT(...) static_cast<void>(0)
    #else
        #ifndef assert
            #include <cassert>
        #endif
        #define SOAGEN_ASSERT(...) assert(__VA_ARGS__)
    #endif
#endif

SOAGEN_ENABLE_WARNINGS;
