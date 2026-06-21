//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.hpp"
SOAGEN_DISABLE_WARNINGS;
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#ifndef SOAGEN_COLUMN_SPAN_TYPE
    #if SOAGEN_CPP >= 20 && SOAGEN_HAS_INCLUDE(<span>)
        #include <span>
        #define SOAGEN_COLUMN_SPAN_TYPE std::span
    #elif SOAGEN_HAS_INCLUDE(<muu/span.h>)
        #include <muu/span.h>
        #define SOAGEN_COLUMN_SPAN_TYPE               muu::span
        #define SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT 1
    #elif SOAGEN_HAS_INCLUDE(<tcb/span.hpp>)
        #include <tcb/span.hpp>
        #define SOAGEN_COLUMN_SPAN_TYPE TCB_SPAN_NAMESPACE_NAME::span
    #endif
#endif
#ifndef SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT
    #define SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT 0
#endif
#ifndef SOAGEN_OPTIONAL_TYPE
    #include <optional>
    #define SOAGEN_OPTIONAL_TYPE std::optional
#endif
SOAGEN_ENABLE_WARNINGS;
#include "header_start.hpp"

/// @brief The root namespace for the library.
namespace soagen
{
    using std::size_t;
    using std::ptrdiff_t;
    using std::intptr_t;
    using std::uintptr_t;
    using std::nullptr_t;

    /// @cond
    // forward declarations (hidden from doxygen)
    template <typename>
    struct allocator_traits;
    struct allocator;
    template <typename...>
    struct emplacer;
    template <typename, size_t...>
    struct row;
    template <typename, size_t...>
    class iterator;
    template <typename>
    class span;
    template <typename...>
    struct table_traits;
    template <typename, typename>
    class table;
    /// @endcond

    /// @brief  Alias for `std::integral_constant<std::size_t, Value>`
    template <auto Value>
    using index_constant = std::integral_constant<size_t, static_cast<size_t>(Value)>;

#ifdef SOAGEN_COLUMN_SPAN_TYPE

    #if SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT

    template <typename T, size_t Align = alignof(T)>
    using column_span = SOAGEN_COLUMN_SPAN_TYPE<T, static_cast<size_t>(-1), Align>;

    #else

    template <typename T, size_t Align = alignof(T)>
    using column_span = SOAGEN_COLUMN_SPAN_TYPE<T>;

    #endif

    template <typename T, size_t Align = alignof(T)>
    using const_column_span = column_span<std::add_const_t<T>, Align>;

#endif

#if SOAGEN_DOXYGEN
    /// @brief The optional type used by the library.
    /// @note This can be overridden by redefining `SOAGEN_OPTIONAL_TYPE`, e.g.: @cpp
    ///
    ///	#define SOAGEN_OPTIONAL_TYPE tl::optional
    ///
    ///	@ecpp
    template <typename T>
    using optional = std::optional<T>;
#else
    using SOAGEN_OPTIONAL_TYPE;
#endif
}

#include "header_end.hpp"
