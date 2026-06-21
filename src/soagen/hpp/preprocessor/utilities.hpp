//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "concat.hpp"
#include "identify_compiler.hpp"
#include "identify_features.hpp"
#include "warnings.hpp"

#ifndef SOAGEN_DELETE_COPY
    #define SOAGEN_DELETE_COPY(T)                                                                                      \
        T(const T&)            = delete;                                                                               \
        T& operator=(const T&) = delete
#endif
/// @def SOAGEN_DELETE_COPY(T)
/// @brief Explicitly deletes the copy constructor and copy-assignment operator of a class or struct.
/// @details @cpp
/// class uncopyable
/// {
/// 	uncopyable() {}
/// 	SOAGEN_DELETE_COPY(uncopyable);
/// };
///
/// //equivalent to:
/// class uncopyable
/// {
/// 	uncopyable() {}
///
/// 	uncopyable(const uncopyable&) = delete;
/// 	uncopyable& operator=(const uncopyable&) = delete;
/// };
/// @endcpp
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_DELETE_MOVE
    #define SOAGEN_DELETE_MOVE(T)                                                                                      \
        T(T&&)            = delete;                                                                                    \
        T& operator=(T&&) = delete
#endif
/// @def SOAGEN_DELETE_MOVE(T)
/// @brief Explicitly deletes the move constructor and move-assignment operator of a class or struct.
/// @details @cpp
/// class immovable_type
/// {
/// 	immovable_type() {}
///
/// 	SOAGEN_DELETE_MOVE(immovable_type);
/// };
///
/// //equivalent to:
/// class immovable_type
/// {
/// 	immovable_type() {}
///
/// 	immovable_type(immovable_type&&) = delete;
/// 	immovable_type& operator=(immovable_type&&) = delete;
/// };
/// @endcpp
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_DELETE_RULE_OF_FOUR
    #define SOAGEN_DELETE_RULE_OF_FOUR(T)                                                                              \
        SOAGEN_DELETE_COPY(T);                                                                                         \
        SOAGEN_DELETE_MOVE(T)
#endif
/// @def SOAGEN_DELETE_RULE_OF_FOUR
/// @brief Explicitly deletes the move and copy constructors and assignment operators.
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_DELETE_RULE_OF_FIVE
    #define SOAGEN_DELETE_RULE_OF_FIVE(T)                                                                              \
        T() = delete;                                                                                                  \
        SOAGEN_DELETE_RULE_OF_FOUR(T)
#endif
/// @def SOAGEN_DELETE_RULE_OF_FIVE
/// @brief Explicitly deletes the default constructor, move and copy constructors and assignment operators.
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_DEFAULT_COPY
    #define SOAGEN_DEFAULT_COPY(T)                                                                                     \
        SOAGEN_PUSH_WARNINGS;                                                                                          \
        SOAGEN_DISABLE_CLANG_WARNING("-Wunused-member-function");                                                      \
        T(const T&)            = default;                                                                              \
        T& operator=(const T&) = default;                                                                              \
        SOAGEN_POP_WARNINGS
#endif
/// @def SOAGEN_DEFAULT_COPY
/// @brief Explicitly defaults the copy constructor and copy-assignment operator of a class or struct.
/// @details @cpp
/// class copyable_type
/// {
/// 	copyable_type() {}
/// 	SOAGEN_DEFAULT_COPY(copyable_type);
/// };
///
/// //equivalent to:
/// class copyable_type
/// {
/// 	copyable_type() {}
///
/// 	copyable_type(const copyable_type&) = default;
/// 	copyable_type& operator=(const copyable_type&) = default;
/// };
/// @endcpp
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_DEFAULT_MOVE
    #define SOAGEN_DEFAULT_MOVE(T)                                                                                     \
        SOAGEN_PUSH_WARNINGS;                                                                                          \
        SOAGEN_DISABLE_CLANG_WARNING("-Wunused-member-function");                                                      \
        T(T&&)            = default;                                                                                   \
        T& operator=(T&&) = default;                                                                                   \
        SOAGEN_POP_WARNINGS
#endif
/// @def SOAGEN_DEFAULT_MOVE
/// @brief Explicitly defaults the move constructor and move-assignment operator of a class or struct.
/// @details @cpp
/// class movable_type
/// {
/// 	movable_type() {}
///
/// 	SOAGEN_DEFAULT_MOVE(movable_type);
/// };
///
/// //equivalent to:
/// class movable_type
/// {
/// 	movable_type() {}
///
/// 	movable_type(movable_type&&) = default;
/// 	movable_type& operator=(movable_type&&) = default;
/// };
/// @endcpp
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_DEFAULT_RULE_OF_FOUR
    #define SOAGEN_DEFAULT_RULE_OF_FOUR(T)                                                                             \
        SOAGEN_DEFAULT_COPY(T);                                                                                        \
        SOAGEN_DEFAULT_MOVE(T)
