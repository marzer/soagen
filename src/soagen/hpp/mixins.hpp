//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "meta.hpp"

namespace soagen::mixins
{
	//--- resize() -----------------------------------------------------------------------------------------------------

	template <typename Derived, bool = has_resize_member<table_type<Derived>, typename table_type<Derived>::size_type>>
	struct SOAGEN_EMPTY_BASES resizable
	{
		using table_type = soagen::table_type<Derived>;
		using size_type	 = typename table_type::size_type;

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& resize(size_type new_size) //
			noexcept(soagen::has_nothrow_resize_member<table_type, size_type>)
		{
			static_cast<Derived&>(*this).table().resize(new_size);
			return static_cast<Derived&>(*this);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES resizable<Derived, false>
	{};

	//--- swap() -------------------------------------------------------------------------------------------------------

	template <typename Derived, bool = has_swap_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES swappable
	{
		using table_type = soagen::table_type<Derived>;

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		void swap(Derived& other) //
			noexcept(soagen::has_nothrow_swap_member<table_type>)
		{
			return static_cast<Derived&>(*this).table().swap(other.table());
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES swappable<Derived, false>
	{};

	//--- equality-comparable-------------------------------------------------------------------------------------------

	template <typename Derived, bool = is_equality_comparable<const table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES equality_comparable
	{
		using table_type = soagen::table_type<Derived>;

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator==(const Derived& lhs, const Derived& rhs) //
			noexcept(soagen::is_nothrow_equality_comparable<table_type>)
		{
			return lhs.table() == rhs.table();
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const Derived& lhs, const Derived& rhs) //
			noexcept(soagen::is_nothrow_equality_comparable<table_type>)
		{
			return lhs.table() != rhs.table();
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES equality_comparable<Derived, false>
	{};

	//--- less-than-comparable-------------------------------------------------------------------------------------------

	template <typename Derived, bool = is_less_than_comparable<const table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES less_than_comparable
	{
		using table_type = soagen::table_type<Derived>;

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<(const Derived& lhs, const Derived& rhs) //
			noexcept(soagen::is_nothrow_less_than_comparable<table_type>)
		{
			return lhs.table() < rhs.table();
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<=(const Derived& lhs, const Derived& rhs) //
			noexcept(soagen::is_nothrow_less_than_comparable<table_type>)
		{
			return lhs.table() <= rhs.table();
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>(const Derived& lhs, const Derived& rhs) //
			noexcept(soagen::is_nothrow_less_than_comparable<table_type>)
		{
			return lhs.table() > rhs.table();
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>=(const Derived& lhs, const Derived& rhs) //
			noexcept(soagen::is_nothrow_less_than_comparable<table_type>)
		{
			return lhs.table() >= rhs.table();
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES less_than_comparable<Derived, false>
	{};

	//--- data() -------------------------------------------------------------------------------------------------------

	template <typename Derived, bool = has_data_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES data_ptr
	{
		using table_type	 = soagen::table_type<Derived>;
		using table_traits	 = soagen::table_traits_type<Derived>;
		using allocator_type = soagen::allocator_type<Derived>;

		SOAGEN_ALIGNED_COLUMN(0)
		constexpr std::byte* data() //
			noexcept(soagen::has_nothrow_data_member<table_type>)
		{
			return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, 0>>(
				static_cast<Derived&>(*this).table().data());
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES data_ptr<Derived, false>
	{};

	//--- data() (const) -----------------------------------------------------------------------------------------------

	template <typename Derived, bool = has_data_member<const table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES const_data_ptr
	{
		using table_type	 = soagen::table_type<Derived>;
		using table_traits	 = soagen::table_traits_type<Derived>;
		using allocator_type = soagen::allocator_type<Derived>;

		SOAGEN_ALIGNED_COLUMN(0)
		constexpr const std::byte* data() const //
			noexcept(soagen::has_nothrow_data_member<const table_type>)
		{
			return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, 0>>(
				static_cast<const Derived&>(*this).table().data());
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES const_data_ptr<Derived, false>
	{};
}
