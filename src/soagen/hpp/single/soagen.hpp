//----------------------------------------------------------------------------------------------------------------------
//
// soagen.hpp v0.7.0
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
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//----------------------------------------------------------------------------------------------------------------------
#ifndef SOAGEN_HPP
#define SOAGEN_HPP

//********  generated/version.hpp  *************************************************************************************

#define SOAGEN_VERSION_MAJOR 0
#define SOAGEN_VERSION_MINOR 7
#define SOAGEN_VERSION_PATCH 0
#define SOAGEN_VERSION_STRING "0.7.0"

//********  generated/preprocessor.hpp  ********************************************************************************

#ifndef SOAGEN_CPP
	#ifdef _MSVC_LANG
		#if _MSVC_LANG > __cplusplus
			#define SOAGEN_CPP _MSVC_LANG
		#endif
	#endif
	#ifndef SOAGEN_CPP
		#define SOAGEN_CPP __cplusplus
	#endif
	#if SOAGEN_CPP >= 202900L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 29
	#elif SOAGEN_CPP >= 202600L
		#undef SOAGEN_CPP
		#define SOAGEN_CPP 26
	#elif SOAGEN_CPP >= 202302L
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

#ifndef SOAGEN_PURE
	#ifdef NDEBUG
		#define SOAGEN_PURE                                                                                            \
			SOAGEN_DECLSPEC(noalias)                                                                                   \
			SOAGEN_ATTR(pure)
	#else
		#define SOAGEN_PURE
	#endif
#endif
#ifndef SOAGEN_CONST
	#ifdef NDEBUG
		#define SOAGEN_CONST                                                                                           \
			SOAGEN_DECLSPEC(noalias)                                                                                   \
			SOAGEN_ATTR(const)
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
	#if SOAGEN_MSVC >= 1929
		#define SOAGEN_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
	#elif SOAGEN_CPP >= 20 && SOAGEN_HAS_CPP_ATTR(no_unique_address) >= 201803
		#define SOAGEN_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#else
		#define SOAGEN_NO_UNIQUE_ADDRESS
	#endif
#endif

#ifndef SOAGEN_HAS_EXCEPTIONS
	#if defined(__EXCEPTIONS) || defined(_CPPUNWIND) || defined(__cpp_exceptions)
		#define SOAGEN_HAS_EXCEPTIONS 1
	#else
		#define SOAGEN_HAS_EXCEPTIONS 0
	#endif
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 202002L
	#define SOAGEN_CPP20_CONSTEXPR constexpr
#else
	#define SOAGEN_CPP20_CONSTEXPR
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 202211L
	#define SOAGEN_CPP23_STATIC_CONSTEXPR static constexpr
#else
	#define SOAGEN_CPP23_STATIC_CONSTEXPR constexpr
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
		#define SOAGEN_ENABLE_IF_T(T, ...) std::enable_if_t<!!(__VA_ARGS__), T>
		#define SOAGEN_ENABLE_IF(...)	   , SOAGEN_ENABLE_IF_T(int, __VA_ARGS__) = 0
	#else
		#define SOAGEN_ENABLE_IF_T(T, ...)
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
		#if SOAGEN_CLANG >= 16
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_16                                                              \
				_Pragma("clang diagnostic ignored \"-Wunsafe-buffer-usage\"")                                          \
				static_assert(true)
		#else
			#define SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_16 static_assert(true)
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
			SOAGEN_DISABLE_SPAM_WARNINGS_CLANG_16;                                                                     \
			static_assert(true)
	#elif SOAGEN_MSVC
		#define SOAGEN_DISABLE_SPAM_WARNINGS                                                                           \
			__pragma(warning(disable : 4127)) /* conditional expr is constant */                                       \
			__pragma(warning(disable : 4324)) /* structure was padded due to alignment specifier */                    \
			__pragma(warning(disable : 4348))                                                                          \
			__pragma(warning(disable : 4464))  /* relative include path contains '..' */                               \
			__pragma(warning(disable : 4505))  /* unreferenced local function removed */                               \
			__pragma(warning(disable : 4514))  /* unreferenced inline function has been removed */                     \
			__pragma(warning(disable : 4582))  /* constructor is not implicitly called */                              \
			__pragma(warning(disable : 4619))  /* there is no warning number 'XXXX' */                                 \
			__pragma(warning(disable : 4623))  /* default constructor was implicitly defined as deleted */             \
			__pragma(warning(disable : 4625))  /* copy constructor was implicitly defined as deleted */                \
			__pragma(warning(disable : 4626))  /* assignment operator was implicitly defined as deleted */             \
			__pragma(warning(disable : 4686))  /* possible change in behavior, change in UDT return callconv */        \
			__pragma(warning(disable : 4710))  /* function not inlined */                                              \
			__pragma(warning(disable : 4711))  /* function selected for automatic expansion */                         \
			__pragma(warning(disable : 4820))  /* N bytes padding added */                                             \
			__pragma(warning(disable : 4866))  /* compiler may not enforce left-to-right evaluation order for call */  \
			__pragma(warning(disable : 4946))  /* reinterpret_cast used between related classes */                     \
			__pragma(warning(disable : 5026))  /* move constructor was implicitly defined as deleted */                \
			__pragma(warning(disable : 5027))  /* move assignment operator was implicitly defined as deleted */        \
			__pragma(warning(disable : 5039))  /* potentially throwing function passed to 'extern "C"' function */     \
			__pragma(warning(disable : 5045))  /* Compiler will insert Spectre mitigation */                           \
			__pragma(warning(disable : 5246))  /* initialization of a subobject should be wrapped in braces */         \
			__pragma(warning(disable : 5264))  /* const variable is not used (false-positive) */                       \
			__pragma(warning(disable : 26490)) /* cg: dont use reinterpret_cast */                                     \
			__pragma(warning(disable : 26812)) /* cg: Prefer 'enum class' over 'enum' */                               \
			__pragma(warning(disable : 4848))  /* msvc::no_unique_address in C++17 is a vendor extension */            \
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
				_Pragma("GCC diagnostic ignored \"-Wctor-dtor-privacy\"")                                              \
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
		#define SOAGEN_HAS_CONSTEVAL SOAGEN_DOXYGEN
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

#ifndef SOAGEN_ALWAYS_OPTIMIZE
	#define SOAGEN_ALWAYS_OPTIMIZE 1
#endif

#ifndef SOAGEN_COMMA
	#define SOAGEN_COMMA ,
#endif

#ifndef SOAGEN_HIDDEN
	#if SOAGEN_DOXYGEN
		#define SOAGEN_HIDDEN(...)
		#define SOAGEN_HIDDEN_BASE(...)
		#define SOAGEN_DOXYGEN_ONLY(...) __VA_ARGS__
		#define SOAGEN_IF_DOXYGEN(A, B)	 A
	#else
		#define SOAGEN_HIDDEN(...) __VA_ARGS__
		#define SOAGEN_HIDDEN_BASE(...)	: __VA_ARGS__
		#define SOAGEN_DOXYGEN_ONLY(...)
		#define SOAGEN_IF_DOXYGEN(A, B) B
	#endif
#endif
#define SOAGEN_HIDDEN_PARAM(...) SOAGEN_HIDDEN(SOAGEN_COMMA __VA_ARGS__)

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

#if !defined(__POXY__) && !defined(POXY_IMPLEMENTATION_DETAIL)
	#define POXY_IMPLEMENTATION_DETAIL(...) __VA_ARGS__
#endif

//********  generated/functions.hpp  ***********************************************************************************

SOAGEN_DISABLE_WARNINGS;
#include <numeric>
#include <type_traits>
#include <memory>
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

	template <typename T, typename U, typename... V>
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

	template <typename T>
	SOAGEN_CONST_GETTER
	constexpr bool has_single_bit(T val) noexcept
	{
		if constexpr (std::is_enum_v<T>)
			return has_single_bit(static_cast<std::underlying_type_t<T>>(val));
		else
		{
			return val != T{} && (val & (val - T{ 1 })) == T{};
		}
	}

	template <size_t N, typename T>
	SOAGEN_CONST_INLINE_GETTER
	SOAGEN_ATTR(assume_aligned(N))
	constexpr T* assume_aligned(T* ptr) noexcept
	{
		static_assert(N > 0 && (N & (N - 1u)) == 0u, "assume_aligned() requires a power-of-two alignment value.");
		static_assert(!std::is_function_v<T>, "assume_aligned may not be used on functions.");

		SOAGEN_IF_CONSTEVAL
		{
			return ptr;
		}
		else
		{
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

#elif defined(__cpp_lib_assume_aligned)

				return std::assume_aligned<N>(ptr);

#else

				return ptr;

#endif
			}
		}
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

//********  core.hpp  **************************************************************************************************

SOAGEN_DISABLE_WARNINGS;

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <type_traits>
#include <new>
#include <utility>
#include <memory>
#include <optional>
#include <iterator>
#if SOAGEN_HAS_EXCEPTIONS
	#include <stdexcept>
#endif

