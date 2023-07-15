//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "meta.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @brief A proxy type for treating (some subset of) an SoA row as if it were a regular AoS struct.
	template <typename Table, size_t... Columns>
	struct SOAGEN_EMPTY_BASES row //
	SOAGEN_HIDDEN_BASE(public detail::col_ref_<Table, Columns>...)
	{
		static_assert(std::is_reference_v<Table>,
					  "Table must be a reference so row members can derive their reference category");

		// columns:

		template <size_t Column>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) column() const noexcept
		{
			static_assert(Column < table_traits_type<remove_cvref<Table>>::column_count, "column index out of range");

			return detail::col_ref_<Table, Column>::get_ref_impl();
		}

		// tuple protocol:

		template <size_t Member>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) get() const noexcept
		{
			static_assert(Member < sizeof...(Columns), "member index out of range");

			return type_at_index<Member, detail::col_ref_<Table, Columns>...>::get_ref_impl();
		}

		/// @name Equality
		/// @availability These operators are only available when all the column types are equality-comparable.
		/// @{

		/// @brief Returns true if all of the elements in two rows are equal.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_equality_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator==(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_equality_comparable)
		{
			return ((lhs.template column<Columns>() == rhs.template column<Columns>()) && ...);
		}

		/// @brief Returns true if not all of the elements in two rows are equal.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_equality_comparable),
									typename T)
		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_equality_comparable)
		{
			return !(lhs == rhs);
		}

		/// @}

		/// @name Comparison
		/// @availability These operators are only available when all the column types are less-than-comparable.
		/// @{

	  private:
		/// @cond
		template <size_t Member, typename T>
		SOAGEN_NODISCARD
		static constexpr int row_compare_impl(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			if (lhs.template get<Member>() < rhs.template get<Member>())
				return -1;

			if (rhs.template get<Member>() < lhs.template get<Member>())
				return 1;

			if constexpr (Member + 1u == sizeof...(Columns))
				return 0;
			else
				return row_compare_impl<Member + 1u>(lhs, rhs);
		}
		/// @endcond

	  public:
		/// @brief Returns true if the RHS row is ordered lexicographically less-than the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) < 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically less-than-or-equal-to the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) <= 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically greater-than the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) > 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically greater-than-or-equal-to the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) >= 0;
		}

		/// @}
	};

	/// @brief True if `T` is an instance of #soagen::row.
	template <typename T>
	inline constexpr bool is_row = POXY_IMPLEMENTATION_DETAIL(false);
	/// @cond
	template <typename Table, size_t... Columns>
	inline constexpr bool is_row<row<Table, Columns...>> = true;
	template <typename T>
	inline constexpr bool is_row<const T> = is_row<T>;
	template <typename T>
	inline constexpr bool is_row<volatile T> = is_row<T>;
	template <typename T>
	inline constexpr bool is_row<const volatile T> = is_row<T>;
	/// @endcond

	/// @cond
	namespace detail
	{
		template <typename Table, size_t... Columns>
		struct table_type_<row<Table, Columns...>>
		{
			using type = remove_cvref<Table>;
		};
		template <typename Table, size_t... Columns>
		struct table_traits_type_<row<Table, Columns...>>
		{
			using type = table_traits_type<remove_cvref<Table>>;
		};

		template <typename Table, typename IndexSequence>
		struct row_type_;
		template <typename Table, size_t... Columns>
		struct row_type_<Table, std::index_sequence<Columns...>>
		{
			using type = row<Table, Columns...>;
		};
		template <typename Table>
		struct row_type_<Table, std::index_sequence<>>
			: row_type_<Table, std::make_index_sequence<table_traits_type<remove_cvref<Table>>::column_count>>
		{};
	}
	/// @endcond

	/// @brief		The #soagen::row for a given SoA type and (some subset of) its columns.
	template <typename Table, size_t... Columns>
	using row_type = POXY_IMPLEMENTATION_DETAIL(
		typename detail::row_type_<coerce_ref<Table>, std::index_sequence<Columns...>>::type);
}

/// @cond
namespace std
{
	template <typename Table, size_t... Columns>
	struct tuple_size<soagen::row<Table, Columns...>> //
		: std::integral_constant<size_t, sizeof...(Columns)>
	{};

	template <size_t Member, typename Table, size_t... Columns>
	struct tuple_element<Member, soagen::row<Table, Columns...>>
	{
		using type = decltype(std::declval<soagen::row<Table, Columns...>>().template get<Member>());
	};
}
/// @endcond

#include "header_end.hpp"
