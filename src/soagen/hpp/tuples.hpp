//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @cond
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
	/// @endcond

	/// @brief	True if `T` implements the tuple protocol.
	template <typename T>
	inline constexpr bool is_tuple = POXY_IMPLEMENTATION_DETAIL(
		std::conjunction_v<detail::has_tuple_size_<T>,
						   detail::has_tuple_element_<T>,
						   std::disjunction<detail::has_tuple_get_member_<T>, detail::has_tuple_get_adl_<T>>>);

	/// @brief	Gets the member at index `I` from tuple-like `T`.
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
			using std::get;
			return get<I>(static_cast<T&&>(tuple));
		}
	}

	/// @cond
	namespace detail
	{
		template <typename...>
		struct is_constructible_by_unpacking_tuple_impl_ : std::false_type
		{};

		template <typename T, typename Tuple, size_t... Members>
		struct is_constructible_by_unpacking_tuple_impl_<T, Tuple, std::index_sequence<Members...>>
			: nested_trait_<types_<T, decltype(get_from_tuple<Members>(std::declval<Tuple>()))...>,
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
	/// @endcond

	/// @brief	True if `T` is constructible from the tuple-like `Tuple` by unpacking its members.
	template <typename T, typename Tuple>
	inline constexpr bool is_constructible_by_unpacking_tuple =
		POXY_IMPLEMENTATION_DETAIL(detail::is_constructible_by_unpacking_tuple_<T, Tuple>::value);
}

#include "header_end.hpp"
