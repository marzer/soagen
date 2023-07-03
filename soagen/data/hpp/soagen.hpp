//----------------------------------------------------------------------------------------------------------------------
//
// soagen.hpp
// https://github.com/marzer/soagen
// SPDX-License-Identifier: MIT
//
//----------------------------------------------------------------------------------------------------------------------
//     !!!!! THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY !!!!!
//----------------------------------------------------------------------------------------------------------------------
//
// MIT License
//
// Copyright (c) Mark Gillard
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
// the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//----------------------------------------------------------------------------------------------------------------------
#ifndef SOAGEN_HPP
#define SOAGEN_HPP

//********  preprocessor.hpp  ******************************************************************************************

#ifndef SOAGEN_CPP
	#ifdef _MSVC_LANG
		#if _MSVC_LANG > __cplusplus
			#define SOAGEN_CPP _MSVC_LANG
		#endif
	#endif
	#ifndef SOAGEN_CPP
		#define SOAGEN_CPP __cplusplus
	#endif
	#if SOAGEN_CPP >= 202600L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 26
	#elif SOAGEN_CPP >= 202300L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 23
	#elif SOAGEN_CPP >= 202002L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 20
	#elif SOAGEN_CPP >= 201703L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 17
	#elif SOAGEN_CPP >= 201402L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 14
	#elif SOAGEN_CPP >= 201103L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 11
	#else
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 0
	#endif
#endif

#ifndef SOAGEN_MAKE_VERSION
	#define SOAGEN_MAKE_VERSION(major, minor, patch) (((major)*10000) + ((minor)*100) + ((patch)))
#endif

#ifndef SOAGEN_INTELLISENSE
	#ifdef __INTELLISENSE__
		#define SOAGEN_INTELLISENSE 1
	#else
		#define SOAGEN_INTELLISENSE 0
	#endif
#endif

#ifndef SOAGEN_DOXYGEN
	#if defined(DOXYGEN) || defined(__DOXYGEN) || defined(__DOXYGEN__) || defined(__doxygen__) || defined(__POXY__)    \
		|| defined(__poxy__)
		#define SOAGEN_DOXYGEN 1
	#else
		#define SOAGEN_DOXYGEN 0
	#endif
#endif

#ifndef SOAGEN_CLANG
	#ifdef __clang__
		#define SOAGEN_CLANG __clang_major__
	#else
		#define SOAGEN_CLANG 0
	#endif

	// special handling for apple clang; see:
	// - https://github.com/marzer/tomlplusplus/issues/189
	// - https://en.wikipedia.org/wiki/Xcode
	// -
	// https://stackoverflow.com/questions/19387043/how-can-i-reliably-detect-the-version-of-clang-at-preprocessing-time
	#if SOAGEN_CLANG && defined(__apple_build_version__)
		#undef SOAGEN_CLANG
		#define SOAGEN_CLANG_VERSION SOAGEN_MAKE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
		#if SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(15, 0, 0)
			#define SOAGEN_CLANG 16
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(14, 3, 0)
			#define SOAGEN_CLANG 15
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(14, 0, 0)
			#define SOAGEN_CLANG 14
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(13, 1, 6)
			#define SOAGEN_CLANG 13
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(13, 0, 0)
			#define SOAGEN_CLANG 12
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(12, 0, 5)
			#define SOAGEN_CLANG 11
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(12, 0, 0)
			#define SOAGEN_CLANG 10
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(11, 0, 3)
			#define SOAGEN_CLANG 9
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(11, 0, 0)
			#define SOAGEN_CLANG 8
		#elif SOAGEN_CLANG_VERSION >= SOAGEN_MAKE_VERSION(10, 0, 1)
			#define SOAGEN_CLANG 7
		#else
			#define SOAGEN_CLANG 6 // not strictly correct but doesn't matter below this
		#endif
		#undef SOAGEN_CLANG_VERSION
	#endif
#endif

#ifndef SOAGEN_ICC
	#ifdef __INTEL_COMPILER
		#define SOAGEN_ICC __INTEL_COMPILER
		#ifdef __ICL
			#define SOAGEN_ICC_CL SOAGEN_ICC
		#else
			#define SOAGEN_ICC_CL 0
		#endif
	#else
		#define SOAGEN_ICC	  0
		#define SOAGEN_ICC_CL 0
	#endif
#endif

#ifndef SOAGEN_MSVC_LIKE
	#ifdef _MSC_VER
		#define SOAGEN_MSVC_LIKE _MSC_VER
	#else
		#define SOAGEN_MSVC_LIKE 0
	#endif
#endif

#ifndef SOAGEN_MSVC
	#if SOAGEN_MSVC_LIKE && !SOAGEN_CLANG && !SOAGEN_ICC
		#define SOAGEN_MSVC SOAGEN_MSVC_LIKE
	#else
		#define SOAGEN_MSVC 0
	#endif
#endif

#ifndef SOAGEN_GCC_LIKE
	#ifdef __GNUC__
		#define SOAGEN_GCC_LIKE __GNUC__
	#else
		#define SOAGEN_GCC_LIKE 0
	#endif
#endif

#ifndef SOAGEN_GCC
	#if SOAGEN_GCC_LIKE && !SOAGEN_CLANG && !SOAGEN_ICC
		#define SOAGEN_GCC SOAGEN_GCC_LIKE
	#else
		#define SOAGEN_GCC 0
	#endif
#endif

#ifndef SOAGEN_CUDA
	#if defined(__CUDACC__) || defined(__CUDA_ARCH__) || defined(__CUDA_LIBDEVICE__)
		#define SOAGEN_CUDA 1
	#else
		#define SOAGEN_CUDA 0
	#endif
#endif

#ifndef SOAGEN_ARCH_ITANIUM
	#if defined(__ia64__) || defined(__ia64) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64)
		#define SOAGEN_ARCH_ITANIUM 1
		#define SOAGEN_ARCH_BITNESS 64
	#else
		#define SOAGEN_ARCH_ITANIUM 0
	#endif
#endif

#ifndef SOAGEN_ARCH_AMD64
	#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64)
		#define SOAGEN_ARCH_AMD64	1
		#define SOAGEN_ARCH_BITNESS 64
	#else
		#define SOAGEN_ARCH_AMD64 0
	#endif
#endif

#ifndef SOAGEN_ARCH_X86
	#if defined(__i386__) || defined(_M_IX86)
		#define SOAGEN_ARCH_X86		1
		#define SOAGEN_ARCH_BITNESS 32
	#else
		#define SOAGEN_ARCH_X86 0
	#endif
#endif

#ifndef SOAGEN_ARCH_ARM
	#if defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64) || defined(_M_ARM64) || defined(__ARM_64BIT_STATE)         \
		|| defined(_M_ARM64EC)
		#define SOAGEN_ARCH_ARM32	0
		#define SOAGEN_ARCH_ARM64	1
		#define SOAGEN_ARCH_ARM		1
		#define SOAGEN_ARCH_BITNESS 64
	#elif defined(__arm__) || defined(_M_ARM) || defined(__ARM_32BIT_STATE)
		#define SOAGEN_ARCH_ARM32	1
		#define SOAGEN_ARCH_ARM64	0
		#define SOAGEN_ARCH_ARM		1
		#define SOAGEN_ARCH_BITNESS 32
	#else
		#define SOAGEN_ARCH_ARM32 0
		#define SOAGEN_ARCH_ARM64 0
		#define SOAGEN_ARCH_ARM	  0
	#endif
#endif

#ifndef SOAGEN_ARCH_BITNESS
	#define SOAGEN_ARCH_BITNESS 0
#endif

#ifndef SOAGEN_ARCH_X64
	#if SOAGEN_ARCH_BITNESS == 64
		#define SOAGEN_ARCH_X64 1
	#else
		#define SOAGEN_ARCH_X64 0
	#endif
#endif

#ifdef _WIN32
	#define SOAGEN_WINDOWS 1
#else
	#define SOAGEN_WINDOWS 0
#endif
#ifdef __unix__
	#define SOAGEN_UNIX 1
#else
	#define SOAGEN_UNIX 0
#endif
#ifdef __linux__
	#define SOAGEN_LINUX 1
#else
	#define SOAGEN_LINUX 0
#endif

#ifndef SOAGEN_HAS_INCLUDE
	#ifdef __has_include
		#define SOAGEN_HAS_INCLUDE(header)		__has_include(header)
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

#ifndef SOAGEN_HAS_ATTR
	#ifdef __has_attribute
		#define SOAGEN_HAS_ATTR(attr) __has_attribute(attr)
	#else
		#define SOAGEN_HAS_ATTR(attr) 0
	#endif
#endif

#ifndef SOAGEN_HAS_CPP_ATTR
	#ifdef __has_cpp_attribute
		#define SOAGEN_HAS_CPP_ATTR(attr) __has_cpp_attribute(attr)
	#else
		#define SOAGEN_HAS_CPP_ATTR(attr) 0
	#endif
#endif

#ifndef SOAGEN_ATTR
	#if SOAGEN_CLANG || SOAGEN_GCC_LIKE
		#define SOAGEN_ATTR(...) __attribute__((__VA_ARGS__))
	#else
		#define SOAGEN_ATTR(...)
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
	#elif SOAGEN_CLANG || SOAGEN_GCC_LIKE || SOAGEN_HAS_ATTR(__always_inline__)
		#define SOAGEN_ALWAYS_INLINE                                                                                   \
			SOAGEN_ATTR(__always_inline__)                                                                             \
			inline
	#else
		#define SOAGEN_ALWAYS_INLINE inline
	#endif
#endif

#ifndef SOAGEN_NEVER_INLINE
	#if SOAGEN_MSVC_LIKE
		#define SOAGEN_NEVER_INLINE SOAGEN_DECLSPEC(noinline)
	#elif SOAGEN_CUDA // https://gitlab.gnome.org/GNOME/glib/-/issues/2555
		#define SOAGEN_NEVER_INLINE SOAGEN_ATTR(noinline)
	#else
		#if SOAGEN_GCC || SOAGEN_CLANG || SOAGEN_HAS_ATTR(__noinline__)
			#define SOAGEN_NEVER_INLINE SOAGEN_ATTR(__noinline__)
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
		#define SOAGEN_NODISCARD	   [[nodiscard]]
		#define SOAGEN_NODISCARD_CLASS [[nodiscard]]
	#elif SOAGEN_CLANG || SOAGEN_GCC_LIKE || SOAGEN_HAS_ATTR(__warn_unused_result__)
		#define SOAGEN_NODISCARD SOAGEN_ATTR(__warn_unused_result__)
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

// clang-format off
#ifndef SOAGEN_PURE_GETTER
	#define SOAGEN_INLINE_GETTER				SOAGEN_NODISCARD	SOAGEN_ALWAYS_INLINE
	#ifdef NDEBUG
		#define SOAGEN_PURE					SOAGEN_DECLSPEC(noalias)	SOAGEN_ATTR(pure)
		#define SOAGEN_CONST					SOAGEN_DECLSPEC(noalias)	SOAGEN_ATTR(const)
		#define SOAGEN_PURE_GETTER				SOAGEN_NODISCARD						SOAGEN_PURE
		#define SOAGEN_CONST_GETTER			SOAGEN_NODISCARD						SOAGEN_CONST
		#define SOAGEN_PURE_INLINE_GETTER		SOAGEN_NODISCARD	SOAGEN_ALWAYS_INLINE	SOAGEN_PURE
		#define SOAGEN_CONST_INLINE_GETTER		SOAGEN_NODISCARD	SOAGEN_ALWAYS_INLINE	SOAGEN_CONST
	#else
		#define SOAGEN_PURE
		#define SOAGEN_CONST
		#define SOAGEN_PURE_GETTER				SOAGEN_NODISCARD
		#define SOAGEN_CONST_GETTER			SOAGEN_NODISCARD
		#define SOAGEN_PURE_INLINE_GETTER		SOAGEN_NODISCARD	SOAGEN_ALWAYS_INLINE
		#define SOAGEN_CONST_INLINE_GETTER		SOAGEN_NODISCARD	SOAGEN_ALWAYS_INLINE
	#endif
#endif
// clang-format on

#ifndef SOAGEN_NO_UNIQUE_ADDRESS
	#if SOAGEN_MSVC >= 1929
		#define SOAGEN_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
	#elif SOAGEN_CPP >= 20 && SOAGEN_HAS_CPP_ATTR(no_unique_address) >= 201803
		#define SOAGEN_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#else
		#define SOAGEN_NO_UNIQUE_ADDRESS
	#endif
#endif

#ifndef SOAGEN_COLUMN
	#define SOAGEN_COLUMN(I)                                                                                           \
		SOAGEN_PURE_INLINE_GETTER                                                                                      \
		SOAGEN_ATTR(returns_nonnull)
#endif

#ifndef SOAGEN_ALIGNED_COLUMN
	#define SOAGEN_ALIGNED_COLUMN(I)                                                                                   \
		SOAGEN_COLUMN(I)                                                                                               \
		SOAGEN_ATTR(assume_aligned(soagen::detail::actual_column_alignment<table_traits, allocator_type, I>))
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 202002L
	#define SOAGEN_CPP20_CONSTEXPR constexpr
#else
	#define SOAGEN_CPP20_CONSTEXPR
#endif

#ifndef SOAGEN_CONCEPTS
	#if defined(__cpp_concepts) && __cpp_concepts >= 201907
		#define SOAGEN_CONCEPTS 1
	#else
		#define SOAGEN_CONCEPTS 0
	#endif
#endif

#ifndef SOAGEN_REQUIRES
	#if !SOAGEN_DOXYGEN && SOAGEN_CONCEPTS
		#define SOAGEN_REQUIRES(...) requires(__VA_ARGS__)
	#else
		#define SOAGEN_REQUIRES(...)
	#endif
#endif

#ifndef SOAGEN_STD_CONCEPT
	#if !SOAGEN_DOXYGEN && defined(__cpp_lib_concepts) && __cpp_lib_concepts >= 202002
		#define SOAGEN_STD_CONCEPT(...) __VA_ARGS__
	#else
		#define SOAGEN_STD_CONCEPT(...) true
	#endif
#endif

#ifndef SOAGEN_ENABLE_IF
	#if !SOAGEN_DOXYGEN
		#define SOAGEN_ENABLE_IF(...) , typename std::enable_if<!!(__VA_ARGS__), int>::type = 0
	#else
		#define SOAGEN_ENABLE_IF(...)
	#endif
#endif

#ifndef SOAGEN_CONSTRAINED_TEMPLATE
	#if !SOAGEN_DOXYGEN
		#define SOAGEN_CONSTRAINED_TEMPLATE(condition, ...)                                                            \
			template <__VA_ARGS__ SOAGEN_ENABLE_IF(!!(condition))>                                                     \
			SOAGEN_REQUIRES(!!(condition))
	#else
		#define SOAGEN_CONSTRAINED_TEMPLATE(condition, ...) template <__VA_ARGS__>
	#endif
#endif

#ifndef SOAGEN_HIDDEN_CONSTRAINT
	#if !SOAGEN_DOXYGEN
		#define SOAGEN_HIDDEN_CONSTRAINT(condition, ...) SOAGEN_CONSTRAINED_TEMPLATE(condition, __VA_ARGS__)
	#else
		#define SOAGEN_HIDDEN_CONSTRAINT(condition, ...)
	#endif
