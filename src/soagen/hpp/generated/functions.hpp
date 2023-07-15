//#---------------------------------------------------------------------------------------------------------------------
//#     !!!!! THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY !!!!!
//#---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "../meta.hpp"
#if SOAGEN_CPP >= 20 && defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806
	#include <bit>
#endif
#include "../header_start.hpp"

namespace soagen
{
	/// \brief	Equivalent to C++20's std::is_constant_evaluated.
	///
	/// \remark Compilers typically implement std::is_constant_evaluated as an intrinsic which is
	/// 		 available regardless of the C++ mode. Using this function on these compilers allows
	/// 		 you to get the same behaviour even when you aren't targeting C++20.
	///
	/// \availability On older compilers lacking support for std::is_constant_evaluated this will always return `false`.
	/// 		   Check for support by examining build::supports_is_constant_evaluated.
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
		/// \brief	True if is_constant_evaluated() is properly supported on this compiler.
		inline constexpr bool supports_is_constant_evaluated = is_constant_evaluated();
	}

	/// \brief	Returns the minimum of two or more values.
	///
	/// \remark This is a variadic version of std::min.
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

	/// \brief	Returns the maximum of two or more values.
	///
	/// \remark This is a variadic version of std::max.
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

	/// \brief Returns the lowest common multiple of two or more integers.
	///
	/// \remark This is a variadic version of std::lcm.
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

	/// \brief	Checks if an integral value has only a single bit set.
	///
	/// \remark	This is equivalent to C++20's std::has_single_bit, with the addition of also being
	/// 		extended to work with enum types.
	///
	/// \tparam	T		An unsigned integer or enum type.
	/// \param 	val		The value to test.
	///
	/// \returns	True if the input value had only a single bit set (and thus was a power of two).
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

	/// \brief	Equivalent to C++20's std::bit_cast.
	///
	/// \remark Compilers implement this as an intrinsic which is typically
	/// 		 available regardless of the C++ mode. Using this function
	/// 		 on these compilers allows you to get the same behaviour
	/// 		 even when you aren't targeting C++20.
	///
	/// \availability On older compilers lacking support for std::bit_cast you won't be able to call this function
	/// 		   in constexpr contexts (since it falls back to a memcpy-based implementation).
	/// 		   Check for constexpr support by examining build::supports_constexpr_bit_cast.
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

	/// \brief	Equivalent to C++20's std::assume_aligned.
	///
	/// \remark Compilers typically implement std::assume_aligned as an intrinsic which is
	/// 		 available regardless of the C++ mode. Using this function on these compilers allows
	/// 		 you to get the same behaviour even when you aren't targeting C++20.
	///
	/// \see [P1007R1: std::assume_aligned](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1007r1.pdf)
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

#include "../header_end.hpp"
