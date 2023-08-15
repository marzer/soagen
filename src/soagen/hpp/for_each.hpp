//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "invoke.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @cond
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
	/// @endcond

	/// @brief Invokes a callable once for each column in an object.
	///
	/// @tparam T		A class type with a `column<N>()` template member function (e.g. tables, spans, rows).
	///
	/// @tparam Func	A callable type compatible with one of the following signatures:<ul>
	/// <li> `void(auto&&, std::integral_constant<std::size_t, N>)`
	/// <li> `void(auto&&, std::size_t)`
	/// <li> `void(std::integral_constant<std::size_t, N>, auto&&)`
	/// <li> `void(std::size_t, auto&&)`
	/// <li> `void(auto&&)`
	/// </ul>
	/// Overload resolution is performed in the order listed above.
	///
	/// @param obj 	The object to invoke the callable on for each column.
	/// @param func The callable to invoke.
	template <typename T, typename Func>
	void for_each_column(T&& obj, Func&& func) //
		noexcept(noexcept(
			detail::for_each_column(std::declval<T&&>(), std::declval<Func&&>(), column_indices<remove_cvref<T>>{})))
	{
		detail::for_each_column(static_cast<T&&>(obj), static_cast<Func&&>(func), column_indices<remove_cvref<T>>{});
	}
}

#include "header_end.hpp"
