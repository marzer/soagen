//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"

#define SOAGEN_DETECT_INTEGER(name, trait)                                                                             \
	template <typename T>                                                                                              \
	using name##_trait_ = decltype(trait);                                                                             \
	template <typename T, bool = is_detected<name##_trait_, T>>                                                        \
	struct name##_ : std::disjunction<std::is_integral<remove_cvref<decltype(trait)>>,                                 \
									  std::is_enum<remove_cvref<decltype(trait)>>,                                     \
									  std::is_constructible<std::size_t, decltype(trait)>>                             \
	{};                                                                                                                \
	template <typename T>                                                                                              \
	struct name##_<T, false> : std::false_type                                                                         \
	{}

namespace soagen
{
	/// @cond
	namespace detail
	{
		// static function T::source_offset()
		SOAGEN_DETECT_INTEGER(has_source_offset_static_func, remove_cvref<T>::source_offset());

		// static var T::source_offset
		SOAGEN_DETECT_INTEGER(has_source_offset_static_var, remove_cvref<T>::source_offset);

		// member function t.source_offset()
		SOAGEN_DETECT_INTEGER(has_source_offset_member_func, std::declval<const T&>().source_offset());

		// member var t.source_offset
		SOAGEN_DETECT_INTEGER(has_source_offset_member_var,
							  std::declval<const T&>().*(&remove_cvref<T>::source_offset));

		// ADL
		namespace adl_dummy
		{
			void source_offset();
			template <typename T>
			using has_source_offset_adl_trait_ = decltype(source_offset(std::declval<const T&>()));
		}
		template <typename T>
		using has_source_offset_adl_ = is_detected_<adl_dummy::has_source_offset_adl_trait_, T>;
	}
	/// @endcond

	/// @brief	True if `T` has a `source_offset()` member function, or one found via ADL.
	template <typename T>
	inline constexpr bool has_source_offset =
		POXY_IMPLEMENTATION_DETAIL(std::disjunction_v<detail::has_source_offset_static_func_<T>,
													  detail::has_source_offset_member_func_<T>,
													  detail::has_source_offset_member_var_<T>,
													  detail::has_source_offset_static_var_<T>,
													  detail::has_source_offset_adl_<T>>);

	/// @brief	Gets the base index offset from an object.
	template <typename T>
	SOAGEN_PURE_INLINE_GETTER
	constexpr std::size_t get_source_offset([[maybe_unused]] const T& object) noexcept
	{
		// note that order is important here -
		// we have to try to detect member functions and vars before static vars

		if constexpr (detail::has_source_offset_static_func_<T>::value)
		{
			return static_cast<size_t>(remove_cvref<T>::source_offset());
		}
		else if constexpr (detail::has_source_offset_member_func_<T>::value)
		{
			return static_cast<size_t>(object.source_offset());
		}
		else if constexpr (detail::has_source_offset_member_var_<T>::value)
		{
			return static_cast<size_t>(object.source_offset);
		}
		else if constexpr (detail::has_source_offset_static_var_<T>::value)
		{
			return static_cast<size_t>(remove_cvref<T>::source_offset);
		}
		else if constexpr (detail::has_source_offset_adl_<T>::value)
		{
			using detail::adl_dummy::source_offset;
			return static_cast<size_t>(source_offset(object));
		}
		else
			return {};
	}
}

#undef SOAGEN_DETECT_INTEGER

#include "header_end.hpp"
