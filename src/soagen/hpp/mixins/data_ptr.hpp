//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
	template <typename Derived, bool = has_data_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES data_ptr
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(assume_aligned(buffer_alignment<table_type>))
		constexpr std::byte* data() //
			noexcept(has_nothrow_data_member<table_type>)
		{
			return soagen::assume_aligned<buffer_alignment<table_type>>(
				static_cast<table_type&>(static_cast<Derived&>(*this)).data());
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(assume_aligned(buffer_alignment<table_type>))
		constexpr const std::byte* data() const //
			noexcept(has_nothrow_data_member<const table_type>)
		{
			return soagen::assume_aligned<buffer_alignment<table_type>>(
				static_cast<const table_type&>(static_cast<const Derived&>(*this)).data());
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES data_ptr<Derived, false>
	{};
}

#include "../header_end.hpp"
