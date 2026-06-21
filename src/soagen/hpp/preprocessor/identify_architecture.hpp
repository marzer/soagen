//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "identify_compiler.hpp"

#ifndef SOAGEN_ARCH_ITANIUM
    #if defined(__ia64__) || defined(__ia64) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64)
        #define SOAGEN_ARCH_ITANIUM 1
        #define SOAGEN_ARCH_BITNESS 64
    #else
        #define SOAGEN_ARCH_ITANIUM 0
    #endif
#endif
/// @def SOAGEN_ARCH_ITANIUM
/// @brief `1` when targeting 64-bit Itanium, otherwise `0`.

#ifndef SOAGEN_ARCH_AMD64
    #if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64)
        #define SOAGEN_ARCH_AMD64   1
        #define SOAGEN_ARCH_BITNESS 64
    #else
        #define SOAGEN_ARCH_AMD64 0
    #endif
#endif
/// @def SOAGEN_ARCH_AMD64
/// @brief `1` when targeting AMD64, otherwise `0`.

#ifndef SOAGEN_ARCH_X86
    #if defined(__i386__) || defined(_M_IX86)
        #define SOAGEN_ARCH_X86     1
        #define SOAGEN_ARCH_BITNESS 32
    #else
        #define SOAGEN_ARCH_X86 0
    #endif
#endif
/// @def SOAGEN_ARCH_X86
/// @brief `1` when targeting 32-bit x86, otherwise `0`.

#ifndef SOAGEN_ARCH_ARM
    #if defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64) || defined(_M_ARM64) || defined(__ARM_64BIT_STATE)         \
        || defined(_M_ARM64EC)
        #define SOAGEN_ARCH_ARM32   0
        #define SOAGEN_ARCH_ARM64   1
        #define SOAGEN_ARCH_ARM     1
        #define SOAGEN_ARCH_BITNESS 64
    #elif defined(__arm__) || defined(_M_ARM) || defined(__ARM_32BIT_STATE)
        #define SOAGEN_ARCH_ARM32   1
        #define SOAGEN_ARCH_ARM64   0
        #define SOAGEN_ARCH_ARM     1
        #define SOAGEN_ARCH_BITNESS 32
    #else
        #define SOAGEN_ARCH_ARM32 0
        #define SOAGEN_ARCH_ARM64 0
        #define SOAGEN_ARCH_ARM   0
    #endif
#endif
/// @def SOAGEN_ARCH_ARM32
/// @brief `1` when targeting 32-bit ARM, otherwise `0`.
///
/// @def SOAGEN_ARCH_ARM64
/// @brief `1` when targeting 64-bit ARM, otherwise `0`.
///
/// @def SOAGEN_ARCH_ARM
/// @brief `1` when targeting any flavour of ARM, otherwise `0`.

#ifndef SOAGEN_ARCH_BITNESS
    #define SOAGEN_ARCH_BITNESS 0
#endif
/// @def SOAGEN_ARCH_BITNESS
/// @brief The 'bitness' of the target architecture (e.g. `64` on AMD64).
/// @see build::bitness

#ifndef SOAGEN_ARCH_X64
    #if SOAGEN_ARCH_BITNESS == 64
        #define SOAGEN_ARCH_X64 1
    #else
        #define SOAGEN_ARCH_X64 0
    #endif
#endif
/// @def SOAGEN_ARCH_X64
/// @brief `1` when targeting any 64-bit architecture, otherwise `0`.

#ifndef SOAGEN_LITTLE_ENDIAN
    #if SOAGEN_DOXYGEN
        #define SOAGEN_LITTLE_ENDIAN 1
        #define SOAGEN_BIG_ENDIAN    0
    #else
        #ifdef SOAGEN_LITTLE_ENDIAN_INTS // cmake feature tests
            #define SOAGEN_LITTLE_ENDIAN (!!SOAGEN_LITTLE_ENDIAN_INTS)
            #define SOAGEN_BIG_ENDIAN    (!!SOAGEN_BIG_ENDIAN_INTS)
            #ifdef SOAGEN_LITTLE_ENDIAN_FLOATS
                #if !!SOAGEN_LITTLE_ENDIAN_FLOATS != SOAGEN_LITTLE_ENDIAN_INTS
                    #error Mixed float and int endianness is not supported.
                #endif
            #endif
        #elif SOAGEN_MSVC_LIKE
            #define SOAGEN_LITTLE_ENDIAN 1
            #define SOAGEN_BIG_ENDIAN    0
        #elif defined(__BYTE_ORDER__)
            #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                #define SOAGEN_LITTLE_ENDIAN 1
                #define SOAGEN_BIG_ENDIAN    0
            #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                #define SOAGEN_LITTLE_ENDIAN 0
                #define SOAGEN_BIG_ENDIAN    1
            #endif
        #else
            #error Unknown platform endianness.
        #endif
    #endif
#endif
#if !(SOAGEN_LITTLE_ENDIAN ^ SOAGEN_BIG_ENDIAN)
    #error Could not uniquely identify platform endianness.
#endif
/// @def SOAGEN_LITTLE_ENDIAN
/// @brief `1` when the target environment is little-endian, otherwise `0`.
/// @see build::is_little_endian
///
/// @def SOAGEN_BIG_ENDIAN
/// @brief `1` when the target environment is big-endian, otherwise `0`.
/// @see build::is_big_endian