#endif
/// @def SOAGEN_DEFAULT_RULE_OF_FOUR
/// @brief Explicitly defaults the move and copy constructors and assignment operators.
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_DEFAULT_RULE_OF_FIVE
    #define SOAGEN_DEFAULT_RULE_OF_FIVE(T)                                                                             \
        SOAGEN_PUSH_WARNINGS;                                                                                          \
        SOAGEN_DISABLE_CLANG_WARNING("-Wunused-member-function");                                                      \
        T() = default;                                                                                                 \
        SOAGEN_DEFAULT_RULE_OF_FOUR(T);                                                                                \
        SOAGEN_POP_WARNINGS
#endif
/// @def SOAGEN_DEFAULT_RULE_OF_FIVE
/// @brief Explicitly defaults the default constructor, move and copy constructors and assignment operators.
/// @see https://cpppatterns.com/patterns/rule-of-five.html

#ifndef SOAGEN_UNUSED
    #define SOAGEN_UNUSED(...) static_cast<void>(__VA_ARGS__)
#endif
/// @def SOAGEN_UNUSED(expr)
/// @brief Explicitly denotes the result of an expression as being unused.

#ifndef SOAGEN_NOOP
    #define SOAGEN_NOOP static_cast<void>(0)
#endif
/// @def SOAGEN_NOOP
/// @brief Expands to a no-op expression.

#ifndef SOAGEN_ASSUME
    #if SOAGEN_MSVC_LIKE
        #define SOAGEN_ASSUME(expr) __assume(expr)
    #elif SOAGEN_ICC || SOAGEN_CLANG || SOAGEN_HAS_BUILTIN(__builtin_assume)
        #define SOAGEN_ASSUME(expr) __builtin_assume(expr)
    #elif SOAGEN_HAS_GNU_ATTR(__assume__)
        #define SOAGEN_ASSUME(expr) __attribute__((__assume__(expr)))
    #elif SOAGEN_HAS_CPP_ATTR(assume) >= 202207
        #define SOAGEN_ASSUME(expr) [[assume(expr)]]
    #else
        #define SOAGEN_ASSUME(expr) static_cast<void>(0)
    #endif
#endif
/// @def SOAGEN_ASSUME
/// @brief Optimizer hint for signalling various assumptions about state at specific points in code.
/// @details @cpp
/// 	void do_the_thing(int flags) noexcept
/// 	{
/// 		SOAGEN_ASSUME(flags > 0);
///			// ... do some some performance-critical code stuff that assumes flags is > 0 here ...
/// 	}
/// @endcpp

#ifndef SOAGEN_UNREACHABLE
    #if SOAGEN_MSVC_LIKE
        #define SOAGEN_UNREACHABLE __assume(0)
    #elif SOAGEN_ICC || SOAGEN_CLANG || SOAGEN_GCC_LIKE || SOAGEN_HAS_BUILTIN(__builtin_unreachable)
        #define SOAGEN_UNREACHABLE __builtin_unreachable()
    #else
        #define SOAGEN_UNREACHABLE static_cast<void>(0)
    #endif
#endif
/// @def SOAGEN_UNREACHABLE
/// @brief Marks a position in the code as being unreachable.
/// @warning Using this incorrectly can lead to seriously mis-compiled code!

#ifndef SOAGEN_MAKE_STRING
    #define SOAGEN_MAKE_STRING_2(s) #s
    #define SOAGEN_MAKE_STRING_1(s) SOAGEN_MAKE_STRING_2(s)
    #define SOAGEN_MAKE_STRING(s)   SOAGEN_MAKE_STRING_1(s)
#endif
/// @def SOAGEN_MAKE_STRING
/// @brief Converts a token into a string literal, expanding any macro arguments in the process.

