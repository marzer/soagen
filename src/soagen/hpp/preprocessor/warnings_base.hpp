//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "identify_compiler.hpp"
#include "identify_features.hpp"

#ifndef SOAGEN_MAKE_STRING
    #define SOAGEN_MAKE_STRING_2(s) #s
    #define SOAGEN_MAKE_STRING_1(s) SOAGEN_MAKE_STRING_2(s)
    #define SOAGEN_MAKE_STRING(s)   SOAGEN_MAKE_STRING_1(s)
#endif

#ifndef SOAGEN_PUSH_WARNINGS
    #if SOAGEN_CLANG
        #define SOAGEN_PUSH_WARNINGS                                                                                   \
            _Pragma("clang diagnostic push")                                                                           \
            static_assert(true)
    #elif SOAGEN_MSVC
        #define SOAGEN_PUSH_WARNINGS                                                                                   \
            __pragma(warning(push))                                                                                    \
            static_assert(true)
    #elif SOAGEN_GCC
        #define SOAGEN_PUSH_WARNINGS                                                                                   \
            _Pragma("GCC diagnostic push")                                                                             \
            static_assert(true)
    #else
        #define SOAGEN_PUSH_WARNINGS static_assert(true)
    #endif
#endif

#ifndef SOAGEN_POP_WARNINGS
    #if SOAGEN_CLANG
        #define SOAGEN_POP_WARNINGS                                                                                    \
            _Pragma("clang diagnostic pop")                                                                            \
            static_assert(true)
    #elif SOAGEN_MSVC
        #define SOAGEN_POP_WARNINGS                                                                                    \
            __pragma(warning(pop))                                                                                     \
            static_assert(true)
    #elif SOAGEN_GCC
        #define SOAGEN_POP_WARNINGS                                                                                    \
            _Pragma("GCC diagnostic pop")                                                                              \
            static_assert(true)
    #else
        #define SOAGEN_POP_WARNINGS static_assert(true)
    #endif
#endif

#ifndef SOAGEN_DISABLE_GCC_WARNING
    #if SOAGEN_GCC
        #define SOAGEN_DISABLE_GCC_WARNING(warning)                                                                    \
            _Pragma(SOAGEN_MAKE_STRING(GCC diagnostic ignored warning))                                                \
            static_assert(true)
    #else
        #define SOAGEN_DISABLE_GCC_WARNING(...) static_assert(true)
    #endif
#endif

#ifndef SOAGEN_DISABLE_CLANG_WARNING
    #if SOAGEN_CLANG
        #define SOAGEN_DISABLE_CLANG_WARNING(warning)                                                                  \
            _Pragma(SOAGEN_MAKE_STRING(clang diagnostic ignored warning))                                              \
            static_assert(true)
    #else
        #define SOAGEN_DISABLE_CLANG_WARNING(...) static_assert(true)
    #endif
#endif

#ifndef SOAGEN_DISABLE_MSVC_WARNING
    #if SOAGEN_MSVC
        #define SOAGEN_DISABLE_MSVC_WARNING(warning_number)                                                            \
            __pragma(warning(disable : warning_number))                                                                \
            static_assert(true)
    #else
        #define SOAGEN_DISABLE_MSVC_WARNING(...) static_assert(true)
    #endif
#endif
