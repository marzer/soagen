//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../header_start.hpp"

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

#include "../header_end.hpp"
