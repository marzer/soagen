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
	template <typename, size_t...>
	struct row;

	namespace detail
	{
		// general rules for allowing implicit conversions:
		// - losing rvalue (T&& -> T&), (const T&& -> const T&)
		// - gaining const (T& -> const T&, T&& -> const T&&)
		// - both

		template <typename From, typename To>
		inline constexpr bool implicit_conversion_ok = false;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T, T> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&, const T&> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&&, T&> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&&, const T&> = true;

		template <typename T>
		inline constexpr bool implicit_conversion_ok<T&&, const T&&> = true;

		// general rules for allowing explicit conversions:

		template <typename From, typename To>
		inline constexpr bool explicit_conversion_ok = false;

		template <typename T>
		inline constexpr bool explicit_conversion_ok<T&, T&&> = true;

		template <typename T>
		inline constexpr bool explicit_conversion_ok<T&, const T&&> = true;

		// tests for compatible column permutations:

		template <typename From, typename To>
		inline constexpr bool column_conversion_ok = false;

		template <size_t... Columns>
		inline constexpr bool column_conversion_ok<std::index_sequence<Columns...>, std::index_sequence<Columns...>> =
			true;

		template <size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool column_conversion_ok<std::index_sequence<ColumnsA...>, std::index_sequence<ColumnsB...>> =
			(any_same_value<ColumnsB, ColumnsA...> && ...);

		// row implicit conversions:

		template <typename From, typename To>
		inline constexpr bool row_implicit_conversion_ok = false;

		template <typename TableA, size_t... ColumnsA, typename TableB, size_t... ColumnsB>
		inline constexpr bool row_implicit_conversion_ok<row<TableA, ColumnsA...>, //
														 row<TableB, ColumnsB...>> =
			implicit_conversion_ok<TableA, TableB>
			&& column_conversion_ok<std::index_sequence<ColumnsA...>, std::index_sequence<ColumnsB...>>;

		// row explicit conversions:

		template <typename From, typename To>
		inline constexpr bool row_explicit_conversion_ok = false;

		template <typename TableA, size_t... ColumnsA, typename TableB, size_t... ColumnsB>
		inline constexpr bool row_explicit_conversion_ok<row<TableA, ColumnsA...>, //
														 row<TableB, ColumnsB...>> =
			explicit_conversion_ok<TableA, TableB>
			&& column_conversion_ok<std::index_sequence<ColumnsA...>, std::index_sequence<ColumnsB...>>;
	}
	/// @endcond

	/// @brief		Base class for soagen::row.
	/// @details	Specialize this to add functionality to all rows of a particular type via CRTP.
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES row_base
	{};

	/// @brief A proxy type for treating (some subset of) an SoA row as if it were a regular AoS struct.
	template <typename Table, size_t... Columns>
	struct SOAGEN_EMPTY_BASES row //
	SOAGEN_HIDDEN_BASE(public detail::column_ref<Table, Columns>..., public row_base<row<Table, Columns...>>)
	{
		static_assert(std::is_reference_v<Table>,
					  "Table must be a reference so row members can derive their reference category");
		static_assert(std::is_empty_v<row_base<row<Table, Columns...>>>,
					  "row_base specializations may not have data members");
		static_assert(std::is_trivial_v<row_base<row<Table, Columns...>>>, "row_base specializations must be trivial");

		// columns:

		template <auto Column>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) column() const noexcept
		{
			static_assert(static_cast<size_t>(Column) < table_traits_type<remove_cvref<Table>>::column_count,
						  "column index out of range");

			return detail::column_ref<Table, static_cast<size_t>(Column)>::get_named_member();
		}

		// tuple protocol:

		template <auto Member>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) get() const noexcept
		{
			static_assert(Member < sizeof...(Columns), "member index out of range");

			return type_at_index<Member, detail::column_ref<Table, Columns>...>::get_named_member();
		}

		/// @name Equality
		/// @availability These operators are only available when all the column types are equality-comparable.
		/// @{

		/// @brief Returns true if all of the elements in two rows are equal.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_equality_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator==(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_equality_comparable)
		{
			return ((lhs.template column<Columns>() == rhs.template column<Columns>()) && ...);
		}

		/// @brief Returns true if not all of the elements in two rows are equal.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>
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
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) < 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically less-than-or-equal-to the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) <= 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically greater-than the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) > 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically greater-than-or-equal-to the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>
									 && table_traits_type<remove_cvref<Table>>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<remove_cvref<Table>>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) >= 0;
		}

		/// @}

		/// @name Conversion
		/// @{

		/// @brief Converts between different rows for the same table type.
		///
		/// @details This operator allows the following conversions, only some of which are implicit: <table>
		/// <tr><th> From				<th> To 				<th> `explicit`?	<th> Note
		/// <tr><td> `Table&`			<td> `const Table&`		<td>				<td> gains `const`
		/// <tr><td> `Table&&`			<td> `Table&` 			<td>				<td> `&&` &rarr; `&`
		/// <tr><td> `Table&&`			<td> `const Table&` 	<td>				<td> `&&` &rarr; `&`, gains `const`
		/// <tr><td> `Table&&`			<td> `const Table&&`	<td>				<td> gains `const`
		/// <tr><td> `const Table&&`	<td> `const Table&` 	<td>				<td> `&&` &rarr; `&`
		/// <tr><td> `Table&`			<td> `Table&&` 			<td> `explicit`		<td> Equivalent to `std::move()`
		/// <tr><td> `Table&`			<td> `const Table&&`	<td> `explicit`		<td> Equivalent to `std::move()`
		/// <tr><td> `const Table&`		<td> `const Table&&`	<td> `explicit`		<td> Equivalent to `std::move()`
		/// </table>
		///
		///	@note	Any of these conversions can also reduce or re-order the columns viewed by the row.
		///
		/// @attention 	There are no conversions provided which offer the equivalent of a `const_cast`-by-proxy.
		///				This is by design.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::row_implicit_conversion_ok<row, row<T, Cols...>>
									 && !detail::row_explicit_conversion_ok<row, row<T, Cols...>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator row<T, Cols...>() const noexcept
		{
			return row<T, Cols...>{ { static_cast<decltype(std::declval<row<T, Cols...>>().template column<Cols>())>(
				this->template column<Cols>()) }... };
		}

		/// @cond

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::row_implicit_conversion_ok<row, row<T, Cols...>>
									 && detail::row_explicit_conversion_ok<row, row<T, Cols...>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator row<T, Cols...>() const noexcept
		{
			return row<T, Cols...>{ { static_cast<decltype(std::declval<row<T, Cols...>>().template column<Cols>())>(
				this->template column<Cols>()) }... };
		}

		/// @endcond

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
	template <typename Table, auto... Columns>
	using row_type = POXY_IMPLEMENTATION_DETAIL(
		typename detail::row_type_<coerce_ref<Table>, std::index_sequence<static_cast<size_t>(Columns)...>>::type);
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
