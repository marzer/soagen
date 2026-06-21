//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "identify_compiler.hpp"

#if defined(__MMX__) || SOAGEN_MSVC
    #define SOAGEN_ISET_MMX 1
#else
    #define SOAGEN_ISET_MMX 0
#endif
/// @def SOAGEN_ISET_MMX
/// @brief `1` when the target supports the MMX instruction set, otherwise `0`.

#if defined(__SSE__) || (SOAGEN_MSVC && (defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)))
    #define SOAGEN_ISET_SSE 1
#else
    #define SOAGEN_ISET_SSE 0
#endif
/// @def SOAGEN_ISET_SSE
/// @brief `1` when the target supports the SSE instruction set, otherwise `0`.

#if defined(__SSE2__) || (SOAGEN_MSVC && (defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)))
    #define SOAGEN_ISET_SSE2 1
#else
    #define SOAGEN_ISET_SSE2 0
#endif
/// @def SOAGEN_ISET_SSE2
/// @brief `1` when the target supports the SSE2 instruction set, otherwise `0`.

#ifdef __AVX__
    #define SOAGEN_ISET_AVX 1
#else
    #define SOAGEN_ISET_AVX 0
#endif
/// @def SOAGEN_ISET_AVX
/// @brief `1` when the target supports the AVX instruction set, otherwise `0`.

#ifdef __AVX2__
    #define SOAGEN_ISET_AVX2 1
#else
    #define SOAGEN_ISET_AVX2 0
#endif
/// @def SOAGEN_ISET_AVX2
/// @brief `1` when the target supports the AVX2 instruction set, otherwise `0`.

#if defined(__AVX512BW__) || defined(__AVX512CD__) || defined(__AVX512DQ__) || defined(__AVX512ER__)                   \
    || defined(__AVX512F__) || defined(__AVX512PF__) || defined(__AVX512VL__)
    #define SOAGEN_ISET_AVX512 1
#else
    #define SOAGEN_ISET_AVX512 0
#endif
/// @def SOAGEN_ISET_AVX512
/// @brief `1` when the target supports any of the AVX512 instruction sets, otherwise `0`.
