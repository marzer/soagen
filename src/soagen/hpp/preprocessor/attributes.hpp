//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "identify_compiler.hpp"
#include "identify_features.hpp"

#ifndef SOAGEN_GNU_ATTR
    #if SOAGEN_CLANG || SOAGEN_GCC_LIKE
        #define SOAGEN_GNU_ATTR(...) __attribute__((__VA_ARGS__))
    #else
        #define SOAGEN_GNU_ATTR(...)
    #endif
#endif

#ifndef SOAGEN_CLANG_ATTR
    #if SOAGEN_CLANG
        #define SOAGEN_CLANG_ATTR(...) __attribute__((__VA_ARGS__))
    #else
        #define SOAGEN_CLANG_ATTR(...)
    #endif
#endif

#ifndef SOAGEN_GCC_ATTR
    #if SOAGEN_GCC
        #define SOAGEN_GCC_ATTR(...) __attribute__((__VA_ARGS__))
    #else
        #define SOAGEN_GCC_ATTR(...)
    #endif
#endif

#ifndef SOAGEN_IN
    #if SOAGEN_GCC >= 10
        #define SOAGEN_IN(...) SOAGEN_GNU_ATTR(__access__(read_only, __VA_ARGS__))
    #else
        #define SOAGEN_IN(...)
    #endif
#endif

#ifndef SOAGEN_OUT
    #if SOAGEN_GCC >= 10
        #define SOAGEN_OUT(...) SOAGEN_GNU_ATTR(__access__(write_only, __VA_ARGS__))
    #else
        #define SOAGEN_OUT(...)
    #endif
#endif

#ifndef SOAGEN_INOUT
    #if SOAGEN_GCC >= 10
        #define SOAGEN_INOUT(...) SOAGEN_GNU_ATTR(__access__(read_write, __VA_ARGS__))
    #else
        #define SOAGEN_INOUT(...)
    #endif
#endif

#ifndef SOAGEN_DECLSPEC
    #if SOAGEN_MSVC_LIKE
        #define SOAGEN_DECLSPEC(...) __declspec(__VA_ARGS__)
    #else
        #define SOAGEN_DECLSPEC(...)
    #endif
#endif

#ifndef SOAGEN_ALWAYS_INLINE
    #if SOAGEN_MSVC_LIKE
        #define SOAGEN_ALWAYS_INLINE __forceinline
    #elif SOAGEN_CLANG || SOAGEN_GCC_LIKE || SOAGEN_HAS_GNU_ATTR(__always_inline__)
        #define SOAGEN_ALWAYS_INLINE                                                                                   \
            SOAGEN_GNU_ATTR(__always_inline__)                                                                         \
            inline
    #else
        #define SOAGEN_ALWAYS_INLINE inline
    #endif
#endif

#ifndef SOAGEN_NEVER_INLINE
    #if SOAGEN_MSVC_LIKE
        #define SOAGEN_NEVER_INLINE SOAGEN_DECLSPEC(noinline)
    #elif SOAGEN_CUDA // https://gitlab.gnome.org/GNOME/glib/-/issues/2555
        #define SOAGEN_NEVER_INLINE SOAGEN_GNU_ATTR(noinline)
    #else
        #if SOAGEN_GCC || SOAGEN_CLANG || SOAGEN_HAS_GNU_ATTR(__noinline__)
            #define SOAGEN_NEVER_INLINE SOAGEN_GNU_ATTR(__noinline__)
        #endif
    #endif
    #ifndef SOAGEN_NEVER_INLINE
        #define SOAGEN_NEVER_INLINE
    #endif
#endif

#ifndef SOAGEN_EMPTY_BASES
    #if SOAGEN_MSVC_LIKE
        #define SOAGEN_EMPTY_BASES SOAGEN_DECLSPEC(empty_bases)
    #else
        #define SOAGEN_EMPTY_BASES
    #endif
#endif

#ifndef SOAGEN_LIKELY
    #if SOAGEN_CPP >= 20 && SOAGEN_HAS_CPP_ATTR(likely) >= 201803
        #define SOAGEN_LIKELY(...) (__VA_ARGS__) [[likely]]
    #elif SOAGEN_CLANG || SOAGEN_GCC_LIKE || SOAGEN_HAS_BUILTIN(__builtin_expect)
        #define SOAGEN_LIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 1))
    #else
        #define SOAGEN_LIKELY(...) (__VA_ARGS__)
    #endif
#endif

#ifndef SOAGEN_UNLIKELY
    #if SOAGEN_CPP >= 20 && SOAGEN_HAS_CPP_ATTR(unlikely) >= 201803
        #define SOAGEN_UNLIKELY(...) (__VA_ARGS__) [[unlikely]]
    #elif SOAGEN_CLANG || SOAGEN_GCC_LIKE || SOAGEN_HAS_BUILTIN(__builtin_expect)
        #define SOAGEN_UNLIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 0))
    #else
        #define SOAGEN_UNLIKELY(...) (__VA_ARGS__)
    #endif
#endif