#endif

#ifndef SOAGEN_CONSTRAINED_COLUMN
	#define SOAGEN_CONSTRAINED_COLUMN(I, ...)                                                                          \
		SOAGEN_CONSTRAINED_TEMPLATE(sfinae_col_idx == (I) && (__VA_ARGS__), size_t sfinae_col_idx = (I))
#endif
#ifndef SOAGEN_CONSTRAINED_COLUMN
	#define SOAGEN_CONSTRAINED_COLUMN(I, ...)
#endif

#ifndef SOAGEN_PUSH_WARNINGS
	#if SOAGEN_CLANG
		#define SOAGEN_PUSH_WARNINGS                                                                                   \
			_Pragma("clang diagnostic push")                                                                           \
			static_assert(true)
	#elif SOAGEN_MSVC || SOAGEN_ICC
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
	#elif SOAGEN_MSVC || SOAGEN_ICC
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

#ifndef SOAGEN_DISABLE_SWITCH_WARNINGS
	#if SOAGEN_CLANG
		#define SOAGEN_DISABLE_SWITCH_WARNINGS                                                                         \
			_Pragma("clang diagnostic ignored \"-Wswitch\"")                                                           \
			_Pragma("clang diagnostic ignored \"-Wcovered-switch-default\"")                                           \
			static_assert(true)
	#elif SOAGEN_MSVC
		#define SOAGEN_DISABLE_SWITCH_WARNINGS                                                                         \
			__pragma(warning(disable : 4061))                                                                          \
			__pragma(warning(disable : 4062))                                                                          \
			__pragma(warning(disable : 4063))                                                                          \
			__pragma(warning(disable : 4468))  /* 'fallthrough': attribute must be followed by a case label */         \
			__pragma(warning(disable : 5262))  /* implicit through */                                                  \
			__pragma(warning(disable : 26819)) /* cg: unannotated fallthrough */                                       \
			static_assert(true)
	#elif SOAGEN_GCC
		#define SOAGEN_DISABLE_SWITCH_WARNINGS                                                                         \
			_Pragma("GCC diagnostic ignored \"-Wswitch\"")                                                             \
			_Pragma("GCC diagnostic ignored \"-Wswitch-enum\"")                                                        \
			_Pragma("GCC diagnostic ignored \"-Wswitch-default\"")                                                     \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_SWITCH_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_DISABLE_LIFETIME_WARNINGS
	#if SOAGEN_CLANG
		#define SOAGEN_DISABLE_LIFETIME_WARNINGS                                                                       \
			_Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")                                       \
			_Pragma("clang diagnostic ignored \"-Wglobal-constructors\"")                                              \
			_Pragma("clang diagnostic ignored \"-Wexit-time-destructors\"")                                            \
			static_assert(true)
	#elif SOAGEN_GCC
		#if SOAGEN_GCC >= 8
			#define SOAGEN_DISABLE_LIFETIME_WARNINGS_GCC_8                                                             \
				_Pragma("GCC diagnostic ignored \"-Wclass-memaccess\"")                                                \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_LIFETIME_WARNINGS_GCC_8 static_assert(true)
		#endif
		#define SOAGEN_DISABLE_LIFETIME_WARNINGS                                                                       \
			_Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")                                         \
			_Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")                                                \
			_Pragma("GCC diagnostic ignored \"-Wuninitialized\"")                                                      \
			SOAGEN_DISABLE_LIFETIME_WARNINGS_GCC_8;                                                                    \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_LIFETIME_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_DISABLE_ARITHMETIC_WARNINGS
	#if SOAGEN_CLANG
		#if SOAGEN_CLANG >= 10
			#define SOAGEN_DISABLE_ARITHMETIC_WARNINGS_CLANG_10                                                        \
				_Pragma("clang diagnostic ignored \"-Wimplicit-int-float-conversion\"")                                \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_ARITHMETIC_WARNINGS_CLANG_10 static_assert(true)
		#endif
		#define SOAGEN_DISABLE_ARITHMETIC_WARNINGS                                                                     \
			_Pragma("clang diagnostic ignored \"-Wfloat-equal\"")                                                      \
			_Pragma("clang diagnostic ignored \"-Wdouble-promotion\"")                                                 \
			_Pragma("clang diagnostic ignored \"-Wchar-subscripts\"")                                                  \
			_Pragma("clang diagnostic ignored \"-Wshift-sign-overflow\"")                                              \
			SOAGEN_DISABLE_ARITHMETIC_WARNINGS_CLANG_10;                                                               \
			static_assert(true)
	#elif SOAGEN_MSVC
		#define SOAGEN_DISABLE_ARITHMETIC_WARNINGS                                                                     \
			__pragma(warning(disable : 4365)) /* argument signed/unsigned mismatch */                                  \
			__pragma(warning(disable : 4738)) /* storing 32-bit float result in memory */                              \
			__pragma(warning(disable : 5219)) /* implicit conversion from integral to float */                         \
			static_assert(true)
	#elif SOAGEN_GCC
		#define SOAGEN_DISABLE_ARITHMETIC_WARNINGS                                                                     \
			_Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")                                                        \
			_Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")                                                    \
			_Pragma("GCC diagnostic ignored \"-Wchar-subscripts\"")                                                    \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_ARITHMETIC_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_DISABLE_SHADOW_WARNINGS
	#if SOAGEN_CLANG
		#define SOAGEN_DISABLE_SHADOW_WARNINGS                                                                         \
			_Pragma("clang diagnostic ignored \"-Wshadow\"")                                                           \
			_Pragma("clang diagnostic ignored \"-Wshadow-field\"")                                                     \
			static_assert(true)
	#elif SOAGEN_MSVC
		#define SOAGEN_DISABLE_SHADOW_WARNINGS                                                                         \
			__pragma(warning(disable : 4458))                                                                          \
			static_assert(true)
	#elif SOAGEN_GCC
		#define SOAGEN_DISABLE_SHADOW_WARNINGS                                                                         \
			_Pragma("GCC diagnostic ignored \"-Wshadow\"")                                                             \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_SHADOW_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_DISABLE_SPAM_WARNINGS
	#if SOAGEN_CLANG
		#if SOAGEN_CLANG >= 8
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_8                                                               \
				_Pragma("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")                                   \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_8 static_assert(true)
		#endif
		#if SOAGEN_CLANG >= 9
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_9                                                               \
				_Pragma("clang diagnostic ignored \"-Wctad-maybe-unsupported\"")                                       \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_9 static_assert(true)
		#endif
		#if SOAGEN_CLANG >= 13
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_13                                                              \
				_Pragma("clang diagnostic ignored \"-Wc++20-compat\"")                                                 \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_13 static_assert(true)
		#endif
		#define SOAGEN_DISABLE_SPAM_WARNINGS                                                                           \
			_Pragma("clang diagnostic ignored \"-Wc++98-compat-pedantic\"")                                            \
			_Pragma("clang diagnostic ignored \"-Wc++98-compat\"")                                                     \
			_Pragma("clang diagnostic ignored \"-Wweak-vtables\"")                                                     \
			_Pragma("clang diagnostic ignored \"-Wdouble-promotion\"")                                                 \
			_Pragma("clang diagnostic ignored \"-Wweak-template-vtables\"")                                            \
			_Pragma("clang diagnostic ignored \"-Wpadded\"")                                                           \
			_Pragma("clang diagnostic ignored \"-Wc++2a-compat\"")                                                     \
			_Pragma("clang diagnostic ignored \"-Wtautological-pointer-compare\"")                                     \
			_Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")                                       \
			_Pragma("clang diagnostic ignored \"-Wpacked\"")                                                           \
			_Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"")                                         \
			_Pragma("clang diagnostic ignored \"-Wused-but-marked-unused\"")                                           \
			_Pragma("clang diagnostic ignored \"-Wcovered-switch-default\"")                                           \
			_Pragma("clang diagnostic ignored \"-Wtautological-pointer-compare\"")                                     \
			SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_8;                                                                      \
			SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_9;                                                                      \
			SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_13;                                                                     \
			static_assert(true)
	#elif SOAGEN_MSVC
		#define SOAGEN_DISABLE_SPAM_WARNINGS                                                                             \
			__pragma(warning(disable : 4127))  /* conditional expr is constant */                                        \
			__pragma(warning(disable : 4324))  /* structure was padded due to alignment specifier */                     \
			__pragma(warning(disable : 4348))                                                                            \
			__pragma(warning(disable : 4464))  /* relative include path contains '..' */                                 \
			__pragma(warning(disable : 4505))  /* unreferenced local function removed */                                 \
			__pragma(warning(disable : 4514))  /* unreferenced inline function has been removed */                       \
			__pragma(warning(disable : 4582))  /* constructor is not implicitly called */                                \
			__pragma(warning(disable : 4619))  /* there is no warning number 'XXXX' */                                   \
			__pragma(warning(disable : 4623))  /* default constructor was implicitly defined as deleted */               \
			__pragma(warning(disable : 4625))  /* copy constructor was implicitly defined as deleted */                  \
			__pragma(warning(disable : 4626))  /* assignment operator was implicitly defined as deleted */               \
			__pragma(warning(disable : 4686))  /* possible change in behavior, change in UDT return calling convention   \
												* \     \                                                                \
												* \                                                                    \ \
												* \                                                                      \
												*/                                                                       \
			__pragma(warning(disable : 4710))  /* function not inlined */                                                \
			__pragma(warning(disable : 4711))  /* function selected for automatic expansion */                           \
			__pragma(warning(disable : 4820))  /* N bytes padding added */                                               \
			__pragma(warning(disable : 4866))  /* compiler may not enforce left-to-right evaluation order for call */    \
			__pragma(warning(disable : 4946))  /* reinterpret_cast used between related classes */                       \
			__pragma(warning(disable : 5026))  /* move constructor was implicitly defined as deleted */                  \
			__pragma(warning(disable : 5027))  /* move assignment operator was implicitly defined as deleted */          \
			__pragma(warning(disable : 5039))  /* potentially throwing function passed to 'extern "C"' function */       \
			__pragma(warning(disable : 5045))  /* Compiler will insert Spectre mitigation */                             \
			__pragma(warning(disable : 5246))  /* initialization of a subobject should be wrapped in braces */           \
			__pragma(warning(disable : 5264))  /* const variable is not used (false-positive) */                         \
			__pragma(warning(disable : 26490)) /* cg: dont use reinterpret_cast */                                       \
			__pragma(warning(disable : 26812)) /* cg: Prefer 'enum class' over 'enum' */                                 \
			__pragma(warning(disable : 4848))  /* msvc::no_unique_address in C++17 is a vendor extension */              \
			static_assert(true)
	#elif SOAGEN_ICC
		#define SOAGEN_DISABLE_SPAM_WARNINGS                                                                           \
			__pragma(warning(disable : 82))	  /* storage class is not first */                                         \
			__pragma(warning(disable : 111))  /* statement unreachable (false-positive) */                             \
			__pragma(warning(disable : 869))  /* unreferenced parameter */                                             \
			__pragma(warning(disable : 1011)) /* missing return (false-positive) */                                    \
			__pragma(warning(disable : 2261)) /* assume expr side-effects discarded */                                 \
			static_assert(true)
	#elif SOAGEN_GCC
		#if SOAGEN_GCC >= 9
			#define SOAGEN_DISABLE_SPAM_WARNINGS_GCC_9                                                                 \
				_Pragma("GCC diagnostic ignored \"-Wattributes\"")                                                     \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_SPAM_WARNINGS_GCC_9 static_assert(true)
		#endif
		#if SOAGEN_GCC >= 12
			#define SOAGEN_DISABLE_SPAM_WARNINGS_GCC_12                                                                \
				_Pragma("GCC diagnostic ignored \"-Winterference-size\"")                                              \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_SPAM_WARNINGS_GCC_12 static_assert(true)
		#endif
		#define SOAGEN_DISABLE_SPAM_WARNINGS                                                                           \
			_Pragma("GCC diagnostic ignored \"-Wpadded\"")                                                             \
			_Pragma("GCC diagnostic ignored \"-Wcast-align\"")                                                         \
			_Pragma("GCC diagnostic ignored \"-Wcomment\"")                                                            \
			_Pragma("GCC diagnostic ignored \"-Wsubobject-linkage\"")                                                  \
			_Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")                                                       \
			_Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")                                         \
			_Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")                                                \
			_Pragma("GCC diagnostic ignored \"-Wtype-limits\"")                                                        \
			SOAGEN_DISABLE_SPAM_WARNINGS_GCC_9;                                                                        \
			SOAGEN_DISABLE_SPAM_WARNINGS_GCC_12;                                                                       \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_SPAM_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_DISABLE_DEPRECATION_WARNINGS
	#if SOAGEN_MSVC
		#define SOAGEN_DISABLE_DEPRECATION_WARNINGS                                                                    \
			__pragma(warning(disable : 4996))                                                                          \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_DEPRECATION_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS
	#if SOAGEN_MSVC
		#if SOAGEN_HAS_INCLUDE(<CodeAnalysis/Warnings.h>)
			#pragma warning(push, 0)
			#include <CodeAnalysis/Warnings.h>
			#pragma warning(pop)
			#define SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS                                                              \
				__pragma(warning(disable : ALL_CODE_ANALYSIS_WARNINGS))                                                \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS static_assert(true)
		#endif
	#else
		#define SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_DISABLE_SUGGESTION_WARNINGS
	#if SOAGEN_GCC
		#define SOAGEN_DISABLE_SUGGESTION_WARNINGS                                                                     \
			_Pragma("GCC diagnostic ignored \"-Wsuggest-attribute=const\"")                                            \
			_Pragma("GCC diagnostic ignored \"-Wsuggest-attribute=pure\"")                                             \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_SUGGESTION_WARNINGS static_assert(true)
	#endif
#endif

// backcompat
#ifndef SOAGEN_DISABLE_SUGGEST_WARNINGS
	#define SOAGEN_DISABLE_SUGGEST_WARNINGS SOAGEN_DISABLE_SUGGESTION_WARNINGS
#endif

