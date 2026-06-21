//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT

//# note: any preprocessor/compiler state modified here must be undone in header_end.hpp

#include "preprocessor.hpp"

// __SOAGEN_HEADER_START{{

// push current states
SOAGEN_PUSH_WARNINGS; // NOLINT(fsw-bugprone-unbalanced-pragmas): intentional
SOAGEN_DISABLE_SPAM_WARNINGS;
SOAGEN_DISABLE_SHADOW_WARNINGS;
SOAGEN_DISABLE_GCC_WARNING("-Wc++14-extensions");
SOAGEN_DISABLE_GCC_WARNING_GE(12, "-Wnonnull-compare");
SOAGEN_DISABLE_CLANG_WARNING("-Wc++14-extensions");
SOAGEN_DISABLE_CLANG_WARNING("-Wc++17-extensions");
SOAGEN_DISABLE_CLANG_WARNING("-Wredundant-consteval-if");
SOAGEN_DISABLE_CLANG_WARNING("-Wnested-anon-types");
SOAGEN_DISABLE_MSVC_WARNING(4201); // nameless struct/union
SOAGEN_DISABLE_MSVC_WARNING(4984); // if constexpr language extension
#if SOAGEN_GCC
    #pragma GCC push_options
#endif
#if SOAGEN_MSVC_LIKE || SOAGEN_GCC_LIKE
    #pragma push_macro("min")
    #pragma push_macro("max")
    #pragma push_macro("finite")
    #undef min
    #undef max
    #undef finite
#endif

// set optimizations
#ifndef SOAGEN_ALWAYS_OPTIMIZE
    #define SOAGEN_ALWAYS_OPTIMIZE 1
#endif
#if SOAGEN_ALWAYS_OPTIMIZE
    #if SOAGEN_MSVC_LIKE
        #pragma inline_recursion(on)
        #if !SOAGEN_CLANG
            #pragma optimize("gt", on)
        #endif
        #pragma runtime_checks("", off)
        #pragma strict_gs_check(push, off)
        #pragma float_control(precise, off, push)
        #pragma float_control(except, off)
    #elif SOAGEN_GCC
        #if (!defined(__OPTIMIZE__) || !defined(NDEBUG))
            #pragma GCC optimize("Os")
        #endif
    #endif
#endif

// }}__SOAGEN_HEADER_START
