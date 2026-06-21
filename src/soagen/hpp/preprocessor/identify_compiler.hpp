//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#ifndef SOAGEN_MAKE_VERSION
    #define SOAGEN_MAKE_VERSION(major, minor, patch) (((major) * 1000000) + ((minor) * 1000) + ((patch)))
#endif
/// @def SOAGEN_MAKE_VERSION(major, minor, patch)
/// @brief Turns a version triplet into an integer (e.g. for version comparison purposes).

#ifndef SOAGEN_CLANG
    #ifdef __clang__
        #define SOAGEN_CLANG __clang_major__
    #else
        #define SOAGEN_CLANG 0
    #endif

    // special handling for apple clang; see:
    // - github.com/marzer/tomlplusplus/issues/189
    // - en.wikipedia.org/wiki/Xcode
    // - stackoverflow.com/questions/19387043/how-can-i-reliably-detect-the-version-of-clang-at-preprocessing-time
    #if SOAGEN_CLANG && defined(__apple_build_version__)
        #undef SOAGEN_CLANG
        #define SOAGEN_CLANG_VERSION SOAGEN_MAKE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
        #if SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(10, 0, 0)
            #define SOAGEN_CLANG 6 // not strictly correct but doesn't matter below this
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(10, 0, 1)
            #define SOAGEN_CLANG 7
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(11, 0, 0)
            #define SOAGEN_CLANG 8
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(11, 0, 3)
            #define SOAGEN_CLANG 9
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(12, 0, 0)
            #define SOAGEN_CLANG 10
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(12, 0, 5)
            #define SOAGEN_CLANG 11
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(13, 0, 0)
            #define SOAGEN_CLANG 12
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(13, 1, 6)
            #define SOAGEN_CLANG 13
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(14, 0, 0)
            #define SOAGEN_CLANG 14
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(14, 0, 3)
            #define SOAGEN_CLANG 15
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(15, 0, 0)
            #define SOAGEN_CLANG 16
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(16, 0, 0)
            #define SOAGEN_CLANG 17
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(17, 0, 0)
            #define SOAGEN_CLANG 19
        #elif SOAGEN_CLANG_VERSION <= SOAGEN_MAKE_VERSION(21, 0, 0)
            #define SOAGEN_CLANG 21
        #else
            #define SOAGEN_CLANG __clang_major__
        #endif
        #undef SOAGEN_CLANG_VERSION
        #define SOAGEN_APPLE_CLANG SOAGEN_CLANG
    #else
        #define SOAGEN_APPLE_CLANG 0
    #endif
#endif
/// @def SOAGEN_CLANG
/// @brief The value of `__clang_major__` when the code is being compiled by LLVM/Clang, otherwise `0`.
/// @see https://sourceforge.net/p/predef/wiki/Compilers/

#ifndef SOAGEN_ICC
    #ifdef __INTEL_COMPILER
        #define SOAGEN_ICC __INTEL_COMPILER
        #ifdef __ICL
            #define SOAGEN_ICC_CL SOAGEN_ICC
        #else
            #define SOAGEN_ICC_CL 0
        #endif
    #else
        #define SOAGEN_ICC    0
        #define SOAGEN_ICC_CL 0
    #endif
#endif
/// @def SOAGEN_ICC
/// @brief The value of `__INTEL_COMPILER` when the code is being compiled by ICC, otherwise `0`.
/// @see http://scv.bu.edu/computation/bladecenter/manpages/icc.html

#ifndef SOAGEN_MSVC_LIKE
    #ifdef _MSC_VER
        #define SOAGEN_MSVC_LIKE _MSC_VER
    #else
        #define SOAGEN_MSVC_LIKE 0
    #endif
#endif
/// @def SOAGEN_MSVC_LIKE
/// @brief The value of `_MSC_VER` when it is defined by an MSVC-compatible compiler, otherwise `0`.
/// @see https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros

#ifndef SOAGEN_MSVC
    #if SOAGEN_MSVC_LIKE && !SOAGEN_CLANG && !SOAGEN_ICC
        #define SOAGEN_MSVC SOAGEN_MSVC_LIKE
    #else
        #define SOAGEN_MSVC 0
    #endif
#endif
/// @def SOAGEN_MSVC
/// @brief The value of `_MSC_VER` when the code is being compiled by MSVC specifically, otherwise `0`.
/// @see https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros

#ifndef SOAGEN_GCC_LIKE
    #ifdef __GNUC__
        #define SOAGEN_GCC_LIKE __GNUC__
    #else
        #define SOAGEN_GCC_LIKE 0
    #endif
#endif
/// @def SOAGEN_GCC_LIKE
/// @brief The value of `__GNUC__` when it is defined by a GCC-compatible compiler, otherwise `0`.
/// @see https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

#ifndef SOAGEN_GCC
    #if SOAGEN_GCC_LIKE && !SOAGEN_CLANG && !SOAGEN_ICC
        #define SOAGEN_GCC SOAGEN_GCC_LIKE
    #else
        #define SOAGEN_GCC 0
    #endif
#endif
/// @def SOAGEN_GCC
/// @brief The value of `__GNUC__` when the code is being compiled by GCC specifically, otherwise `0`.
/// @see https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

#ifndef SOAGEN_CUDA
    #if defined(__CUDACC__) || defined(__CUDA_ARCH__) || defined(__CUDA_LIBDEVICE__)
        #define SOAGEN_CUDA 1
    #else
        #define SOAGEN_CUDA 0
    #endif
#endif
/// @def SOAGEN_CUDA
/// @brief `1` when compiled with nVIDIA's CUDA compiler, otherwise `0`.
/// @see https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

#ifndef SOAGEN_INTELLISENSE
    #ifdef __INTELLISENSE__
        #define SOAGEN_INTELLISENSE 1
    #else
        #define SOAGEN_INTELLISENSE 0
    #endif
#endif
/// @def SOAGEN_INTELLISENSE
/// @brief `1` when the code being compiled by an IDE's 'intellisense' compiler, otherwise `0`.

#ifndef SOAGEN_DOXYGEN
    #if defined(DOXYGEN) || defined(__DOXYGEN) || defined(__DOXYGEN__) || defined(__doxygen__) || defined(__POXY__)    \
        || defined(__poxy__)
        #define SOAGEN_DOXYGEN 1
    #else
        #define SOAGEN_DOXYGEN 0
    #endif
#endif
/// @def SOAGEN_DOXYGEN
/// @brief `1` when the code being interpreted by Doygen or some other documentation generator, otherwise `0`.

#if (!!SOAGEN_CLANG + !!SOAGEN_ICC + !!SOAGEN_MSVC + !!SOAGEN_GCC) != 1
    #error Could not uniquely identify compiler. Please file an issue at https://github.com/iceye-ltd/fsw-cpp-building-blocks/issues.
#endif