#ifndef SOAGEN_DISABLE_WARNINGS
	#if SOAGEN_CLANG
		#define SOAGEN_DISABLE_WARNINGS                                                                                \
			SOAGEN_PUSH_WARNINGS;                                                                                      \
			_Pragma("clang diagnostic ignored \"-Weverything\"")                                                       \
			static_assert(true, "")
	#elif SOAGEN_MSVC
		#define SOAGEN_DISABLE_WARNINGS                                                                                \
			__pragma(warning(push, 0))                                                                                 \
			__pragma(warning(disable : 4348))                                                                          \
			__pragma(warning(disable : 4668))                                                                          \
			__pragma(warning(disable : 5105))                                                                          \
			SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS;                                                                     \
			SOAGEN_DISABLE_SWITCH_WARNINGS;                                                                            \
			SOAGEN_DISABLE_SHADOW_WARNINGS;                                                                            \
			SOAGEN_DISABLE_DEPRECATION_WARNINGS;                                                                       \
			SOAGEN_DISABLE_SPAM_WARNINGS;                                                                              \
			SOAGEN_DISABLE_ARITHMETIC_WARNINGS;                                                                        \
			static_assert(true)
	#elif SOAGEN_ICC
		#define SOAGEN_DISABLE_WARNINGS                                                                                \
			__pragma(warning(push, 0))                                                                                 \
			static_assert(true)
	#elif SOAGEN_GCC
		#define SOAGEN_DISABLE_WARNINGS                                                                                \
			SOAGEN_PUSH_WARNINGS;                                                                                      \
			_Pragma("GCC diagnostic ignored \"-Wall\"")                                                                \
			_Pragma("GCC diagnostic ignored \"-Wextra\"")                                                              \
			_Pragma("GCC diagnostic ignored \"-Wpedantic\"")                                                           \
			SOAGEN_DISABLE_SWITCH_WARNINGS;                                                                            \
			SOAGEN_DISABLE_LIFETIME_WARNINGS;                                                                          \
			SOAGEN_DISABLE_ARITHMETIC_WARNINGS;                                                                        \
			SOAGEN_DISABLE_SHADOW_WARNINGS;                                                                            \
			SOAGEN_DISABLE_SUGGEST_WARNINGS;                                                                           \
			SOAGEN_DISABLE_SPAM_WARNINGS;                                                                              \
			static_assert(true)
	#else
		#define SOAGEN_DISABLE_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_ENABLE_WARNINGS
	#if SOAGEN_CLANG || SOAGEN_MSVC || SOAGEN_ICC || SOAGEN_GCC
		#define SOAGEN_ENABLE_WARNINGS                                                                                 \
			SOAGEN_POP_WARNINGS;                                                                                       \
			static_assert(true)
	#else
		#define SOAGEN_ENABLE_WARNINGS static_assert(true)
	#endif
#endif

#ifndef SOAGEN_HAS_CONSTEVAL
	#if defined(__cpp_consteval) && __cpp_consteval >= 201811 && (!SOAGEN_MSVC || SOAGEN_MSVC >= 1934)                 \
		&& (!SOAGEN_CLANG || SOAGEN_CLANG >= 15)
		#define SOAGEN_HAS_CONSTEVAL 1
	#else
		#ifdef SOAGEN_DOXYGEN
			#define SOAGEN_HAS_CONSTEVAL SOAGEN_DOXYGEN
		#else
			#define SOAGEN_HAS_CONSTEVAL 0
		#endif
	#endif
#endif

#ifndef SOAGEN_CONSTEVAL
	#if SOAGEN_HAS_CONSTEVAL
		#define SOAGEN_CONSTEVAL consteval
	#else
		#define SOAGEN_CONSTEVAL constexpr
	#endif
#endif

#ifndef SOAGEN_HAS_IF_CONSTEVAL
	#if defined(__cpp_if_consteval) && __cpp_if_consteval >= 202106
		#define SOAGEN_HAS_IF_CONSTEVAL 1
	#else
		#define SOAGEN_HAS_IF_CONSTEVAL 0
	#endif
#endif

#ifndef SOAGEN_IF_CONSTEVAL
	#if SOAGEN_HAS_IF_CONSTEVAL
		#define SOAGEN_IF_CONSTEVAL if consteval
		#define SOAGEN_IF_RUNTIME	if !consteval
	#else
		#define SOAGEN_IF_CONSTEVAL if (::soagen::is_constant_evaluated())
		#define SOAGEN_IF_RUNTIME	if (!::soagen::is_constant_evaluated())
	#endif
#endif

#ifndef SOAGEN_ASSERT
	#ifdef NDEBUG
		#define SOAGEN_ASSERT(cond) static_cast<void>(0)
	#else
		#ifndef assert
SOAGEN_DISABLE_WARNINGS;
			#include <cassert>
SOAGEN_ENABLE_WARNINGS;
		#endif
		#define SOAGEN_ASSERT(cond) assert(cond)
	#endif
#endif
#ifdef NDEBUG
	// ensure any overrides respect NDEBUG
	#undef SOAGEN_ASSERT
	#define SOAGEN_ASSERT(cond) static_cast<void>(0)
#endif

#ifndef SOAGEN_ASSUME
	#if SOAGEN_MSVC_LIKE
		#define SOAGEN_ASSUME(expr) __assume(expr)
	#elif SOAGEN_ICC || SOAGEN_CLANG || SOAGEN_HAS_BUILTIN(__builtin_assume)
		#define SOAGEN_ASSUME(expr) __builtin_assume(expr)
	#elif SOAGEN_HAS_CPP_ATTR(assume) >= 202207
		#define SOAGEN_ASSUME(expr) [[assume(expr)]]
	#elif SOAGEN_HAS_ATTR(__assume__)
		#define SOAGEN_ASSUME(expr) __attribute__((__assume__(expr)))
	#else
		#define SOAGEN_ASSUME(expr) static_cast<void>(0)
	#endif
#endif

#ifndef SOAGEN_CONSTEXPR_SAFE_ASSERT
	#ifdef NDEBUG
		#define SOAGEN_CONSTEXPR_SAFE_ASSERT(cond) static_cast<void>(0)
	#else
		#define SOAGEN_CONSTEXPR_SAFE_ASSERT(cond)                                                                     \
			do                                                                                                         \
			{                                                                                                          \
				if constexpr (SOAGEN_HAS_IF_CONSTEVAL || ::soagen::build::supports_is_constant_evaluated)              \
				{                                                                                                      \
					SOAGEN_IF_RUNTIME                                                                                  \
					{                                                                                                  \
						SOAGEN_ASSERT(cond);                                                                           \
					}                                                                                                  \
				}                                                                                                      \
			}                                                                                                          \
			while (false)
	#endif
#endif

#ifndef NDEBUG
	#undef SOAGEN_ASSUME
	#define SOAGEN_ASSUME(cond) SOAGEN_CONSTEXPR_SAFE_ASSERT(cond)
#endif

#ifndef SOAGEN_ALWAYS_OPTIMIZE
	#define SOAGEN_ALWAYS_OPTIMIZE 1
#endif

#ifndef SOAGEN_DELETE_COPY
	#define SOAGEN_DELETE_COPY(T)                                                                                      \
		T(const T&)			   = delete;                                                                               \
		T& operator=(const T&) = delete
#endif

#ifndef SOAGEN_DELETE_MOVE
	#define SOAGEN_DELETE_MOVE(T)                                                                                      \
		T(T&&)			  = delete;                                                                                    \
		T& operator=(T&&) = delete
#endif

#ifndef SOAGEN_DEFAULT_COPY
	#define SOAGEN_DEFAULT_COPY(T)                                                                                     \
		T(const T&)			   = default;                                                                              \
		T& operator=(const T&) = default
#endif

#ifndef SOAGEN_DEFAULT_MOVE
	#define SOAGEN_DEFAULT_MOVE(T)                                                                                     \
		T(T&&)			  = default;                                                                                   \
		T& operator=(T&&) = default
#endif

#ifndef SOAGEN_DEFAULT_RULE_OF_FOUR
	#define SOAGEN_DEFAULT_RULE_OF_FOUR(T)                                                                             \
		SOAGEN_DEFAULT_COPY(T);                                                                                        \
		SOAGEN_DEFAULT_MOVE(T)
#endif

#ifndef SOAGEN_DEFAULT_RULE_OF_FIVE
	#define SOAGEN_DEFAULT_RULE_OF_FIVE(T)                                                                             \
		T() = default;                                                                                                 \
		SOAGEN_DEFAULT_RULE_OF_FOUR(T)
#endif

//********  core.hpp  **************************************************************************************************

SOAGEN_DISABLE_WARNINGS;
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <numeric>
#include <type_traits>
#include <new>
#include <utility>
#include <memory>
#if SOAGEN_CPP >= 20 && defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806
	#include <bit>
#endif
SOAGEN_ENABLE_WARNINGS;

SOAGEN_PUSH_WARNINGS;
SOAGEN_DISABLE_SPAM_WARNINGS;

#if SOAGEN_MSVC_LIKE
	#pragma push_macro("min")
	#pragma push_macro("max")
	#undef min
	#undef max
#endif

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma inline_recursion(on)
		#pragma optimize("gt", on)
		#pragma runtime_checks("", off)
		#pragma strict_gs_check(push, off)
	#elif SOAGEN_GCC
		#pragma GCC push_options
		#pragma GCC optimize("O2")
	#endif
#endif

#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
	#define SOAGEN_LAUNDER(...) std::launder(__VA_ARGS__)
#elif SOAGEN_CLANG >= 8 || SOAGEN_GCC >= 7 || SOAGEN_ICC >= 1910 || SOAGEN_MSVC >= 1914                                \
	|| SOAGEN_HAS_BUILTIN(__builtin_launder)
	#define SOAGEN_LAUNDER(...) __builtin_launder(__VA_ARGS__)
#else
	#define SOAGEN_LAUNDER(...) __VA_ARGS__
#endif

//--- typedefs and type traits -----------------------------------------------------------------------------------------

namespace soagen
{
	using std::size_t;
	using std::ptrdiff_t;
	using std::intptr_t;
	using std::uintptr_t;
	using std::nullptr_t;

	template <typename T>
	using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename T>
	inline constexpr bool is_cv = !std::is_same_v<std::remove_cv_t<T>, T>;

	template <typename T>
	inline constexpr bool is_cvref = !std::is_same_v<remove_cvref<T>, T>;

	template <typename T>
	inline constexpr bool is_integer = std::is_integral_v<T> && !std::is_same_v<T, bool>;

	template <typename... T>
	inline constexpr bool all_integer = (!!sizeof...(T) && ... && is_integer<T>);

	template <typename T>
	inline constexpr bool is_unsigned = is_integer<T> && std::is_unsigned_v<T>;

	template <typename T, typename... U>
	inline constexpr bool any_same = (false || ... || std::is_same_v<T, U>);

	template <auto Value>
	using index_tag = std::integral_constant<size_t, static_cast<size_t>(Value)>;

#if SOAGEN_HAS_BUILTIN(__type_pack_element)

	template <size_t I, typename... T>
	using type_at_index = __type_pack_element<I, T...>;

#else

	namespace detail
	{
		template <size_t I, typename T, typename... U>
		struct type_at_index_impl
		{
			using type = typename type_at_index_impl<I - 1, U...>::type;
		};

		template <typename T, typename... U>
		struct type_at_index_impl<0, T, U...>
		{
			using type = T;
		};
	}
	template <size_t I, typename... T>
	using type_at_index = typename detail::type_at_index_impl<I, T...>::type;

#endif

	template <typename T>
	inline constexpr bool is_trivially_manifestable =
		(std::is_class_v<remove_cvref<T>> || std::is_union_v<remove_cvref<T>>) //
		&&std::is_empty_v<remove_cvref<T>>									   //
		&& std::is_trivially_default_constructible_v<remove_cvref<T>>		   //
		&& std::is_trivially_destructible_v<remove_cvref<T>>;

	namespace detail
	{
		template <template <typename...> typename Trait, typename Enabler, typename... Args>
		struct is_detected_impl : std::false_type
		{};
		template <template <typename...> typename Trait, typename... Args>
		struct is_detected_impl<Trait, std::void_t<Trait<Args...>>, Args...> : std::true_type
		{};
		template <template <typename...> typename Trait, typename... Args>
		inline constexpr auto is_detected_ = is_detected_impl<Trait, void, Args...>::value;
	}
	template <template <typename...> typename Trait, typename... Args>
	inline constexpr auto is_detected = detail::is_detected_<Trait, Args...>;

	namespace detail
	{
		template <typename T>
		using has_swap_member_ = decltype(std::declval<T&>().swap(std::declval<T&>()));

		template <typename T>
		using has_resize_member_ = decltype(std::declval<T&>().resize(size_t{}));
	}

	template <typename T>
	inline constexpr bool has_swap_member = is_detected<detail::has_swap_member_, T>;

	template <typename T>
	inline constexpr bool has_resize_member = is_detected<detail::has_resize_member_, T>;

	namespace detail
	{
		template <typename T, bool = has_swap_member<T>>
		struct has_nothrow_swap_member_ : std::bool_constant<noexcept(std::declval<T&>().swap(std::declval<T&>()))>
		{};
		template <typename T>
		struct has_nothrow_swap_member_<T, false> : std::false_type
		{};

		template <typename T, bool = has_resize_member<T>>
		struct has_nothrow_resize_member_ : std::bool_constant<noexcept(std::declval<T&>().resize(size_t{}))>
		{};
		template <typename T>
		struct has_nothrow_resize_member_<T, false> : std::false_type
		{};
	}

	template <typename T>
	inline constexpr bool has_nothrow_swap_member = detail::has_nothrow_swap_member_<T>::value;

	template <typename T>
	inline constexpr bool has_nothrow_resize_member = detail::has_nothrow_resize_member_<T>::value;

	template <typename T>
	inline constexpr bool is_soa = false;
}

//--- functions --------------------------------------------------------------------------------------------------------

namespace soagen
{
	SOAGEN_CONST_INLINE_GETTER
	constexpr bool is_constant_evaluated() noexcept
	{
#if SOAGEN_HAS_IF_CONSTEVAL

		if consteval
		{
			return true;
		}
		else
		{
			return false;
		}

#elif SOAGEN_CLANG >= 9 || SOAGEN_GCC >= 9 || SOAGEN_MSVC >= 1925 || SOAGEN_HAS_BUILTIN(is_constant_evaluated)

		return __builtin_is_constant_evaluated();

#elif defined(__cpp_lib_is_constant_evaluated) && __cpp_lib_is_constant_evaluated >= 201811

		return std::is_constant_evaluated();

#else

		return false;

#endif
	}

	namespace build
	{
		inline constexpr bool supports_is_constant_evaluated = is_constant_evaluated();
	}

	template <typename T, typename... U>
	SOAGEN_PURE_GETTER
	constexpr const T& min(const T& val1, const T& val2, const U&... vals) noexcept
	{
		if constexpr (sizeof...(vals) == 0u)
		{
			return val1 < val2 ? val1 : val2;
		}
		else if constexpr (sizeof...(vals) == 2u)
		{
			return soagen::min(soagen::min(val1, val2), soagen::min(vals...));
		}
		else
		{
			return soagen::min(soagen::min(val1, val2), vals...);
		}
	}

	template <typename T, typename... U>
	SOAGEN_PURE_GETTER
	constexpr const T& max(const T& val1, const T& val2, const U&... vals) noexcept
	{
		if constexpr (sizeof...(vals) == 0u)
		{
			return val1 < val2 ? val2 : val1;
		}
		else if constexpr (sizeof...(vals) == 2u)
		{
			return soagen::max(soagen::max(val1, val2), soagen::max(vals...));
		}
		else
		{
			return soagen::max(soagen::max(val1, val2), vals...);
		}
	}

	SOAGEN_CONSTRAINED_TEMPLATE((all_integer<T, U, V...>), typename T, typename U, typename... V)
	SOAGEN_CONST_GETTER
	constexpr std::common_type_t<T, U, V...> lcm(T val1, U val2, V... vals) noexcept
	{
		if constexpr (sizeof...(vals) == 0u)
		{
			return std::lcm(val1, val2);
		}
		else if constexpr (sizeof...(vals) == 2u)
		{
			return std::lcm(std::lcm(val1, val2), std::lcm(vals...));
		}
		else
		{
			return soagen::lcm(std::lcm(val1, val2), vals...);
		}
	}