#ifndef SOAGEN_NODISCARD
    #if SOAGEN_HAS_CPP_ATTR(nodiscard) >= 201603
        #define SOAGEN_NODISCARD       [[nodiscard]]
        #define SOAGEN_NODISCARD_CLASS [[nodiscard]]
    #elif SOAGEN_CLANG || SOAGEN_GCC_LIKE || SOAGEN_HAS_GNU_ATTR(__warn_unused_result__)
        #define SOAGEN_NODISCARD SOAGEN_GNU_ATTR(__warn_unused_result__)
    #else
        #define SOAGEN_NODISCARD
    #endif
    #ifndef SOAGEN_NODISCARD_CLASS
        #define SOAGEN_NODISCARD_CLASS
    #endif
    #if SOAGEN_HAS_CPP_ATTR(nodiscard) >= 201907
        #define SOAGEN_NODISCARD_CTOR [[nodiscard]]
    #else
        #define SOAGEN_NODISCARD_CTOR
    #endif
#endif

#ifndef SOAGEN_PURE
    #ifdef NDEBUG
        #define SOAGEN_PURE SOAGEN_GNU_ATTR(pure)
    #else
        #define SOAGEN_PURE
    #endif
#endif

#ifndef SOAGEN_CONST
    #ifdef NDEBUG
        #define SOAGEN_CONST SOAGEN_GNU_ATTR(const)
    #else
        #define SOAGEN_CONST
    #endif
#endif

#ifndef SOAGEN_INLINE_GETTER
    #define SOAGEN_INLINE_GETTER                                                                                       \
        SOAGEN_NODISCARD                                                                                               \
        SOAGEN_ALWAYS_INLINE
#endif

#ifndef SOAGEN_PURE_GETTER
    #define SOAGEN_PURE_GETTER                                                                                         \
        SOAGEN_NODISCARD                                                                                               \
        SOAGEN_PURE
#endif

#ifndef SOAGEN_PURE_INLINE_GETTER
    #define SOAGEN_PURE_INLINE_GETTER                                                                                  \
        SOAGEN_NODISCARD                                                                                               \
        SOAGEN_ALWAYS_INLINE                                                                                           \
        SOAGEN_PURE
#endif

#ifndef SOAGEN_CONST_GETTER
    #define SOAGEN_CONST_GETTER                                                                                        \
        SOAGEN_NODISCARD                                                                                               \
        SOAGEN_CONST
#endif

#ifndef SOAGEN_CONST_INLINE_GETTER
    #define SOAGEN_CONST_INLINE_GETTER                                                                                 \
        SOAGEN_NODISCARD                                                                                               \
        SOAGEN_ALWAYS_INLINE                                                                                           \
        SOAGEN_CONST
#endif

#ifndef SOAGEN_NO_UNIQUE_ADDRESS
    #if SOAGEN_CPP && (SOAGEN_MSVC >= 1928 || (SOAGEN_CLANG && SOAGEN_MSVC_LIKE))
        #define SOAGEN_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
    #elif (SOAGEN_CPP >= 20 && SOAGEN_HAS_CPP_ATTR(no_unique_address) >= 201803)                                       \
           || ((SOAGEN_GCC >= 9 || SOAGEN_CLANG >= 9)) // https://clang.llvm.org/docs/AttributeReference.html
        #define SOAGEN_NO_UNIQUE_ADDRESS [[no_unique_address]]
    #else
        #define SOAGEN_NO_UNIQUE_ADDRESS
    #endif
#endif

#ifndef SOAGEN_CONSTEXPR_20
    #if SOAGEN_HAS_CONSTEXPR_20
        #define SOAGEN_CONSTEXPR_20 constexpr
    #else
        #define SOAGEN_CONSTEXPR_20
    #endif
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 202211L
    #define SOAGEN_STATIC_CONSTEXPR_23 static constexpr
#else
    #define SOAGEN_STATIC_CONSTEXPR_23 constexpr
#endif

#ifndef SOAGEN_LIFETIME_BOUND
    #if SOAGEN_CLANG >= 7
        #define SOAGEN_LIFETIME_BOUND [[clang::lifetimebound]]
    #elif SOAGEN_MSVC >= 1941
        #define SOAGEN_LIFETIME_BOUND [[msvc::lifetimebound]]
    #else
        #define SOAGEN_LIFETIME_BOUND
    #endif
#endif

#ifndef SOAGEN_OWNER
    #if SOAGEN_CLANG >= 9
        #define SOAGEN_OWNER [[gsl::Owner]]
    #else
        #define SOAGEN_OWNER
    #endif
#endif

#ifndef SOAGEN_VIEW
    #if SOAGEN_CLANG >= 9
        #define SOAGEN_VIEW [[gsl::Pointer]]
    #else
        #define SOAGEN_VIEW
    #endif
#endif

#ifndef SOAGEN_RESETTER
    #if SOAGEN_CLANG >= 7
        #define SOAGEN_RESETTER [[clang::reinitializes]]
    #else
        #define SOAGEN_RESETTER
    #endif
#endif
