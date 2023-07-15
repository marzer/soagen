//#---------------------------------------------------------------------------------------------------------------------
//#     !!!!! THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY !!!!!
//#---------------------------------------------------------------------------------------------------------------------

#pragma once

#include "preprocessor.hpp"
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
SOAGEN_ENABLE_WARNINGS;
#include "../header_start.hpp"

#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
	#define SOAGEN_LAUNDER(...) std::launder(__VA_ARGS__)
#elif SOAGEN_CLANG >= 8 || SOAGEN_GCC >= 7 || SOAGEN_ICC >= 1910 || SOAGEN_MSVC >= 1914                                \
	|| SOAGEN_HAS_BUILTIN(__builtin_launder)
	#define SOAGEN_LAUNDER(...) __builtin_launder(__VA_ARGS__)
#else
	#define SOAGEN_LAUNDER(...) __VA_ARGS__
#endif

//--- typedefs and type traits -----------------------------------------------------------------------------------------

/// @brief The root namespace for the library.
namespace soagen
{
	using std::size_t;
	using std::ptrdiff_t;
	using std::intptr_t;
	using std::uintptr_t;
	using std::nullptr_t;

#if SOAGEN_HAS_BUILTIN(__type_pack_element)

	template <size_t I, typename... T>
	using type_at_index = __type_pack_element<I, T...>;

#else

	/// \cond
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
	/// \endcond

	/// \brief	The type at the given index in the list.
	template <size_t I, typename... T>
	using type_at_index = POXY_IMPLEMENTATION_DETAIL(typename detail::type_at_index_impl<I, T...>::type);

#endif

	/// \cond
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
	/// \endcond

	/// \brief Detects if a type supports an interface.
	/// \see
	///		- [Detection Idiom](https://blog.tartanllama.xyz/detection-idiom/)
	///		- [std::experimental::is_detected](https://en.cppreference.com/w/cpp/experimental/is_detected)
	template <template <typename...> typename Trait, typename... Args>
	inline constexpr auto is_detected = detail::is_detected_<Trait, Args...>;

}

#include "../header_end.hpp"