	SOAGEN_CONSTRAINED_TEMPLATE(is_unsigned<T>, typename T)
	SOAGEN_CONST_GETTER
	constexpr bool has_single_bit(T val) noexcept
	{
		static_assert(!is_cvref<T>);

		if constexpr (std::is_enum_v<T>)
			return has_single_bit(static_cast<std::underlying_type_t<T>>(val));
		else
		{
			return val != T{} && (val & (val - T{ 1 })) == T{};
		}
	}

#if SOAGEN_CPP >= 20 && defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806

	#define SOAGEN_HAS_INTRINSIC_BIT_CAST 1

	using std::bit_cast;

#else

	SOAGEN_CONSTRAINED_TEMPLATE((std::is_trivially_copyable_v<From>	 //
								 && std::is_trivially_copyable_v<To> //
								 && sizeof(From) == sizeof(To)),
								typename To,
								typename From)
	SOAGEN_PURE_INLINE_GETTER
	constexpr To bit_cast(const From& from) noexcept
	{
		static_assert(!std::is_reference_v<To> && !std::is_reference_v<From>);

	#if SOAGEN_CLANG >= 11 || SOAGEN_GCC >= 11 || SOAGEN_MSVC >= 1926                                                  \
		|| (!SOAGEN_CLANG && !SOAGEN_GCC && SOAGEN_HAS_BUILTIN(__builtin_bit_cast))

		#define SOAGEN_HAS_INTRINSIC_BIT_CAST 1
		return __builtin_bit_cast(To, from);

	#else

		#define SOAGEN_HAS_INTRINSIC_BIT_CAST 0

		if constexpr (std::is_same_v<std::remove_cv_t<From>, std::remove_cv_t<To>>)
		{
			return from;
		}
		else if constexpr (!std::is_nothrow_default_constructible_v<std::remove_cv_t<To>>)
		{
			union proxy_t
			{
				alignas(To) unsigned char dummy[sizeof(To)];
				std::remove_cv_t<To> to;

				proxy_t() noexcept
				{}
			};

			proxy_t proxy;
			std::memcpy(&proxy.to, &from, sizeof(To));
			return proxy.to;
		}
		else
		{
			static_assert(std::is_nothrow_default_constructible_v<std::remove_cv_t<To>>,
						  "Bit-cast fallback requires the To type be nothrow default-constructible");

			std::remove_cv_t<To> to;
			std::memcpy(&to, &from, sizeof(To));
			return to;
		}
	#endif
	}

#endif

#if SOAGEN_CPP >= 20 && defined(__cpp_lib_assume_aligned) && __cpp_lib_assume_aligned >= 201811

	using std::assume_aligned;

#else

	template <size_t N, typename T>
	SOAGEN_CONST_INLINE_GETTER
	SOAGEN_ATTR(assume_aligned(N))
	constexpr T* assume_aligned(T* ptr) noexcept
	{
		static_assert(N > 0 && (N & (N - 1u)) == 0u, "assume_aligned() requires a power-of-two alignment value.");
		static_assert(!std::is_function_v<T>, "assume_aligned may not be used on functions.");

		SOAGEN_ASSUME((reinterpret_cast<uintptr_t>(ptr) & (N - uintptr_t{ 1 })) == 0);

		if constexpr (std::is_volatile_v<T>)
		{
			return static_cast<T*>(soagen::assume_aligned<N>(const_cast<std::remove_volatile_t<T>*>(ptr)));
		}
		else
		{
	#if SOAGEN_CLANG || SOAGEN_GCC || SOAGEN_HAS_BUILTIN(__builtin_assume_aligned)

			return static_cast<T*>(__builtin_assume_aligned(ptr, N));

	#elif SOAGEN_MSVC

			if constexpr (N < 16384)
				return static_cast<T*>(__builtin_assume_aligned(ptr, N));
			else
				return ptr;

	#elif SOAGEN_ICC

			__assume_aligned(ptr, N);
			return ptr;

	#else

			return ptr;

	#endif
		}
	}

#endif
}

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma strict_gs_check(pop)
		#pragma runtime_checks("", restore)
		#pragma optimize("", on)
		#pragma inline_recursion(off)
	#elif SOAGEN_GCC
		#pragma GCC pop_options
	#endif
#endif

#if SOAGEN_MSVC_LIKE
	#pragma pop_macro("min")
	#pragma pop_macro("max")
#endif

SOAGEN_POP_WARNINGS;

//********  compressed_pair.hpp  ***************************************************************************************

SOAGEN_PUSH_WARNINGS;
SOAGEN_DISABLE_SPAM_WARNINGS;

#if SOAGEN_MSVC_LIKE
	#pragma push_macro("min")
	#pragma push_macro("max")
	#undef min
	#undef max
#endif

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma inline_recursion(on)
		#pragma optimize("gt", on)
		#pragma runtime_checks("", off)
		#pragma strict_gs_check(push, off)
	#elif SOAGEN_GCC
		#pragma GCC push_options
		#pragma GCC optimize("O2")
	#endif
#endif

namespace soagen
{
	namespace detail
	{
#define SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(T, name, expression)                                                       \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr T& name()& noexcept                                                                                      \
	{                                                                                                                  \
		return static_cast<T&>(expression);                                                                            \
	}                                                                                                                  \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr const T& name() const& noexcept                                                                          \
	{                                                                                                                  \
		return static_cast<const T&>(expression);                                                                      \
	}                                                                                                                  \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr T&& name()&& noexcept                                                                                    \
	{                                                                                                                  \
		return static_cast<T&&>(expression);                                                                           \
	}                                                                                                                  \
	SOAGEN_PURE_INLINE_GETTER                                                                                          \
	constexpr const T&& name() const&& noexcept                                                                        \
	{                                                                                                                  \
		return static_cast<const T&&>(expression);                                                                     \
	}                                                                                                                  \
	static_assert(true)

		// primary template - neither element can be a base
		template <typename First,
				  typename Second,
				  bool FirstCanBeBase  = std::is_empty_v<First> && !std::is_final_v<First>,
				  bool SecondCanBeBase = std::is_empty_v<Second> && !std::is_final_v<Second>>
		class compressed_pair_base
		{
		  private:
			static_assert(!FirstCanBeBase);
			static_assert(!SecondCanBeBase);
			SOAGEN_NO_UNIQUE_ADDRESS First first_;
			SOAGEN_NO_UNIQUE_ADDRESS Second second_;

		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: first_{ static_cast<F&&>(first_init) },
				  second_{ static_cast<S&&>(second_init) }
			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, first_);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, second_);
		};

		// secondary template - First is a base
		template <typename First, typename Second>
		class SOAGEN_EMPTY_BASES compressed_pair_base<First, Second, true, false> //
			: private First
		{
		  private:
			SOAGEN_NO_UNIQUE_ADDRESS Second second_;

		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: First{ static_cast<F&&>(first_init) },
				  second_{ static_cast<S&&>(second_init) }
			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, *this);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, second_);
		};

		// secondary template - Second is a base
		template <typename First, typename Second>
		class SOAGEN_EMPTY_BASES compressed_pair_base<First, Second, false, true> //
			: private Second
		{
		  private:
			SOAGEN_NO_UNIQUE_ADDRESS First first_;

		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: Second{ static_cast<S&&>(second_init) },
				  first_{ static_cast<F&&>(first_init) }

			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, first_);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, *this);
		};

		// secondary template - both are bases
		template <typename First, typename Second>
		class SOAGEN_EMPTY_BASES compressed_pair_base<First, Second, true, true> //
			: private First, private Second
		{
		  public:
			compressed_pair_base() = default;
			SOAGEN_DEFAULT_MOVE(compressed_pair_base);
			SOAGEN_DEFAULT_COPY(compressed_pair_base);

			SOAGEN_CONSTRAINED_TEMPLATE((std::is_constructible_v<First, F&&> && std::is_constructible_v<Second, S&&>),
										typename F,
										typename S)
			constexpr compressed_pair_base(F&& first_init, S&& second_init) //
				noexcept(std::is_nothrow_constructible_v<First, F&&>&& std::is_nothrow_constructible_v<Second, S&&>)
				: First{ static_cast<F&&>(first_init) },
				  Second{ static_cast<S&&>(second_init) }
			{}

			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(First, first, *this);
			SOAGEN_COMPRESSED_PAIR_BASE_GETTERS(Second, second, *this);
		};

#undef SOAGEN_COMPRESSED_PAIR_BASE_DEFAULTS
#undef SOAGEN_COMPRESSED_PAIR_BASE_GETTERS
	}
	template <typename First, typename Second>
	class SOAGEN_EMPTY_BASES compressed_pair //
		: public detail::compressed_pair_base<First, Second>
	{
	  private:
		using base = detail::compressed_pair_base<First, Second>;

	  public:
		using first_type  = First;
		using second_type = Second;

		SOAGEN_NODISCARD_CTOR
		compressed_pair() = default;

		SOAGEN_NODISCARD_CTOR
		compressed_pair(const compressed_pair&) = default;

		SOAGEN_NODISCARD_CTOR
		compressed_pair(compressed_pair&&) = default;

		compressed_pair& operator=(const compressed_pair&) = default;

		compressed_pair& operator=(compressed_pair&&) = default;

#if SOAGEN_DOXYGEN
#else
		using detail::compressed_pair_base<First, Second>::compressed_pair_base; // inherit constructor
#endif

	  private:
		template <size_t I, typename T>
		SOAGEN_PURE_INLINE_GETTER
		static constexpr decltype(auto) do_get(T&& cp) noexcept
		{
			static_assert(I <= 1);
			if constexpr (I == 0)
				return static_cast<T&&>(cp).first();
			else
				return static_cast<T&&>(cp).second();
		}

	  public:
		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr auto& get() & noexcept
		{
			return do_get<I>(*this);
		}

		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr auto&& get() && noexcept
		{
			return do_get<I>(static_cast<compressed_pair&&>(*this));
		}

		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const auto& get() const& noexcept
		{
			return do_get<I>(*this);
		}

		template <size_t I>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const auto&& get() const&& noexcept
		{
			return do_get<I>(static_cast<const compressed_pair&&>(*this));
		}

		SOAGEN_HIDDEN_CONSTRAINT(std::is_swappable_v<F>&& std::is_swappable_v<S>,
								 typename F = First,
								 typename S = Second)
		void swap(compressed_pair& other) noexcept(std::is_nothrow_swappable_v<F>&& std::is_nothrow_swappable_v<S>)
		{
			using std::swap;
			swap(base::first(), other.first());
			swap(base::second(), other.second());
		}
	};

	template <typename F, typename S>
	compressed_pair(F&&, S&&) -> compressed_pair<remove_cvref<F>, remove_cvref<S>>;

	SOAGEN_CONSTRAINED_TEMPLATE(std::is_swappable_v<F>&& std::is_swappable_v<S>, typename F, typename S)
	SOAGEN_ALWAYS_INLINE
	void swap(compressed_pair<F, S>& lhs,
			  compressed_pair<F, S>& rhs) noexcept(std::is_nothrow_swappable_v<F>&& std::is_nothrow_swappable_v<S>)
	{
		lhs.swap(rhs);
	}
}

namespace std
{
	template <typename First, typename Second>
	struct tuple_size<soagen::compressed_pair<First, Second>>
	{
		static constexpr size_t value = 2;
	};

	template <size_t I, typename First, typename Second>
	struct tuple_element<I, soagen::compressed_pair<First, Second>>
	{
		static_assert(I < 2);
		using type = std::conditional_t<I == 1, Second, First>;
	};
}

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma strict_gs_check(pop)
		#pragma runtime_checks("", restore)
		#pragma optimize("", on)
		#pragma inline_recursion(off)
	#elif SOAGEN_GCC
		#pragma GCC pop_options
	#endif
#endif

#if SOAGEN_MSVC_LIKE
	#pragma pop_macro("min")
	#pragma pop_macro("max")
#endif

SOAGEN_POP_WARNINGS;

//********  allocator.hpp  *********************************************************************************************

SOAGEN_PUSH_WARNINGS;
SOAGEN_DISABLE_SPAM_WARNINGS;

#if SOAGEN_MSVC_LIKE
	#pragma push_macro("min")
	#pragma push_macro("max")
	#undef min
	#undef max
#endif

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma inline_recursion(on)
		#pragma optimize("gt", on)
		#pragma runtime_checks("", off)
		#pragma strict_gs_check(push, off)
	#elif SOAGEN_GCC
		#pragma GCC push_options
		#pragma GCC optimize("O2")
	#endif
#endif

#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

namespace soagen
{
	struct allocator
	{
		using value_type							 = std::byte;
		using pointer								 = value_type*;
		using const_pointer							 = const value_type*;
		using void_pointer							 = std::byte*;
		using const_void_pointer					 = const std::byte*;
		using size_type								 = size_t;
		using difference_type						 = ptrdiff_t;
		using is_always_equal						 = std::true_type;
		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap			 = std::false_type;

		static constexpr size_t min_alignment =
			max(size_t{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ }, alignof(std::max_align_t), alignof(value_type));

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		value_type* allocate(size_t size, std::align_val_t alignment = std::align_val_t{ min_alignment })
		{
#if SOAGEN_WINDOWS
			auto ptr = _aligned_malloc(size, max(static_cast<size_t>(alignment), min_alignment));
#else
			auto ptr = std::aligned_alloc(max(static_cast<size_t>(alignment), min_alignment), size);
#endif
			if (!ptr)
				throw std::bad_alloc{};

			return soagen::assume_aligned<min_alignment>(static_cast<pointer>(ptr));
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_ATTR(nonnull)
		void deallocate(value_type* ptr, size_t /* size */) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);

#if SOAGEN_WINDOWS
			_aligned_free(ptr);
#else
			std::free(ptr);
#endif
		}

	  private:
		SOAGEN_CONST_INLINE_GETTER
		friend bool operator==(const allocator&, const allocator&) noexcept
		{
			return true;
		}

