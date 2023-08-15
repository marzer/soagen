//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../header_start.hpp"

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

#include "../header_end.hpp"