#ifndef SOAGEN_MAKE_FLAGS
    #define SOAGEN_MAKE_FLAGS_2(T, op)                                                                                 \
        SOAGEN_CONST_INLINE_GETTER                                                                                     \
        constexpr T operator op(T lhs, T rhs) noexcept                                                                 \
        {                                                                                                              \
            using underlying = std::underlying_type<T>::type;                                                          \
            /* NOLINTNEXTLINE(*.EnumCastOutOfRange) */                                                                 \
            return static_cast<T>(static_cast<underlying>(lhs) op static_cast<underlying>(rhs));                       \
        }                                                                                                              \
                                                                                                                       \
        SOAGEN_CONSTEXPR_14                                                                                            \
        inline T& operator SOAGEN_CONCAT(op, =)(T & lhs, T rhs) noexcept                                               \
        {                                                                                                              \
            return lhs = (lhs op rhs); /* NOLINT(*.EnumCastOutOfRange) */                                              \
        }                                                                                                              \
                                                                                                                       \
        static_assert(true, "")

    #define SOAGEN_MAKE_FLAGS_1(T)                                                                                     \
        static_assert(std::is_enum<T>::value, "SOAGEN_MAKE_FLAGS requires an enum type argument.");                    \
        SOAGEN_PUSH_WARNINGS;                                                                                          \
        SOAGEN_DISABLE_CLANG_WARNING("-Wunused-function");                                                             \
                                                                                                                       \
        SOAGEN_MAKE_FLAGS_2(T, &);                                                                                     \
        SOAGEN_MAKE_FLAGS_2(T, |);                                                                                     \
        SOAGEN_MAKE_FLAGS_2(T, ^);                                                                                     \
                                                                                                                       \
        SOAGEN_CONST_INLINE_GETTER                                                                                     \
        constexpr T operator~(T val) noexcept                                                                          \
        {                                                                                                              \
            using underlying = std::underlying_type<T>::type;                                                          \
            return static_cast<T>(~static_cast<underlying>(val)); /* NOLINT(*.EnumCastOutOfRange) */                   \
        }                                                                                                              \
                                                                                                                       \
        SOAGEN_CONST_INLINE_GETTER                                                                                     \
        constexpr bool operator!(T val) noexcept                                                                       \
        {                                                                                                              \
            using underlying = std::underlying_type<T>::type;                                                          \
            return !static_cast<underlying>(val); /* NOLINT(*.EnumCastOutOfRange) */                                   \
        }                                                                                                              \
                                                                                                                       \
        SOAGEN_POP_WARNINGS

    #define SOAGEN_MAKE_FLAGS(T) SOAGEN_MAKE_FLAGS_1(T)
#endif
/// @def SOAGEN_MAKE_FLAGS(T)
/// @brief Stamps out operators for enum 'flags' types.
/// @details @cpp
/// enum class my_flags : unsigned
/// {
/// 	none   = 0,
/// 	alpha  = 1,
/// 	beta   = 2,
/// 	gamma  = 4
/// };
/// SOAGEN_MAKE_FLAGS(my_flags);
///
/// // emits these operators:
/// constexpr my_flags  operator &  (my_flags,  my_flags) noexcept;
/// constexpr my_flags& operator &= (my_flags&, my_flags) noexcept;
/// constexpr my_flags  operator |  (my_flags,  my_flags) noexcept;
/// constexpr my_flags& operator |= (my_flags&, my_flags) noexcept;
/// constexpr my_flags  operator ^  (my_flags,  my_flags) noexcept;
/// constexpr my_flags& operator ^= (my_flags&, my_flags) noexcept;
/// constexpr my_flags  operator ~  (my_flags) noexcept;
/// constexpr bool      operator !  (my_flags) noexcept;
/// @endcpp

#ifndef SOAGEN_REQUIRES
    #if !SOAGEN_DOXYGEN && SOAGEN_HAS_CONCEPTS
        #define SOAGEN_REQUIRES(...) requires(__VA_ARGS__)
    #else
        #define SOAGEN_REQUIRES(...)
    #endif
#endif

#if !defined(POXY_IMPLEMENTATION_DETAIL)
    #define POXY_IMPLEMENTATION_DETAIL(...) __VA_ARGS__
#endif

#ifndef SOAGEN_COMMA
    #define SOAGEN_COMMA ,
#endif

#ifndef SOAGEN_HIDDEN
    #if SOAGEN_DOXYGEN
        #define SOAGEN_HIDDEN(...)
        #define SOAGEN_HIDDEN_BASE(...)
        #define SOAGEN_DOXYGEN_ONLY(...)  __VA_ARGS__
        #define SOAGEN_IF_DOXYGEN(A, ...) A
    #else
        #define SOAGEN_HIDDEN(...) __VA_ARGS__
        #define SOAGEN_HIDDEN_BASE(...)	: __VA_ARGS__
        #define SOAGEN_DOXYGEN_ONLY(...)
        #define SOAGEN_IF_DOXYGEN(A, ...) __VA_ARGS__
    #endif
#endif
#define SOAGEN_HIDDEN_PARAM(...)  SOAGEN_HIDDEN(SOAGEN_COMMA __VA_ARGS__)
#define SOAGEN_HIDDEN_PARAMS(...) SOAGEN_HIDDEN_PARAM(__VA_ARGS__)

#if SOAGEN_DOXYGEN
    #define SOAGEN_DETAIL_START
    #define SOAGEN_DETAIL_END