		SOAGEN_CONST_INLINE_GETTER
		friend bool operator!=(const allocator&, const allocator&) noexcept
		{
			return false;
		}
	};
	static_assert(std::is_trivially_default_constructible_v<allocator>);
	static_assert(std::is_trivially_move_constructible_v<allocator>);
	static_assert(std::is_trivially_copy_constructible_v<allocator>);
	static_assert(std::is_trivially_move_assignable_v<allocator>);
	static_assert(std::is_trivially_copy_assignable_v<allocator>);
	static_assert(std::is_trivially_destructible_v<allocator>);
	static_assert(std::is_nothrow_swappable_v<allocator>);

	namespace detail
	{
		// can we specify alignment when allocating?

		template <typename Allocator>
		using has_aligned_allocate_ = decltype(std::declval<Allocator&>().allocate(size_t{}, std::align_val_t{}));

		template <typename Allocator>
		inline constexpr bool has_aligned_allocate = is_detected<has_aligned_allocate_, Allocator>;

		// does the allocator know it's minimum possible alignment value?

		template <typename Allocator>
		using has_min_alignment_ = decltype(Allocator::min_alignment);

		template <typename Allocator>
		inline constexpr bool has_min_alignment = is_detected<has_min_alignment_, Allocator>;

		// what is the _actual_ minimum alignment value that makes sense?

		template <typename Allocator, bool = has_min_alignment<Allocator>>
		inline constexpr size_t alloc_min_alignment =
			max(Allocator::min_alignment, alignof(typename Allocator::value_type));
		template <typename Allocator>
		inline constexpr size_t alloc_min_alignment<Allocator, false> = alignof(typename Allocator::value_type);

		// internal base type

		template <typename Allocator>
		struct allocator_traits_base : public std::allocator_traits<Allocator>
		{
			static_assert(!is_cvref<Allocator>, "allocators must not be cvref-qualified");
			static_assert(any_same<typename Allocator::value_type, std::byte, char, unsigned char>,
						  "allocators must have either std::byte, char or unsigned char as their value_type");

			using base_traits = std::allocator_traits<Allocator>;

			static constexpr size_t min_alignment = detail::alloc_min_alignment<Allocator>;
			static_assert(has_single_bit(min_alignment), "allocator min_alignment must be a power of two");

			SOAGEN_PURE_INLINE_GETTER
			static constexpr bool equal([[maybe_unused]] const Allocator& a,
										[[maybe_unused]] const Allocator& b) noexcept
			{
				if constexpr (base_traits::is_always_equal::value)
					return true;
				else
					return a == b;
			}

			//                                 always take ownership (de-allocate existing + move allocation)?
			// 1. propagating,     equal       yes
			// 2. propagating,     non-equal   yes
			// 3. non-propagating, equal       yes
			// 4. non-propagating, non-equal   no (need to re-use existing capacity + move elementwise)
			static constexpr bool container_move_assign_always_takes_ownership =
				base_traits::propagate_on_container_move_assignment::value || base_traits::is_always_equal::value;
		};
	}

	// primary template - allocator has an aligned-allocation overload
	template <typename Allocator, bool = detail::has_aligned_allocate<Allocator>>
	struct allocator_traits : public detail::allocator_traits_base<Allocator>
	{
		using base_traits = detail::allocator_traits_base<Allocator>;
		using typename base_traits::value_type;
		using typename base_traits::size_type;

		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(base_traits::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type num, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{}, std::align_val_t{})))
		{
			return soagen::assume_aligned<base_traits::min_alignment>(
				alloc.allocate(num,
							   std::align_val_t{ max(static_cast<size_t>(alignment), base_traits::min_alignment) }));
		}
	};

	// secondary template - we have to implement the alignment management manually :(:(
	template <typename Allocator>
	struct allocator_traits<Allocator, false> : public detail::allocator_traits_base<Allocator>
	{
		using base_traits = detail::allocator_traits_base<Allocator>;
		using typename base_traits::value_type;
		using typename base_traits::size_type;
		using typename base_traits::pointer;

	  private:
		struct alloc_info
		{
			size_type requested_size;
			size_type actual_size;
			pointer ptr;
		};

	  public:
		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(base_traits::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type n, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{})))
		{
			static_assert(sizeof(typename Allocator::value_type) == 1u);

			alignment = std::align_val_t{ max(static_cast<size_type>(alignment), base_traits::min_alignment) };

			const size_type offset = (static_cast<size_type>(alignment) - 1u) + sizeof(alloc_info);
			pointer ptr			   = alloc.allocate(n + offset);
			SOAGEN_ASSUME(ptr != nullptr);

			alloc_info* info = reinterpret_cast<alloc_info*>((reinterpret_cast<uintptr_t>(ptr) + offset)
															 & ~(static_cast<size_type>(alignment) - 1u));
			info[-1]		 = { n, n + offset, ptr };
			return soagen::assume_aligned<base_traits::min_alignment>(reinterpret_cast<pointer>(info));
		}

		// note that this hides std::allocator_traits::deallocate - this is intentional
		SOAGEN_ATTR(nonnull)
		static constexpr void deallocate(Allocator& alloc, value_type* ptr, [[maybe_unused]] size_type n) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);

			const auto info = reinterpret_cast<alloc_info*>(ptr)[-1];
			SOAGEN_ASSERT(n == info.requested_size);

			alloc.deallocate(info.ptr, info.actual_size);
		}
	};
}

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma strict_gs_check(pop)
		#pragma runtime_checks("", restore)
		#pragma optimize("", on)
		#pragma inline_recursion(off)
	#elif SOAGEN_GCC
		#pragma GCC pop_options
	#endif
#endif

#if SOAGEN_MSVC_LIKE
	#pragma pop_macro("min")
	#pragma pop_macro("max")
#endif

SOAGEN_POP_WARNINGS;

//********  column_traits.hpp  *****************************************************************************************

SOAGEN_PUSH_WARNINGS;
SOAGEN_DISABLE_SPAM_WARNINGS;

#if SOAGEN_MSVC_LIKE
	#pragma push_macro("min")
	#pragma push_macro("max")
	#undef min
	#undef max
#endif

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma inline_recursion(on)
		#pragma optimize("gt", on)
		#pragma runtime_checks("", off)
		#pragma strict_gs_check(push, off)
	#elif SOAGEN_GCC
		#pragma GCC push_options
		#pragma GCC optimize("O2")
	#endif
#endif

#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

namespace soagen::detail
{
	// a base class for the column traits that handles all the non-alignment-dependent stuff
	// (to minimize template instantiation explosion)
	template <typename StorageType>
	struct column_traits_base
	{
		using storage_type = StorageType;
		static_assert(!is_cvref<storage_type>, "column storage_type may not be cvref-qualified");
		static_assert(!std::is_void_v<storage_type>, "column storage_type may not be void");
		static_assert(std::is_destructible_v<storage_type>, "column storage_type must be destructible");

		//--- dereferencing --------------------------------------------------------------------------------------------

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& get(std::byte* ptr) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);