#ifndef SOAGEN_COLUMN_SPAN_TYPE
	#if SOAGEN_CPP >= 20 && SOAGEN_HAS_INCLUDE(<span>)
		#include <span>
		#define SOAGEN_COLUMN_SPAN_TYPE std::span
	#elif SOAGEN_HAS_INCLUDE(<muu/span.h>)
		#include <muu/span.h>
		#define SOAGEN_COLUMN_SPAN_TYPE				  muu::span
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
	using std::size_t;
	using std::ptrdiff_t;
	using std::intptr_t;
	using std::uintptr_t;
	using std::nullptr_t;

	// forward declarations
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

	template <typename T>
	using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename T>
	using coerce_ref = std::conditional_t<std::is_reference_v<T>, T, std::add_lvalue_reference_t<T>>;

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

	template <auto Value, auto... Values>
	inline constexpr bool any_same_value = ((Value == Values) || ...);

	template <typename T, bool Cond>
	using conditionally_add_const = std::conditional_t<Cond, std::add_const_t<T>, T>;

	template <typename T, bool Cond>
	using conditionally_add_volatile = std::conditional_t<Cond, std::add_volatile_t<T>, T>;

	template <typename T, bool Cond>
	using conditionally_remove_cvref = std::conditional_t<Cond, remove_cvref<T>, T>;

	namespace detail
	{
		template <typename T>
		using is_implicit_lifetime_type_ = std::disjunction<std::is_scalar<T>,
															std::is_array<T>,
															std::conjunction<std::is_aggregate<T>,
																			 std::is_trivially_constructible<T>,
																			 std::is_trivially_destructible<T>>>;

		template <typename T, typename>
		struct copy_cv_
		{
			using type = T;
		};
		template <typename T, typename CopyFrom>
		struct copy_cv_<T, const CopyFrom> : std::add_const<T>
		{};
		template <typename T, typename CopyFrom>
		struct copy_cv_<T, volatile CopyFrom> : std::add_volatile<T>
		{};
		template <typename T, typename CopyFrom>
		struct copy_cv_<T, const volatile CopyFrom> : std::add_cv<T>
		{};

		template <typename T, typename>
		struct copy_ref_
		{
			using type = T;
		};
		template <typename T, typename CopyFrom>
		struct copy_ref_<T, CopyFrom&> : std::add_lvalue_reference<T>
		{};
		template <typename T, typename CopyFrom>
		struct copy_ref_<T, CopyFrom&&> : std::add_rvalue_reference<T>
		{};

		template <typename T>
		struct remove_lvalue_ref_
		{
			using type = T;
		};
		template <typename T>
		struct remove_lvalue_ref_<T&>
		{
			using type = T;
		};
	}

	template <typename T>
	inline constexpr bool is_implicit_lifetime_type =
		detail::is_implicit_lifetime_type_<T>::value;

	template <typename T, typename CopyFrom>
	using copy_cv = typename detail::copy_cv_<std::remove_cv_t<T>, CopyFrom>::type;

	template <typename T, typename CopyFrom>
	using copy_ref = typename detail::copy_ref_<std::remove_reference_t<T>, CopyFrom>::type;

	template <typename T, typename CopyFrom>
	using copy_cvref =
		copy_ref<copy_cv<remove_cvref<T>, std::remove_reference_t<CopyFrom>>, CopyFrom>;

	template <typename T>
	using remove_lvalue_ref = typename detail::remove_lvalue_ref_<T>::type;

	template <auto Value>
	using index_constant = std::integral_constant<size_t, static_cast<size_t>(Value)>;

	template <typename T>
	using forward_type =
		std::conditional_t<std::is_lvalue_reference_v<T>, T, std::add_rvalue_reference_t<T>>;

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

	using SOAGEN_OPTIONAL_TYPE;

	namespace detail
	{
		template <template <typename...> typename Trait, typename Enabler, typename... Args>
		struct is_detected_impl : std::false_type
		{};
		template <template <typename...> typename Trait, typename... Args>
		struct is_detected_impl<Trait, std::void_t<Trait<Args...>>, Args...> : std::true_type
		{};
		template <template <typename...> typename Trait, typename... Args>
		using is_detected_ = is_detected_impl<Trait, void, Args...>;
	}

	template <template <typename...> typename Trait, typename... Args>
	inline constexpr auto is_detected = detail::is_detected_<Trait, Args...>::value;

	namespace detail
	{
		template <typename T>
		using has_swap_member_impl_ = decltype(std::declval<T&>().swap(std::declval<T&>()));
		template <typename T>
		using has_swap_member_ = is_detected_<has_swap_member_impl_, T>;

		template <typename T, typename Arg>
		using has_resize_member_impl_ = decltype(std::declval<T&>().resize(std::declval<const Arg&>()));
		template <typename T, typename Arg = size_t>
		using has_resize_member_ = is_detected_<has_resize_member_impl_, T, Arg>;

		template <typename T, typename Arg>
		using has_erase_member_impl_ = decltype(std::declval<T&>().erase(std::declval<const Arg&>()));
		template <typename T, typename Arg = size_t>
		using has_erase_member_ = is_detected_<has_erase_member_impl_, T, Arg>;

		template <typename T, typename Arg>
		using has_unordered_erase_member_impl_ =
			decltype(std::declval<T&>().unordered_erase(std::declval<const Arg&>()));
		template <typename T, typename Arg = size_t>
		using has_unordered_erase_member_ = is_detected_<has_unordered_erase_member_impl_, T, Arg>;

		template <typename T>
		using has_data_member_impl_ = decltype(std::declval<T&>().data());
		template <typename T>
		using has_data_member_ = is_detected_<has_data_member_impl_, T>;

		template <typename T, typename Pos, typename... Args>
		using has_emplace_member_impl_ =
			decltype(std::declval<T&>().emplace(std::declval<Pos>(), std::declval<Args>()...));
		template <typename T, typename Pos, typename... Args>
		using has_emplace_member_ = is_detected_<has_emplace_member_impl_, Pos, Args...>;

		template <typename T, typename... Args>
		using has_emplace_back_member_impl_ = decltype(std::declval<T&>().emplace_back(std::declval<Args>()...));
		template <typename T, typename... Args>
		using has_emplace_back_member_ = is_detected_<has_emplace_back_member_impl_, Args...>;

		template <typename T, typename U>
		using is_equality_comparable_impl_ = decltype(std::declval<const std::remove_reference_t<T>&>()
													  == std::declval<const std::remove_reference_t<U>&>());
		template <typename T, typename U = T>
		using is_equality_comparable_ = is_detected_<is_equality_comparable_impl_, T, U>;

		template <typename T, typename U>
		using is_less_than_comparable_impl_ = decltype(std::declval<const std::remove_reference_t<T>&>()
													   < std::declval<const std::remove_reference_t<U>&>());
		template <typename T, typename U = T>
		using is_less_than_comparable_ = is_detected_<is_less_than_comparable_impl_, T, U>;
	}

	template <typename... T>
	inline constexpr bool has_swap_member =
		std::conjunction<detail::has_swap_member_<T>...>::value;

	template <typename... T>
	inline constexpr bool has_resize_member =
		std::conjunction<detail::has_resize_member_<T, size_t>...>::value;

	template <typename... T>
	inline constexpr bool has_erase_member =
		std::conjunction<detail::has_erase_member_<T, size_t>...>::value;

	template <typename... T>
	inline constexpr bool has_unordered_erase_member =
		std::conjunction<detail::has_unordered_erase_member_<T, size_t>...>::value;

	template <typename... T>
	inline constexpr bool has_data_member =
		std::conjunction<detail::has_data_member_<T>...>::value;

	template <typename T, typename Pos, typename... Args>
	inline constexpr bool has_emplace_member =
		detail::has_emplace_member_<T, Pos, Args...>::value;

	template <typename T, typename... Args>
	inline constexpr bool has_emplace_back_member =
		detail::has_emplace_back_member_<T, Args...>::value;

	template <typename T, typename U = T>
	inline constexpr bool is_equality_comparable =
		detail::is_equality_comparable_<T, U>::value;

	template <typename T, typename U = T>
	inline constexpr bool is_less_than_comparable =
		detail::is_less_than_comparable_<T, U>::value;

	namespace detail
	{
		template <typename T, bool = has_swap_member<T>>
		struct has_nothrow_swap_member_ : std::bool_constant<noexcept(std::declval<T&>().swap(std::declval<T&>()))>
		{};
		template <typename T>
		struct has_nothrow_swap_member_<T, false> : std::false_type
		{};

		template <typename T, typename Arg = size_t, bool = has_resize_member<T, Arg>>
		struct has_nothrow_resize_member_
			: std::bool_constant<noexcept(std::declval<T&>().resize(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_resize_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, typename Arg = size_t, bool = has_erase_member<T, Arg>>
		struct has_nothrow_erase_member_
			: std::bool_constant<noexcept(std::declval<T&>().erase(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_erase_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, typename Arg = size_t, bool = has_unordered_erase_member<T, Arg>>
		struct has_nothrow_unordered_erase_member_
			: std::bool_constant<noexcept(std::declval<T&>().unordered_erase(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_unordered_erase_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, bool = has_data_member<T>>
		struct has_nothrow_data_member_ : std::bool_constant<noexcept(std::declval<T&>().data())>
		{};
		template <typename T>
		struct has_nothrow_data_member_<T, false> : std::false_type
		{};

		template <bool /* has_emplace_member<T, Pos, Args> */, typename T, typename Pos, typename... Args>
		struct has_nothrow_emplace_member_
			: std::bool_constant<noexcept(std::declval<T&>().emplace(std::declval<Pos>(), std::declval<Args>()...))>
		{};
		template <typename T, typename Pos, typename... Args>
		struct has_nothrow_emplace_member_<false, T, Pos, Args...> : std::false_type
		{};

		template <bool /* has_emplace_back_member<T, Args> */, typename T, typename... Args>
		struct has_nothrow_emplace_back_member_
			: std::bool_constant<noexcept(std::declval<T&>().emplace_back(std::declval<Args>()...))>
		{};
		template <typename T, typename... Args>
		struct has_nothrow_emplace_back_member_<false, T, Args...> : std::false_type
		{};

		template <typename T, typename U = T, bool = is_equality_comparable<T, U>>
		struct is_nothrow_equality_comparable_
			: std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
										  == std::declval<const std::remove_reference_t<U>&>())>
		{};
		template <typename T, typename U>
		struct is_nothrow_equality_comparable_<T, U, false> : std::false_type
		{};

		template <typename T, typename U = T, bool = is_less_than_comparable<T, U>>
		struct is_nothrow_less_than_comparable_
			: std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
										  < std::declval<const std::remove_reference_t<U>&>())>
		{};
		template <typename T, typename U>
		struct is_nothrow_less_than_comparable_<T, U, false> : std::false_type
		{};
	}

	template <typename... T>
	inline constexpr bool has_nothrow_swap_member =
		std::conjunction<detail::has_nothrow_swap_member_<T>...>::value;

	template <typename... T>
	inline constexpr bool has_nothrow_resize_member =
		std::conjunction<detail::has_nothrow_resize_member_<T, size_t>...>::value;

	template <typename... T>
	inline constexpr bool has_nothrow_erase_member =
		std::conjunction<detail::has_nothrow_erase_member_<T, size_t>...>::value;

	template <typename... T>
	inline constexpr bool has_nothrow_unordered_erase_member =
		std::conjunction<detail::has_nothrow_unordered_erase_member_<T, size_t>...>::value;

	template <typename... T>
	inline constexpr bool has_nothrow_data_member =
		std::conjunction<detail::has_nothrow_data_member_<T>...>::value;

	template <typename T, typename Pos, typename... Args>
	inline constexpr bool has_nothrow_emplace_member =
		detail::has_nothrow_emplace_member_<has_emplace_member<T, Pos, Args...>, T, Pos, Args...>::value;

	template <typename T, typename... Args>
	inline constexpr bool has_nothrow_emplace_back_member =
		detail::has_nothrow_emplace_back_member_<has_emplace_back_member<T, Args...>, T, Args...>::value;

	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_equality_comparable =
		detail::is_nothrow_equality_comparable_<T, U>::value;

	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_less_than_comparable =
		detail::is_nothrow_less_than_comparable_<T, U>::value;

#if !SOAGEN_DOXYGEN && SOAGEN_HAS_BUILTIN(__type_pack_element)

	template <auto I, typename... T>
	using type_at_index = __type_pack_element<static_cast<size_t>(I), T...>;

#else

	namespace detail
	{
		template <size_t, typename...>
		struct type_at_index_;

		template <size_t I, typename T, typename... U>
		struct type_at_index_<I, T, U...> : type_at_index_<I - 1, U...>
		{};

		template <typename T, typename... U>
		struct type_at_index_<0, T, U...>
		{
			using type = T;
		};
	}

	template <auto I, typename... T>
	using type_at_index =
		typename detail::type_at_index_<static_cast<size_t>(I), T...>::type;

#endif

	namespace detail
	{
		template <typename>
		struct is_table_ : std::false_type
		{};
		template <typename... Args>
		struct is_table_<table<Args...>> : std::true_type
		{};

		template <typename T>
		struct is_soa_ : is_table_<T> // specialized in the generated code
		{};

		template <typename>
		struct is_row_ : std::false_type
		{};
		template <typename Soa, size_t... Columns>
		struct is_row_<row<Soa, Columns...>> : std::true_type
		{};

		template <typename>
		struct is_span_ : std::false_type
		{};
		template <typename Soa>
		struct is_span_<span<Soa>> : std::true_type
		{};

		template <typename>
		struct is_iterator_ : std::false_type
		{};
		template <typename Soa, size_t... Columns>
		struct is_iterator_<iterator<Soa, Columns...>> : std::true_type
		{};
	}

	template <typename T>
	inline constexpr bool is_table = detail::is_table_<std::remove_cv_t<T>>::value;

	template <typename T>
	inline constexpr bool is_soa = detail::is_soa_<std::remove_cv_t<T>>::value;

	template <typename T>
	inline constexpr bool is_row = detail::is_row_<std::remove_cv_t<T>>::value;

	template <typename T>
	inline constexpr bool is_span = detail::is_span_<std::remove_cv_t<T>>::value;

	template <typename T>
	inline constexpr bool is_iterator = detail::is_iterator_<std::remove_cv_t<T>>::value;

	namespace detail
	{
		template <typename T>
		struct soa_type_cvref_unwrap_;

		template <typename T, bool = is_soa<remove_cvref<T>>>
		struct soa_type_cvref_
		{
			using type = T;
		};
		template <typename T>
		struct soa_type_cvref_<T, false> //
			: soa_type_cvref_unwrap_<remove_cvref<T>>
		{};

		template <template <typename> typename T, typename Soa>
		struct soa_type_cvref_unwrap_<T<Soa>> : soa_type_cvref_<Soa>
		{};

		template <template <typename, size_t...> typename T, typename Soa, size_t... Columns>
		struct soa_type_cvref_unwrap_<T<Soa, Columns...>> : soa_type_cvref_<Soa>
		{};

		template <typename T>
		using soa_type_cvref = typename soa_type_cvref_<T>::type;
	}

	template <typename T>
	using soa_type = remove_cvref<detail::soa_type_cvref<remove_cvref<T>>>;

	namespace detail
	{
		template <typename T>
		struct table_type_
		{
			using type = typename T::table_type;
		};
		template <typename... Args>
		struct table_type_<table<Args...>>
		{
			using type = table<Args...>;
		};
	}

	template <typename T>
	using table_type = typename detail::table_type_<soa_type<T>>::type;

	template <typename A, typename B>
	inline constexpr bool same_table_type = std::is_same_v<table_type<A>, table_type<B>>;

	namespace detail
	{
		template <typename T>
		struct table_traits_type_
		{
			using type = typename table_type<T>::table_traits;
		};
		template <typename... Args>
		struct table_traits_type_<table_traits<Args...>>
		{
			using type = table_traits<Args...>;
		};
		template <typename Traits, typename Allocator>
		struct table_traits_type_<table<Traits, Allocator>>
		{
			using type = Traits;
		};
	}

	template <typename T>
	using table_traits_type = typename detail::table_traits_type_<remove_cvref<T>>::type;

	template <typename T, auto Column>
	using value_type =
		typename table_traits_type<T>::template column<static_cast<size_t>(Column)>::value_type;

	template <typename T, auto Column>
	using value_ref =
		copy_ref<					//
								   conditionally_add_volatile<	//
									   conditionally_add_const< //
										   value_type<T, Column>,
										   std::is_const_v<std::remove_reference_t<detail::soa_type_cvref<T>>>>,
									   std::is_volatile_v<std::remove_reference_t<detail::soa_type_cvref<T>>>>,
								   coerce_ref<detail::soa_type_cvref<T>>>;

	namespace detail
	{
		template <typename T>
		struct allocator_type_
		{
			using type = typename table_type<T>::allocator_type;
		};
		template <>
		struct allocator_type_<allocator>
		{
			using type = allocator;
		};
		template <typename Traits, typename Allocator>
		struct allocator_type_<table<Traits, Allocator>>
		{
			using type = Allocator;
		};
	}

	template <typename T>
	using allocator_type = typename detail::allocator_type_<remove_cvref<T>>::type;

	namespace detail
	{
		template <typename, typename>
		struct column_indices_;

		// use the override if it is specified
		template <typename T, size_t... Columns>
		struct column_indices_<std::index_sequence<Columns...>, T>
		{
			static_assert(sizeof...(Columns));

			using type = std::index_sequence<Columns...>;
		};

		// otherwise use the column_count for tables, spans and generated types
		template <typename T>
		struct column_indices_<std::index_sequence<>, T>
		{
			using type = std::make_index_sequence<table_traits_type<T>::column_count>;

			static_assert(!std::is_same_v<type, std::index_sequence<>>);
		};

		// and use the viewed columns for rows and iterators
		template <size_t... Indices>
		struct nested_index_sequence
		{
			using type = std::index_sequence<Indices...>;
		};
		template <typename Soa, size_t... Columns>
		struct column_indices_<std::index_sequence<>, row<Soa, Columns...>> : nested_index_sequence<Columns...>
		{};
		template <typename Soa, size_t... Columns>
		struct column_indices_<std::index_sequence<>, iterator<Soa, Columns...>> : nested_index_sequence<Columns...>
		{};
	}

	template <typename T, auto... Columns>
	using column_indices =
		typename detail::column_indices_<std::index_sequence<static_cast<size_t>(Columns)...>, remove_cvref<T>>::type;

	namespace detail
	{
		template <typename>
		struct add_const_to_first_type_arg_;
		template <template <typename> typename Type, typename T>
		struct add_const_to_first_type_arg_<Type<T>>
		{
			using type = Type<copy_ref<std::add_const_t<std::remove_reference_t<T>>, T>>;
		};
		template <template <typename, typename...> typename Type, typename T, typename... Args>
		struct add_const_to_first_type_arg_<Type<T, Args...>>
		{
			using type = Type<copy_ref<std::add_const_t<std::remove_reference_t<T>>, T>, Args...>;
		};
		template <template <typename, size_t...> typename Type, typename T, size_t... Columns>
		struct add_const_to_first_type_arg_<Type<T, Columns...>>
		{
			using type = Type<copy_ref<std::add_const_t<std::remove_reference_t<T>>, T>, Columns...>;
		};
		template <typename T>
		using add_const_to_first_type_arg = typename add_const_to_first_type_arg_<T>::type;
	}

	template <typename T>
	using span_type = span<remove_lvalue_ref<detail::soa_type_cvref<remove_lvalue_ref<T>>>>;

	template <typename T>
	using const_span_type = detail::add_const_to_first_type_arg<span_type<T>>;

	namespace detail
	{
		template <template <typename, size_t...> typename, typename...>
		struct derive_view_type_;

		template <template <typename, size_t...> typename Type, typename T, size_t... Columns>
		struct derive_view_type_<Type, T, std::index_sequence<Columns...>, std::true_type>
		{
			using type = Type<remove_lvalue_ref<soa_type_cvref<remove_lvalue_ref<T>>>, Columns...>;
		};

		template <template <typename, size_t...> typename Type, typename T, size_t... Columns>
		struct derive_view_type_<Type, T, std::index_sequence<Columns...>>
			: derive_view_type_<Type, T, column_indices<remove_cvref<T>, Columns...>, std::true_type>
		{};
		template <template <typename, size_t...> typename Type, typename... Args>
		using derive_view_type = typename derive_view_type_<Type, Args...>::type;
	}

	template <typename T, auto... Columns>
	using row_type =
		detail::derive_view_type<row, T, std::index_sequence<static_cast<size_t>(Columns)...>>;

	template <typename T, auto... Columns>
	using const_row_type =
		detail::add_const_to_first_type_arg<row_type<T, static_cast<size_t>(Columns)...>>;

	template <typename T, auto... Columns>
	using iterator_type =
		detail::derive_view_type<iterator, T, std::index_sequence<static_cast<size_t>(Columns)...>>;

	template <typename T, auto... Columns>
	using const_iterator_type =
		detail::add_const_to_first_type_arg<iterator_type<T, static_cast<size_t>(Columns)...>>;

	namespace detail
	{
		template <typename ValueType>
		struct storage_type_
		{
			using type = ValueType;
		};
		// store all pointers as void*
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
		// strip off const and volatile
		template <typename T>
		struct storage_type_<const T> : public storage_type_<T>
		{};
		template <typename T>
		struct storage_type_<volatile T> : public storage_type_<T>
		{};
		template <typename T>
		struct storage_type_<const volatile T> : public storage_type_<T>
		{};
		// store byte-like types as std::byte (since these pointers can legally alias each other)
		template <>
		struct storage_type_<char> : public storage_type_<std::byte>
		{};
		template <>
		struct storage_type_<unsigned char> : public storage_type_<std::byte>
		{};
	}

	template <typename ValueType>
	using storage_type = typename detail::storage_type_<ValueType>::type;

	namespace detail
	{
		template <typename T>
		struct param_type_
		{
			static_assert(!std::is_reference_v<T>);

			using type = std::conditional_t<
				// move-only types
				!std::is_copy_constructible_v<T> && std::is_move_constructible_v<T>,
				std::add_rvalue_reference_t<std::remove_cv_t<T>>,
				std::conditional_t<
					// small + trivial types
					std::is_scalar_v<T> || std::is_fundamental_v<T>
						|| (std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(void*) * 2),
					std::remove_cv_t<T>,
					// everything else
					std::add_lvalue_reference_t<std::add_const_t<T>>>>;
		};
		// references are kept as-is
		template <typename T>
		struct param_type_<T&>
		{
			using type = T&;
		};
		template <typename T>
		struct param_type_<T&&>
		{
			using type = T&&;
		};
		// ... except const rvalues, which are converted to const lvalues (because const T&& is nonsense)
		template <typename T>
		struct param_type_<const T&&> : param_type_<const T&>
		{};
		template <typename T>
		struct param_type_<const volatile T&&> : param_type_<const volatile T&>
		{};
	}

	template <typename ValueType>
	using param_type = typename detail::param_type_<ValueType>::type;

	namespace detail
	{
		template <typename T>
		struct rvalue_type_
		{
			using type = std::conditional_t<
				// if the param_type of unreferenced, unqualified T would be a value or an rvalue, use that
				std::is_rvalue_reference_v<param_type<remove_cvref<T>>>
					|| !std::is_reference_v<param_type<remove_cvref<T>>>,
				param_type<remove_cvref<T>>,
				std::conditional_t<
					// copy-only things
					!std::is_move_constructible_v<remove_cvref<T>>,
					std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>,
					// rvalues
					std::add_rvalue_reference_t<remove_cvref<T>>>>;
		};
	}

	template <typename ParamType>
	using rvalue_type = typename detail::rvalue_type_<ParamType>::type;

	namespace detail
	{
		inline static constexpr size_t min_actual_column_alignment =
			max(size_t{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ }, alignof(std::max_align_t), size_t{ 16 });

		template <typename T>
		struct columns_always_aligned_ : std::bool_constant<is_table<T> || is_soa<T>>
		{};
	}

	template <typename T, auto Column>
	inline constexpr size_t actual_alignment =
		max(alignof(value_type<T, Column>),
			detail::columns_always_aligned_<remove_cvref<T>>::value
				? max(table_traits_type<T>::template column<Column>::alignment, detail::min_actual_column_alignment)
				: 0u);

	template <typename T>
	inline constexpr size_t buffer_alignment =
		max(allocator_traits<allocator_type<T>>::min_alignment, table_traits_type<T>::largest_alignment);

	namespace detail
	{
		// machinery to only instantiate one or more secondary traits when the primary one is true
		// e.g. for traits that come in pairs like is_invocable / is_nothrow_invocable

		template <typename...>
		struct types;

		template <bool, template <typename...> typename, typename...>
		struct nested_trait_indirect_ : std::false_type
		{};

		template <template <typename...> typename Trait, typename... Args>
		struct nested_trait_indirect_<true, Trait, Args...> : Trait<Args...>
		{};

		template <typename, template <typename...> typename, template <typename...> typename...>
		struct nested_trait_;

		template <template <typename...> typename Trait,
				  template <typename...>
				  typename... NestedTraits,
				  typename... Args>
		struct nested_trait_<types<Args...>, Trait, NestedTraits...> : Trait<Args...>
		{
			template <size_t Index>
			using nested =
				type_at_index<Index, nested_trait_indirect_<Trait<Args...>::value, NestedTraits, Args...>...>;
		};
	}
}

//********  generated/compressed_pair.hpp  *****************************************************************************

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
		using first_type = First;

		using second_type = Second;

		SOAGEN_NODISCARD_CTOR
		compressed_pair() = default;

		SOAGEN_NODISCARD_CTOR
		compressed_pair(const compressed_pair&) = default;

		SOAGEN_NODISCARD_CTOR
		compressed_pair(compressed_pair&&) = default;

		compressed_pair& operator=(const compressed_pair&) = default;

		compressed_pair& operator=(compressed_pair&&) = default;

		using detail::compressed_pair_base<First, Second>::compressed_pair_base; // inherit constructor

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
	compressed_pair(F&&, S&&)
		-> compressed_pair<std::remove_cv_t<std::remove_reference_t<F>>, std::remove_cv_t<std::remove_reference_t<S>>>;

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

//********  allocator.hpp  *********************************************************************************************

namespace soagen
{
	struct allocator
	{
		using value_type		 = std::byte;
		using pointer			 = value_type*;
		using const_pointer		 = const value_type*;
		using void_pointer		 = std::byte*;
		using const_void_pointer = const std::byte*;
		using size_type			 = std::size_t;
		using difference_type	 = std::ptrdiff_t;

		using is_always_equal = std::true_type;

		using propagate_on_container_copy_assignment = std::false_type;

		using propagate_on_container_move_assignment = std::false_type;

		using propagate_on_container_swap = std::false_type;

		static constexpr size_type min_alignment =
			max(size_t{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ }, alignof(std::max_align_t), alignof(value_type));
		static_assert(has_single_bit(min_alignment));

		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		value_type* allocate(size_type size, std::align_val_t alignment = std::align_val_t{ min_alignment })
		{
			SOAGEN_ASSUME(size);
			SOAGEN_ASSUME(static_cast<size_t>(alignment));

			size	  = (size + static_cast<size_t>(alignment) - 1u) & ~(static_cast<size_t>(alignment) - 1u);
			alignment = std::align_val_t{ max(static_cast<size_t>(alignment), min_alignment) };

			SOAGEN_ASSUME((static_cast<size_t>(alignment) & (static_cast<size_t>(alignment) - 1u)) == 0u);

#if SOAGEN_WINDOWS
			auto ptr = static_cast<pointer>(_aligned_malloc(size, static_cast<size_t>(alignment)));
#else
			auto ptr = static_cast<pointer>(std::aligned_alloc(static_cast<size_t>(alignment), size));
#endif
			if SOAGEN_UNLIKELY(!ptr)
				throw std::bad_alloc{};

			return soagen::assume_aligned<min_alignment>(ptr);
		}

		SOAGEN_ATTR(nonnull)
		void deallocate(value_type* ptr, [[maybe_unused]] size_type size) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);
			SOAGEN_ASSUME(size);

#if SOAGEN_WINDOWS
			_aligned_free(ptr);
#else
			std::free(ptr);
#endif
		}

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
}

namespace soagen::detail
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

	//--- base traits --------------------------------------------------------------------------------------------------

	template <typename Allocator>
	struct allocator_traits_base : public std::allocator_traits<Allocator>
	{
	  private:
		using base = std::allocator_traits<Allocator>;

	  public:
		static_assert(!is_cvref<Allocator>, "allocators must not be cvref-qualified");
		static_assert(any_same<typename Allocator::value_type, std::byte, char, unsigned char>,
					  "allocators must have either std::byte, char or unsigned char as their value_type");

		static constexpr size_t min_alignment = detail::alloc_min_alignment<Allocator>;
		static_assert(has_single_bit(min_alignment), "allocator min_alignment must be a power of two");

		SOAGEN_PURE_INLINE_GETTER
		static constexpr bool equal([[maybe_unused]] const Allocator& a, [[maybe_unused]] const Allocator& b) noexcept
		{
			if constexpr (base::is_always_equal::value)
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
			base::propagate_on_container_move_assignment::value || base::is_always_equal::value;
	};

	//--- alignment-aware allocate() -----------------------------------------------------------------------------------

	template <typename Allocator, bool = has_aligned_allocate<Allocator>>
	struct allocator_traits_alloc : allocator_traits_base<Allocator>
	{
		// primary template - allocator has an aligned-allocation overload

	  private:
		using base = allocator_traits_base<Allocator>;

	  public:
		using typename base::value_type;
		using typename base::size_type;

		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(base::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type size, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{}, std::align_val_t{})))
		{
			return soagen::assume_aligned<base::min_alignment>(
				alloc.allocate(size, std::align_val_t{ max(static_cast<size_t>(alignment), base::min_alignment) }));
		}
	};

	template <typename Allocator>
	struct allocator_traits_alloc<Allocator, false> : allocator_traits_base<Allocator>
	{
		// secondary template - we have to implement the alignment management manually :(:(
	  private:
		using base = allocator_traits_base<Allocator>;

	  public:
		using typename base::value_type;
		using typename base::size_type;
		using typename base::pointer;

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
		SOAGEN_ATTR(assume_aligned(base::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type size, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{})))
		{
			static_assert(sizeof(typename Allocator::value_type) == 1u);

			alignment = std::align_val_t{ max(static_cast<size_type>(alignment), base::min_alignment) };

			const size_type offset = (static_cast<size_type>(alignment) - 1u) + sizeof(alloc_info);
			pointer ptr			   = alloc.allocate(size + offset);
			SOAGEN_ASSUME(ptr != nullptr);

			alloc_info* info = reinterpret_cast<alloc_info*>((reinterpret_cast<uintptr_t>(ptr) + offset)
															 & ~(static_cast<size_type>(alignment) - 1u));
			info[-1]		 = { size, size + offset, ptr };
			return soagen::assume_aligned<base::min_alignment>(reinterpret_cast<pointer>(info));
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

namespace soagen
{
	template <typename Allocator>
	struct allocator_traits //
		: public detail::allocator_traits_alloc<Allocator>
	{};
}

//********  emplacer.hpp  **********************************************************************************************

namespace soagen
{
	template <typename... Args>
	struct emplacer
	{
		static_assert(sizeof...(Args));
		static_assert((std::is_reference_v<Args> && ...));

		void* ptrs[sizeof...(Args)];

		SOAGEN_DEFAULT_RULE_OF_FIVE(emplacer);

		SOAGEN_NODISCARD_CTOR
		constexpr emplacer(Args&&... args) noexcept //
			: ptrs{ const_cast<void*>(static_cast<const volatile void*>(&args))... }
		{}
	};

	template <>
	struct emplacer<>
	{};

	template <typename... Args>
	emplacer(Args&&...) -> emplacer<Args&&...>;

	namespace detail
	{
		template <typename>
		struct is_emplacer_ : std::false_type
		{};
		template <typename... Args>
		struct is_emplacer_<emplacer<Args...>> : std::true_type
		{};
	}

	template <typename T>

	inline constexpr bool is_emplacer = detail::is_emplacer_<std::remove_cv_t<T>>::value;
}

namespace std
{
	template <typename... Args>
	struct tuple_size<soagen::emplacer<Args...>> //
		: std::integral_constant<size_t, sizeof...(Args)>
	{};
}

//********  tuples.hpp  ************************************************************************************************

namespace soagen
{
	namespace detail
	{
		template <typename T>
		using has_tuple_size_impl_ = decltype(std::tuple_size<remove_cvref<T>>::value);
		template <typename T>
		using has_tuple_element_impl_ = decltype(std::declval<typename std::tuple_element<0, remove_cvref<T>>::type>());
		template <typename T>
		using has_tuple_get_member_impl_ = decltype(std::declval<T>().template get<0>());
		namespace adl_dummy
		{
			template <size_t>
			auto get();
			template <typename T>
			using has_tuple_get_adl_impl_ = decltype(get<0>(std::declval<T>()));
		}

		template <typename T>
		using has_tuple_size_ = is_detected_<has_tuple_size_impl_, remove_cvref<T>>;
		template <typename T>
		using has_tuple_element_ = is_detected_<has_tuple_element_impl_, remove_cvref<T>>;
		template <typename T>
		using has_tuple_get_member_ = is_detected_<has_tuple_get_member_impl_, T>;
		template <typename T>
		using has_tuple_get_adl_ = is_detected_<adl_dummy::has_tuple_get_adl_impl_, T>;
	}

	template <typename T>
	inline constexpr bool is_tuple =
		std::conjunction_v<detail::has_tuple_size_<T>,
						   detail::has_tuple_element_<T>,
						   std::disjunction<detail::has_tuple_get_member_<T>, detail::has_tuple_get_adl_<T>>>;

	template <size_t I, typename T>
	SOAGEN_NODISCARD
	SOAGEN_ALWAYS_INLINE
	constexpr decltype(auto) get_from_tuple(T&& tuple) noexcept
	{
		if constexpr (detail::has_tuple_get_member_<T&&>::value)
		{
			return static_cast<T&&>(tuple).template get<I>();
		}
		else if constexpr (detail::has_tuple_get_adl_<T&&>::value)
		{
			using detail::adl_dummy::get;
			return get<I>(static_cast<T&&>(tuple));
		}
	}

	namespace detail
	{
		template <typename...>
		struct is_constructible_by_unpacking_tuple_impl_ : std::false_type
		{};

		template <typename T, typename Tuple, size_t... Members>
		struct is_constructible_by_unpacking_tuple_impl_<T, Tuple, std::index_sequence<Members...>>
			: nested_trait_<types<T, decltype(get_from_tuple<Members>(std::declval<Tuple>()))...>,
							std::is_constructible,
							std::is_nothrow_constructible,
							std::is_trivially_constructible>
		{};

		template <typename T, typename Tuple, bool = is_tuple<Tuple>>
		struct is_constructible_by_unpacking_tuple_ : std::false_type
		{
			using is_nothrow = std::false_type;
			using is_trivial = std::false_type;
		};

		template <typename T, typename Tuple>
		struct is_constructible_by_unpacking_tuple_<T, Tuple, true>
			: is_constructible_by_unpacking_tuple_impl_<
				  T,
				  Tuple,
				  std::make_index_sequence<std::tuple_size_v<remove_cvref<Tuple>>>>
		{
			using base = is_constructible_by_unpacking_tuple_impl_<
				T,
				Tuple,
				std::make_index_sequence<std::tuple_size_v<remove_cvref<Tuple>>>>;

			using is_nothrow = typename base::template nested<0>;
			using is_trivial = typename base::template nested<1>;
		};
	}

	template <typename T, typename Tuple>
	inline constexpr bool is_constructible_by_unpacking_tuple =
		detail::is_constructible_by_unpacking_tuple_<T, Tuple>::value;
}

//********  column_traits.hpp  *****************************************************************************************

#ifndef SOAGEN_LAUNDER
	#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
		#define SOAGEN_LAUNDER(...) std::launder(__VA_ARGS__)
	#elif SOAGEN_CLANG >= 8 || SOAGEN_GCC >= 7 || SOAGEN_ICC >= 1910 || SOAGEN_MSVC >= 1914                            \
		|| SOAGEN_HAS_BUILTIN(__builtin_launder)
		#define SOAGEN_LAUNDER(...) __builtin_launder(__VA_ARGS__)
	#else
		#define SOAGEN_LAUNDER(...) __VA_ARGS__
	#endif
#endif

#ifndef SOAGEN_COLUMN
	#define SOAGEN_COLUMN(Table, Column)                                                                               \
		SOAGEN_PURE_INLINE_GETTER                                                                                      \
		SOAGEN_ATTR(returns_nonnull)                                                                                   \
		SOAGEN_ATTR(                                                                                                   \
			assume_aligned(soagen::actual_alignment<soagen::remove_cvref<Table>, static_cast<std::size_t>(Column)>))
#endif

#define soagen_aligned_storage(...) soagen::assume_aligned<alignof(storage_type)>(__VA_ARGS__)

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

		//--- pointers -------------------------------------------------------------------------------------------------

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type* ptr(std::byte* p) noexcept
		{
			SOAGEN_ASSUME(p != nullptr);

			if constexpr (std::is_same_v<storage_type, std::byte>)
				return p;
			else
				return SOAGEN_LAUNDER(reinterpret_cast<storage_type*>(p));
		}

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr const storage_type* ptr(const std::byte* p) noexcept
		{
			SOAGEN_ASSUME(p != nullptr);

			if constexpr (std::is_same_v<storage_type, std::byte>)
				return p;
			else
				return SOAGEN_LAUNDER(reinterpret_cast<const storage_type*>(p));
		}

		//--- dereferencing --------------------------------------------------------------------------------------------

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& get(std::byte* p) noexcept
		{
			return *ptr(p);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr const storage_type& get(const std::byte* p) noexcept
		{
			SOAGEN_ASSUME(p != nullptr);

			return *ptr(p);
		}

		//--- default construction -------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_default_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& default_construct(std::byte* destination) //
			noexcept(std::is_nothrow_default_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(destination != nullptr);

#if defined(__cpp_lib_start_lifetime_as) && __cpp_lib_start_lifetime_as >= 202207
			if constexpr (is_implicit_lifetime_type<storage_type>)
			{
				return *(std::start_lifetime_as<storage_type>(destination));
			}
			else
			{
#endif
				return *(::new (static_cast<void*>(destination)) storage_type);

#if defined(__cpp_lib_start_lifetime_as) && __cpp_lib_start_lifetime_as >= 202207
			}
#endif
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_default_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& default_construct(std::byte* buffer, size_t index) //
			noexcept(std::is_nothrow_default_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			return default_construct(buffer + index * sizeof(storage_type));
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_default_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct(std::byte* buffer, size_t index, size_t count) //
			noexcept(std::is_nothrow_default_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);
			SOAGEN_ASSUME(count);

#if defined(__cpp_lib_start_lifetime_as) && __cpp_lib_start_lifetime_as >= 2022071
			if constexpr (is_implicit_lifetime_type<storage_type>)
			{
				std::start_lifetime_as_array<storage_type>(destination, count);
			}
			else
#endif
				if constexpr (std::is_nothrow_default_constructible_v<storage_type>
							  || std::is_trivially_destructible_v<storage_type>)
			{
				for (const size_t e = index + count; index < e; index++)
					default_construct(buffer, index);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = index;

				try
				{
					for (const size_t e = index + count; i < e; i++)
						default_construct(buffer, i);
				}
				catch (...)
				{
					for (; i-- > index;)
						destruct(buffer, i);
					throw;
				}
			}
		}

		//--- construction ---------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_copyable = std::is_trivially_copyable_v<storage_type>;

		// constructibility using memcpy

	  private:
		template <typename Arg>
		struct is_constructible_with_memcpy_
			: std::conjunction<
				  std::bool_constant<sizeof(storage_type) == sizeof(remove_cvref<Arg>)>, //
				  std::is_trivially_copyable<storage_type>,								 //
				  is_implicit_lifetime_type_<storage_type>,								 //
				  std::is_trivially_copyable<remove_cvref<Arg>>,						 //
				  std::disjunction<
					  std::is_same<storage_type, remove_cvref<Arg>>,
					  std::bool_constant<any_same<storage_type, char, unsigned char, std::byte>
										 && any_same<remove_cvref<Arg>, char, unsigned char, std::byte>>,
					  std::conjunction<std::is_same<storage_type, void*>, std::is_pointer<remove_cvref<Arg>>>>>
		{};

	  public:
		template <typename Arg>
		static constexpr bool is_constructible_with_memcpy = is_constructible_with_memcpy_<Arg>::value;

		// constructibility by unpacking a tuple

	  private:
		template <typename Arg>
		struct is_constructible_by_unpacking_tuple_ //
			: detail::is_constructible_by_unpacking_tuple_<storage_type, Arg>
		{};

	  public:
		template <typename Arg>
		static constexpr bool is_constructible_by_unpacking_tuple = is_constructible_by_unpacking_tuple_<Arg>::value;

		// trivial-constructibility

	  private:
		template <typename... Args>
		struct is_trivially_constructible_ : std::is_trivially_constructible<storage_type, remove_cvref<Args>...>
		{};

		template <typename Arg>
		struct is_trivially_constructible_<Arg>
			: std::disjunction<std::is_trivially_constructible<storage_type, remove_cvref<Arg>>,
							   is_constructible_with_memcpy_<remove_cvref<Arg>>,
							   typename is_constructible_by_unpacking_tuple_<Arg>::is_trivial>
		{};

	  public:
		template <typename... Args>
		static constexpr bool is_trivially_constructible = is_trivially_constructible_<Args...>::value;

		// constructibility from arbitrary args

		template <typename... Args>
		struct is_constructible_trait : std::disjunction<is_trivially_constructible_<Args...>, //
														 std::is_constructible<storage_type, Args...>>
		{};
		template <typename Arg>
		struct is_constructible_trait<Arg> : std::disjunction<is_trivially_constructible_<Arg>, //
															  std::is_constructible<storage_type, Arg>,
															  is_constructible_by_unpacking_tuple_<Arg>>
		{};
		template <typename T>
		struct is_constructible_trait<T*> : std::disjunction<std::is_same<storage_type, void*>, //
															 is_trivially_constructible_<T*>,	//
															 std::is_constructible<storage_type, T*>>
		{};
		template <typename T>
		struct is_constructible_trait<T*&> : std::disjunction<std::is_same<storage_type, void*>, //
															  is_trivially_constructible_<T*&>,	 //
															  std::is_constructible<storage_type, T*&>>
		{};
		template <typename T>
		struct is_constructible_trait<T*&&> : std::disjunction<std::is_same<storage_type, void*>, //
															   is_trivially_constructible_<T*&&>, //
															   std::is_constructible<storage_type, T*&&>>
		{};
		template <typename... Args>
		struct is_constructible_trait<emplacer<Args...>&&> : is_constructible_trait<Args...>
		{};

		template <typename... Args>
		static constexpr bool is_constructible = is_constructible_trait<Args...>::value;

		// nothrow-constructibility from arbitrary args

		template <typename... Args>
		struct is_nothrow_constructible_trait : std::disjunction<is_trivially_constructible_<Args...>,
																 std::is_nothrow_constructible<storage_type, Args...>>
		{};
		template <typename Arg>
		struct is_nothrow_constructible_trait<Arg>
			: std::disjunction<is_trivially_constructible_<Arg>,
							   std::is_nothrow_constructible<storage_type, Arg>,
							   typename is_constructible_by_unpacking_tuple_<Arg>::is_nothrow>
		{};
		template <typename T>
		struct is_nothrow_constructible_trait<T*> : std::disjunction<std::is_same<storage_type, void*>, //
																	 is_trivially_constructible_<T*>,	//
																	 std::is_nothrow_constructible<storage_type, T*>>
		{};
		template <typename T>
		struct is_nothrow_constructible_trait<T*&> : std::disjunction<std::is_same<storage_type, void*>, //
																	  is_trivially_constructible_<T*&>,	 //
																	  std::is_nothrow_constructible<storage_type, T*&>>
		{};
		template <typename T>
		struct is_nothrow_constructible_trait<T*&&>
			: std::disjunction<std::is_same<storage_type, void*>, //
							   is_trivially_constructible_<T*&&>, //
							   std::is_nothrow_constructible<storage_type, T*&&>>
		{};
		template <typename... Args>
		struct is_nothrow_constructible_trait<emplacer<Args...>&&> : is_nothrow_constructible_trait<Args...>
		{};

		template <typename... Args>
		static constexpr bool is_nothrow_constructible = is_nothrow_constructible_trait<Args...>::value;

	  private:
		template <typename... Args, size_t... Indices>
		SOAGEN_ATTR(nonnull)
		SOAGEN_ALWAYS_INLINE
		static constexpr storage_type& construct_from_emplacer(std::byte* destination,
															   emplacer<Args...>&& args,
															   std::index_sequence<Indices...>) //
			noexcept(is_nothrow_constructible<Args...>)
		{
			static_assert((std::is_reference_v<Args> && ...));
			static_assert(sizeof...(Args) == sizeof...(Indices));
			SOAGEN_ASSUME(destination != nullptr);

			return construct(destination,
							 static_cast<Args>(*static_cast<std::add_pointer_t<std::remove_reference_t<Args>>>(
								 args.ptrs[Indices]))...);
		}

		template <typename Tuple, size_t... Indices>
		SOAGEN_ATTR(nonnull)
		SOAGEN_ALWAYS_INLINE
		static constexpr storage_type& construct_from_tuple(std::byte* destination,
															Tuple&& tuple,
															std::index_sequence<Indices...>) //
			noexcept(is_constructible_by_unpacking_tuple_<Tuple&&>::is_nothrow::value)
		{
			SOAGEN_ASSUME(destination != nullptr);

			return construct(destination, get_from_tuple<Indices>(static_cast<Tuple&&>(tuple))...);
		}

	  public:
		SOAGEN_CONSTRAINED_TEMPLATE(is_constructible<Args&&...>, typename... Args)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct(std::byte* destination, Args&&... args) //
			noexcept(is_nothrow_constructible<Args&&...>)
		{
			SOAGEN_ASSUME(destination != nullptr);

			if constexpr (sizeof...(Args) == 0)
			{
				return default_construct(destination);
			}
			else
			{
				if constexpr (sizeof...(Args) == 1 && (is_emplacer<std::remove_reference_t<Args>> && ...))
				{
					return construct_from_emplacer(
						destination,
						static_cast<Args&&>(args)...,
						std::make_index_sequence<std::tuple_size_v<remove_cvref<Args>>>{}...);
				}
				else if constexpr (sizeof...(Args) == 1 && (is_constructible_with_memcpy<Args&&> && ...))
				{
					std::memcpy(soagen_aligned_storage(destination), &args..., sizeof(storage_type));

					return get(destination);
				}
				else if constexpr (std::is_constructible_v<storage_type, Args&&...>)
				{
					if constexpr (std::is_aggregate_v<storage_type>)
					{
						return *(::new (static_cast<void*>(destination)) storage_type{ static_cast<Args&&>(args)... });
					}
					else
					{
						return *(::new (static_cast<void*>(destination)) storage_type(static_cast<Args&&>(args)...));
					}
				}
				else if constexpr (sizeof...(Args) == 1 && (is_constructible_by_unpacking_tuple<Args&&> && ...))
				{
					return construct_from_tuple(destination,
												static_cast<Args&&>(args)...,
												std::make_index_sequence<std::tuple_size_v<remove_cvref<Args>>>{}...);
				}
			}
		}

		SOAGEN_CONSTRAINED_TEMPLATE(is_constructible<Args&&...>, typename... Args)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct_at(std::byte* buffer, size_t index, Args&&... args) //
			noexcept(is_nothrow_constructible<Args&&...>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			if constexpr (sizeof...(Args) == 0)
			{
				return default_construct(buffer + index * sizeof(storage_type));
			}
			else
			{
				return construct(buffer + index * sizeof(storage_type), static_cast<Args&&>(args)...);
			}
		}

		//--- move-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_trivially_move_constructible = is_trivially_constructible<storage_type&&>;

		static constexpr bool is_move_constructible =
			is_trivially_move_constructible				  //
			|| std::is_move_constructible_v<storage_type> //
			|| (std::is_default_constructible_v<storage_type> && std::is_move_assignable_v<storage_type>);

		static constexpr bool is_nothrow_move_constructible =
			is_trivially_move_constructible
				? true
				: (std::is_move_constructible_v<storage_type> ? std::is_nothrow_move_constructible_v<storage_type>
															  : std::is_nothrow_default_constructible_v<storage_type>
																	&& std::is_nothrow_move_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_trivially_move_constructible || std::is_move_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<storage_type&&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_move_assignable_v<storage_type>);

				default_construct(destination);

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
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_construct(std::byte* dest_buffer,
											size_t dest_index,
											std::byte* source_buffer,
											size_t source_index) //
			noexcept(std::is_nothrow_move_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_construct(dest_buffer + dest_index * sizeof(storage_type),
								  source_buffer + source_index * sizeof(storage_type));
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_trivially_copy_constructible = is_trivially_constructible<const storage_type&>;

		static constexpr bool is_copy_constructible =
			is_trivially_copy_constructible				  //
			|| std::is_copy_constructible_v<storage_type> //
			|| (std::is_default_constructible_v<storage_type> && std::is_copy_assignable_v<storage_type>);

		static constexpr bool is_nothrow_copy_constructible =
			is_trivially_copy_constructible
				? true
				: (std::is_copy_constructible_v<storage_type> ? std::is_nothrow_copy_constructible_v<storage_type>
															  : std::is_nothrow_default_constructible_v<storage_type>
																	&& std::is_nothrow_copy_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_construct(std::byte* destination, const std::byte* source) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_trivially_copy_constructible || std::is_copy_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<const storage_type&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_copy_assignable_v<storage_type>);

				default_construct(destination);

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
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_construct(std::byte* dest_buffer,
											size_t dest_index,
											const std::byte* source_buffer,
											size_t source_index) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_construct(dest_buffer + dest_index * sizeof(storage_type),
								  source_buffer + source_index * sizeof(storage_type));
		}

		//--- move-or-copy-construction (whichever is possible) --------------------------------------------------------

		static constexpr bool is_move_or_copy_constructible = is_move_constructible || is_copy_constructible;

		static constexpr bool is_nothrow_move_or_copy_constructible =
			is_move_constructible ? is_nothrow_move_constructible : is_nothrow_copy_constructible;

		static constexpr bool is_trivially_move_or_copy_constructible =
			is_move_constructible ? is_trivially_move_constructible : is_trivially_copy_constructible;

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_move_constructible)
			{
				return move_construct(destination, source);
			}
			else
			{
				return copy_construct(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_construct(std::byte* dest_buffer,
													size_t dest_index,
													std::byte* source_buffer,
													size_t source_index) //
			noexcept(is_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_or_copy_construct(dest_buffer + dest_index * sizeof(storage_type),
										  source_buffer + source_index * sizeof(storage_type));
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
		static constexpr void destruct([[maybe_unused]] std::byte* buffer, //
									   [[maybe_unused]] size_t index)	   //
			noexcept(std::is_nothrow_destructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			if constexpr (!std::is_trivially_destructible_v<storage_type>)
			{
				destruct(buffer + index * sizeof(storage_type));
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_move_assignable =
			is_constructible_with_memcpy<storage_type&&> || std::is_trivially_move_assignable_v<storage_type>;

		static constexpr bool is_move_assignable =
			is_trivially_move_assignable			   //
			|| std::is_move_assignable_v<storage_type> //
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_move_constructible_v<storage_type>);

		static constexpr bool is_nothrow_move_assignable =
			is_trivially_move_assignable
				? true
				: (std::is_move_assignable_v<storage_type> ? std::is_nothrow_move_assignable_v<storage_type>
														   : std::is_nothrow_destructible_v<storage_type>
																 && std::is_nothrow_move_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_assign(std::byte* destination, void* source) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_constructible_with_memcpy<storage_type&&>)
			{
				std::memcpy(soagen_aligned_storage(destination),
							soagen_aligned_storage(static_cast<std::byte*>(source)),
							sizeof(storage_type));

				return get(destination);
			}
			else if constexpr (is_trivially_move_assignable || std::is_move_assignable_v<storage_type>)
			{
				return get(destination) = static_cast<storage_type&&>(get(static_cast<std::byte*>(source)));
			}
			else
			{
				// note we only fall back to this if they're nothrow because we don't want to leave the destination
				// in a half-constructed state (it existed before the assignment, it should still exist after)
				static_assert(std::is_nothrow_destructible_v<storage_type>);
				static_assert(std::is_nothrow_move_constructible_v<storage_type>);

				destruct(destination);
				return move_construct(destination, static_cast<std::byte*>(source));
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_assign(std::byte* dest_buffer,
										 size_t dest_index,
										 std::byte* source_buffer,
										 size_t source_index) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_assign(dest_buffer + dest_index * sizeof(storage_type),
							   source_buffer + source_index * sizeof(storage_type));
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_copy_assignable =
			is_constructible_with_memcpy<const storage_type&> || std::is_trivially_copy_assignable_v<storage_type>;

		static constexpr bool is_copy_assignable =
			is_trivially_copy_assignable			   //
			|| std::is_copy_assignable_v<storage_type> //
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_copy_constructible_v<storage_type>);

		static constexpr bool is_nothrow_copy_assignable =
			is_trivially_copy_assignable
				? true
				: (std::is_copy_assignable_v<storage_type> ? std::is_nothrow_copy_assignable_v<storage_type>
														   : std::is_nothrow_destructible_v<storage_type>
																 && std::is_nothrow_copy_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_assign(std::byte* destination, const std::byte* source) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_constructible_with_memcpy<storage_type&&>)
			{
				std::memcpy(soagen_aligned_storage(destination),
							soagen_aligned_storage(static_cast<const std::byte*>(source)),
							sizeof(storage_type));

				return get(destination);
			}
			else if constexpr (is_trivially_copy_assignable || std::is_copy_assignable_v<storage_type>)
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
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_assign(std::byte* dest_buffer,
										 size_t dest_index,
										 const std::byte* source_buffer,
										 size_t source_index) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_assign(dest_buffer + dest_index * sizeof(storage_type),
							   source_buffer + source_index * sizeof(storage_type));
		}

		//--- move-or-copy-assignment (whichever is possible) --------------------------------------------------------

		static constexpr bool is_move_or_copy_assignable = is_move_assignable || is_copy_assignable;

		static constexpr bool is_nothrow_move_or_copy_assignable =
			is_move_assignable ? is_nothrow_move_assignable : is_nothrow_copy_assignable;

		static constexpr bool is_trivially_move_or_copy_assignable =
			is_move_assignable ? is_trivially_move_assignable : is_trivially_copy_assignable;

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_assign(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_move_assignable)
			{
				return move_assign(destination, source);
			}
			else
			{
				return copy_assign(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_assign(std::byte* dest_buffer,
												 size_t dest_index,
												 std::byte* source_buffer,
												 size_t source_index) //
			noexcept(is_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_or_copy_assign(dest_buffer + dest_index * sizeof(storage_type),
									   source_buffer + source_index * sizeof(storage_type));
		}

		//--- swap -----------------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_swappable =
			is_trivially_copyable
			&& (std::is_scalar_v<storage_type> || std::is_fundamental_v<storage_type>
				|| !std::is_swappable_v<storage_type> // we don't want to usurp user-defined swap() functions
			);

		static constexpr bool is_swappable = is_trivially_swappable //
										  || std::is_swappable_v<storage_type>
										  || (is_move_or_copy_constructible && is_move_or_copy_assignable);

		static constexpr bool is_nothrow_swappable =
			is_trivially_swappable
			|| (std::is_swappable_v<storage_type>
					? std::is_nothrow_swappable_v<storage_type>
					: (is_nothrow_move_or_copy_constructible && is_nothrow_move_or_copy_assignable));

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_swappable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static void swap(std::byte* lhs, std::byte* rhs) //
			noexcept(is_nothrow_swappable)
		{
			SOAGEN_ASSUME(lhs != nullptr);
			SOAGEN_ASSUME(rhs != nullptr);
			SOAGEN_ASSUME(lhs != rhs);

			if constexpr (is_trivially_swappable)
			{
				alignas(storage_type) std::byte temp[sizeof(storage_type)];
				std::memcpy(soagen_aligned_storage(temp), soagen_aligned_storage(lhs), sizeof(storage_type));
				std::memcpy(soagen_aligned_storage(lhs), soagen_aligned_storage(rhs), sizeof(storage_type));
				std::memcpy(soagen_aligned_storage(rhs), soagen_aligned_storage(temp), sizeof(storage_type));
			}
			else if constexpr (std::is_swappable_v<storage_type>)
			{
				using std::swap;
				swap(get(lhs), get(rhs));
			}
			else
			{
				static_assert(is_move_or_copy_constructible && is_move_or_copy_assignable);

				alignas(storage_type) std::byte temp[sizeof(storage_type)];

				if constexpr ((is_nothrow_move_or_copy_constructible && is_nothrow_move_or_copy_assignable)
							  || std::is_trivially_destructible_v<storage_type>)
				{
					move_or_copy_construct(soagen_aligned_storage(temp), lhs);
					move_or_copy_assign(lhs, rhs);
					move_or_copy_assign(rhs, soagen_aligned_storage(temp));
					destruct(temp);
				}
				else
				{
					bool needs_destruct = false;
					try
					{
						move_or_copy_construct(soagen_aligned_storage(temp), lhs);
						needs_destruct = true;
						move_or_copy_assign(lhs, rhs);
						move_or_copy_assign(rhs, soagen_aligned_storage(temp));
					}
					catch (...)
					{
						if (needs_destruct)
							destruct(temp);
						throw;
					}
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_swappable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static void swap(std::byte* lhs_buffer,
						 size_t lhs_index,
						 std::byte* rhs_buffer,
						 size_t rhs_index,
						 size_t count = 1) //
			noexcept(is_nothrow_swappable)
		{
			SOAGEN_ASSUME(lhs_buffer != nullptr);
			SOAGEN_ASSUME(rhs_buffer != nullptr);

			if SOAGEN_UNLIKELY(!count)
				return;

			lhs_buffer = soagen_aligned_storage(lhs_buffer + lhs_index * sizeof(storage_type));
			rhs_buffer = soagen_aligned_storage(rhs_buffer + rhs_index * sizeof(storage_type));

			[[maybe_unused]] SOAGEN_CPP23_STATIC_CONSTEXPR size_t trivial_buf_size = 2048u / sizeof(storage_type);

			if constexpr (is_trivially_swappable && trivial_buf_size > 1)
			{
				for (size_t i = 0; i < count; i += trivial_buf_size)
				{
					const auto num = soagen::min(trivial_buf_size, count - i);

					alignas(storage_type) std::byte buf[sizeof(storage_type) * trivial_buf_size];
					std::memcpy(buf, lhs_buffer, sizeof(storage_type) * num);
					std::memcpy(lhs_buffer, rhs_buffer, sizeof(storage_type) * num);
					std::memcpy(rhs_buffer, buf, sizeof(storage_type) * num);

					lhs_buffer = soagen_aligned_storage(lhs_buffer + trivial_buf_size * sizeof(storage_type));
					rhs_buffer = soagen_aligned_storage(rhs_buffer + trivial_buf_size * sizeof(storage_type));
				}
			}
			else
			{
				const auto end = lhs_buffer + count * sizeof(storage_type);
				for (; lhs_buffer < end; lhs_buffer += sizeof(storage_type), rhs_buffer += sizeof(storage_type))
				{
					swap(lhs_buffer, rhs_buffer);
				}
			}
		}

		//--- memmove ---------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_trivially_copyable)
		static constexpr void memmove(std::byte* dest_buffer,
									  size_t dest_index,
									  const std::byte* source_buffer,
									  size_t source_index,
									  size_t count = 1) noexcept
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			std::memmove(soagen_aligned_storage(dest_buffer + dest_index * sizeof(storage_type)),
						 soagen_aligned_storage(source_buffer + source_index * sizeof(storage_type)),
						 count * sizeof(storage_type));
		}

		//--- equality -------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_equality_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool equal(const std::byte* lhs, const std::byte* rhs) //
			noexcept(is_nothrow_equality_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs != nullptr);
			SOAGEN_ASSUME(rhs != nullptr);

			return get(lhs) == get(rhs);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_equality_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool equal(const std::byte* lhs_buffer,
									size_t lhs_index,
									const std::byte* rhs_buffer,
									size_t rhs_index,
									size_t count = 1) //
			noexcept(is_nothrow_equality_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs_buffer != nullptr);
			SOAGEN_ASSUME(rhs_buffer != nullptr);

			lhs_buffer += lhs_index * sizeof(storage_type);
			rhs_buffer += rhs_index * sizeof(storage_type);
			const auto end = lhs_buffer + count * sizeof(storage_type);

			for (; lhs_buffer < end; lhs_buffer += sizeof(storage_type), rhs_buffer += sizeof(storage_type))
			{
				if (!equal(lhs_buffer, rhs_buffer))
					return false;
			}

			return true;
		}

		//--- less-than ------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_less_than_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool less_than(const std::byte* lhs, const std::byte* rhs) //
			noexcept(is_nothrow_less_than_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs != nullptr);
			SOAGEN_ASSUME(rhs != nullptr);

			return get(lhs) < get(rhs);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_less_than_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool less_than(const std::byte* lhs_buffer,
										size_t lhs_index,
										const std::byte* rhs_buffer,
										size_t rhs_index) //
			noexcept(is_nothrow_less_than_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs_buffer != nullptr);
			SOAGEN_ASSUME(rhs_buffer != nullptr);

			return less_than(lhs_buffer + lhs_index * sizeof(storage_type),
							 rhs_buffer + rhs_index * sizeof(storage_type));
		}
	};
}

namespace soagen
{
	template <typename ValueType,
			  size_t Align		 = alignof(ValueType),
			  typename ParamType = soagen::param_type<ValueType>>
	struct SOAGEN_EMPTY_BASES column_traits //
		SOAGEN_HIDDEN_BASE(public detail::column_traits_base<storage_type<ValueType>>)
	{
		using base_traits = detail::column_traits_base<storage_type<ValueType>>;

		using value_type = ValueType;
		static_assert(!std::is_reference_v<value_type>, "column value_type may not be a reference");
		static_assert(!std::is_void_v<value_type>, "column value_type may not be void");
		static_assert(alignof(value_type) == alignof(typename base_traits::storage_type));
		static_assert(sizeof(value_type) == sizeof(typename base_traits::storage_type));

		static constexpr size_t alignment = max(Align, alignof(value_type));
		static_assert(has_single_bit(alignment), "column alignment must be a power of two");

		static constexpr size_t aligned_stride = lcm(alignment, sizeof(value_type)) / sizeof(value_type);

		using param_type = ParamType;
		static_assert(!std::is_void_v<param_type>, "column param_type may not be void");
		static_assert(std::is_reference_v<param_type> || !is_cv<param_type>,
					  "value parameters may not be cv-qualified");
		static_assert(base_traits::template is_constructible<param_type>);

		using param_forward_type = forward_type<param_type>;

		using rvalue_type = soagen::rvalue_type<param_type>;
		static_assert(base_traits::template is_constructible<rvalue_type>);

		using rvalue_forward_type = forward_type<rvalue_type>;

		using default_emplace_type =
			std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<rvalue_type>>>;
	};

	namespace detail
	{
		template <typename>
		struct is_column_traits_ : std::false_type
		{};
		template <typename ValueType, size_t Align, typename ParamType>
		struct is_column_traits_<column_traits<ValueType, Align, ParamType>> : std::true_type
		{};
		template <typename StorageType>
		struct is_column_traits_<detail::column_traits_base<StorageType>> : std::true_type
		{};
	}

	template <typename T>
	inline constexpr bool is_column_traits =
		detail::is_column_traits_<std::remove_cv_t<T>>::value;
}

namespace soagen::detail
{
	template <typename T>
	struct to_base_traits_;
	template <typename ValueType, size_t Align, typename ParamType>
	struct to_base_traits_<column_traits<ValueType, Align, ParamType>>
	{
		using type = column_traits_base<storage_type<ValueType>>;

		static_assert(std::is_base_of_v<type, column_traits<ValueType, Align, ParamType>>);
	};
	template <typename T>
	using to_base_traits = typename to_base_traits_<T>::type;

	template <typename T>
	struct as_column_
	{
		using type = column_traits<T>;
	};
	template <typename ValueType, size_t Align, typename ParamType>
	struct as_column_<column_traits<ValueType, Align, ParamType>>
	{
		using type = column_traits<ValueType, Align, ParamType>;
	};
	template <typename StorageType>
	struct as_column_<detail::column_traits_base<StorageType>>
	{
		using type = detail::column_traits_base<StorageType>;
	};
	template <typename T>
	using as_column = typename as_column_<T>::type;
}

#undef soagen_aligned_storage

//********  names.hpp  *************************************************************************************************

#ifndef SOAGEN_MAKE_NAME
	#define SOAGEN_MAKE_NAME(Name)                                                                                     \
                                                                                                                       \
		struct name_tag_##Name;                                                                                        \
                                                                                                                       \
		template <>                                                                                                    \
		struct SOAGEN_EMPTY_BASES name_constant<name_tag_##Name>                                                       \
		{                                                                                                              \
			static constexpr const char value[] = #Name;                                                               \
		};                                                                                                             \
                                                                                                                       \
		template <typename T>                                                                                          \
		struct SOAGEN_EMPTY_BASES named_ref<name_tag_##Name, T>                                                        \
		{                                                                                                              \
			static_assert(std::is_reference_v<T>);                                                                     \
                                                                                                                       \
			T Name;                                                                                                    \
                                                                                                                       \
		  protected:                                                                                                   \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr T get_ref() const noexcept                                                                       \
			{                                                                                                          \
				return static_cast<T>(Name);                                                                           \
			}                                                                                                          \
		};                                                                                                             \
                                                                                                                       \
		template <typename Derived, size_t Column>                                                                     \
		struct SOAGEN_EMPTY_BASES named_func<name_tag_##Name, Derived, Column>                                         \
		{                                                                                                              \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr decltype(auto) Name() noexcept                                                                   \
			{                                                                                                          \
				using return_type = decltype(static_cast<Derived&>(*this).template column<Column>());                  \
				if constexpr (std::is_reference_v<return_type>)                                                        \
					return static_cast<return_type>(static_cast<Derived&>(*this).template column<Column>());           \
				else                                                                                                   \
					return static_cast<Derived&>(*this).template column<Column>();                                     \
			}                                                                                                          \
                                                                                                                       \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr decltype(auto) Name() const noexcept                                                             \
			{                                                                                                          \
				using return_type = decltype(static_cast<const Derived&>(*this).template column<Column>());            \
				if constexpr (std::is_reference_v<return_type>)                                                        \
					return static_cast<return_type>(static_cast<const Derived&>(*this).template column<Column>());     \
				else                                                                                                   \
					return static_cast<const Derived&>(*this).template column<Column>();                               \
			}                                                                                                          \
		}
#endif

#ifndef SOAGEN_MAKE_NAMED_COLUMN
	#define SOAGEN_MAKE_NAMED_COLUMN(Soa, Column, Name)                                                                \
                                                                                                                       \
		template <>                                                                                                    \
		struct SOAGEN_EMPTY_BASES column_name_tag_<Soa, Column>                                                        \
		{                                                                                                              \
			using type = name_tag_##Name;                                                                              \
		}
#endif

#ifndef SOAGEN_MAKE_GENERIC_NAMED_COLUMN
	#define SOAGEN_MAKE_GENERIC_NAMED_COLUMN(Column, Name)                                                             \
                                                                                                                       \
		SOAGEN_MAKE_NAME(Name);                                                                                        \
                                                                                                                       \
		template <typename Soa>                                                                                        \
		struct SOAGEN_EMPTY_BASES column_name_tag_<Soa, Column>                                                        \
		{                                                                                                              \
			using type = name_tag_##Name;                                                                              \
		}
#endif

namespace soagen::detail
{
	//--- name constants ---------

	template <typename Tag>
	struct name_constant;

	template <>
	struct name_constant<void>
	{
		static constexpr const char value[] = "";
	};

	//--- named ref type ---------

	template <typename Tag, typename>
	struct named_ref;

	template <typename T>
	struct SOAGEN_EMPTY_BASES named_ref<void, T> // unnamed columns
	{
		static_assert(std::is_reference_v<T>);

	  protected:
		T val_;

		SOAGEN_PURE_INLINE_GETTER
		constexpr T get_ref() const noexcept
		{
			return static_cast<T>(val_);
		}

	  public:
		template <typename Val>
		SOAGEN_NODISCARD_CTOR
		constexpr named_ref(Val&& val) noexcept //
			: val_{ static_cast<Val&&>(val) }
		{}

		SOAGEN_DEFAULT_RULE_OF_FIVE(named_ref);
	};

	//--- named func type ---------

	template <typename Tag, typename Derived, size_t>
	struct named_func;

	template <typename Derived, size_t Column>
	struct SOAGEN_EMPTY_BASES named_func<void, Derived, Column>
	{
		// unnamed columns do not contribute any functions
	};

	//--- column names ---------

	template <typename Soa, size_t Column>
	struct column_name_tag_
	{
		using type = void; // unnamed columns
	};
	template <typename Soa, auto Column>
	using column_name_tag = typename column_name_tag_<soa_type<Soa>, static_cast<size_t>(Column)>::type;

	template <typename Soa, auto Column>
	using column_name = name_constant<column_name_tag<Soa, Column>>;

	//--- generic fallback names ---------

	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(0, first);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(1, second);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(2, third);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(3, fourth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(4, fifth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(5, sixth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(6, seventh);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(7, eighth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(8, ninth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(9, tenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(10, eleventh);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(11, twelfth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(12, thirteenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(13, fourteenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(14, fifteenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(15, sixteenth);

	//--- column references ---------

	template <typename Soa, size_t Column>
	struct SOAGEN_EMPTY_BASES column_ref //
		: named_ref<column_name_tag<Soa, Column>, value_ref<Soa, Column>>
	{
		static_assert(!std::is_lvalue_reference_v<Soa>);
	};

	//--- column functions ---------

	template <typename Soa, size_t Column>
	struct SOAGEN_EMPTY_BASES column_func //
		: named_func<column_name_tag<Soa, Column>, Soa, Column>
	{
		static_assert(!is_cvref<Soa>);
	};
}

//********  invoke.hpp  ************************************************************************************************

namespace soagen
{
	namespace detail
	{
		template <typename Func, typename... Args>
		struct is_invocable_ : nested_trait_<types<Func, Args...>, std::is_invocable, std::is_nothrow_invocable>
		{
			using base = nested_trait_<types<Func, Args...>, std::is_invocable, std::is_nothrow_invocable>;

			using is_nothrow = typename base::template nested<0>;
		};
	}

	template <typename Func, typename... Args>
	inline constexpr bool is_invocable = detail::is_invocable_<Func, Args...>::value;

	template <typename Func, typename... Args>
	inline constexpr bool is_nothrow_invocable =
		detail::is_invocable_<Func, Args...>::is_nothrow::value;

	namespace detail
	{
		template <size_t I, typename Func, typename Arg>
		struct is_invocable_with_optional_index_							//
			: std::disjunction<is_invocable_<Func, Arg, index_constant<I>>, //
							   is_invocable_<Func, Arg, size_t>,			//
							   is_invocable_<Func, index_constant<I>, Arg>, //
							   is_invocable_<Func, size_t, Arg>,			//
							   is_invocable_<Func, Arg>>
		{};
	}

	template <size_t I, typename Func, typename Arg>
	inline constexpr bool is_invocable_with_optional_index =
		detail::is_invocable_with_optional_index_<I, Func, Arg>::value;

	template <size_t I, typename Func, typename Arg>
	inline constexpr bool is_nothrow_invocable_with_optional_index =
		detail::is_invocable_with_optional_index_<I, Func, Arg>::is_nothrow::value;

	template <size_t I, typename Func, typename Arg>
	SOAGEN_ALWAYS_INLINE
	constexpr decltype(auto) invoke_with_optional_index(Func&& func,
														Arg&& arg) //
		noexcept(is_nothrow_invocable_with_optional_index<I, Func&&, Arg&&>)
	{
		static_assert(is_invocable_with_optional_index<I, Func&&, Arg&&>);

		if constexpr (is_invocable<Func&&, Arg&&, index_constant<I>>)
		{
			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg), index_constant<I>{});
		}
		else if constexpr (is_invocable<Func&&, Arg&&, size_t>)
		{
			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg), I);
		}
		else if constexpr (is_invocable<Func&&, index_constant<I>, Arg&&>)
		{
			return static_cast<Func&&>(func)(index_constant<I>{}, static_cast<Arg&&>(arg));
		}
		else if constexpr (is_invocable<Func&&, size_t, Arg&&>)
		{
			return static_cast<Func&&>(func)(I, static_cast<Arg&&>(arg));
		}
		else
		{
			static_assert(is_invocable<Func&&, Arg&&>);

			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg));
		}
	}
}

//********  for_each.hpp  **********************************************************************************************

namespace soagen
{
	namespace detail
	{
		template <typename T, typename Func, size_t... Columns>
		SOAGEN_ALWAYS_INLINE
		void for_each_column(T&& obj, Func&& func, std::index_sequence<Columns...>) //
			noexcept(noexcept((invoke_with_optional_index<Columns>(std::declval<Func&&>(),
																   std::declval<T&&>().template column<Columns>()),
							   ...)))
		{
			(invoke_with_optional_index<Columns>(static_cast<Func&&>(func),
												 static_cast<T&&>(obj).template column<Columns>()),
			 ...);
		}
	}

	template <typename T, typename Func>
	void for_each_column(T&& obj, Func&& func) //
		noexcept(noexcept(
			detail::for_each_column(std::declval<T&&>(), std::declval<Func&&>(), column_indices<remove_cvref<T>>{})))
	{
		detail::for_each_column(static_cast<T&&>(obj), static_cast<Func&&>(func), column_indices<remove_cvref<T>>{});
	}
}

//********  row.hpp  ***************************************************************************************************

namespace soagen
{
	namespace detail
	{
		// general rules for allowing implicit conversions:
		// - losing rvalue (T&& -> T&), (const T&& -> const T&)
		// - gaining const (T& -> const T&, T&& -> const T&&)
		// - both

		template <typename From, typename To>
		inline constexpr bool implicit_conversion_ok = false;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T, T> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&, const T&> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&&, T&> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&&, const T&> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&&, const T&&> = true;

		// general rules for allowing explicit conversions:
		template <typename From, typename To>
		inline constexpr bool explicit_conversion_ok = false;

		template <typename T>
		inline constexpr bool explicit_conversion_ok<T&, T&&> = true;

		template <typename T>
		inline constexpr bool explicit_conversion_ok<T&, const T&&> = true;

		// tests for compatible column permutations:
		template <typename From, typename To>
		inline constexpr bool column_conversion_ok = false;

		template <size_t... Columns>
		inline constexpr bool column_conversion_ok<std::index_sequence<Columns...>, std::index_sequence<Columns...>> =
			true;

		template <size_t... FromColumns, size_t... ToColumns>
		inline constexpr bool
			column_conversion_ok<std::index_sequence<FromColumns...>, std::index_sequence<ToColumns...>> =
				(any_same_value<ToColumns, FromColumns...> && ...);

		// row implicit conversions:
		template <typename From, typename To>
		inline constexpr bool row_implicit_conversion_ok = false;

		template <typename From, size_t... FromColumns, typename To, size_t... ToColumns>
		inline constexpr bool row_implicit_conversion_ok<row<From, FromColumns...>, //
														 row<To, ToColumns...>> =
			implicit_conversion_ok<coerce_ref<From>, coerce_ref<To>>
			&& column_conversion_ok<std::index_sequence<FromColumns...>, std::index_sequence<ToColumns...>>;

		// row explicit conversions:
		template <typename From, typename To>
		inline constexpr bool row_explicit_conversion_ok = false;

		template <typename From, size_t... FromColumns, typename To, size_t... ToColumns>
		inline constexpr bool row_explicit_conversion_ok<row<From, FromColumns...>, //
														 row<To, ToColumns...>> =
			explicit_conversion_ok<coerce_ref<From>, coerce_ref<To>>
			&& column_conversion_ok<std::index_sequence<FromColumns...>, std::index_sequence<ToColumns...>>;
	}

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES row_base
	{};

	template <typename Soa, size_t... Columns>
	struct SOAGEN_EMPTY_BASES row //
	SOAGEN_HIDDEN_BASE(public detail::column_ref<Soa, Columns>..., public row_base<row<Soa, Columns...>>)
	{
		static_assert(is_soa<remove_cvref<Soa>>, "Soa must be a table or soagen-generated SoA type.");
		static_assert(!std::is_lvalue_reference_v<Soa>, "Soa may not be an lvalue reference.");
		static_assert(sizeof...(Columns), "Column index list may not be empty.");
		static_assert(std::is_empty_v<row_base<row<Soa, Columns...>>>,
					  "row_base specializations may not have data members");
		static_assert(std::is_trivial_v<row_base<row<Soa, Columns...>>>, "row_base specializations must be trivial");

		SOAGEN_NODISCARD_CTOR
		constexpr row(value_ref<Soa, Columns>... args) noexcept //
			: detail::column_ref<Soa, Columns>{ static_cast<value_ref<Soa, Columns>>(args) }...
		{}

		SOAGEN_DEFAULT_RULE_OF_FIVE(row);

		template <auto Column>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) column() const noexcept
		{
			static_assert(static_cast<size_t>(Column) < table_traits_type<Soa>::column_count,
						  "column index out of range");

			return detail::column_ref<Soa, static_cast<size_t>(Column)>::get_ref();
		}

		template <typename Func>
		SOAGEN_ALWAYS_INLINE
		constexpr void for_each_column(Func&& func) const //
			noexcept(noexcept(soagen::for_each_column(std::declval<const row&>(), std::declval<Func&&>())))
		{
			soagen::for_each_column(*this, static_cast<Func&&>(func));
		}

		template <auto Member>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) get() const noexcept
		{
			static_assert(Member < sizeof...(Columns), "member index out of range");

			return type_at_index<static_cast<size_t>(Member), detail::column_ref<Soa, Columns>...>::get_ref();
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_equality_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator==(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_equality_comparable)
		{
			if constexpr (std::is_same_v<Soa, T>)
			{
				if (&lhs == &rhs)
					return true;
			}
			return ((lhs.template column<Columns>() == rhs.template column<Columns>()) && ...);
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_equality_comparable),
									typename T)
		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_equality_comparable)
		{
			return !(lhs == rhs);
		}

	  private:
		template <size_t Member, typename T>
		SOAGEN_NODISCARD
		static constexpr int row_compare_impl(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			if constexpr (std::is_same_v<Soa, T>)
			{
				if (&lhs == &rhs)
					return 0;
			}

			if (lhs.template get<Member>() < rhs.template get<Member>())
				return -1;

			if (rhs.template get<Member>() < lhs.template get<Member>())
				return 1;

			if constexpr (Member + 1u == sizeof...(Columns))
				return 0;
			else
				return row_compare_impl<Member + 1u>(lhs, rhs);
		}

	  public:
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) < 0;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) <= 0;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) > 0;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) >= 0;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((detail::row_implicit_conversion_ok<row, row<T, Cols...>>
									 && !detail::row_explicit_conversion_ok<row, row<T, Cols...>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator row<T, Cols...>() const noexcept
		{
			return row<T, Cols...>{ static_cast<value_ref<T, Cols>>(this->template column<Cols>())... };
		}

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::row_implicit_conversion_ok<row, row<T, Cols...>>
									 && detail::row_explicit_conversion_ok<row, row<T, Cols...>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator row<T, Cols...>() const noexcept
		{
			return row<T, Cols...>{ static_cast<value_ref<T, Cols>>(this->template column<Cols>())... };
		}
	};
}

namespace std
{
	template <typename Soa, size_t... Columns>
	struct tuple_size<soagen::row<Soa, Columns...>> //
		: std::integral_constant<size_t, sizeof...(Columns)>
	{};

	template <size_t Member, typename Soa, size_t... Columns>
	struct tuple_element<Member, soagen::row<Soa, Columns...>>
	{
		using type = decltype(std::declval<soagen::row<Soa, Columns...>>().template get<Member>());
	};
}

//********  table_traits.hpp  ******************************************************************************************

namespace soagen::detail
{
	// a base class for the table traits that handles all the non-alignment-dependent stuff -
	// the 'Columns' argument seen by this class should be the column_base_traits, NOT soagen::column_traits
	// (to minimize template instantiation explosion)
	template <typename...>
	struct table_traits_base;

	template <size_t... I, typename... Columns>
	struct table_traits_base<std::index_sequence<I...>, Columns...>
	{
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		static_assert(std::is_same_v<std::index_sequence<I...>, std::make_index_sequence<sizeof...(Columns)>>,
					  "index sequence must match columns");

		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		// columns

		template <auto Index>
		using column = type_at_index<static_cast<size_t>(Index), Columns...>;

		template <typename IndexConstant>
		using column_from_ic = type_at_index<static_cast<size_t>(remove_cvref<IndexConstant>::value), Columns...>;

		template <auto Index>
		using storage_type = typename column<static_cast<size_t>(Index)>::storage_type;

		using column_pointers		= std::byte* [column_count];
		using const_column_pointers = std::byte* const[column_count];

		static constexpr size_t column_sizes[column_count] = { sizeof(typename Columns::storage_type)... };

		// default constructibility

		static constexpr bool all_default_constructible =
			std::conjunction<std::is_default_constructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_default_constructible =
			std::conjunction<std::is_nothrow_default_constructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_trivially_default_constructible =
			std::conjunction<std::is_trivially_default_constructible<typename Columns::storage_type>...>::value;

		// trivial-copyability (memcpy + memmove)

		static constexpr bool all_trivially_copyable = (Columns::is_trivially_copyable && ...);

		static constexpr bool any_trivially_copyable = (false || ... || Columns::is_trivially_copyable);

		// move-constructibility

		static constexpr bool all_move_constructible = (Columns::is_move_constructible && ...);

		static constexpr bool all_nothrow_move_constructible = (Columns::is_nothrow_move_constructible && ...);

		static constexpr bool all_trivially_move_constructible = (Columns::is_trivially_move_constructible && ...);

		// move-assignability

		static constexpr bool all_move_assignable = (Columns::is_move_assignable && ...);

		static constexpr bool all_nothrow_move_assignable = (Columns::is_nothrow_move_assignable && ...);

		static constexpr bool all_trivially_move_assignable = (Columns::is_trivially_move_assignable && ...);

		// copy-constructibility

		static constexpr bool all_copy_constructible = (Columns::is_copy_constructible && ...);

		static constexpr bool all_nothrow_copy_constructible = (Columns::is_nothrow_copy_constructible && ...);

		static constexpr bool all_trivially_copy_constructible = (Columns::is_trivially_copy_constructible && ...);

		// copy-assignability

		static constexpr bool all_copy_assignable = (Columns::is_copy_assignable && ...);

		static constexpr bool all_nothrow_copy_assignable = (Columns::is_nothrow_copy_assignable && ...);

		static constexpr bool all_trivially_copy_assignable = (Columns::is_trivially_copy_assignable && ...);

		// move-or-copy-constructibility

		static constexpr bool all_move_or_copy_constructible = (Columns::is_move_or_copy_constructible && ...);

		static constexpr bool all_nothrow_move_or_copy_constructible =
			(Columns::is_nothrow_move_or_copy_constructible && ...);

		static constexpr bool all_trivially_move_or_copy_constructible =
			(Columns::is_trivially_move_or_copy_constructible && ...);

		// move-or-copy-assignability

		static constexpr bool all_move_or_copy_assignable = (Columns::is_move_or_copy_assignable && ...);

		static constexpr bool all_nothrow_move_or_copy_assignable =
			(Columns::is_nothrow_move_or_copy_assignable && ...);

		static constexpr bool all_trivially_move_or_copy_assignable =
			(Columns::is_trivially_move_or_copy_assignable && ...);

		// destructibility

		static constexpr bool all_destructible =
			std::conjunction<std::is_destructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_destructible =
			std::conjunction<std::is_nothrow_destructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_trivially_destructible =
			std::conjunction<std::is_trivially_destructible<typename Columns::storage_type>...>::value;

		// swappability

		static constexpr bool all_swappable = (Columns::is_swappable && ...);

		static constexpr bool all_nothrow_swappable = (Columns::is_nothrow_swappable && ...);

		// equality comparability

		static constexpr bool all_equality_comparable =
			std::conjunction<is_equality_comparable_<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_equality_comparable =
			std::conjunction<is_nothrow_equality_comparable_<typename Columns::storage_type>...>::value;

		// less-than comparability

		static constexpr bool all_less_than_comparable =
			std::conjunction<is_less_than_comparable_<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_less_than_comparable =
			std::conjunction<is_nothrow_less_than_comparable_<typename Columns::storage_type>...>::value;

		// row constructibility

	  private:
		template <size_t, typename...>
		struct row_constructible_from_tuple_ : std::false_type
		{};
		template <typename Tuple, size_t... Members>
		struct row_constructible_from_tuple_<column_count, Tuple, std::index_sequence<Members...>>
			: std::conjunction<typename Columns::template is_constructible_trait<decltype(get_from_tuple<Members>(
				  std::declval<Tuple>()))>...>
		{
			static_assert(std::is_same_v<std::index_sequence<Members...>, std::make_index_sequence<column_count>>);
		};

		template <bool, size_t, typename... Args>
		struct row_constructible_from_ : std::false_type
		{};
		template <typename... Args>
		struct row_constructible_from_<false, column_count, Args...>
			: std::conjunction<typename Columns::template is_constructible_trait<Args>...>
		{
			static_assert(sizeof...(Args) == column_count);
		};
		template <typename Tuple>
		struct row_constructible_from_<true, 1, Tuple>
			: row_constructible_from_tuple_<std::tuple_size_v<remove_cvref<Tuple>>,
											remove_cvref<Tuple>,
											std::make_index_sequence<std::tuple_size_v<remove_cvref<Tuple>>>>
		{};

	  public:
		template <typename... Args>
		static constexpr bool row_constructible_from =
			row_constructible_from_<(is_tuple<remove_cvref<Args>> && ...), sizeof...(Args), Args...>::value;

		// row constructibility (nothrow)

	  private:
		template <size_t, typename...>
		struct row_nothrow_constructible_from_tuple_ : std::false_type
		{};
		template <typename Tuple, size_t... Members>
		struct row_nothrow_constructible_from_tuple_<column_count, Tuple, std::index_sequence<Members...>>
			: std::conjunction<typename Columns::template is_nothrow_constructible_trait<
				  decltype(get_from_tuple<Members>(std::declval<Tuple>()))>...>
		{
			static_assert(std::is_same_v<std::index_sequence<Members...>, std::make_index_sequence<column_count>>);
		};

		template <bool, size_t, typename... Args>
		struct row_nothrow_constructible_from_ : std::false_type
		{};
		template <typename... Args>
		struct row_nothrow_constructible_from_<false, column_count, Args...>
			: std::conjunction<typename Columns::template is_nothrow_constructible_trait<Args>...>
		{
			static_assert(sizeof...(Args) == column_count);
		};
		template <typename Tuple>
		struct row_nothrow_constructible_from_<true, 1, Tuple>
			: row_nothrow_constructible_from_tuple_<std::tuple_size_v<remove_cvref<Tuple>>,
													remove_cvref<Tuple>,
													std::make_index_sequence<std::tuple_size_v<remove_cvref<Tuple>>>>
		{};

	  public:
		template <typename... Args>
		static constexpr bool row_nothrow_constructible_from =
			row_nothrow_constructible_from_<(is_tuple<remove_cvref<Args>> && ...), sizeof...(Args), Args...>::value;

		//--- memmove --------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_trivially_copyable)
		SOAGEN_CPP20_CONSTEXPR
		static void memmove(column_pointers& dest,
							size_t dest_start,
							column_pointers& source,
							size_t source_start,
							size_t count) //
			noexcept
		{
			(column<I>::memmove(dest[I], dest_start, source[I], source_start, count), ...);
		}

		//--- destruction ----------------------------------------------------------------------------------------------

		static constexpr void destruct_row(column_pointers& columns,
										   size_t index,
										   size_t leftmost_column,
										   size_t rightmost_column) noexcept
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
					static constexpr size_t column_index = decltype(ic)::value;

					if constexpr (!std::is_trivially_destructible_v<storage_type<column_index>>)
					{
						if (column_index >= leftmost_column && column_index <= rightmost_column)
							column<column_index>::destruct(columns[column_index], index);
					}
				};

				(destructor(index_constant<column_count - I - 1u>{}), ...);
			}
		}

		static constexpr void destruct_row(column_pointers& columns, size_t index) noexcept
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
					static constexpr size_t column_index = decltype(ic)::value;

					if constexpr (!std::is_trivially_destructible_v<storage_type<column_index>>)
					{
						column<column_index>::destruct(columns[column_index], index);
					}
				};

				(destructor(index_constant<column_count - I - 1u>{}), ...);
			}
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

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_default_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_row(column_pointers& columns,
										  size_t index) //
			noexcept(all_nothrow_default_constructible) //
		{
			static_assert(all_default_constructible);

			if constexpr (all_nothrow_default_constructible || all_trivially_destructible)
			{
				(column<I>::default_construct(columns[I], index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor = [&](auto ic) //
					noexcept(std::is_nothrow_default_constructible_v<storage_type<decltype(ic)::value>>)
				{
					column_from_ic<decltype(ic)>::default_construct(columns[decltype(ic)::value], index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(columns, index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_default_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_rows([[maybe_unused]] column_pointers& columns,
										   [[maybe_unused]] size_t start,
										   [[maybe_unused]] size_t count) //
			noexcept(all_nothrow_destructible)
		{
			if constexpr (all_nothrow_default_constructible || all_trivially_destructible)
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
					destruct_rows(columns, start, i - start);
					throw;
				}
			}
		}

		//--- construction ---------------------------------------------------------------------------------------------

	  private:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, typename Tuple, auto sfinae = row_constructible_from<Tuple&&>)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row_from_tuple(column_pointers& columns,
											 size_t index,
											 Tuple&& tuple) //
			noexcept(row_nothrow_constructible_from<Tuple&&>)
		{
			static_assert(std::tuple_size_v<remove_cvref<Tuple>> == column_count);

			construct_row(columns, index, get_from_tuple<I>(static_cast<Tuple&&>(tuple))...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, typename... Args, auto sfinae = row_constructible_from<Args&&...>)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row(column_pointers& columns, size_t index, Args&&... args) //
			noexcept(row_nothrow_constructible_from<Args&&...>)
		{
			if constexpr (sizeof...(Args) == 0)
			{
				default_construct_row(columns, index, static_cast<Args&&>(args)...);
			}
			else if constexpr (sizeof...(Args) == 1 && (is_tuple<remove_cvref<Args>> && ...))
			{
				construct_row_from_tuple(columns, index, static_cast<Args&&>(args)...);
			}
			else
			{
				static_assert(sizeof...(Args) == sizeof...(I));

				if constexpr (row_nothrow_constructible_from<Args&&...> || all_trivially_destructible)
				{
					(column<I>::construct_at(columns[I], index, static_cast<Args&&>(args)), ...);
				}
				else
				{
					// machinery to provide strong-exception guarantee

					size_t constructed_columns = {};

					const auto constructor =
						[&](auto ic, auto&& arg) noexcept(
							column_from_ic<decltype(ic)>::template is_nothrow_constructible<decltype(arg)>)
					{
						static_assert(std::is_reference_v<decltype(arg)>);

						column_from_ic<decltype(ic)>::construct_at(columns[decltype(ic)::value],
																   index,
																   static_cast<decltype(arg)>(arg));

						constructed_columns++;
					};

					try
					{
						(constructor(index_constant<I>{}, static_cast<Args&&>(args)), ...);
					}
					catch (...)
					{
						if (constructed_columns)
							destruct_row(columns, index, 0u, constructed_columns - 1u);
						throw;
					}
				}
			}
		}

		//--- move-construction ----------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_row(column_pointers& dest,
									   size_t dest_index,
									   column_pointers& source,
									   size_t source_index) //
			noexcept(all_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			if constexpr (all_nothrow_move_constructible || all_trivially_destructible)
			{
				(column<I>::move_construct(dest[I], dest_index, source[I], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(column_from_ic<decltype(ic)>::is_nothrow_move_constructible)
				{
					column_from_ic<decltype(ic)>::move_construct(dest[decltype(ic)::value],
																 dest_index,
																 source[decltype(ic)::value],
																 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest, dest_index, 0u, constructed_columns - 1u);
					throw;
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_rows(column_pointers& dest,
										size_t dest_start,
										column_pointers& source,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				[[maybe_unused]] size_t i = 0;

				const auto constructor = [&]() noexcept(all_nothrow_move_constructible)
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i-- > 0u;)
							move_construct_row(dest, dest_start + i, source, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							move_construct_row(dest, dest_start + i, source, source_start + i);
					}
				};

				if constexpr (all_nothrow_move_constructible)
				{
					constructor();
				}
				else
				{
					// machinery to provide strong-exception guarantee

					try
					{
						constructor();
					}
					catch (...)
					{
						destruct_rows(dest, dest_start, i - dest_start);
						throw;
					}
				}
			}
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_row(column_pointers& dest,
									   size_t dest_index,
									   const const_column_pointers& source,
									   size_t source_index) //
			noexcept(all_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			if constexpr (all_nothrow_copy_constructible || all_trivially_destructible)
			{
				(column<I>::copy_construct(dest[I], dest_index, source[I], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(column_from_ic<decltype(ic)>::is_nothrow_copy_constructible)
				{
					column_from_ic<decltype(ic)>::copy_construct(dest[decltype(ic)::value],
																 dest_index,
																 source[decltype(ic)::value],
																 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_rows(column_pointers& dest,
										size_t dest_start,
										const const_column_pointers& source,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				[[maybe_unused]] size_t i = 0;

				const auto constructor = [&]() noexcept(all_nothrow_copy_constructible)
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i-- > 0u;)
							copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
				};

				if constexpr (all_nothrow_copy_constructible)
				{
					constructor();
				}
				else
				{
					// machinery to provide strong-exception guarantee

					try
					{
						constructor();
					}
					catch (...)
					{
						destruct_rows(dest, dest_start, i - dest_start);
						throw;
					}
				}
			}
		}

		//--- move-or-copy-construction (whichever is possible per-column) ---------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_construct_row(column_pointers& dest,
											   size_t dest_index,
											   column_pointers& source,
											   size_t source_index) //
			noexcept(all_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			if constexpr (all_nothrow_move_or_copy_constructible || all_trivially_destructible)
			{
				(column<I>::move_or_copy_construct(dest[I], dest_index, source[I], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(column_from_ic<decltype(ic)>::is_nothrow_move_or_copy_constructible)
				{
					column_from_ic<decltype(ic)>::move_or_copy_construct(dest[decltype(ic)::value],
																		 dest_index,
																		 source[decltype(ic)::value],
																		 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_construct_rows(column_pointers& dest,
												size_t dest_start,
												column_pointers& source,
												size_t source_start,
												size_t count) //
			noexcept(all_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				[[maybe_unused]] size_t i = 0;

				const auto constructor = [&]() noexcept(all_nothrow_move_or_copy_constructible)
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i-- > 0u;)
							move_or_copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							move_or_copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
				};

				if constexpr (all_nothrow_move_or_copy_constructible)
				{
					constructor();
				}
				else
				{
					// machinery to provide strong-exception guarantee

					try
					{
						constructor();
					}
					catch (...)
					{
						destruct_rows(dest, dest_start, i - dest_start);
						throw;
					}
				}
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_row(column_pointers& dest,
									size_t dest_index,
									column_pointers& source,
									size_t source_index) //
			noexcept(all_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<I>::move_assign(dest[I], dest_index, source[I], source_index), ...);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_rows(column_pointers& dest,
									 size_t dest_start,
									 column_pointers& source,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_assign_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_assign_row(dest, dest_start + i, source, source_start + i);
				}
			}
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest,
									size_t dest_index,
									const const_column_pointers& source,
									size_t source_index) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<I>::copy_assign(dest[I], dest_index, source[I], source_index), ...);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_rows(column_pointers& dest,
									 size_t dest_start,
									 const const_column_pointers& source,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
			}
		}

		//--- move-or-copy-assignment (whichever is possible per-column) -----------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_assign_row(column_pointers& dest,
											size_t dest_index,
											column_pointers& source,
											size_t source_index) //
			noexcept(all_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<I>::move_or_copy_assign(dest[I], dest_index, source[I], source_index), ...);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_assign_rows(column_pointers& dest,
											 size_t dest_start,
											 column_pointers& source,
											 size_t source_start,
											 size_t count) //
			noexcept(all_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_or_copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_or_copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
			}
		}

		//--- swap rows ------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_swappable)
		SOAGEN_CPP20_CONSTEXPR
		static void swap_rows(column_pointers& lhs,
							  size_t lhs_index,
							  column_pointers& rhs,
							  size_t rhs_index) //
			noexcept(all_nothrow_swappable)
		{
			(column<I>::swap(lhs[I], lhs_index, rhs[I], rhs_index), ...);
		}

		//--- swap columns ---------------------------------------------------------------------------------------------

		template <auto A, auto B>
		static constexpr bool can_swap_columns =
			static_cast<size_t>(A) == static_cast<size_t>(B)
			|| (std::is_same_v<storage_type<A>, storage_type<B>> && column<A>::is_swappable);

		template <auto A, auto B>
		static constexpr bool can_nothrow_swap_columns =
			static_cast<size_t>(A) == static_cast<size_t>(B)
			|| (std::is_same_v<storage_type<A>, storage_type<B>> && column<A>::is_nothrow_swappable);

		SOAGEN_HIDDEN_CONSTRAINT((can_swap_columns<A, B>), auto A, auto B)
		SOAGEN_CPP20_CONSTEXPR
		static void swap_columns([[maybe_unused]] column_pointers& columns,
								 [[maybe_unused]] size_t start,
								 [[maybe_unused]] size_t count) //
			noexcept(can_nothrow_swap_columns<A, B>)
		{
			if constexpr (static_cast<size_t>(A) != static_cast<size_t>(B))
			{
				static_assert(std::is_same_v<storage_type<A>, storage_type<B>>);
				static_assert(column<A>::is_swappable);
				static_assert(column<B>::is_swappable);

				column<A>::swap(columns[A], start, columns[B], start, count);
			}
		}

		//--- equality -------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_equality_comparable)
		SOAGEN_NODISCARD
		static constexpr bool equal(const const_column_pointers& lhs,
									size_t lhs_start,
									const const_column_pointers& rhs,
									size_t rhs_start,
									size_t count) //
			noexcept(all_nothrow_equality_comparable)
		{
			// note that the equality-comparison is done column-major for speed (cache-locality) because
			// equality-comparison does not need to know anything about the order of the columns,
			// just that they are all equal (there is no lexicographic constraint)

			return (column<I>::equal(lhs[I], lhs_start, rhs[I], rhs_start, count) && ...);
		}

		//--- compare --------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_less_than_comparable)
		SOAGEN_NODISCARD
		static constexpr int compare(const const_column_pointers& lhs,
									 size_t lhs_start,
									 const const_column_pointers& rhs,
									 size_t rhs_start,
									 size_t count) //
			noexcept(all_nothrow_less_than_comparable)
		{
			for (size_t i = 0; i < count; i++)
			{
				if ((false || ... || column<I>::less_than(lhs[I], lhs_start + i, rhs[I], rhs_start + i)))
					return -1;

				if ((false || ... || column<I>::less_than(rhs[I], rhs_start + i, lhs[I], lhs_start + i)))
					return 1;
			}

			return 0;
		}
	};

	// a more specialzed base that sees the full column traits, not just the base version.
	// (this is just for things that need the specialized information _and_ the column indices)
	template <typename...>
	struct table_traits_base_specialized;

	template <size_t... I, typename... Columns>
	struct table_traits_base_specialized<std::index_sequence<I...>, Columns...> //
		: public table_traits_base<std::index_sequence<I...>, to_base_traits<Columns>...>
	{
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		//
	};
}

namespace soagen
{
	template <typename... Columns>
	struct SOAGEN_EMPTY_BASES table_traits //
		SOAGEN_HIDDEN_BASE(public detail::table_traits_base_specialized<std::make_index_sequence<sizeof...(Columns)>,
																		detail::as_column<Columns>...>)
	{
		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");

		static constexpr size_t aligned_stride = lcm(size_t{ 1 }, detail::as_column<Columns>::aligned_stride...);

		// columns
		// (note that these hide the base class typedefs - this is intentional)

		template <auto Index>
		using column = type_at_index<static_cast<size_t>(Index), detail::as_column<Columns>...>;

		template <typename IndexConstant>
		using column_from_ic =
			type_at_index<static_cast<size_t>(remove_cvref<IndexConstant>::value), detail::as_column<Columns>...>;

		static constexpr size_t column_alignments[column_count] = { detail::as_column<Columns>::alignment... };

		static constexpr size_t largest_alignment = max(size_t{ 1 }, detail::as_column<Columns>::alignment...);

		static constexpr bool rvalues_are_distinct =
			!(std::is_same_v<typename detail::as_column<Columns>::param_type,
														typename detail::as_column<Columns>::rvalue_type>
										 && ...);

		template <typename BackingTable, typename... Args>
		static constexpr bool emplace_back_is_nothrow = has_nothrow_emplace_back_member<BackingTable, Args...>;

		template <typename BackingTable>
		static constexpr bool push_back_is_nothrow =
			emplace_back_is_nothrow<BackingTable, typename detail::as_column<Columns>::param_forward_type...>;

		template <typename BackingTable>
		static constexpr bool rvalue_push_back_is_nothrow =
			emplace_back_is_nothrow<BackingTable, typename detail::as_column<Columns>::rvalue_forward_type...>;

		template <typename BackingTable, typename Row>
		static constexpr bool row_push_back_is_nothrow = emplace_back_is_nothrow<BackingTable, Row>;

		template <typename BackingTable, typename... Args>
		static constexpr bool emplace_is_nothrow = has_nothrow_emplace_member<BackingTable, std::size_t, Args...>;

		template <typename BackingTable>
		static constexpr bool insert_is_nothrow =
			emplace_is_nothrow<BackingTable, typename detail::as_column<Columns>::param_forward_type...>;

		template <typename BackingTable>
		static constexpr bool rvalue_insert_is_nothrow =
			emplace_is_nothrow<BackingTable, typename detail::as_column<Columns>::rvalue_forward_type...>;

		template <typename BackingTable, typename Row>
		static constexpr bool row_insert_is_nothrow = emplace_is_nothrow<BackingTable, Row>;
	};

	namespace detail
	{
		template <typename>
		struct is_table_traits_ : std::false_type
		{};
		template <typename... Columns>
		struct is_table_traits_<table_traits<Columns...>> : std::true_type
		{};
		template <typename... Columns>
		struct is_table_traits_<detail::table_traits_base<Columns...>> : std::true_type
		{};
	}

	template <typename T>
	inline constexpr bool is_table_traits =
		detail::is_table_traits_<std::remove_cv_t<T>>::value;
}

namespace soagen::detail
{
	template <typename... Columns>
	struct to_base_traits_<table_traits<Columns...>>
	{
		using type = table_traits_base<to_base_traits<as_column<Columns>>...>;

		static_assert(std::is_base_of_v<type, table_traits<Columns...>>);
	};
}

//********  iterator.hpp  **********************************************************************************************

namespace soagen
{
	namespace detail
	{
		template <typename T>
		struct arrow_proxy
		{
			mutable T value;

			SOAGEN_PURE_INLINE_GETTER
			constexpr T* operator->() const noexcept
			{
				return &value;
			}
		};

		template <typename Soa>
		struct iterator_storage
		{
			static_assert(!is_cvref<Soa>);

			Soa* soa;
			ptrdiff_t offset;
		};
	}

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES iterator_base
	{};

	template <typename Soa, size_t... Columns>
	class SOAGEN_EMPTY_BASES iterator
		SOAGEN_HIDDEN_BASE(protected detail::iterator_storage<remove_cvref<Soa>>,
						   public iterator_base<iterator<Soa, Columns...>>)
	{
		static_assert(is_soa<remove_cvref<Soa>>, "Soa must be a table or soagen-generated SoA type.");
		static_assert(!std::is_lvalue_reference_v<Soa>, "Soa may not be an lvalue reference.");
		static_assert(sizeof...(Columns), "Column index list may not be empty.");
		static_assert(std::is_empty_v<iterator_base<iterator<Soa, Columns...>>>,
					  "iterator_base specializations may not have data members");
		static_assert(std::is_trivial_v<iterator_base<iterator<Soa, Columns...>>>,
					  "iterator_base specializations must be trivial");

	  public:
		using soa_type = remove_cvref<Soa>;

		using soa_ref = coerce_ref<Soa>;
		static_assert(std::is_reference_v<soa_ref>);

		using size_type = std::size_t;

		using difference_type = std::ptrdiff_t;

		using row_type = soagen::row_type<Soa, Columns...>;

		using value_type = row_type;

		using reference = row_type;

		using iterator_category = std::random_access_iterator_tag;

#if SOAGEN_CPP <= 17
		using pointer = void;
#endif

	  private:
		template <typename, size_t...>
		friend class iterator;

		using base = detail::iterator_storage<remove_cvref<Soa>>;

		SOAGEN_NODISCARD_CTOR
		constexpr iterator(base b) noexcept //
			: base{ b }
		{}

	  public:
		SOAGEN_NODISCARD_CTOR
		constexpr iterator() noexcept //
			: base{}
		{}

		SOAGEN_NODISCARD_CTOR
		iterator(const iterator&) = default;

		iterator& operator=(const iterator&) = default;

		SOAGEN_NODISCARD_CTOR
		constexpr iterator(soa_ref src, difference_type pos) noexcept //
			: base{ const_cast<soa_type*>(&src), pos }
		{}

		friend constexpr iterator& operator++(iterator& it) noexcept // pre
		{
			++it.offset;
			return it;
		}

		friend constexpr iterator operator++(iterator& it, int) noexcept // post
		{
			iterator pre = it;
			++it.offset;
			return pre;
		}

		friend constexpr iterator& operator+=(iterator& it, difference_type n) noexcept
		{
			it.offset += n;
			return it;
		}

		SOAGEN_PURE_GETTER
		friend constexpr iterator operator+(const iterator& it, difference_type n) noexcept
		{
			auto it2 = it;
			it2 += n;
			return it2;
		}

		friend constexpr iterator& operator--(iterator& it) noexcept // pre
		{
			--it.offset;
			return it;
		}

		friend constexpr iterator operator--(iterator& it, int) noexcept // post
		{
			iterator pre = it;
			--it.offset;
			return pre;
		}

		friend constexpr iterator& operator-=(iterator& it, difference_type n) noexcept
		{
			return it += (-n);
		}

		SOAGEN_PURE_INLINE_GETTER
		friend constexpr iterator operator-(const iterator& it, difference_type n) noexcept
		{
			return it + (-n);
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_GETTER
		constexpr difference_type operator-(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset - rhs.offset;
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr reference operator*() const noexcept
		{
			SOAGEN_ASSUME(!!base::soa);
			SOAGEN_ASSUME(base::offset >= 0);

			return static_cast<soa_ref>(*base::soa).template row<Columns...>(static_cast<size_type>(base::offset));
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr detail::arrow_proxy<row_type> operator->() const noexcept
		{
			return { *(*this) };
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr reference operator[](difference_type offset) const noexcept
		{
			SOAGEN_ASSUME(!!base::soa);
			SOAGEN_ASSUME(base::offset + offset >= 0);

			return static_cast<soa_ref>(*base::soa)
				.template row<Columns...>(static_cast<size_type>(base::offset + offset));
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_GETTER
		constexpr bool operator==(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::soa == rhs.soa && base::offset == rhs.offset;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const iterator& lhs, const iterator<T, Cols...>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset < rhs.offset;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<=(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset <= rhs.offset;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator>(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset > rhs.offset;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator>=(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset >= rhs.offset;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && !detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator iterator<T, Cols...>() const noexcept
		{
			return iterator<T, Cols...>{ static_cast<const base&>(*this) };
		}

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator iterator<T, Cols...>() const noexcept
		{
			return iterator<T, Cols...>{ static_cast<const base&>(*this) };
		}

		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator size_type() const noexcept
		{
			SOAGEN_ASSUME(base::offset >= 0);

			return static_cast<size_type>(base::offset);
		}

		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator difference_type() const noexcept
		{
			return base::offset;
		}
	};

	template <typename Soa, size_t... Columns>
	SOAGEN_PURE_INLINE_GETTER
	constexpr iterator<Soa, Columns...> operator+(typename iterator<Soa, Columns...>::difference_type n,
												  const iterator<Soa, Columns...>& it) noexcept
	{
		return it + n;
	}
}

//********  mixins/columns.hpp  ****************************************************************************************

namespace soagen::mixins
{
	template <typename Derived, typename...>
	struct SOAGEN_EMPTY_BASES columns;

	template <typename Derived, size_t... Columns>
	struct SOAGEN_EMPTY_BASES columns<Derived, std::index_sequence<Columns...>>
		: detail::column_func<Derived, Columns>...
	{
		static_assert(!is_cvref<Derived>);
		static_assert(sizeof...(Columns));
		static_assert(std::is_same_v<column_indices<Derived>, std::index_sequence<Columns...>>);
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES columns<Derived> //
		: columns<Derived, column_indices<Derived>>
	{
		template <typename Func>
		SOAGEN_ALWAYS_INLINE
		constexpr void for_each_column(Func&& func) //
			noexcept(noexcept(soagen::for_each_column(std::declval<Derived&>(), std::declval<Func&&>())))
		{
			soagen::for_each_column(static_cast<Derived&>(*this), static_cast<Func&&>(func));
		}

		template <typename Func>
		SOAGEN_ALWAYS_INLINE
		constexpr void for_each_column(Func&& func) const //
			noexcept(noexcept(soagen::for_each_column(std::declval<const Derived&>(), std::declval<Func&&>())))
		{
			soagen::for_each_column(static_cast<const Derived&>(*this), static_cast<Func&&>(func));
		}
	};
}

//********  span.hpp  **************************************************************************************************

SOAGEN_DISABLE_SHADOW_WARNINGS;

namespace soagen
{
	namespace detail
	{
		template <typename Soa>
		struct span_storage
		{
			static_assert(!is_cvref<Soa>);

			Soa* soa;
			size_t start;
			size_t count;
		};

		SOAGEN_CONST_GETTER
		constexpr size_t calc_span_count(size_t start, size_t src_count, size_t desired_count) noexcept
		{
			return desired_count == static_cast<size_t>(-1) ? src_count - min(start, src_count) : desired_count;
		}
	}

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES span_base
	{};

	template <typename Soa>
	class SOAGEN_EMPTY_BASES span //
		SOAGEN_HIDDEN_BASE(protected detail::span_storage<remove_cvref<Soa>>,
						   public span_base<span<Soa>>,
						   public mixins::columns<span<Soa>>)
	{
		static_assert(is_soa<remove_cvref<Soa>>, "Soa must be a table or soagen-generated SoA type.");
		static_assert(!std::is_lvalue_reference_v<Soa>, "Soa may not be an lvalue reference.");
		static_assert(std::is_empty_v<span_base<span<Soa>>>, "span_base specializations may not have data members");
		static_assert(std::is_trivial_v<span_base<span<Soa>>>, "span_base specializations must be trivial");

	  public:
		using soa_type = remove_cvref<Soa>;

		using soa_ref = coerce_ref<Soa>;
		static_assert(std::is_reference_v<soa_ref>);

		using size_type = std::size_t;

		using difference_type = std::ptrdiff_t;

		using row_type = soagen::row_type<Soa>;

		using iterator = soagen::iterator_type<Soa>;

		using const_iterator = soagen::const_iterator_type<Soa>;

		using span_type = span;

		using const_span_type = soagen::const_span_type<Soa>;

	  private:
		template <typename>
		friend class span;

		using base = detail::span_storage<remove_cvref<Soa>>;

		SOAGEN_NODISCARD_CTOR
		constexpr span(base b) noexcept //
			: base{ b }
		{}

		SOAGEN_NODISCARD_CTOR
		constexpr span(soa_ref soa, size_type start, size_type count, std::true_type) noexcept //
			: base{ const_cast<soa_type*>(&soa), start, count }
		{
			SOAGEN_CONSTEXPR_SAFE_ASSERT(start < soa.size());
			SOAGEN_CONSTEXPR_SAFE_ASSERT(start + count <= soa.size());
		}

	  public:
		SOAGEN_NODISCARD_CTOR
		constexpr span() noexcept //
			: base{}
		{}

		SOAGEN_NODISCARD_CTOR
		span(const span&) = default;

		span& operator=(const span&) = default;

		SOAGEN_NODISCARD_CTOR
		constexpr span(soa_ref soa, size_type start, size_type count = static_cast<size_type>(-1)) noexcept //
			: span{ static_cast<soa_ref>(soa),
					start,
					detail::calc_span_count(start, soa.size(), count),
					std::true_type{} }
		{}

		SOAGEN_NODISCARD_CTOR
		explicit constexpr span(soa_ref soa) noexcept //
			: span{ static_cast<soa_ref>(soa),		  //
					0u,
					soa.size(),
					std::true_type{} }
		{}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type size() const noexcept
		{
			return base::count;
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr bool empty() const noexcept
		{
			return !size();
		}

		template <auto Column>
		SOAGEN_COLUMN(span, Column)
		constexpr auto* column() const noexcept
		{
			return static_cast<soa_ref>(*base::soa).template column<Column>() + base::start;
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Soa, Cols...> row(size_type index) const noexcept
		{
			return static_cast<soa_ref>(*base::soa).template row<static_cast<size_type>(Cols)...>(base::start + index);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type operator[](size_type index) const noexcept
		{
			return row(index);
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Soa, Cols...> at(size_type index) const
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= size())
				throw std::out_of_range{ "bad element access" };
#endif
			return static_cast<soa_ref>(*base::soa).template at<static_cast<size_type>(Cols)...>(base::start + index);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Soa, Cols...> front() const noexcept
		{
			return row<static_cast<size_type>(Cols)...>(0u);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Soa, Cols...> back() const noexcept
		{
			return row<static_cast<size_type>(Cols)...>(size() - 1u);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::iterator_type<Soa, Cols...> begin() const noexcept
		{
			return { static_cast<soa_ref>(*base::soa), //
					 static_cast<difference_type>(base::start) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::iterator_type<Soa, Cols...> end() const noexcept
		{
			return { static_cast<soa_ref>(*base::soa), //
					 static_cast<difference_type>(base::start + base::count) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::const_iterator_type<Soa, Cols...> cbegin() const noexcept
		{
			return { static_cast<soa_ref>(*base::soa), //
					 static_cast<difference_type>(base::start) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::const_iterator_type<Soa, Cols...> cend() const noexcept
		{
			return { static_cast<soa_ref>(*base::soa), //
					 static_cast<difference_type>(base::start + base::count) };
		}

		SOAGEN_PURE_INLINE_GETTER
		span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) const noexcept
		{
			return { static_cast<soa_ref>(*base::soa),
					 base::start + start,
					 detail::calc_span_count(start, base::count, count),
					 std::true_type{} };
		}

		SOAGEN_PURE_INLINE_GETTER
		const_span_type const_subspan(size_type start, size_type count = static_cast<size_type>(-1)) const noexcept
		{
			return { static_cast<soa_ref>(*base::soa),
					 base::start + start,
					 detail::calc_span_count(start, base::count, count),
					 std::true_type{} };
		}

		SOAGEN_CONSTRAINED_TEMPLATE((detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && !detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
									typename T)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator span<T>() const noexcept
		{
			return span<T>{ static_cast<const base&>(*this) };
		}

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
									typename T)
		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator span<T>() const noexcept
		{
			return span<T>{ static_cast<const base&>(*this) };
		}
	};
}

//********  mixins/rows.hpp  *******************************************************************************************

namespace soagen::mixins
{
	template <typename Derived, typename...>
	struct SOAGEN_EMPTY_BASES rows;

	template <typename Derived, size_t... Columns>
	struct SOAGEN_EMPTY_BASES rows<Derived, std::index_sequence<Columns...>>
	{
		static_assert(!is_cvref<Derived>);
		static_assert(std::is_same_v<column_indices<Derived>, std::index_sequence<Columns...>>);

		using size_type		  = std::size_t;
		using row_type		  = soagen::row_type<Derived>;
		using rvalue_row_type = soagen::row_type<Derived&&>;
		using const_row_type  = soagen::const_row_type<Derived>;

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived, Cols...> row(size_type index) & noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { static_cast<value_ref<Derived, Cols>>(
					static_cast<Derived&>(*this).template column<static_cast<size_type>(Cols)>()[index])... };
			}
			else
			{
				return { static_cast<value_ref<Derived, Columns>>(
					static_cast<Derived&>(*this).template column<static_cast<size_type>(Columns)>()[index])... };
			}
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived&&, Cols...> row(size_type index) && noexcept
		{
#if SOAGEN_MSVC
			// https://developercommunity.visualstudio.com/t/C:-Corrupt-references-when-creating-a/10446877
			return static_cast<soagen::row_type<Derived&&, Cols...>>(
				static_cast<Derived&>(*this).template row<Cols...>(index));
#else

			if constexpr (sizeof...(Cols))
			{
				return { static_cast<value_ref<Derived&&, Cols>>(
					static_cast<Derived&&>(*this).template column<static_cast<size_type>(Cols)>()[index])... };
			}
			else
			{
				return { static_cast<value_ref<Derived&&, Columns>>(
					static_cast<Derived&&>(*this).template column<static_cast<size_type>(Columns)>()[index])... };
			}
#endif
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::const_row_type<Derived, Cols...> row(size_type index) const& noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { static_cast<value_ref<const Derived, Cols>>(
					static_cast<const Derived&>(*this).template column<static_cast<size_type>(Cols)>()[index])... };
			}
			else
			{
				return { static_cast<value_ref<const Derived, Columns>>(
					static_cast<const Derived&>(*this).template column<static_cast<size_type>(Columns)>()[index])... };
			}
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type operator[](size_type index) & noexcept
		{
			return row(index);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		rvalue_row_type operator[](size_type index) && noexcept
		{
			return static_cast<rows&&>(*this).row(index);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		const_row_type operator[](size_type index) const& noexcept
		{
			return row(index);
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived, Cols...> at(size_type index) &
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= static_cast<const Derived&>(*this).size())
				throw std::out_of_range{ "bad element access" };
#endif
			return row<static_cast<size_type>(Cols)...>(index);
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived&&, Cols...> at(size_type index) &&
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= static_cast<const Derived&>(*this).size())
				throw std::out_of_range{ "bad element access" };
#endif
			return static_cast<rows&&>(*this).template row<static_cast<size_type>(Cols)...>(index);
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::const_row_type<Derived, Cols...> at(size_type index) const&
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= static_cast<const Derived&>(*this).size())
				throw std::out_of_range{ "bad element access" };
#endif
			return row<static_cast<size_type>(Cols)...>(index);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived, Cols...> front() & noexcept
		{
			return row<static_cast<size_type>(Cols)...>(0u);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived, Cols...> back() & noexcept
		{
			return row<static_cast<size_type>(Cols)...>(static_cast<const Derived&>(*this).size() - 1u);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived&&, Cols...> front() && noexcept
		{
			return static_cast<rows&&>(*this).template row<static_cast<size_type>(Cols)...>(0u);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived&&, Cols...> back() && noexcept
		{
			return static_cast<rows&&>(*this).template row<static_cast<size_type>(Cols)...>(
				static_cast<const Derived&>(*this).size() - 1u);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::const_row_type<Derived, Cols...> front() const& noexcept
		{
			return row<static_cast<size_type>(Cols)...>(0u);
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::const_row_type<Derived, Cols...> back() const& noexcept
		{
			return row<static_cast<size_type>(Cols)...>(static_cast<const Derived&>(*this).size() - 1u);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES rows<Derived> : rows<Derived, column_indices<Derived>>
	{};
}

//********  mixins/iterators.hpp  **************************************************************************************

namespace soagen::mixins
{
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES iterators
	{
		static_assert(!is_cvref<Derived>);

		using difference_type = std::ptrdiff_t;
		using iterator		  = soagen::iterator_type<Derived>;
		using rvalue_iterator = soagen::iterator_type<Derived&&>;
		using const_iterator  = soagen::const_iterator_type<Derived>;

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived, Cols...> begin() & noexcept
		{
			return { static_cast<Derived&>(*this), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived, Cols...> end() & noexcept
		{
			return { static_cast<Derived&>(*this),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived&&, Cols...> begin() && noexcept
		{
			return { static_cast<Derived&&>(*this), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived&&, Cols...> end() && noexcept
		{
			return { static_cast<Derived&&>(*this),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> begin() const& noexcept
		{
			return { static_cast<const Derived&>(*this), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> end() const& noexcept
		{
			return { static_cast<const Derived&>(*this),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> cbegin() const noexcept
		{
			return begin<Cols...>();
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> cend() const noexcept
		{
			return end<Cols...>();
		}
	};
}

//********  mixins/spans.hpp  ******************************************************************************************

namespace soagen::mixins
{
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES spans
	{
		static_assert(!is_cvref<Derived>);

		using size_type		   = std::size_t;
		using span_type		   = soagen::span_type<Derived>;
		using rvalue_span_type = soagen::span_type<Derived&&>;
		using const_span_type  = soagen::const_span_type<Derived>;

		SOAGEN_PURE_INLINE_GETTER
		span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) & noexcept
		{
			return { static_cast<Derived&>(*this), start, count };
		}

		SOAGEN_PURE_INLINE_GETTER
		rvalue_span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) && noexcept
		{
			return { static_cast<Derived&&>(*this), start, count };
		}

		SOAGEN_PURE_INLINE_GETTER
		const_span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) const& noexcept
		{
			return { static_cast<const Derived&>(*this), start, count };
		}

		SOAGEN_PURE_INLINE_GETTER
		const_span_type const_subspan(size_type start, size_type count = static_cast<size_type>(-1)) const noexcept
		{
			return subspan(start, count);
		}
	};
}

//********  table.hpp  *************************************************************************************************

namespace soagen::detail
{
	SOAGEN_CONSTRAINED_TEMPLATE(is_unsigned<T>, typename T)
	SOAGEN_NODISCARD
	constexpr bool add_without_overflowing(T lhs, T rhs, T& result) noexcept
	{
		if (lhs > static_cast<T>(-1) - rhs)
			return false;

		result = lhs + rhs;
		return true;
	};

	//------------------------------------------------------------------------------------------------------------------
	// generic allocation classes for tracking the column pointers and the actual size in bytes
	//------------------------------------------------------------------------------------------------------------------

	struct table_allocation_base
	{
		std::byte* ptr;
		size_t size;

		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return !!ptr;
		}
	};

	template <size_t ColumnCount>
	struct table_allocation : table_allocation_base
	{
		std::byte* columns[ColumnCount];
	};

	//------------------------------------------------------------------------------------------------------------------
	// base class for handling allocation and deallocation
	//------------------------------------------------------------------------------------------------------------------

	template <typename Allocator>
	class SOAGEN_EMPTY_BASES table_allocate //
	{
		static_assert(!is_cvref<Allocator>, "allocators may not be cvref-qualified");

	  public:
		using allocator_type = Allocator;

	  protected:
		static constexpr bool allocate_is_nothrow =
			noexcept(allocator_traits<Allocator>::allocate(std::declval<Allocator&>(), size_t{}, std::align_val_t{}));

		// guard against allocators with incorrect pointer typedefs where possible
		using allocator_pointer_type = std::remove_reference_t<
			decltype(allocator_traits<Allocator>::allocate(std::declval<Allocator&>(), size_t{}, std::align_val_t{}))>;
		static_assert(std::is_pointer_v<allocator_pointer_type>);

		SOAGEN_NODISCARD
		SOAGEN_NEVER_INLINE
		constexpr table_allocation_base allocate(allocator_type& alloc,
												 size_t size,
												 size_t alignment) noexcept(allocate_is_nothrow)
		{
			SOAGEN_ASSUME(size);
			SOAGEN_ASSUME((static_cast<size_t>(alignment) & (static_cast<size_t>(alignment) - 1u)) == 0u);

			const auto ptr = soagen::assume_aligned<allocator_traits<Allocator>::min_alignment>(
				allocator_traits<Allocator>::allocate(
					alloc,
					size,
					std::align_val_t{ max(alignment, allocator_traits<Allocator>::min_alignment) }));

			SOAGEN_ASSUME(ptr != nullptr);

			if constexpr (std::is_same_v<allocator_pointer_type, std::byte*>)
				return { ptr, size };
			else
				return { reinterpret_cast<std::byte*>(ptr), size };
		}

		SOAGEN_NEVER_INLINE
		constexpr void deallocate(allocator_type& alloc, const table_allocation_base& al) noexcept
		{
			if constexpr (std::is_same_v<allocator_pointer_type, std::byte*>)
				allocator_traits<Allocator>::deallocate(alloc, al.ptr, al.size);
			else
				allocator_traits<Allocator>::deallocate(alloc,
														reinterpret_cast<allocator_pointer_type>(al.ptr),
														al.size);
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// base class for handling general boilerplate
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_allocate
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE SOAGEN_BASE_NAME<Allocator>

	template <size_t ColumnCount, typename Allocator>
	class SOAGEN_EMPTY_BASES table_storage //
		: public SOAGEN_BASE_TYPE
	{
		static_assert(ColumnCount, "tables must have at least one column");

	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;
		using allocator_type  = Allocator;

	  protected:
		using allocation = table_allocation<ColumnCount>;

		allocation alloc_ = {};
		size_t count_	  = {};
		compressed_pair<size_t, Allocator> capacity_;

	  public:
		SOAGEN_NODISCARD_CTOR
		explicit constexpr table_storage(const Allocator& alloc) noexcept //
			: capacity_{ size_t{}, alloc }
		{
			static_assert(std::is_nothrow_copy_constructible_v<Allocator>,
						  "allocators must be nothrow copy-constructible");
		}

		SOAGEN_NODISCARD_CTOR
		explicit constexpr table_storage(Allocator&& alloc) noexcept //
			: capacity_{ size_t{}, static_cast<Allocator&&>(alloc) }
		{
			static_assert(std::is_nothrow_move_constructible_v<Allocator>,
						  "allocators must be nothrow move-constructible");
		}

		SOAGEN_NODISCARD_CTOR
		constexpr table_storage(table_storage&& other) noexcept //
			: alloc_{ std::exchange(other.alloc_, allocation{}) },
			  count_{ std::exchange(other.count_, size_t{}) },
			  capacity_{ std::exchange(other.capacity_.first(), size_t{}), static_cast<Allocator&&>(other.allocator()) }
		{
			static_assert(std::is_nothrow_move_constructible_v<Allocator>,
						  "allocators must be nothrow move-constructible");
		}

		// conditionally-implemented in specialized child classes:
		table_storage()								   = delete;
		table_storage& operator=(table_storage&&)	   = delete;
		table_storage(const table_storage&)			   = delete;
		table_storage& operator=(const table_storage&) = delete;

		~table_storage() noexcept
		{
			static_assert(std::is_nothrow_destructible_v<Allocator>, "allocators must be nothrow destructible");

			// element destructors are run in a more specialized child class

			if (alloc_)
				base::deallocate(allocator(), alloc_);
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

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t allocation_size() const noexcept
		{
			return alloc_.size;
		}

		SOAGEN_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		Allocator get_allocator() const noexcept
		{
			return capacity_.second();
		}

	  protected:
		SOAGEN_PURE_INLINE_GETTER
		constexpr Allocator& allocator() noexcept
		{
			return capacity_.second();
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr const Allocator& allocator() const noexcept
		{
			return capacity_.second();
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructibility
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_storage
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE SOAGEN_BASE_NAME<ColumnCount, Allocator>

	template <size_t ColumnCount,
			  typename Allocator,
			  bool = std::is_default_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_default_construct //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FOUR(table_default_construct);

		SOAGEN_NODISCARD_CTOR
		constexpr table_default_construct() noexcept //
			: SOAGEN_BASE_TYPE{ Allocator{} }
		{
			static_assert(std::is_nothrow_default_constructible_v<Allocator>,
						  "allocators must be nothrow default-constructible, or not default-constructible at all");
		}
	};

	template <size_t ColumnCount, typename Allocator>
	class SOAGEN_EMPTY_BASES table_default_construct<ColumnCount, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (un-typed base parts only)
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_default_construct

	template <size_t ColumnCount,
			  typename Allocator,
			  bool = !allocator_traits<Allocator>::propagate_on_container_move_assignment::value
				  || std::is_move_assignable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_move_assignable_base //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  protected:
		using allocation = table_allocation<ColumnCount>;

		constexpr bool move_from_by_taking_ownership(table_move_assignable_base&& rhs) noexcept
		{
			SOAGEN_ASSUME(&rhs != this);

			const auto take_ownership = [&]() noexcept
			{
				if (base::alloc_)
					base::deallocate(base::allocator(), base::alloc_);

				base::alloc_			= std::exchange(rhs.alloc_, allocation{});
				base::count_			= std::exchange(rhs.count_, size_t{});
				base::capacity_.first() = std::exchange(rhs.capacity_.first(), size_t{});

				if constexpr (allocator_traits<Allocator>::propagate_on_container_move_assignment::value)
				{
					static_assert(std::is_nothrow_move_assignable_v<Allocator>,
								  "move-propagated allocators must be nothrow move-assignable");

					base::allocator() = static_cast<Allocator&&>(rhs.allocator());
				}
			};

			if constexpr (allocator_traits<Allocator>::container_move_assign_always_takes_ownership)
			{
				take_ownership();
				return true;
			}
			else
			{
				if (allocator_traits<Allocator>::equal(base::allocator(), rhs.allocator()))
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

		template <typename T>
		constexpr bool move_from_by_taking_ownership(T&& rhs, std::true_type) noexcept
		{
			return move_from_by_taking_ownership(static_cast<table_move_assignable_base&&>(rhs));
		}

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

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

	template <size_t ColumnCount, typename Allocator>
	class SOAGEN_EMPTY_BASES table_move_assignable_base<ColumnCount, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assignable_base);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: swap
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_move_assignable_base

	template <size_t ColumnCount,
			  typename Allocator,
			  bool = !allocator_traits<Allocator>::propagate_on_container_swap::value || std::is_swappable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_swap //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);

		constexpr void swap(table_swap& other) noexcept
		{
			if SOAGEN_UNLIKELY(&other == this)
				return;

			if constexpr (!allocator_traits<Allocator>::propagate_on_container_swap::value
						  && !allocator_traits<Allocator>::is_always_equal::value)
			{
				SOAGEN_ASSERT(base::allocator() == other.allocator()
							  && "allocators must compare equal in a non-propagating swap");
			}

			using std::swap;
			swap(base::alloc_, other.alloc_);
			swap(base::count_, other.count_);
			swap(base::capacity_.first(), other.capacity_.first());

			if constexpr (allocator_traits<Allocator>::propagate_on_container_swap::value)
			{
				static_assert(std::is_nothrow_swappable_v<Allocator>,
							  "swap-propagated allocators must be nothrow-swappable");

				swap(base::allocator(), other.allocator());
			}
		}
	};

	template <size_t ColumnCount, typename Allocator>
	class SOAGEN_EMPTY_BASES table_swap<ColumnCount, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: typed base
	//
	// note: specializations from here down take the full Traits, not just the ColumnCount
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_swap
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE SOAGEN_BASE_NAME<Traits::column_count, Allocator>

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_typed_base //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using table_type  = table<Traits, Allocator>;
		using allocation  = table_allocation<Traits::column_count>;
		using column_ends = size_t[Traits::column_count];
		using base		  = SOAGEN_BASE_TYPE;

		static constexpr void calc_column_ends(column_ends& ends, size_t capacity) noexcept
		{
			// pad ends so the next column starts at the right alignment for the storage type
			size_t prev = {};
			for (size_t i = 0; i < Traits::column_count - 1u; i++)
			{
				const auto align = max(Traits::column_alignments[i + 1u], min_actual_column_alignment);

				ends[i] = prev + Traits::column_sizes[i] * capacity;
				ends[i] = (ends[i] + align - 1u) & ~(align - 1u);
				prev	= ends[i];
			}

			// last end doesn't need to be aligned (it's just the total buffer size)
			ends[Traits::column_count - 1u] = prev + Traits::column_sizes[Traits::column_count - 1u] * capacity;
		}

		SOAGEN_NODISCARD
		constexpr allocation allocate(const column_ends& ends) noexcept(base::allocate_is_nothrow)
		{
			SOAGEN_ASSUME(ends[Traits::column_count - 1u]);

			allocation alloc = {
				base::allocate(base::allocator(), ends[Traits::column_count - 1u], buffer_alignment<table_type>),
				{}
			};
			SOAGEN_ASSUME(alloc.ptr);
			SOAGEN_ASSUME(alloc.size == ends[Traits::column_count - 1u]);

			alloc.columns[0] = soagen::assume_aligned<buffer_alignment<table_type>>(alloc.ptr);
			for (size_t i = 1; i < Traits::column_count; i++)
				alloc.columns[i] = alloc.columns[0] + ends[i - 1u];

			return alloc;
		}

		SOAGEN_CPP20_CONSTEXPR
		SOAGEN_NEVER_INLINE
		void adjust_capacity(size_t new_capacity) noexcept(
			base::allocate_is_nothrow
			&& (Traits::all_nothrow_move_or_copy_constructible
				|| (Traits::all_nothrow_default_constructible && Traits::all_nothrow_move_or_copy_assignable)
				|| Traits::all_nothrow_copy_constructible
				|| (Traits::all_nothrow_default_constructible && Traits::all_nothrow_copy_assignable)))
		{
			SOAGEN_ASSUME(new_capacity);
			SOAGEN_ASSUME(new_capacity >= base::count_);

			if SOAGEN_UNLIKELY(new_capacity == base::capacity())
				return;

			// get new ends
			column_ends new_ends{};
			calc_column_ends(new_ends, new_capacity);

			// alloc + move
			auto new_alloc = allocate(new_ends);
			SOAGEN_ASSERT(new_alloc);
			if (base::count_)
			{
				SOAGEN_ASSERT(base::alloc_);

				// this looks a bit convoluted and is probably worth an explainer:
				// we really, really, _really_ don't want reserve() and shrink_to_fit() to throw if we can avoid it,
				// since it potentially leaves the container in a sliced state (i.e. some elements were moved already
				// when an exception is raised), so we're moving or copying the elements according to the 'most nothrow'
				// path that still fulfills the brief.

				if constexpr (Traits::all_nothrow_move_or_copy_constructible)
				{
					Traits::move_or_copy_construct_rows(new_alloc.columns, {}, base::alloc_.columns, {}, base::count_);
				}
				else if constexpr (Traits::all_nothrow_default_constructible
								   && Traits::all_nothrow_move_or_copy_assignable)
				{
					Traits::default_construct_rows(new_alloc.columns, {}, base::count_);
					Traits::move_or_copy_assign_rows(new_alloc.columns, {}, base::alloc_.columns, {}, base::count_);
				}
				else if constexpr (Traits::all_nothrow_copy_constructible)
				{
					Traits::copy_construct_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
				}
				else if constexpr (Traits::all_nothrow_default_constructible && Traits::all_nothrow_copy_assignable)
				{
					Traits::default_construct_rows(new_alloc, {}, base::count_);
					Traits::copy_assign_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
				}
				else
				{
					[[maybe_unused]] bool needs_destruct = false;
					try
					{
						if constexpr (Traits::all_move_or_copy_constructible)
						{
							Traits::move_or_copy_construct_rows(new_alloc,
																{},
																base::alloc_.columns,
																{},
																base::count_); // strong
						}
						else if constexpr (Traits::all_default_constructible && Traits::all_move_or_copy_assignable)
						{
							Traits::default_construct_rows(new_alloc, {}, base::count_); // strong
							needs_destruct = !Traits::all_trivially_destructible;
							Traits::move_or_copy_assign_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
						}
						else if constexpr (Traits::all_copy_constructible)
						{
							Traits::copy_construct_rows(new_alloc,
														{},
														base::alloc_.columns,
														{},
														base::count_); // strong
						}
						else if constexpr (Traits::all_default_constructible && Traits::all_copy_assignable)
						{
							Traits::default_construct_rows(new_alloc, {}, base::count_); // strong
							needs_destruct = !Traits::all_trivially_destructible;
							Traits::copy_assign_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
						}
					}
					catch (...)
					{
						if (needs_destruct)
							Traits::destruct_rows(new_alloc, {}, base::count_);
						base::deallocate(base::allocator(), new_alloc);
						throw;
					}
				}

				if constexpr (!Traits::all_trivially_destructible)
				{
					Traits::destruct_rows(base::alloc_.columns, {}, base::count_);
				}

				base::deallocate(base::allocator(), base::alloc_);
			}
			base::alloc_			= new_alloc;
			base::capacity_.first() = new_capacity;
		}

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_typed_base);

		~table_typed_base() noexcept
		{
			clear();
		}

		constexpr void clear() noexcept
		{
			if constexpr (!Traits::all_trivially_destructible)
			{
				static_assert(Traits::all_nothrow_destructible, "column storage_types must be nothrow-destructible");

				if (base::count_)
					Traits::destruct_rows(base::alloc_.columns, {}, base::count_);
			}
			base::count_ = {};
		}

		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		void reserve(size_t new_capacity) noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if SOAGEN_UNLIKELY(!new_capacity)
				return;

			if constexpr (Traits::aligned_stride > 1)
			{
				if (const size_t rem = new_capacity % Traits::aligned_stride; rem > 0u)
				{
					static_cast<void>(
						add_without_overflowing(new_capacity, Traits::aligned_stride - rem, new_capacity));
				}
			}
			if (new_capacity <= base::capacity())
				return;

			adjust_capacity(new_capacity);
		}

		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		void shrink_to_fit() noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if (!base::count_)
			{
				if (base::alloc_)
				{
					base::deallocate(base::allocator(), base::alloc_);
					base::alloc_			= {};
					base::capacity_.first() = {};
				}
				return;
			}

			auto new_capacity = base::count_;
			if constexpr (Traits::aligned_stride > 1)
			{
				if (const size_t rem = new_capacity % Traits::aligned_stride; rem > 0u)
				{
					static_cast<void>(
						add_without_overflowing(new_capacity, Traits::aligned_stride - rem, new_capacity));
				}
			}
			if (new_capacity >= base::capacity())
				return;

			adjust_capacity(new_capacity);
		}

		SOAGEN_CPP20_CONSTEXPR
		void pop_back(size_t num = 1) noexcept
		{
			static_assert(Traits::all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			num = min(base::count_, num);
			if constexpr (!Traits::all_trivially_destructible)
			{
				Traits::destruct_rows(base::alloc_.columns, base::count_ - num, num);
			}
			base::count_ -= num;
		}

	  public:
		static constexpr size_t max_capacity = []() -> size_t
		{
			if constexpr (Traits::column_count == 1)
			{
				return static_cast<size_t>(-1) / Traits::column_sizes[0];
			}
			else
			{
				// todo: i'm sure there's a smarter way to do this

				constexpr auto capacity_ok = [](size_t cap) noexcept
				{
					size_t buf_end = {};
					for (size_t i = 0u; i < Traits::column_count; i++)
					{
						if (i)
						{
							const auto align = max(Traits::column_alignments[i - 1u], min_actual_column_alignment);

							if (const size_t rem = buf_end % align; rem > 0u)
							{
								if (!add_without_overflowing(buf_end, align - rem, buf_end))
									return false;
							}
						}
						if (!add_without_overflowing(buf_end, Traits::column_sizes[i] * cap, buf_end))
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

	  private:
		SOAGEN_CPP20_CONSTEXPR
		SOAGEN_NEVER_INLINE
		void grow_if_necessary(size_t new_elements)
		{
			SOAGEN_ASSUME(new_elements);

			// determine the minimum required allocation size to store the new elements.
			// if this calculation would overflow or the result would exceed max_capacity then we've
			// hit system limits and have no choice but to raise std::bad_alloc.
			auto new_size = base::count_;
			if SOAGEN_UNLIKELY(!add_without_overflowing(new_size, new_elements, new_size) || new_size > max_capacity)
				throw std::bad_alloc{};

			// already enough capacity, no work to do.
			if SOAGEN_LIKELY(new_size <= base::capacity())
				return;

			// again we need to check system limits, this time when applying the growth factor.
			// if we overflow or would exceed the max capacity then clamp the result to max_capacity
			// (the user is right on the edge of hitting system limits and will hit them at the next growth)
			auto new_cap = new_size;
			if SOAGEN_UNLIKELY(!add_without_overflowing(new_cap, new_cap, new_cap) || new_cap > max_capacity)
				new_cap = max_capacity;

			reserve(new_cap);
		}

	  public:
		SOAGEN_CONSTRAINED_TEMPLATE(Traits::template row_constructible_from<Args&&...>, typename... Args)
		SOAGEN_CPP20_CONSTEXPR
		void emplace_back(Args&&... args) noexcept(					   //
			Traits::template row_nothrow_constructible_from<Args&&...> //
				&& noexcept(this->grow_if_necessary(size_t{})))
		{
			grow_if_necessary(1u);
			Traits::construct_row(base::alloc_.columns, base::count_, static_cast<Args&&>(args)...);
			base::count_++;
		}

		SOAGEN_CONSTRAINED_TEMPLATE((Traits::all_move_or_copy_constructible //
									 && Traits::all_move_or_copy_assignable //
									 && Traits::template row_constructible_from<Args&&...>),
									typename... Args)
		SOAGEN_CPP20_CONSTEXPR
		void emplace(size_t position, Args&&... args) noexcept(					  //
			Traits::all_nothrow_move_or_copy_constructible						  //
				&& Traits::all_nothrow_move_or_copy_assignable					  //
					&& Traits::template row_nothrow_constructible_from<Args&&...> //
						&& noexcept(this->grow_if_necessary(size_t{})))
		{
			SOAGEN_ASSUME(position <= base::count_);

			// delegating to emplace_back for the end position is ideal
			// because emplace_back never needs to move+destruct elements (modulo re-allocation)
			if (position == base::count_)
			{
				emplace_back(static_cast<Args&&>(args)...);
				return;
			}

			// move-construct the last element (since it is new)
			grow_if_necessary(1u);
			Traits::move_or_copy_construct_row(base::alloc_.columns,
											   base::count_,
											   base::alloc_.columns,
											   base::count_ - 1u);

			// move-assign the rest of the elements (they were already alive)
			if (position + 1u < base::count_)
			{
				Traits::move_or_copy_assign_rows(base::alloc_.columns,
												 position + 1u,
												 base::alloc_.columns,
												 position,
												 base::count_ - position - 1u);
			}

			// destruct the existing element if necessary
			if constexpr (!Traits::all_trivially_destructible)
			{
				Traits::destruct_row(base::alloc_.columns, position);
			}

			// insert the new element by direct construction
			Traits::construct_row(base::alloc_.columns, position, static_cast<Args&&>(args)...);

			base::count_++;
		}

	  private:
		template <size_t A, size_t B>
		static constexpr bool can_swap_columns =
			Traits::template can_swap_columns<A, B>
			|| (std::is_same_v<typename Traits::template storage_type<A>, typename Traits::template storage_type<B>>
				&& actual_alignment<table_type, A> == actual_alignment<table_type, B>);

		template <size_t A, size_t B>
		static constexpr bool can_nothrow_swap_columns =
			Traits::template can_nothrow_swap_columns<A, B>
			|| (std::is_same_v<typename Traits::template storage_type<A>, typename Traits::template storage_type<B>>
				&& actual_alignment<table_type, A> == actual_alignment<table_type, B>);

	  public:
		template <auto A, auto B>
		SOAGEN_CPP20_CONSTEXPR
		void swap_columns() //
			noexcept(can_nothrow_swap_columns<static_cast<size_t>(A), static_cast<size_t>(B)>)
		{
			static_assert(can_swap_columns<static_cast<size_t>(A), static_cast<size_t>(B)>);

			if constexpr (static_cast<size_t>(A) != static_cast<size_t>(B))
			{
				using storage_a = typename Traits::template storage_type<A>;
				using storage_b = typename Traits::template storage_type<B>;
				static_assert(std::is_same_v<storage_a, storage_b>);

				// if they have the same base alignment, we can just swap the two pointers
				// rather than having to do an element-wise swap
				if constexpr (actual_alignment<table_type, static_cast<size_t>(A)>
							  == actual_alignment<table_type, static_cast<size_t>(B)>)
				{
					const auto temp								 = base::alloc_.columns[static_cast<size_t>(A)];
					base::alloc_.columns[static_cast<size_t>(A)] = base::alloc_.columns[static_cast<size_t>(B)];
					base::alloc_.columns[static_cast<size_t>(B)] = temp;
				}
				else
				{
					Traits::template swap_columns<static_cast<size_t>(A), static_cast<size_t>(B)>(base::alloc_.columns,
																								  {},
																								  base::count_);
				}
			}
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructible column types
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_typed_base
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE SOAGEN_BASE_NAME<Traits, Allocator>

	template <typename Traits, //
			  typename Allocator,
			  bool = Traits::all_default_constructible>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);

		SOAGEN_CPP20_CONSTEXPR
		void resize(size_t num) noexcept(			  //
			Traits::all_nothrow_default_constructible //
				&& noexcept(this->pop_back(size_t{})) //
					&& noexcept(this->reserve(size_t{})))
		{
			if (base::size() > num)
			{
				base::pop_back(base::size() - num);
			}
			else if (base::size() < num)
			{
				base::reserve(num);
				for (size_t i = base::size(); i < num; i++)
				{
					Traits::default_construct_row(base::alloc_.columns, i);
					base::count_++;
				}
			}
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (typed)
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_default_constructible_columns

	template <typename Traits, //
			  typename Allocator,
			  bool UseBase = !std::is_move_assignable_v<SOAGEN_BASE_TYPE>
						  || allocator_traits<Allocator>::container_move_assign_always_takes_ownership,
			  bool Movable = (Traits::all_move_or_copy_assignable && Traits::all_move_or_copy_constructible)>
	class SOAGEN_EMPTY_BASES table_move_assign //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

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
			Traits::all_nothrow_move_or_copy_assignable					//
				&& Traits::all_nothrow_move_or_copy_constructible		//
					&& noexcept(base::clear())							//
						&& noexcept(base::pop_back())					//
							&& noexcept(base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			static_assert(!allocator_traits<Allocator>::propagate_on_container_move_assignment::value);
			static_assert(!allocator_traits<Allocator>::is_always_equal::value);

			if (base::move_from_by_taking_ownership(rhs, std::true_type{}))
				return *this;

			SOAGEN_ASSERT(!allocator_traits<Allocator>::equal(base::allocator(), rhs.allocator()));

			if (rhs.empty())
			{
				base::clear();
				return *this;
			}

			if (rhs.size() < base::size())
				base::pop_back(base::size() - rhs.size());

			base::reserve(rhs.size());

			const auto assigned_end = min(base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				Traits::move_or_copy_assign_row(base::alloc_.columns, i, rhs.alloc_.columns, i);

			const auto constructed_end = max(base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				Traits::move_or_copy_construct_row(base::alloc_.columns, i, rhs.alloc_.columns, i);
				base::count_++;
			}

			return *this;
		}
	};

	template <typename Traits, typename Allocator, bool Movable>
	class SOAGEN_EMPTY_BASES table_move_assign<Traits, Allocator, /*UseBase*/ true, Movable> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assign);
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_move_assign<Traits, Allocator, /*UseBase*/ false, /*Movable*/ false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

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

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_move_assign

	template <typename Traits, //
			  typename Allocator,
			  bool = Traits::all_copy_constructible&& std::is_copy_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_copy_construct //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		table_copy_construct()										 = default;
		table_copy_construct(table_copy_construct&&)				 = default;
		table_copy_construct& operator=(table_copy_construct&&)		 = default;
		table_copy_construct& operator=(const table_copy_construct&) = default;

		table_copy_construct(const table_copy_construct& other) noexcept(Traits::all_nothrow_copy_constructible //
																			 && noexcept(base::reserve(size_t{})))
			: base{ allocator_traits<Allocator>::select_on_container_copy_construction(base::allocator()) }
		{
			static_assert(std::is_nothrow_copy_constructible_v<Allocator>,
						  "allocators must be nothrow copy-constructible");

			base::reserve(other.size());
			Traits::copy_construct_rows(base::alloc_.columns, {}, other.alloc_.columns, {}, other.size());
			base::count_ = other.size();
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_copy_construct<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: copy-assign
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_copy_construct

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_copy_assignable		   //
						 && Traits::all_copy_constructible //
				  && (!allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
					  || std::is_copy_assignable_v<Allocator>)>
	class SOAGEN_EMPTY_BASES table_copy_assign //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		table_copy_assign()								  = default;
		table_copy_assign(table_copy_assign&&)			  = default;
		table_copy_assign& operator=(table_copy_assign&&) = default;
		table_copy_assign(const table_copy_assign&)		  = default;

		constexpr table_copy_assign& operator=(const table_copy_assign& rhs) noexcept( // wtb: noexcept(auto)
			Traits::all_nothrow_copy_assignable										   //
				&& Traits::all_nothrow_copy_constructible							   //
					&& noexcept(base::clear())										   //
						&& noexcept(base::pop_back())								   //
							&& noexcept(base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			if constexpr (allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
						  || !allocator_traits<Allocator>::is_always_equal::value)
			{
				if (!allocator_traits<Allocator>::equal(base::allocator(), rhs.allocator()))
				{
					base::clear();
					base::shink_to_fit();
					SOAGEN_ASSERT(base::empty());
					SOAGEN_ASSERT(!base::alloc_);
				}
			}

			if constexpr (allocator_traits<Allocator>::propagate_on_container_copy_assignment::value)
			{
				static_assert(std::is_nothrow_copy_assignable_v<Allocator>,
							  "copy-propagated allocators must be nothrow copy-assignable");

				base::allocator() = rhs.allocator();
			}

			if (rhs.empty())
			{
				base::clear();
				return *this;
			}

			if (rhs.size() < base::size())
				base::pop_back(base::size() - rhs.size());

			base::reserve(rhs.size());

			const auto assigned_end = min(base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				Traits::copy_assign_row(base::alloc_.columns, i, rhs.alloc_.columns, i);

			const auto constructed_end = max(base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				Traits::copy_construct_row(base::alloc_.columns, i, rhs.alloc_.columns, i);
				base::count_++;
			}

			return *this;
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_copy_assign<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_assign);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: row erasure
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_copy_assign

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_move_or_copy_assignable>
	class SOAGEN_EMPTY_BASES table_row_erasure //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure);

		void erase(size_t pos)									 //
			noexcept(Traits::all_nothrow_move_or_copy_assignable //
						 && noexcept(base::pop_back()))
		{
			SOAGEN_ASSUME(pos < base::count_);

			if (pos + 1u < base::count_)
				Traits::move_or_copy_assign_rows(base::alloc_.columns,
												 pos,
												 base::alloc_.columns,
												 pos + 1u,
												 base::count_ - pos - 1u);

			base::pop_back();
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_row_erasure<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: row erasure (unordered)
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_row_erasure

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_swappable>
	class SOAGEN_EMPTY_BASES table_row_erasure_unordered //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure_unordered);

		soagen::optional<size_t> unordered_erase(size_t pos) //
			noexcept(Traits::all_nothrow_swappable			 //
						 && noexcept(base::pop_back()))
		{
			SOAGEN_ASSUME(pos < base::count_);

			if (pos + 1u == base::count_)
			{
				base::pop_back();
				return {};
			}

			Traits::swap_rows(base::alloc_.columns, pos, base::alloc_.columns, base::count_ - 1u);
			base::pop_back();
			return soagen::optional<size_t>{ base::count_ };
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_row_erasure_unordered<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure_unordered);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: EqualityComparable
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_row_erasure_unordered

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_equality_comparable>
	class SOAGEN_EMPTY_BASES table_equality_compare //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_equality_compare);

		SOAGEN_NODISCARD
		friend constexpr bool operator==(const table_equality_compare& lhs,
										 const table_equality_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			if (&lhs == &rhs || (lhs.empty() && rhs.empty()))
				return true;

			if (lhs.size() != rhs.size())
				return false;

			return Traits::equal(lhs.alloc_.columns, {}, rhs.alloc_.columns, {}, lhs.size());
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const table_equality_compare& lhs,
										 const table_equality_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return !(lhs == rhs);
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_equality_compare<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_equality_compare);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: LessThanComparable
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_equality_compare

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_less_than_comparable>
	class SOAGEN_EMPTY_BASES table_less_than_compare //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_less_than_compare);

		SOAGEN_NODISCARD
		constexpr int compare(const table_less_than_compare& rhs) const //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			if (&rhs == this)
				return 0;
			if (base::empty())
				return rhs.empty() ? 0 : -1;
			if (rhs.empty())
				return 1;

			const auto comp = Traits::compare(base::alloc_.columns, //
											  {},
											  rhs.alloc_.columns,
											  {},
											  min(base::size(), rhs.size()));

			return comp != 0 ? comp : (base::size() == rhs.size() ? 0 : (base::size() < rhs.size() ? -1 : 1));
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<(const table_less_than_compare& lhs,
										const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) < 0;
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<=(const table_less_than_compare& lhs,
										 const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) <= 0;
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>(const table_less_than_compare& lhs,
										const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) > 0;
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>=(const table_less_than_compare& lhs,
										 const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) >= 0;
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_less_than_compare<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_less_than_compare);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: data()
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_less_than_compare

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_trivially_copyable>
	class SOAGEN_EMPTY_BASES table_data_ptr //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_data_ptr);

		using table_type	 = table<Traits, Allocator>;
		using table_traits	 = Traits;
		using allocator_type = Allocator;

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(assume_aligned(buffer_alignment<table_type>))
		constexpr std::byte* data() noexcept
		{
			return soagen::assume_aligned<buffer_alignment<table_type>>(base::alloc_.ptr);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(assume_aligned(buffer_alignment<table_type>))
		constexpr const std::byte* data() const noexcept
		{
			return soagen::assume_aligned<buffer_alignment<table_traits>>(base::alloc_.ptr);
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_data_ptr<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_data_ptr);
	};
}

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_data_ptr
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE detail::SOAGEN_BASE_NAME<Traits, Allocator>

namespace soagen
{
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES table_base
	{};

	template <typename Traits,
			  typename Allocator = soagen::allocator>
	class SOAGEN_EMPTY_BASES table //
		SOAGEN_HIDDEN_BASE(public SOAGEN_BASE_TYPE,
						   public table_base<table<Traits, Allocator>>,
						   public mixins::columns<table<Traits, Allocator>>,
						   public mixins::rows<table<Traits, Allocator>>,
						   public mixins::iterators<table<Traits, Allocator>>)
	{
		static_assert(std::is_empty_v<table_base<table>>, "table_base specializations may not have data members");
		static_assert(std::is_trivial_v<table_base<table>>, "table_base specializations must be trivial");

	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using size_type = std::size_t;

		using difference_type = std::ptrdiff_t;

		using allocator_type = Allocator;
		static_assert(!is_cvref<allocator_type>, "allocators may not be cvref-qualified");

		using table_traits = Traits;
		static_assert(is_table_traits<table_traits>, "Traits must be an instance of soagen::table_traits");
		static_assert(!is_cvref<table_traits>, "table traits may not be cvref-qualified");

		static constexpr size_type column_count = table_traits::column_count;

		template <auto Column>
		using column_traits =
			typename table_traits::template column<static_cast<size_type>(Column)>;

		template <auto Column>
		using column_type =
			typename column_traits<static_cast<size_type>(Column)>::value_type;

		static constexpr size_t aligned_stride = table_traits::aligned_stride;

		SOAGEN_NODISCARD_CTOR
		table() = default;

		SOAGEN_NODISCARD_CTOR
		table(table&&) = default;

		table& operator=(table&&) = default;

		SOAGEN_NODISCARD_CTOR
		table(const table&) = default;

		table& operator=(const table&) = default;

		~table() = default;

		template <auto Column>
		SOAGEN_COLUMN(table, Column)
		constexpr column_type<Column>* column() noexcept
		{
			static_assert(static_cast<size_type>(Column) < table_traits::column_count, "column index out of range");

			using column	   = column_traits<static_cast<size_type>(Column)>;
			using storage_type = typename column::storage_type;
			using value_type   = typename column::value_type;

			if constexpr (std::is_pointer_v<storage_type>)
			{
				static_assert(std::is_same_v<storage_type, void*>);

				return soagen::assume_aligned<actual_alignment<table, Column>>(SOAGEN_LAUNDER(
					reinterpret_cast<value_type*>(base::alloc_.columns[static_cast<size_type>(Column)])));
			}
			else
			{
				static_assert(std::is_same_v<storage_type, std::remove_cv_t<value_type>>);

				return soagen::assume_aligned<actual_alignment<table, Column>>(
					column::ptr(base::alloc_.columns[static_cast<size_type>(Column)]));
			}
		}

		template <auto Column>
		SOAGEN_COLUMN(table, Column)
		constexpr std::add_const_t<column_type<Column>>* column() const noexcept
		{
			return const_cast<table&>(*this).template column<static_cast<size_type>(Column)>();
		}

		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;
	};

	SOAGEN_CONSTRAINED_TEMPLATE((has_swap_member<table<Args...>>), typename... Args)
	SOAGEN_ALWAYS_INLINE
	constexpr void swap(table<Args...>& lhs, table<Args...>& rhs) //
		noexcept(soagen::has_nothrow_swap_member<table<Args...>>)
	{
		lhs.swap(rhs);
	}
}

#undef SOAGEN_BASE_NAME
#undef SOAGEN_BASE_TYPE

//********  mixins/data_ptr.hpp  ***************************************************************************************

namespace soagen::mixins
{
	template <typename Derived, bool = has_data_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES data_ptr
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(assume_aligned(buffer_alignment<table_type>))
		constexpr std::byte* data() //
			noexcept(has_nothrow_data_member<table_type>)
		{
			return soagen::assume_aligned<buffer_alignment<table_type>>(
				static_cast<table_type&>(static_cast<Derived&>(*this)).data());
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(assume_aligned(buffer_alignment<table_type>))
		constexpr const std::byte* data() const //
			noexcept(has_nothrow_data_member<const table_type>)
		{
			return soagen::assume_aligned<buffer_alignment<table_type>>(
				static_cast<const table_type&>(static_cast<const Derived&>(*this)).data());
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES data_ptr<Derived, false>
	{};
}

//********  mixins/equality_comparable.hpp  ****************************************************************************

namespace soagen::mixins
{
	template <typename Derived, bool = is_equality_comparable<const table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES equality_comparable
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator==(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_equality_comparable<const table_type>)
		{
			return static_cast<const table_type&>(lhs) == static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_equality_comparable<const table_type>)
		{
			return static_cast<const table_type&>(lhs) != static_cast<const table_type&>(rhs);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES equality_comparable<Derived, false>
	{};
}

//********  mixins/less_than_comparable.hpp  ***************************************************************************

namespace soagen::mixins
{
	template <typename Derived, bool = is_less_than_comparable<const table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES less_than_comparable
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<const table_type>)
		{
			return static_cast<const table_type&>(lhs) < static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<=(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<const table_type>)
		{
			return static_cast<const table_type&>(lhs) <= static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<const table_type>)
		{
			return static_cast<const table_type&>(lhs) > static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>=(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<const table_type>)
		{
			return static_cast<const table_type&>(lhs) >= static_cast<const table_type&>(rhs);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES less_than_comparable<Derived, false>
	{};
}

//********  mixins/resizable.hpp  **************************************************************************************

namespace soagen::mixins
{
	template <typename Derived, bool = has_resize_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES resizable
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;
		using size_type	 = std::size_t;

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& resize(size_type new_size) //
			noexcept(has_nothrow_resize_member<table_type>)
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).resize(new_size);
			return static_cast<Derived&>(*this);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES resizable<Derived, false>
	{};
}

//********  mixins/size_and_capacity.hpp  ******************************************************************************

namespace soagen::mixins
{
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES size_and_capacity
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;
		using size_type	 = std::size_t;

		SOAGEN_PURE_INLINE_GETTER
		constexpr bool empty() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).empty();
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type size() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).size();
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type max_size() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).max_size();
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type allocation_size() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).allocation_size();
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& reserve(size_type new_cap) //
			noexcept(noexcept(std::declval<table_type&>().reserve(size_type{})))
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).reserve(new_cap);
			return static_cast<Derived&>(*this);
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type capacity() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).capacity();
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& shrink_to_fit() //
			noexcept(noexcept(std::declval<table_type&>().shrink_to_fit()))
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).shrink_to_fit();
			return static_cast<Derived&>(*this);
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& pop_back(size_type num = 1) //
			noexcept(noexcept(std::declval<table_type&>().pop_back(size_type{})))
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).pop_back(num);
			return static_cast<Derived&>(*this);
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& clear() noexcept
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).clear();
			return static_cast<Derived&>(*this);
		}
	};
}

//********  mixins/swappable.hpp  **************************************************************************************

namespace soagen::mixins
{
	template <typename Derived, bool = has_swap_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES swappable
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		void swap(Derived& other) //
			noexcept(has_nothrow_swap_member<table_type>)
		{
			return static_cast<table_type&>(static_cast<Derived&>(*this)).swap(static_cast<table_type&>(other));
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES swappable<Derived, false>
	{};
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

//********  mixins/all.hpp  ********************************************************************************************

#endif // SOAGEN_HPP