#else
    #define SOAGEN_DETAIL_START                                                                                        \
        namespace detail                                                                                               \
        {                                                                                                              \
            static_assert(true, "")

    #define SOAGEN_DETAIL_END                                                                                          \
        }                                                                                                              \
        static_assert(true, "")
#endif

#ifndef SOAGEN_LAUNDER
    #if SOAGEN_CLANG >= 8 || SOAGEN_GCC >= 7 || SOAGEN_ICC >= 1910 || SOAGEN_HAS_BUILTIN(__builtin_launder)
        #define SOAGEN_LAUNDER(...) __builtin_launder(__VA_ARGS__)
    #elif defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
        #define SOAGEN_LAUNDER(...) std::launder(__VA_ARGS__)
    #else
        #define SOAGEN_LAUNDER(...) __VA_ARGS__
    #endif
#endif

#ifndef SOAGEN_OUTLINE_STATIC_CONSTEXPR_MEMBER
    #if SOAGEN_CPP >= 17 || SOAGEN_DOXYGEN
        #define SOAGEN_OUTLINE_STATIC_CONSTEXPR_MEMBER(name) static_assert(true, "")
    #else
        // out-of-line defs are required pre-C++17 (constexpr static members aren't implicitly inline until C++17).
        // clang 17+ emits -Wdeprecated-redundant-constexpr-static-def under -Weverything regardless of language mode.
        #if SOAGEN_MSVC
            #define SOAGEN_DETAIL_OUTLINE_STATIC_CONSTEXPR_MEMBER_DEF(name) constexpr decltype(name) name
        #else
            #define SOAGEN_DETAIL_OUTLINE_STATIC_CONSTEXPR_MEMBER_DEF(name) inline constexpr decltype(name) name
        #endif
        #define SOAGEN_OUTLINE_STATIC_CONSTEXPR_MEMBER(name)                                                           \
            SOAGEN_PUSH_WARNINGS;                                                                                      \
            SOAGEN_DISABLE_CLANG_WARNING_GE(17, "-Wdeprecated-redundant-constexpr-static-def");                        \
            SOAGEN_DETAIL_OUTLINE_STATIC_CONSTEXPR_MEMBER_DEF(name);                                                   \
            SOAGEN_POP_WARNINGS
    #endif
#endif

#ifndef SOAGEN_ADDRESS_OF
    #if SOAGEN_CLANG || SOAGEN_GCC || SOAGEN_HAS_BUILTIN(__builtin_addressof)
        #define SOAGEN_ADDRESS_OF(...)       __builtin_addressof(__VA_ARGS__)
        #define SOAGEN_HAS_BUILTIN_ADDRESSOF 1
    #else
        #define SOAGEN_ADDRESS_OF(...) std::addressof(__VA_ARGS__)
    #endif
#endif
#ifndef SOAGEN_HAS_BUILTIN_ADDRESSOF
    #define SOAGEN_HAS_BUILTIN_ADDRESSOF 0
#endif

#ifndef SOAGEN_TYPE_PACK_ELEMENT
    #if SOAGEN_HAS_BUILTIN(__type_pack_element) || SOAGEN_CLANG >= 6 // older clang implemented __has_builtin poorly
        #define SOAGEN_TYPE_PACK_ELEMENT(Index, ...) __type_pack_element<Index, __VA_ARGS__>
    #elif SOAGEN_GCC_LIKE && SOAGEN_HAS_BUILTIN(__builtin_type_pack_element)
        #define SOAGEN_TYPE_PACK_ELEMENT(Index, ...) __builtin_type_pack_element(Index, __VA_ARGS__)
    #endif
#endif
#undef SOAGEN_HAS_TYPE_PACK_ELEMENT
#ifdef SOAGEN_TYPE_PACK_ELEMENT
    #define SOAGEN_HAS_TYPE_PACK_ELEMENT 1
#else
    #define SOAGEN_HAS_TYPE_PACK_ELEMENT 0
#endif

#ifndef SOAGEN_DEFINE_BUILD_SUPPORT_FLAG
    #define SOAGEN_DEFINE_BUILD_SUPPORT_FLAG(name, value)                                                              \
        SOAGEN_INLINE_CONSTEXPR_VARIABLE                                                                               \
        bool SOAGEN_CONCAT(has_, name) = !!(value)

#endif

#ifndef SOAGEN_TRY
    #if SOAGEN_HAS_EXCEPTIONS
        #define SOAGEN_TRY try
    #else
        #define SOAGEN_TRY
    #endif
#endif

#ifndef SOAGEN_THROW
    #if SOAGEN_HAS_EXCEPTIONS
        #define SOAGEN_THROW(...) throw __VA_ARGS__
    #else
        #define SOAGEN_THROW(...) std::terminate()
    #endif
#endif
