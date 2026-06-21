//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "identify_language.hpp"
#include "identify_compiler.hpp"

#ifndef SOAGEN_HAS_INCLUDE
    #ifdef __has_include
        #define SOAGEN_HAS_INCLUDE(header)	__has_include(header)
    #else
        #define SOAGEN_HAS_INCLUDE(header) 0
    #endif
#endif

#if SOAGEN_CPP >= 20 && SOAGEN_HAS_INCLUDE(<version>)
    #include <version>
#endif

#ifndef SOAGEN_HAS_BUILTIN
    #ifdef __has_builtin
        #define SOAGEN_HAS_BUILTIN(name) __has_builtin(name)
    #else
        #define SOAGEN_HAS_BUILTIN(name) 0
    #endif
#endif

#ifndef SOAGEN_HAS_FEATURE
    #ifdef __has_feature
        #define SOAGEN_HAS_FEATURE(name) __has_feature(name)
    #else
        #define SOAGEN_HAS_FEATURE(name) 0
    #endif
#endif

#ifndef SOAGEN_HAS_GNU_ATTR
    #ifdef __has_attribute
        #define SOAGEN_HAS_GNU_ATTR(attr) __has_attribute(attr)
    #else
        #define SOAGEN_HAS_GNU_ATTR(attr) 0
    #endif
#endif

#ifndef SOAGEN_HAS_CPP_ATTR
    #ifdef __has_cpp_attribute
        #define SOAGEN_HAS_CPP_ATTR(attr) __has_cpp_attribute(attr)
    #else
        #define SOAGEN_HAS_CPP_ATTR(attr) 0
    #endif
#endif

#ifndef SOAGEN_HAS_EXCEPTIONS
    #if SOAGEN_DOXYGEN || defined(__EXCEPTIONS) || defined(_CPPUNWIND) || defined(__cpp_exceptions)
        #define SOAGEN_HAS_EXCEPTIONS 1
    #else
        #define SOAGEN_HAS_EXCEPTIONS 0
    #endif
#endif

#ifndef SOAGEN_HAS_RTTI
    #if defined(_CPPRTTI) || defined(__GXX_RTTI) || SOAGEN_HAS_FEATURE(cxx_rtti)
        #define SOAGEN_HAS_RTTI 1
    #else
        #define SOAGEN_HAS_RTTI 0
    #endif
#endif

#ifndef SOAGEN_HAS_CONCEPTS
    #if defined(__cpp_concepts) && __cpp_concepts >= 201907
        #define SOAGEN_HAS_CONCEPTS 1
    #else
        #define SOAGEN_HAS_CONCEPTS 0
    #endif
#endif

#ifndef SOAGEN_HAS_CONSTEVAL
    #if defined(__cpp_consteval) && __cpp_consteval >= 201811 && (!SOAGEN_MSVC || SOAGEN_MSVC >= 1934)                 \
        && (!SOAGEN_CLANG || SOAGEN_CLANG >= 15)
        #define SOAGEN_HAS_CONSTEVAL 1
    #else
        #define SOAGEN_HAS_CONSTEVAL SOAGEN_DOXYGEN
    #endif
#endif

#ifndef SOAGEN_HAS_IF_CONSTEVAL
    #if defined(__cpp_if_consteval) && __cpp_if_consteval >= 202106
        #define SOAGEN_HAS_IF_CONSTEVAL 1
    #else
        #define SOAGEN_HAS_IF_CONSTEVAL 0
    #endif
#endif

#ifndef SOAGEN_HAS_CTAD
    #if !SOAGEN_DOXYGEN && defined(__cpp_deduction_guides) && __cpp_deduction_guides >= 201703
        #define SOAGEN_HAS_CTAD 1
    #else
        #define SOAGEN_HAS_CTAD 0
    #endif
#endif

#ifndef SOAGEN_HAS_CTAD_20
    #if !SOAGEN_DOXYGEN && defined(__cpp_deduction_guides) && __cpp_deduction_guides >= 201907                         \
        && (!SOAGEN_GCC || SOAGEN_GCC >= 10)
        #define SOAGEN_HAS_CTAD_20 1
    #else
        #define SOAGEN_HAS_CTAD_20 0
    #endif
#endif

#ifndef SOAGEN_HAS_CONSTEXPR_14
    #if defined(__cpp_constexpr) && __cpp_constexpr >= 201304
        #define SOAGEN_HAS_CONSTEXPR_14 1
    #else
        #define SOAGEN_HAS_CONSTEXPR_14 SOAGEN_DOXYGEN
    #endif
#endif

#ifndef SOAGEN_HAS_CONSTEXPR_20
    #if defined(__cpp_constexpr) && __cpp_constexpr >= 201907
        #define SOAGEN_HAS_CONSTEXPR_20 1
    #else
        #define SOAGEN_HAS_CONSTEXPR_20 SOAGEN_DOXYGEN
    #endif
#endif

#ifndef SOAGEN_HAS_CHAR8
    #if defined(__cpp_char8_t) && __cpp_char8_t >= 201811
        #define SOAGEN_HAS_CHAR8 1
    #else
        #define SOAGEN_HAS_CHAR8 SOAGEN_DOXYGEN
    #endif
#endif

#ifndef SOAGEN_HAS_GUARANTEED_COPY_ELISION
    #if SOAGEN_DOXYGEN || (defined(__cpp_guaranteed_copy_elision) && __cpp_guaranteed_copy_elision >= 201606)
        #define SOAGEN_HAS_GUARANTEED_COPY_ELISION 1
    #else
        #define SOAGEN_HAS_GUARANTEED_COPY_ELISION 0
    #endif
#endif

#if defined(__cpp_conditional_explicit) && __cpp_conditional_explicit >= 201806L && SOAGEN_CPP >= 20 && !SOAGEN_GCC
    #define SOAGEN_HAS_CONDITIONAL_EXPLICIT 1
#else
    #define SOAGEN_HAS_CONDITIONAL_EXPLICIT 0
#endif

#ifndef SOAGEN_HAS_FOLD_EXPRESSIONS
    #if defined(__cpp_fold_expressions) && __cpp_fold_expressions >= 201603L
        #define SOAGEN_HAS_FOLD_EXPRESSIONS 1
    #else
        #define SOAGEN_HAS_FOLD_EXPRESSIONS SOAGEN_DOXYGEN
    #endif
#endif

#if (defined(__cpp_return_type_deduction) && __cpp_return_type_deduction >= 201304L) || SOAGEN_DOXYGEN
    #define SOAGEN_HAS_RETURN_TYPE_DEDUCTION 1
#else
    #define SOAGEN_HAS_RETURN_TYPE_DEDUCTION 0
#endif
