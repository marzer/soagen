//#---------------------------------------------------------------------------------------------------------------------
//#     !!!!! THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY !!!!!
//#---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "preprocessor.hpp"

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

#include "header_start.hpp"

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

#include "header_end.hpp"
