//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../names.hpp"
#include "../for_each.hpp"
#include "../header_start.hpp"

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

#include "../header_end.hpp"
