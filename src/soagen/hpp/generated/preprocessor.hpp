//#---------------------------------------------------------------------------------------------------------------------
//#     !!!!! THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY !!!!!
//#---------------------------------------------------------------------------------------------------------------------

#pragma once

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
/// \brief `1` when targeting any 64-bit architecture, otherwise `0`.

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

/// @cond
#ifndef SOAGEN_CONSTRAINED_COLUMN
	#define SOAGEN_CONSTRAINED_COLUMN(I, ...)                                                                          \
		SOAGEN_CONSTRAINED_TEMPLATE(sfinae_col_idx == (I) && (__VA_ARGS__), size_t sfinae_col_idx = (I))
#endif
/// @endcond
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
		#define SOAGEN_DISABLE_SPAM_WARNINGS                                                                           \
			__pragma(warning(disable : 4127))  /* conditional expr is constant */                                      \
			__pragma(warning(disable : 4324))  /* structure was padded due to alignment specifier */                   \
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

/// \brief Re-enables compiler warnings again after using #SOAGEN_DISABLE_WARNINGS.

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