			return *SOAGEN_LAUNDER(reinterpret_cast<storage_type*>(soagen::assume_aligned<alignof(storage_type)>(ptr)));
		}

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr const storage_type& get(const std::byte* ptr) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);

			return *SOAGEN_LAUNDER(
				reinterpret_cast<const storage_type*>(soagen::assume_aligned<alignof(storage_type)>(ptr)));
		}

		//--- construction ---------------------------------------------------------------------------------------------

		template <typename... Args>
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct(std::byte* destination, Args&&... args) //
			noexcept(std::is_nothrow_constructible_v<storage_type, Args&&...>)
		{
			SOAGEN_ASSUME(destination != nullptr);

			if constexpr (std::is_aggregate_v<storage_type>)
			{
				return *(::new (static_cast<void*>(soagen::assume_aligned<alignof(storage_type)>(destination)))
							 storage_type{ static_cast<Args&&>(args)... });
			}
			else
			{
				return *(::new (static_cast<void*>(soagen::assume_aligned<alignof(storage_type)>(destination)))
							 storage_type(static_cast<Args&&>(args)...));
			}
		}

		template <typename... Args>
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct_at(std::byte* buffer, size_t element_index, Args&&... args) //
			noexcept(std::is_nothrow_constructible_v<storage_type, Args&&...>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			return construct(buffer + element_index * sizeof(storage_type), static_cast<Args&&>(args)...);
		}

		//--- move-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_move_constructible =
			std::is_move_constructible_v<storage_type>
			|| (std::is_default_constructible_v<storage_type> && std::is_move_assignable_v<storage_type>);

		static constexpr bool is_nothrow_move_constructible = std::is_move_constructible_v<storage_type>
																? std::is_nothrow_move_constructible_v<storage_type>
																: (std::is_nothrow_default_constructible_v<storage_type>
																   && std::is_nothrow_move_assignable_v<storage_type>);

		static constexpr bool is_trivially_move_constructible =
			std::is_move_constructible_v<storage_type> ? std::is_trivially_move_constructible_v<storage_type>
													   : (std::is_trivially_default_constructible_v<storage_type>
														  && std::is_trivially_move_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_move_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<storage_type&&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_move_assignable_v<storage_type>);

				construct(destination);

				if constexpr (std::is_nothrow_move_assignable_v<storage_type>)
				{
					return move_assign(destination, source);
				}
				else
				{
					try
					{
						return move_assign(destination, source);
					}
					catch (...)
					{
						destruct(destination);
						throw;
					}
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_move_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& move_construct_at(std::byte* dest_buffer,
														 size_t dest_element_index,
														 std::byte* source_buffer,
														 size_t source_element_index) //
			noexcept(std::is_nothrow_move_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_construct(dest_buffer + dest_element_index * sizeof(storage_type),
								  source_buffer + source_element_index * sizeof(storage_type));
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_copy_constructible =
			std::is_copy_constructible_v<storage_type>
			|| (std::is_default_constructible_v<storage_type> && std::is_copy_assignable_v<storage_type>);

		static constexpr bool is_nothrow_copy_constructible = std::is_copy_constructible_v<storage_type>
																? std::is_nothrow_copy_constructible_v<storage_type>
																: (std::is_nothrow_default_constructible_v<storage_type>
																   && std::is_nothrow_copy_assignable_v<storage_type>);

		static constexpr bool is_trivially_copy_constructible =
			std::is_copy_constructible_v<storage_type> ? std::is_trivially_copy_constructible_v<storage_type>
													   : (std::is_trivially_default_constructible_v<storage_type>
														  && std::is_trivially_copy_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_copy_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<const storage_type&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_copy_assignable_v<storage_type>);

				construct(destination);

				if constexpr (std::is_nothrow_copy_assignable_v<storage_type>)
				{
					return copy_assign(destination, source);
				}
				else
				{
					try
					{
						return copy_assign(destination, source);
					}
					catch (...)
					{
						destruct(destination);
						throw;
					}
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_constructible)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& copy_construct_at(std::byte* dest_buffer,
														 size_t dest_element_index,
														 std::byte* source_buffer,
														 size_t source_element_index) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_construct(dest_buffer + dest_element_index * sizeof(storage_type),
								  source_buffer + source_element_index * sizeof(storage_type));
		}

		//--- destruction ----------------------------------------------------------------------------------------------

		SOAGEN_ATTR(nonnull)
		static constexpr void destruct([[maybe_unused]] std::byte* target) //
			noexcept(std::is_nothrow_destructible_v<storage_type>)
		{
			SOAGEN_ASSUME(target != nullptr);

			if constexpr (!std::is_trivially_destructible_v<storage_type>)
			{
				get(target).~storage_type();
			}
		}

		SOAGEN_ATTR(nonnull)
		static constexpr void destruct_at([[maybe_unused]] std::byte* buffer,	 //
										  [[maybe_unused]] size_t element_index) //
			noexcept(std::is_nothrow_destructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			if constexpr (!std::is_trivially_destructible_v<storage_type>)
			{
				destruct(buffer + element_index * sizeof(storage_type));
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_move_assignable =
			std::is_move_assignable_v<storage_type>
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_move_constructible_v<storage_type>);

		static constexpr bool is_nothrow_move_assignable =
			std::is_move_assignable_v<storage_type>
				? std::is_nothrow_move_assignable_v<storage_type>
				: (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_move_constructible_v<storage_type>);

		static constexpr bool is_trivially_move_assignable =
			std::is_move_assignable_v<storage_type> ? std::is_trivially_move_assignable_v<storage_type>
													: (std::is_trivially_destructible_v<storage_type>
													   && std::is_trivially_move_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& move_assign(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_move_assignable_v<storage_type>)
			{
				return get(destination) = static_cast<storage_type&&>(get(source));
			}
			else
			{
				// note we only fallback to this if they're nothrow because we don't want to leave the destination
				// in a half-constructed state (it existed before the assignment, it should still exist after)
				static_assert(std::is_nothrow_destructible_v<storage_type>);
				static_assert(std::is_nothrow_move_constructible_v<storage_type>);

				destruct(destination);
				return move_construct(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& move_assign_at(std::byte* dest_buffer,
													  size_t dest_element_index,
													  std::byte* source_buffer,
													  size_t source_element_index) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_assign(dest_buffer + dest_element_index * sizeof(storage_type),
							   source_buffer + source_element_index * sizeof(storage_type));
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_copy_assignable =
			std::is_copy_assignable_v<storage_type>
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_copy_constructible_v<storage_type>);

		static constexpr bool is_nothrow_copy_assignable =
			std::is_copy_assignable_v<storage_type>
				? std::is_nothrow_copy_assignable_v<storage_type>
				: (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_copy_constructible_v<storage_type>);

		static constexpr bool is_trivially_copy_assignable =
			std::is_copy_assignable_v<storage_type> ? std::is_trivially_copy_assignable_v<storage_type>
													: (std::is_trivially_destructible_v<storage_type>
													   && std::is_trivially_copy_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& copy_assign(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_copy_assignable_v<storage_type>)
			{
				return get(destination) = static_cast<const storage_type&>(get(source));
			}
			else
			{
				// note we only fallback to this if they're nothrow because we don't want to leave the destination
				// in a half-constructed state (it existed before the assignment, it should still exist after)
				static_assert(std::is_nothrow_destructible_v<storage_type>);
				static_assert(std::is_nothrow_move_constructible_v<storage_type>);

				destruct(destination);
				return copy_construct(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& copy_assign_at(std::byte* dest_buffer,
													  size_t dest_element_index,
													  std::byte* source_buffer,
													  size_t source_element_index) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_assign(dest_buffer + dest_element_index * sizeof(storage_type),
							   source_buffer + source_element_index * sizeof(storage_type));
		}
	};

	// trait for determining the actual storage type for a column.
	// we can strip off const/volatile and coerce all pointers to be void* to reduce template instantiation burden
	template <typename ValueType>
	struct storage_type_
	{
		using type = ValueType;
	};
	template <typename T>
	struct storage_type_<T*>
	{
		using type = void*;
	};
	template <typename T>
	struct storage_type_<const T*> : public storage_type_<T*>
	{};
	template <typename T>
	struct storage_type_<volatile T*> : public storage_type_<T*>
	{};
	template <typename T>
	struct storage_type_<const volatile T*> : public storage_type_<T*>
	{};
	template <typename T>
	struct storage_type_<const T> : public storage_type_<T>
	{};
	template <typename T>
	struct storage_type_<volatile T> : public storage_type_<T>
	{};
	template <typename T>
	struct storage_type_<const volatile T> : public storage_type_<T>
	{};
	template <typename ValueType>
	using storage_type = typename detail::storage_type_<ValueType>::type;

	// trait for determining the default parameter type for a column.
	// ideally we want to pass small+fast things by value, move-only things by rvalue,
	// and everything else as const lvalue.
	template <typename ValueType,
			  bool Value = std::is_scalar_v<ValueType>		//
						|| std::is_fundamental_v<ValueType> //
						|| (std::is_trivially_copyable_v<ValueType> && sizeof(ValueType) <= sizeof(void*) * 2),
			  bool Move = !std::is_copy_constructible_v<ValueType> && std::is_move_constructible_v<ValueType>>
	struct param_type_
	{
		using type = ValueType;
	};
	template <typename ValueType>
	struct param_type_<ValueType, false, true>
	{
		using type = std::add_rvalue_reference_t<ValueType>;
	};
	template <typename ValueType>
	struct param_type_<ValueType, false, false>
	{
		using type = std::add_lvalue_reference_t<std::add_const_t<ValueType>>;
	};
}

namespace soagen
{
	template <typename ValueType>
	using param_type = typename detail::param_type_<ValueType>::type;

	template <typename ValueType,
			  typename ParamType = param_type<ValueType>,
			  size_t Align		 = alignof(ValueType),
			  typename Base		 = detail::column_traits_base<detail::storage_type<ValueType>>>
	struct column_traits : public Base
	{
		using value_type = ValueType;
		static_assert(!std::is_reference_v<value_type>, "column value_type may not be a reference");
		static_assert(!std::is_void_v<value_type>, "column value_type may not be void");
		static_assert(alignof(value_type) == alignof(typename Base::storage_type));
		static_assert(sizeof(value_type) == sizeof(typename Base::storage_type));

		using param_type = ParamType;
		static_assert(!std::is_void_v<param_type>, "column param_type may not be void");
		static_assert(std::is_convertible_v<param_type, value_type> || std::is_constructible_v<value_type, param_type>
						  || (std::is_pointer_v<param_type> && std::is_same_v<typename Base::storage_type, void*>),
					  "column value_type must be constructible or convertible from param_type");

		static constexpr size_t alignment = max(Align, alignof(value_type));
		static_assert(has_single_bit(alignment), "column alignment must be a power of two");

		static constexpr size_t max_capacity = static_cast<size_t>(-1) / sizeof(value_type);

		static constexpr size_t aligned_stride = lcm(alignment, sizeof(value_type)) / sizeof(value_type);
	};

	template <typename>
	inline constexpr bool is_column_traits = false;

	template <typename StorageType>
	inline constexpr bool is_column_traits<detail::column_traits_base<StorageType>> = true;

	template <typename ValueType, typename ParamType, size_t Align, typename Base>
	inline constexpr bool is_column_traits<column_traits<ValueType, ParamType, Align, Base>> = is_column_traits<Base>;
}

namespace soagen::detail
{
	template <typename T>
	struct to_base_traits_;

	template <typename ValueType, typename ParamType, size_t Align>
	struct to_base_traits_<column_traits<ValueType, ParamType, Align>>
	{
		using type = column_traits_base<ValueType>;
	};

	template <typename T>
	using to_base_traits = typename to_base_traits_<T>::type;
}

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma strict_gs_check(pop)
		#pragma runtime_checks("", restore)
		#pragma optimize("", on)
		#pragma inline_recursion(off)
	#elif SOAGEN_GCC
		#pragma GCC pop_options
	#endif
#endif

#if SOAGEN_MSVC_LIKE
	#pragma pop_macro("min")
	#pragma pop_macro("max")
#endif

SOAGEN_POP_WARNINGS;

//********  table_traits.hpp  ******************************************************************************************

SOAGEN_PUSH_WARNINGS;
SOAGEN_DISABLE_SPAM_WARNINGS;

#if SOAGEN_MSVC_LIKE
	#pragma push_macro("min")
	#pragma push_macro("max")
	#undef min
	#undef max
#endif

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma inline_recursion(on)
		#pragma optimize("gt", on)
		#pragma runtime_checks("", off)
		#pragma strict_gs_check(push, off)
	#elif SOAGEN_GCC
		#pragma GCC push_options
		#pragma GCC optimize("O2")
	#endif
#endif

#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

namespace soagen::detail
{
	// a base class for the table traits that handles all the non-alignment-dependent stuff
	// (to minimize template instantiation explosion)
	template <typename... Columns>
	struct table_traits_base
	{
		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		// columns

		template <size_t Index>
		using column = type_at_index<Index, Columns...>;

		template <typename IntegralConstant>
		using column_from_ic = type_at_index<IntegralConstant::value, Columns...>;

		template <size_t Index>
		using column_storage_type = typename column<Index>::storage_type;

		template <typename IntegralConstant>
		using column_storage_type_from_ic = typename column_from_ic<IntegralConstant>::storage_type;

		using column_pointers		= std::byte* [column_count];
		using const_column_pointers = std::byte* const[column_count];

		static constexpr size_t column_sizes[column_count] = { sizeof(typename Columns::storage_type)... };

		// default constructibility

		static constexpr bool all_default_constructible =
			(std::is_default_constructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_default_constructible =
			(std::is_nothrow_default_constructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_trivially_default_constructible =
			(std::is_trivially_default_constructible_v<typename Columns::storage_type> && ...);

		// copy-constructibility

		static constexpr bool all_copy_constructible = (Columns::is_copy_constructible && ...);

		static constexpr bool all_nothrow_copy_constructible = (Columns::is_nothrow_copy_constructible && ...);

		static constexpr bool all_trivially_copy_constructible = (Columns::is_trivially_copy_constructible && ...);

		// move-constructibility

		static constexpr bool all_move_constructible = (Columns::is_move_constructible && ...);

		static constexpr bool all_nothrow_move_constructible = (Columns::is_nothrow_move_constructible && ...);

		static constexpr bool all_trivially_move_constructible = (Columns::is_trivially_move_constructible && ...);

		// copy-assignability

		static constexpr bool all_copy_assignable = (Columns::is_copy_assignable && ...);

		static constexpr bool all_nothrow_copy_assignable = (Columns::is_nothrow_copy_assignable && ...);

		static constexpr bool all_trivially_copy_assignable = (Columns::is_trivially_copy_assignable && ...);

		// move-assignability

		static constexpr bool all_move_assignable = (Columns::is_move_assignable && ...);

		static constexpr bool all_nothrow_move_assignable = (Columns::is_nothrow_move_assignable && ...);

		static constexpr bool all_trivially_move_assignable = (Columns::is_trivially_move_assignable && ...);

		// destructibility

		static constexpr bool all_destructible = (std::is_destructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_destructible =
			(std::is_nothrow_destructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_trivially_destructible =
			(std::is_trivially_destructible_v<typename Columns::storage_type> && ...);

		// swappability

		static constexpr bool all_swappable = (std::is_swappable_v<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_swappable =
			(std::is_nothrow_swappable_v<typename Columns::storage_type> && ...);

		// row constructibility

		template <typename... Args>
		static constexpr bool row_constructible_from =
			sizeof...(Args) == column_count && (std::is_constructible_v<typename Columns::storage_type, Args> && ...);

		template <typename... Args>
		static constexpr bool row_nothrow_constructible_from =
			sizeof...(Args) == column_count && (std::is_constructible_v<typename Columns::storage_type, Args> && ...);

		//--- destruction ----------------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static constexpr void destruct_row([[maybe_unused]] column_pointers& columns,
										   [[maybe_unused]] size_t index,
										   [[maybe_unused]] size_t leftmost_column,
										   [[maybe_unused]] size_t rightmost_column,
										   std::index_sequence<Cols...>) noexcept
		{
			static_assert(all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			if constexpr (all_trivially_destructible)
			{
				// nothing to do in this case :)
			}
			else
			{
				const auto destructor = [&](auto ic) noexcept
				{
					static constexpr size_t column_index = column_count - decltype(ic)::value - 1u;

					if constexpr (!std::is_trivially_destructible_v<column_storage_type<column_index>>)
					{
						SOAGEN_ASSUME(columns[column_index]);
						SOAGEN_ASSUME(leftmost_column <= rightmost_column);
						SOAGEN_ASSUME(leftmost_column < column_count);
						SOAGEN_ASSUME(rightmost_column < column_count);

						if (column_index >= leftmost_column && column_index <= rightmost_column)
							column<column_index>::destruct_at(columns[column_index], index);
					}
				};

				(destructor(index_tag<Cols>{}), ...);
			}
		}

		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static constexpr void destruct_row([[maybe_unused]] column_pointers& columns,
										   [[maybe_unused]] size_t index,
										   std::index_sequence<Cols...>) noexcept
		{
			static_assert(all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			if constexpr (all_trivially_destructible)
			{
				// nothing to do in this case :)
			}
			else
			{
				const auto destructor = [&](auto ic) noexcept
				{
					static constexpr size_t column_index = column_count - decltype(ic)::value - 1u;

					if constexpr (!std::is_trivially_destructible_v<column_storage_type<column_index>>)
					{
						SOAGEN_ASSUME(columns[column_index]);

						column<column_index>::destruct_at(columns[column_index], index);
					}
				};

				(destructor(index_tag<Cols>{}), ...);
			}
		}

	  public:
		SOAGEN_ALWAYS_INLINE
		static constexpr void destruct_row(column_pointers& columns,
										   size_t index,
										   size_t leftmost_column,
										   size_t rightmost_column) noexcept
		{
			destruct_row(columns, index, leftmost_column, rightmost_column, std::make_index_sequence<column_count>{});
		}

		SOAGEN_ALWAYS_INLINE
		static constexpr void destruct_row(column_pointers& columns, size_t index) noexcept
		{
			destruct_row(columns, index, std::make_index_sequence<column_count>{});
		}

		static constexpr void destruct_rows([[maybe_unused]] column_pointers& columns,
											[[maybe_unused]] size_t start,
											[[maybe_unused]] size_t count) noexcept
		{
			static_assert(all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			if constexpr (all_trivially_destructible)
			{
				// nothing to do in this case :)
			}
			else
			{
				for (size_t i = start + count; i-- > start;)
					destruct_row(columns, i);
			}
		}

		//--- default-construction -------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_row(column_pointers& columns,
										  size_t index,
										  std::index_sequence<Cols...>) //
			noexcept(all_nothrow_default_constructible)					//
			SOAGEN_REQUIRES(all_default_constructible)
		{
			static_assert(all_default_constructible);
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (all_nothrow_default_constructible || all_trivially_destructible)
			{
				(column<Cols>::construct_at(columns[Cols], index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor = [&](auto ic) //
					noexcept(std::is_nothrow_default_constructible_v<column_storage_type<decltype(ic)::value>>)
				{
					column_from_ic<decltype(ic)>::construct_at(columns[decltype(ic)::value], index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(columns, index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_default_constructible)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_row(column_pointers& columns,
										  size_t index) //
			noexcept(all_nothrow_default_constructible) //
		{
			default_construct_row(columns, index, std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_default_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_rows([[maybe_unused]] column_pointers& columns,
										   [[maybe_unused]] size_t start,
										   [[maybe_unused]] size_t count) //
			noexcept(all_nothrow_destructible)
		{
			if constexpr (all_nothrow_default_constructible)
			{
				for (size_t i = start, e = start + count; i < e; i++)
					default_construct_row(columns, i);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = start;

				try
				{
					for (const size_t e = start + count; i < e; i++)
						default_construct_row(columns, i);
				}
				catch (...)
				{
					for (; i-- > start;)
						destruct_row(columns, i);
					throw;
				}
			}
		}

		//--- construction ---------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row(column_pointers& columns,
								  size_t index,
								  std::index_sequence<Cols...>,
								  Args&&... args) //
			noexcept(row_nothrow_constructible_from<Args&&...>)
		{
			static_assert(row_constructible_from<Args&&...>);
			static_assert(sizeof...(Cols) == sizeof...(Args));
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (row_nothrow_constructible_from<Args&&...> || all_trivially_destructible)
			{
				(column<Cols>::construct_at(columns[Cols], index, static_cast<Args&&>(args)), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic, auto&& arg) noexcept(
						std::is_nothrow_constructible_v<column_storage_type_from_ic<decltype(ic)>, decltype(arg)&&>)
				{
					column_from_ic<decltype(ic)>::construct_at(columns[decltype(ic)::value],
															   index,
															   static_cast<decltype(arg)&&>(arg));

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}, static_cast<Args&&>(args)), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(columns, index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		template <typename... Args>
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row(column_pointers& columns, size_t index, Args&&... args) //
			noexcept(row_nothrow_constructible_from<Args&&...>)
		{
			construct_row(columns, index, std::make_index_sequence<column_count>{}, static_cast<Args&&>(args)...);
		}

		//--- move-construction ----------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   column_pointers& source_columns,
									   size_t source_index,
									   std::index_sequence<Cols...>) //
			noexcept(all_nothrow_move_constructible)
		{
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (all_nothrow_move_constructible || all_trivially_destructible)
			{
				(column<Cols>::move_construct_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index),
				 ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(
						std::is_nothrow_move_constructible_v<column_storage_type_from_ic<decltype(ic)>>)
				{
					column_from_ic<decltype(ic)>::move_construct_at(dest_columns[decltype(ic)::value],
																	dest_index,
																	source_columns[decltype(ic)::value],
																	source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest_columns, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   column_pointers& source_columns,
									   size_t source_index) //
			noexcept(all_nothrow_move_constructible)
		{
			move_construct_row(dest_columns,
							   dest_index,
							   source_columns,
							   source_index,
							   std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_rows(column_pointers& dest_columns,
										size_t dest_start,
										column_pointers& source_columns,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_move_constructible)
		{
			if constexpr (all_nothrow_move_constructible)
			{
				for (size_t i = 0; i < count; i++)
					move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					for (; i < count; i++)
						move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				catch (...)
				{
					for (; i-- > 0u;)
						destruct_row(dest_columns, dest_start + i);
					throw;
				}
			}
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   const_column_pointers& source_columns,
									   size_t source_index,
									   std::index_sequence<Cols...>) //
			noexcept(all_nothrow_copy_constructible)
		{
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (all_nothrow_copy_constructible || all_trivially_destructible)
			{
				(column<Cols>::copy_construct_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index),
				 ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(
						std::is_nothrow_copy_constructible_v<column_storage_type_from_ic<decltype(ic)>>)
				{
					column_from_ic<decltype(ic)>::copy_construct_at(dest_columns[decltype(ic)::value],
																	dest_index,
																	source_columns[decltype(ic)::value],
																	source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest_columns, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   const_column_pointers& source_columns,
									   size_t source_index) //
			noexcept(all_nothrow_copy_constructible)
		{
			copy_construct_row(dest_columns,
							   dest_index,
							   source_columns,
							   source_index,
							   std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_rows(column_pointers& dest_columns,
										size_t dest_start,
										const_column_pointers& source_columns,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_copy_constructible)
		{
			if constexpr (all_nothrow_copy_constructible)
			{
				for (size_t i = 0; i < count; i++)
					copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					for (; i < count; i++)
						copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				catch (...)
				{
					for (; i-- > 0u;)
						destruct_row(dest_columns, dest_start + i);
					throw;
				}
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									column_pointers& source_columns,
									size_t source_index,
									std::index_sequence<Cols...>) //
			noexcept(all_nothrow_move_assignable)
		{
			static_assert(sizeof...(Cols) == column_count);

			// todo: how to provide a strong-exception guarantee here?

			(column<Cols>::move_assign_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									column_pointers& source_columns,
									size_t source_index) //
			noexcept(all_nothrow_move_assignable)
		{
			move_assign_row(dest_columns,
							dest_index,
							source_columns,
							source_index,
							std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_rows(column_pointers& dest_columns,
									 size_t dest_start,
									 const_column_pointers& source_columns,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_move_assignable)
		{
			// todo: how to provide a strong-exception guarantee here?

			for (size_t i = 0; i < count; i++)
				move_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									const_column_pointers& source_columns,
									size_t source_index,
									std::index_sequence<Cols...>) //
			noexcept(all_nothrow_copy_assignable)
		{
			static_assert(sizeof...(Cols) == column_count);

			// todo: how to provide a strong-exception guarantee here?

			(column<Cols>::copy_assign_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									const_column_pointers& source_columns,
									size_t source_index) //
			noexcept(all_nothrow_copy_assignable)
		{
			copy_assign_row(dest_columns,
							dest_index,
							source_columns,
							source_index,
							std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_rows(column_pointers& dest_columns,
									 size_t dest_start,
									 const_column_pointers& source_columns,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			// todo: how to provide a strong-exception guarantee here?

			for (size_t i = 0; i < count; i++)
				copy_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
		}
	};
}

namespace soagen
{
	template <typename... Columns>
	struct table_traits : public detail::table_traits_base<detail::to_base_traits<Columns>...>
	{
		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		static constexpr size_t aligned_stride = lcm(size_t{ 1 }, Columns::aligned_stride...);

		// columns
		// (note that these hide the base class typedefs - this is intentional)

		template <size_t Index>
		using column = type_at_index<Index, Columns...>;

		template <size_t Index>
		using column_type = typename column<Index>::value_type;

		template <size_t Index>
		using column_storage_type = typename column<Index>::storage_type;

		template <typename IntegralConstant>
		using column_from_ic = type_at_index<IntegralConstant::value, Columns...>;

		template <typename IntegralConstant>
		using column_type_from_ic = typename column_from_ic<IntegralConstant>::value_type;

		template <typename IntegralConstant>
		using column_storage_type_from_ic = typename column_from_ic<IntegralConstant>::storage_type;

		static constexpr size_t column_alignments[column_count] = { Columns::alignment... };

		static constexpr size_t largest_alignment = max(Columns::alignment...);
	};

	template <typename>
	inline constexpr bool is_table_traits = false;

	template <typename... Columns>
	inline constexpr bool is_table_traits<table_traits<Columns...>> = true;

	template <typename... Columns>
	inline constexpr bool is_table_traits<detail::table_traits_base<Columns...>> = true;
}

namespace soagen::detail
{
	template <typename... Columns>
	struct to_base_traits_<table_traits<Columns...>>
	{
		using type = table_traits_base<to_base_traits<Columns>...>;
	};
}

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma strict_gs_check(pop)
		#pragma runtime_checks("", restore)
		#pragma optimize("", on)
		#pragma inline_recursion(off)
	#elif SOAGEN_GCC
		#pragma GCC pop_options
	#endif
#endif

#if SOAGEN_MSVC_LIKE
	#pragma pop_macro("min")
	#pragma pop_macro("max")
#endif

SOAGEN_POP_WARNINGS;

//********  table.hpp  *************************************************************************************************

SOAGEN_PUSH_WARNINGS;
SOAGEN_DISABLE_SPAM_WARNINGS;

#if SOAGEN_MSVC_LIKE
	#pragma push_macro("min")
	#pragma push_macro("max")
	#undef min
	#undef max
#endif

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma inline_recursion(on)
		#pragma optimize("gt", on)
		#pragma runtime_checks("", off)
		#pragma strict_gs_check(push, off)
	#elif SOAGEN_GCC
		#pragma GCC push_options
		#pragma GCC optimize("O2")
	#endif
#endif

#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

namespace soagen::detail
{
	//------------------------------------------------------------------------------------------------------------------
	// generic allocation class for tracking the column pointers and the actual size in bytes
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount>
	struct table_allocation
	{
		using column_pointers		= std::byte* [ColumnCount];
		using const_column_pointers = std::byte* const[ColumnCount];

		column_pointers columns;
		size_t size_in_bytes;

		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return !!columns[0];
		}

		SOAGEN_PURE_INLINE_GETTER
		/* implicit */ constexpr operator column_pointers&() noexcept
		{
			return columns;
		}

		SOAGEN_PURE_INLINE_GETTER
		/* implicit */ constexpr operator const_column_pointers&() const noexcept
		{
			return columns;
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// base class for handling most allocation-related boilerplate
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount, typename Allocator>
	class table_base
	{
		static_assert(ColumnCount, "tables must have at least one column");
		static_assert(!is_cvref<Allocator>, "allocators may not be cvref-qualified");

	  public:
		using size_type		   = size_t;
		using difference_type  = ptrdiff_t;
		using allocator_type   = Allocator;
		using allocator_traits = soagen::allocator_traits<allocator_type>;

	  protected:
		using allocation = table_allocation<ColumnCount>;

		allocation alloc_ = {};
		size_t count_	  = {};
		compressed_pair<size_t, Allocator> capacity_;

	  public:
		SOAGEN_NODISCARD_CTOR
		explicit constexpr table_base(const Allocator& alloc) noexcept //
			: capacity_{ size_t{}, alloc }
		{
			static_assert(std::is_nothrow_copy_constructible_v<Allocator>,
						  "allocators must be nothrow copy-constructible");
		}

		SOAGEN_NODISCARD_CTOR
		constexpr table_base(table_base&& other) noexcept //
			: alloc_{ std::exchange(other.alloc_, allocation{}) },
			  count_{ std::exchange(other.count_, size_t{}) },
			  capacity_{ std::exchange(other.capacity_.first(), size_t{}), static_cast<Allocator&&>(other.allocator()) }
		{
			static_assert(std::is_nothrow_move_constructible_v<Allocator>,
						  "allocators must be nothrow move-constructible");
		}

		// conditionally-implemented in specialized child classes:
		table_base()							 = delete;
		table_base& operator=(table_base&&)		 = delete;
		table_base(const table_base&)			 = delete;
		table_base& operator=(const table_base&) = delete;

		~table_base() noexcept
		{
			static_assert(std::is_nothrow_destructible_v<Allocator>, "allocators must be nothrow destructible");

			// element destructors are run in a more specialized child class

			if (alloc_)
				deallocate(alloc_);
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t size() const noexcept
		{
			return count_;
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr bool empty() const noexcept
		{
			return !count_;
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t capacity() const noexcept
		{
			return capacity_.first();
		}

		SOAGEN_INLINE_GETTER
		constexpr Allocator get_allocator() const noexcept
		{
			return capacity_.second();
		}

	  protected:
		SOAGEN_INLINE_GETTER
		constexpr Allocator& allocator() noexcept
		{
			return capacity_.second();
		}

		SOAGEN_INLINE_GETTER
		constexpr const Allocator& allocator() const noexcept
		{
			return capacity_.second();
		}

		static constexpr bool allocate_is_nothrow =
			noexcept(allocator_traits::allocate(std::declval<Allocator&>(), size_t{}, std::align_val_t{}));

		constexpr allocation allocate(size_t n_bytes, size_t alignment) noexcept(allocate_is_nothrow)
		{
			SOAGEN_ASSUME(n_bytes);
			SOAGEN_ASSUME(has_single_bit(alignment));

			const auto do_alloc = [&]() noexcept(allocate_is_nothrow) -> auto*
			{
				return allocator_traits::allocate(allocator(),
												  n_bytes,
												  std::align_val_t{ max(alignment, allocator_traits::min_alignment) });
			};

#if SOAGEN_CPP >= 20
			// guard against allocators with incorrect pointer typedefs where possible
			using alloc_type = std::remove_reference_t<decltype(do_alloc())>;
#else
			using alloc_type = typename allocator_traits::pointer;
#endif

			// avoid doing reinterpret_cast where unnecessary (for constexpr)
			if constexpr (std::is_same_v<alloc_type, std::byte*>)
				return { { do_alloc() }, n_bytes };
			else
				return { { reinterpret_cast<std::byte*>(do_alloc()) }, n_bytes };
		}

		constexpr void deallocate(const allocation& al) noexcept
		{
			SOAGEN_ASSUME(al.columns[0]);
			SOAGEN_ASSUME(al.size_in_bytes);

			allocator_traits::deallocate(allocator(),
										 static_cast<typename allocator_traits::pointer>(al.columns[0]),
										 al.size_in_bytes);
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructibility
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount,
			  typename Allocator,
			  typename Base = table_base<ColumnCount, Allocator>,
			  bool			= std::is_default_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_default_construct //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FOUR(table_default_construct);

		SOAGEN_NODISCARD_CTOR
		constexpr table_default_construct() noexcept //
			: Base{ Allocator{} }
		{
			static_assert(std::is_nothrow_default_constructible_v<Allocator>,
						  "allocators must be nothrow default-constructible, or not default-constructible at all");
		}
	};

	template <size_t ColumnCount, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_default_construct<ColumnCount, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (un-typed base parts only)
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount,
			  typename Allocator,
			  typename Base = table_default_construct<ColumnCount, Allocator>,
			  bool			= !allocator_traits<Allocator>::propagate_on_container_move_assignment::value
				  || std::is_move_assignable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_move_assignable_base //
		: public Base
	{
	  protected:
		using allocation	   = table_allocation<ColumnCount>;
		using allocator_traits = allocator_traits<Allocator>;

		constexpr bool move_from_by_taking_ownership(table_move_assignable_base&& rhs) noexcept
		{
			SOAGEN_ASSUME(&rhs != this);

			const auto take_ownership = [&]() noexcept
			{
				if (Base::alloc_)
					Base::deallocate(Base::alloc_);

				Base::alloc_			= std::exchange(rhs.alloc_, allocation{});
				Base::count_			= std::exchange(rhs.count_, size_t{});
				Base::capacity_.first() = std::exchange(rhs.capacity_.first(), size_t{});

				if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
				{
					static_assert(std::is_nothrow_move_assignable_v<Allocator>,
								  "move-propagated allocators must be nothrow move-assignable");

					Base::allocator() = static_cast<Allocator&&>(rhs.allocator());
				}
			};

			if constexpr (allocator_traits::container_move_assign_always_takes_ownership)
			{
				take_ownership();
				return true;
			}
			else
			{
				if (allocator_traits::equal(Base::allocator(), rhs.allocator()))
				{
					take_ownership();
					return true;
				}

				// if they compare non-equal then we need to do elementwise move-assignment,
				// which can't be implemented at this untyped level - must be implemented as part of a more specialized
				// child class.

				return false;
			}
		}

	  public:
		using Base::Base;

		table_move_assignable_base()											 = default;
		table_move_assignable_base(table_move_assignable_base&&)				 = default;
		table_move_assignable_base(const table_move_assignable_base&)			 = default;
		table_move_assignable_base& operator=(const table_move_assignable_base&) = default;

		constexpr table_move_assignable_base& operator=(table_move_assignable_base&& rhs) noexcept
		{
			if SOAGEN_LIKELY(&rhs != this)
				move_from_by_taking_ownership(static_cast<table_move_assignable_base&&>(rhs));
			return *this;
		}
	};

	template <size_t ColumnCount, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_move_assignable_base<ColumnCount, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assignable_base);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: swap
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount,
			  typename Allocator,
			  typename Base = table_move_assignable_base<ColumnCount, Allocator>,
			  bool = !allocator_traits<Allocator>::propagate_on_container_swap::value || std::is_swappable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_swap //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);

		constexpr void swap(table_swap& other) noexcept
		{
			if SOAGEN_UNLIKELY(&other == this)
				return;

			using allocator_traits = soagen::allocator_traits<Allocator>;

			if constexpr (!allocator_traits::propagate_on_container_swap::value
						  && !allocator_traits::is_always_equal::value)
			{
				SOAGEN_ASSERT(Base::allocator() == other.allocator()
							  && "allocators must compare equal in a non-propagating swap");
			}

			using std::swap;
			swap(Base::alloc_, other.alloc_);
			swap(Base::count_, other.count_);
			swap(Base::capacity_.first(), other.capacity_.first());

			if constexpr (allocator_traits::propagate_on_container_swap::value)
			{
				static_assert(std::is_nothrow_swappable_v<Allocator>,
							  "swap-propagated allocators must be nothrow-swappable");

				swap(Base::allocator(), other.allocator());
			}
		}
	};

	template <size_t ColumnCount, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_swap<ColumnCount, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: typed base
	//
	// note: specializations from here down take the full TableTraits, not just the ColumnCount
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits,
			  typename Allocator,
			  typename Base = table_swap<TableTraits::column_count, Allocator>>
	class SOAGEN_EMPTY_BASES table_typed_base //
		: public Base
	{
	  private:
		using allocation	 = table_allocation<TableTraits::column_count>;
		using column_offsets = size_t[TableTraits::column_count + 1u]; // +1 == end of the buffer (total size)

		constexpr allocation allocate(const column_offsets& offsets) noexcept(Base::allocate_is_nothrow)
		{
			auto alloc = Base::allocate(offsets[TableTraits::column_count], TableTraits::largest_alignment);
			SOAGEN_ASSUME(alloc.columns[0]);
			SOAGEN_ASSUME(alloc.size_in_bytes == offsets[TableTraits::column_count]);

			for (size_t i = 1; i < TableTraits::column_count; i++)
			{
				alloc.columns[i] = alloc.columns[0] + offsets[i];
				SOAGEN_ASSUME(reinterpret_cast<uintptr_t>(alloc.columns[i]) % TableTraits::column_alignments[i] == 0u);
			}

			return alloc;
		}

		static constexpr void calc_column_offsets(column_offsets& offsets, size_t capacity) noexcept
		{
			SOAGEN_ASSUME(capacity % TableTraits::aligned_stride == 0u);

			// aligned offsets:
			offsets[0] = {};
			for (size_t i = 1; i < TableTraits::column_count; i++)
			{
				offsets[i] = offsets[i - 1u] + TableTraits::column_sizes[i - 1u] * capacity;
				offsets[i] = (offsets[i] + TableTraits::column_alignments[i] - 1u) //
						   & ~(TableTraits::column_alignments[i] - 1u);

				SOAGEN_ASSUME(offsets[i] % TableTraits::column_alignments[i] == 0u);
			}

			// end of buffer:
			offsets[TableTraits::column_count] = offsets[TableTraits::column_count - 1u]
											   + TableTraits::column_sizes[TableTraits::column_count - 1u] * capacity;
		}

		SOAGEN_CPP20_CONSTEXPR
		void adjust_capacity(size_t new_capacity) noexcept(
			Base::allocate_is_nothrow
			&& (TableTraits::all_nothrow_move_constructible
				|| (TableTraits::all_nothrow_default_constructible && TableTraits::all_nothrow_move_assignable)
				|| TableTraits::all_nothrow_copy_constructible
				|| (TableTraits::all_nothrow_default_constructible && TableTraits::all_nothrow_copy_assignable)))
		{
			SOAGEN_ASSUME(new_capacity);
			SOAGEN_ASSUME(new_capacity >= Base::count_);
			SOAGEN_ASSUME(new_capacity % TableTraits::aligned_stride == 0u);

			if (new_capacity == Base::capacity())
				return;

			// get new offsets
			column_offsets new_offsets{};
			calc_column_offsets(new_offsets, new_capacity);

			// alloc + move
			auto new_alloc = allocate(new_offsets);
			if (Base::count_)
			{
				SOAGEN_ASSERT(Base::alloc_);

				// this looks a bit convoluted and is probably worth an explainer:
				// we really, really, _really_ don't want reserve() and shrink_to_fit() to throw if we can avoid it,
				// since it potentially leaves the container in a sliced state (i.e. some elements were moved already
				// when an exception is raised), so we're moving or copying the elements according to the 'most nothrow'
				// path that still fufills the brief.

				if constexpr (TableTraits::all_nothrow_move_constructible)
				{
					TableTraits::move_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else if constexpr (TableTraits::all_nothrow_default_constructible
								   && TableTraits::all_nothrow_move_assignable)
				{
					TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
					TableTraits::move_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else if constexpr (TableTraits::all_nothrow_copy_constructible)
				{
					TableTraits::copy_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else if constexpr (TableTraits::all_nothrow_default_constructible
								   && TableTraits::all_nothrow_copy_assignable)
				{
					TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
					TableTraits::copy_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else
				{
					try
					{
						if constexpr (TableTraits::all_move_constructible)
						{
							TableTraits::move_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
						else if constexpr (TableTraits::all_default_constructible && TableTraits::all_move_assignable)
						{
							TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
							TableTraits::move_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
						else if constexpr (TableTraits::all_copy_constructible)
						{
							TableTraits::copy_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
						else if constexpr (TableTraits::all_default_constructible && TableTraits::all_copy_assignable)
						{
							TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
							TableTraits::copy_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
					}
					catch (...)
					{
						Base::deallocate(new_alloc);
						throw;
					}
				}
				TableTraits::destruct_rows(Base::alloc_, {}, Base::count_);
				Base::deallocate(Base::alloc_);
			}
			Base::alloc_			= new_alloc;
			Base::capacity_.first() = new_capacity;
		}

	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_typed_base);

		~table_typed_base() noexcept
		{
			clear();
		}

		constexpr void clear() noexcept
		{
			if constexpr (!TableTraits::all_trivially_destructible)
			{
				static_assert(TableTraits::all_nothrow_destructible,
							  "column storage_types must be nothrow-destructible");

				if (Base::count_)
					TableTraits::destruct_rows(Base::alloc_, {}, Base::count_);
			}
			Base::count_ = {};
		}

		SOAGEN_CPP20_CONSTEXPR
		void reserve(size_t new_capacity) noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if (!new_capacity)
				return;

			if (const size_t rem = new_capacity % TableTraits::aligned_stride; rem > 0u)
				new_capacity += TableTraits::aligned_stride - rem;
			if (new_capacity <= Base::capacity())
				return;

			adjust_capacity(new_capacity);
		}

		SOAGEN_CPP20_CONSTEXPR
		void shrink_to_fit() noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if (!Base::count_)
			{
				if (Base::alloc_)
				{
					Base::deallocate(Base::alloc_);
					Base::alloc_			= {};
					Base::capacity_.first() = {};
				}
				return;
			}

			auto new_capacity = Base::count_;
			if (const size_t rem = new_capacity % TableTraits::aligned_stride; rem > 0u)
				new_capacity += TableTraits::aligned_stride - rem;
			if (new_capacity == Base::capacity())
				return;
			SOAGEN_ASSERT(new_capacity < Base::capacity());
			SOAGEN_ASSERT(new_capacity >= Base::count_);

			adjust_capacity(new_capacity);
		}

		SOAGEN_CPP20_CONSTEXPR
		void pop_back(size_t num = 1) noexcept
		{
			static_assert(TableTraits::all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			num = min(Base::count_, num);
			TableTraits::destruct_rows(Base::alloc_, Base::count_ - num, num);
			Base::count_ -= num;
		}

	  public:
		static constexpr size_t max_capacity = []() -> size_t
		{
			if constexpr (TableTraits::column_count == 1)
			{
				return static_cast<size_t>(-1) / TableTraits::column_sizes[0];
			}
			else
			{
				// todo: i'm sure there's a smarter way to do this

				constexpr auto capacity_ok = [](size_t cap) noexcept
				{
					constexpr auto add_without_overflowing = [](size_t lhs, size_t rhs, size_t& result) noexcept -> bool
					{
						if (lhs > static_cast<size_t>(-1) - rhs)
							return false;

						result = lhs + rhs;
						return true;
					};

					size_t buf_end = {};
					for (size_t i = 0u; i < TableTraits::column_count; i++)
					{
						if (i)
						{
							if (const size_t rem = buf_end % TableTraits::column_alignments[i - 1u]; rem > 0u)
							{
								if (!add_without_overflowing(buf_end,
															 TableTraits::column_alignments[i - 1u] - rem,
															 buf_end))
									return false;
							}
						}
						if (!add_without_overflowing(buf_end, TableTraits::column_sizes[i] * cap, buf_end))
							return false;
					}
					return true;
				};

				size_t lower  = static_cast<size_t>(-1);
				size_t higher = lower;
				while (lower && !capacity_ok(lower))
				{
					higher = lower;
					lower /= 16u;
				}
				if (!lower)
					return 0;

				while (true)
				{
					const auto midpoint = lower + (higher - lower) / 2u;
					if (capacity_ok(midpoint))
						lower = midpoint;
					else
						higher = midpoint;

					if ((higher - lower) <= 1u)
						return lower;
				}

				return 0;
			}
		}();

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t max_size() const noexcept
		{
			return max_capacity;
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructible column types
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits, //
			  typename Allocator,
			  typename Base = table_typed_base<TableTraits, Allocator>,
			  bool			= TableTraits::all_default_constructible>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);

		SOAGEN_CPP20_CONSTEXPR
		void resize(size_t num) noexcept(				   //
			TableTraits::all_nothrow_default_constructible //
				&& noexcept(this->pop_back(size_t{}))	   //
					&& noexcept(this->reserve(size_t{})))
		{
			if (Base::size() > num)
			{
				Base::pop_back(Base::size() - num);
			}
			else if (Base::size() < num)
			{
				Base::reserve(num);
				for (size_t i = Base::size(); i < num; i++)
				{
					TableTraits::default_construct_row(Base::alloc_, i);
					Base::count_++;
				}
			}
		}
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns<TableTraits, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (typed)
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits, //
			  typename Allocator,
			  typename Base = table_default_constructible_columns<TableTraits, Allocator>,
			  bool UseBase	= !std::is_move_assignable_v<Base>
						  || allocator_traits<Allocator>::container_move_assign_always_takes_ownership,
			  bool Movable = (TableTraits::all_move_assignable && TableTraits::all_move_constructible)>
	class SOAGEN_EMPTY_BASES table_move_assign //
		: public Base
	{
	  protected:
		using allocator_traits = allocator_traits<Allocator>;

	  public:
		using Base::Base;

		table_move_assign()									   = default;
		table_move_assign(table_move_assign&&)				   = default;
		table_move_assign(const table_move_assign& other)	   = default;
		table_move_assign& operator=(const table_move_assign&) = default;

		// note that this is only specialized when allocator_traits::container_move_assign_always_takes_ownership
		// is false - if it's true then the untyped base move-assign operator is already complete and no elementwise
		// moving will ever need to take place
		static_assert(!UseBase);
		static_assert(Movable);

		table_move_assign& operator=(table_move_assign&& rhs) noexcept( //
			TableTraits::all_nothrow_move_assignable					//
				&& TableTraits::all_nothrow_move_constructible			//
					&& noexcept(Base::clear())							//
						&& noexcept(Base::pop_back())					//
							&& noexcept(Base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			static_assert(!allocator_traits::propagate_on_container_move_assignment::value);
			static_assert(!allocator_traits::is_always_equal::value);

			if (Base::move_from_by_taking_ownership(static_cast<table_move_assignable_base&&>(rhs)))
				return *this;

			SOAGEN_ASSERT(!allocator_traits::equal(Base::allocator(), rhs.allocator()));

			if (rhs.empty())
			{
				Base::clear();
				return *this;
			}

			if (rhs.size() < Base::size())
				Base::pop_back(Base::size() - rhs.size());

			Base::reserve(rhs.size());

			const auto assigned_end = min(Base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				TableTraits::move_assign_row(Base::alloc_, i, rhs.alloc_, i);

			const auto constructed_end = max(Base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				TableTraits::move_construct_row(Base::alloc_, i, rhs.alloc_, i);
				Base::count_++;
			}

			return *this;
		}
	};

	template <typename TableTraits, typename Allocator, typename Base, bool Movable>
	class SOAGEN_EMPTY_BASES table_move_assign<TableTraits, Allocator, Base, /*UseBase*/ true, Movable> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assign);
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_move_assign<TableTraits, Allocator, Base, /*UseBase*/ false, /*Movable*/ false> //
		: public Base
	{
	  public:
		using Base::Base;

		table_move_assign()									   = default;
		table_move_assign(table_move_assign&&)				   = default;
		table_move_assign(const table_move_assign&)			   = default;
		table_move_assign& operator=(const table_move_assign&) = default;

		// base operator is insufficient but the elements aren't movable so no choice but to explicitly delete
		table_move_assign& operator=(table_move_assign&&) = delete;
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: copy-construct
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits, //
			  typename Allocator,
			  typename Base = table_move_assign<TableTraits, Allocator>,
			  bool			= TableTraits::all_copy_constructible&& std::is_copy_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_copy_construct //
		: public Base
	{
	  protected:
		using allocator_traits = allocator_traits<Allocator>;

	  public:
		using Base::Base;

		table_copy_construct()										 = default;
		table_copy_construct(table_copy_construct&&)				 = default;
		table_copy_construct& operator=(table_copy_construct&&)		 = default;
		table_copy_construct& operator=(const table_copy_construct&) = default;

		table_copy_construct(const table_copy_construct& other) noexcept(TableTraits::all_nothrow_copy_constructible //
																			 && noexcept(Base::reserve(size_t{})))
			: Base{ allocator_traits::select_on_container_copy_construction(Base::allocator()) }
		{
			static_assert(std::is_nothrow_copy_constructible_v<Allocator>,
						  "allocators must be nothrow copy-constructible");

			Base::reserve(other.size());
			TableTraits::copy_construct_rows(Base::alloc_, {}, other.alloc_, {}, other.size());
			Base::count_ = other.size();
		}
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_copy_construct<TableTraits, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: copy-assign
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits,
			  typename Allocator,
			  typename Base = table_copy_construct<TableTraits, Allocator>,
			  bool			= TableTraits::all_copy_assignable	//
						 && TableTraits::all_copy_constructible //
				  && (!allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
					  || std::is_copy_assignable_v<Allocator>)>
	class SOAGEN_EMPTY_BASES table_copy_assign //
		: public Base
	{
	  protected:
		using allocator_traits = allocator_traits<Allocator>;

	  public:
		using Base::Base;

		table_copy_assign()								  = default;
		table_copy_assign(table_copy_assign&&)			  = default;
		table_copy_assign& operator=(table_copy_assign&&) = default;
		table_copy_assign(const table_copy_assign&)		  = default;

		constexpr table_copy_assign& operator=(const table_copy_assign& rhs) noexcept( // wtb: noexcept(auto)
			TableTraits::all_nothrow_copy_assignable								   //
				&& TableTraits::all_nothrow_copy_constructible						   //
					&& noexcept(Base::clear())										   //
						&& noexcept(Base::pop_back())								   //
							&& noexcept(Base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			if constexpr (allocator_traits::propagate_on_container_copy_assignment::value
						  || !allocator_traits::is_always_equal::value)
			{
				if (!allocator_traits::equal(Base::allocator(), rhs.allocator()))
				{
					Base::clear();
					Base::shink_to_fit();
					SOAGEN_ASSERT(Base::empty());
					SOAGEN_ASSERT(!Base::alloc_);
				}
			}

			if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
			{
				static_assert(std::is_nothrow_copy_assignable_v<Allocator>,
							  "copy-propagated allocators must be nothrow copy-assignable");

				Base::allocator() = rhs.allocator();
			}

			if (rhs.empty())
			{
				Base::clear();
				return *this;
			}

			if (rhs.size() < Base::size())
				Base::pop_back(Base::size() - rhs.size());

			Base::reserve(rhs.size());

			const auto assigned_end = min(Base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				TableTraits::copy_assign_row(Base::alloc_, i, rhs.alloc_, i);

			const auto constructed_end = max(Base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				TableTraits::copy_construct_row(Base::alloc_, i, rhs.alloc_, i);
				Base::count_++;
			}

			return *this;
		}
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_copy_assign<TableTraits, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_assign);
	};

	//------------------------------------------------------------------------------------------------------------------
	// misc
	//------------------------------------------------------------------------------------------------------------------

	// trait for determining the _actual_ alignment of a table column, taking the allocator and table-allocation
	// semantics into account (since the full allocation for a table is always has alignment ==
	// table_traits::largest_alignment).
	//
	// note that this has absolutely nothing to do with the aligned_stride; that is still calculated according to
	// the user's specified alignment requirements. this trait is _only_ used to help the compiler via assume_aligned.
	template <typename TableTraits, typename Allocator, size_t ColumnIndex>
	inline constexpr size_t actual_column_alignment = TableTraits::template column<ColumnIndex>::alignment;

	template <typename TableTraits, typename Allocator>
	inline constexpr size_t actual_column_alignment<TableTraits, Allocator, 0> =
		max(TableTraits::template column<0>::alignment,
			allocator_traits<Allocator>::min_alignment,
			TableTraits::largest_alignment);
}

namespace soagen
{
	template <typename TableTraits,
			  typename Allocator = allocator,
			  typename Base		 = detail::table_copy_assign<TableTraits, Allocator>>
	class SOAGEN_EMPTY_BASES table //
		: public Base
	{
		static_assert(is_table_traits<TableTraits>, "TableTraits must be an instance of soagen::table_traits");
		static_assert(!is_cvref<TableTraits>, "table traits may not be cvref-qualified");
		static_assert(!is_cvref<Allocator>, "allocators may not be cvref-qualified");

	  public:
		using size_type		   = size_t;
		using difference_type  = ptrdiff_t;
		using allocator_type   = Allocator;
		using allocator_traits = allocator_traits<Allocator>;
		using table_traits	   = TableTraits;

		template <size_type I>
		using column_traits = typename table_traits::template column<I>;

		template <size_type I>
		using column_type = typename column_traits<I>::value_type;

		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table);

		template <size_t I>
		SOAGEN_ALIGNED_COLUMN(I)
		column_type<I>* column() noexcept
		{
			static_assert(I < table_traits::column_count, "column index out of range");

			return soagen::assume_aligned<detail::actual_column_alignment<table_traits, allocator_type, I>>(
				SOAGEN_LAUNDER(reinterpret_cast<column_type<I>*>(Base::alloc_.columns[I])));
		}

		template <size_t I>
		SOAGEN_ALIGNED_COLUMN(I)
		std::add_const_t<column_type<I>>* column() const noexcept
		{
			static_assert(I < table_traits::column_count, "column index out of range");

			return soagen::assume_aligned<detail::actual_column_alignment<table_traits, allocator_type, I>>(
				SOAGEN_LAUNDER(reinterpret_cast<column_type<I>*>(Base::alloc_.columns[I])));
		}
	};

	template <typename>
	inline constexpr bool is_table = false;

	template <typename... Args>
	inline constexpr bool is_table<table<Args...>> = true;

	template <typename... Args>
	inline constexpr bool is_soa<table<Args...>> = true;

	SOAGEN_CONSTRAINED_TEMPLATE((has_swap_member<table<Traits, Alloc>>), typename Traits, typename Alloc)
	SOAGEN_ALWAYS_INLINE
	constexpr void swap(table<Traits, Alloc>& lhs, table<Traits, Alloc>& rhs) //
		noexcept(noexcept(std::declval<table<Traits, Alloc>&>().swap(std::declval<table<Traits, Alloc>&>())))
	{
		lhs.swap(rhs);
	}
}

#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma strict_gs_check(pop)
		#pragma runtime_checks("", restore)
		#pragma optimize("", on)
		#pragma inline_recursion(off)
	#elif SOAGEN_GCC
		#pragma GCC pop_options
	#endif
#endif

#if SOAGEN_MSVC_LIKE
	#pragma pop_macro("min")
	#pragma pop_macro("max")
#endif

SOAGEN_POP_WARNINGS;

#endif // SOAGEN_HPP
