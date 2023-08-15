//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "names.hpp"
#include "for_each.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @cond
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

		template <size_t... FromColumns, size_t... ToColumns>
		inline constexpr bool
			column_conversion_ok<std::index_sequence<FromColumns...>, std::index_sequence<ToColumns...>> =
				(any_same_value<ToColumns, FromColumns...> && ...);

		// row implicit conversions:

		template <typename From, typename To>
		inline constexpr bool row_implicit_conversion_ok = false;

		template <typename From, size_t... FromColumns, typename To, size_t... ToColumns>
		inline constexpr bool row_implicit_conversion_ok<row<From, FromColumns...>, //
														 row<To, ToColumns...>> =
			implicit_conversion_ok<coerce_ref<From>, coerce_ref<To>>
			&& column_conversion_ok<std::index_sequence<FromColumns...>, std::index_sequence<ToColumns...>>;

		// row explicit conversions:

		template <typename From, typename To>
		inline constexpr bool row_explicit_conversion_ok = false;

		template <typename From, size_t... FromColumns, typename To, size_t... ToColumns>
		inline constexpr bool row_explicit_conversion_ok<row<From, FromColumns...>, //
														 row<To, ToColumns...>> =
			explicit_conversion_ok<coerce_ref<From>, coerce_ref<To>>
			&& column_conversion_ok<std::index_sequence<FromColumns...>, std::index_sequence<ToColumns...>>;
	}
	/// @endcond

	/// @brief		Base class for soagen::row.
	/// @details	Specialize this to add functionality to all rows of a particular type via CRTP.
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES row_base
	{};

	/// @brief A proxy type for treating (some subset of) an SoA row as if it were a regular AoS struct.
	/// @details	This type has reference semantics; In addition to the member functions described below,
	///				rows for `soagen`-generated SoA types will named member references variables matching their
	///				columns, e.g.:
	///
	///				@code{.toml}
	///				[structs.employees]
	///				variables = [
	///					{name = "id",     type = "unsigned long long"},
	///					{name = "name",   type = "std::string"},
	///					{name = "salary", type = "long long"},
	///				]
	///				@endcode
	///
	///				Would correspond to:
	///
	///				@cpp
	///				struct soagen::row<employees>
	///				{
	///					unsigned long long& id;
	///					std::string&        name;
	///					long long&          salary;
	///				};
	///				@ecpp
	///
	///				Alternatively, if you're not using the generator and are just using a soagen::table directly,
	///				you'll get ordinally-named reference members for columns 0-15:
	///
	///				@cpp
	///				// a row for this type:
	///				using employees = soagen::table<unsigned long long, std::string, long long /* ... */>;
	///
	///				// would look like this:
	///				struct soagen::row<employees>
	///				{
	///					unsigned long long& first;
	///					std::string&        second;
	///					long long&          third;
	///
	///					// ...and so on, up to "sixteenth".
	///				};
	///				@ecpp
	template <typename Soa, size_t... Columns>
	struct SOAGEN_EMPTY_BASES row //
	SOAGEN_HIDDEN_BASE(public detail::column_ref<Soa, Columns>..., public row_base<row<Soa, Columns...>>)
	{
		static_assert(is_soa<remove_cvref<Soa>>, "Soa must be a table or soagen-generated SoA type.");
		static_assert(!std::is_lvalue_reference_v<Soa>, "Soa may not be an lvalue reference.");
		static_assert(sizeof...(Columns), "Column index list may not be empty.");
		static_assert(std::is_empty_v<row_base<row<Soa, Columns...>>>,
					  "row_base specializations may not have data members");
		static_assert(std::is_trivial_v<row_base<row<Soa, Columns...>>>, "row_base specializations must be trivial");

		/// @name Columns
		/// @{

		/// @brief Returns a reference to the specified column's value.
		template <auto Column>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) column() noexcept
		{
			static_assert(static_cast<size_t>(Column) < table_traits_type<Soa>::column_count,
						  "column index out of range");

			return detail::column_ref<Soa, static_cast<size_t>(Column)>::get_ref();
		}

		/// @brief Returns a reference to the specified column's value (const overload).
		template <auto Column>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) column() const noexcept
		{
			static_assert(static_cast<size_t>(Column) < table_traits_type<Soa>::column_count,
						  "column index out of range");

			return detail::column_ref<Soa, static_cast<size_t>(Column)>::get_ref();
		}

		/// @brief Invokes a callable once for each column in the row.
		/// @see soagen::for_each_column
		template <typename Func>
		SOAGEN_ALWAYS_INLINE
		constexpr void for_each_column(Func&& func) //
			noexcept(noexcept(soagen::for_each_column(std::declval<row&>(), std::declval<Func&&>())))
		{
			soagen::for_each_column(*this, static_cast<Func&&>(func));
		}

		/// @brief Invokes a callable once for each column in the row (const overload).
		/// @see soagen::for_each_column
		template <typename Func>
		SOAGEN_ALWAYS_INLINE
		constexpr void for_each_column(Func&& func) const //
			noexcept(noexcept(soagen::for_each_column(std::declval<const row&>(), std::declval<Func&&>())))
		{
			soagen::for_each_column(*this, static_cast<Func&&>(func));
		}

		/// @}

		/// @name Tuple protocol
		/// @{

		/// @brief Returns a reference to the specified member's value.
		/// @note `Member` corresponds to the column at the specified position of the `Columns` template argument.
		template <auto Member>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) get() noexcept
		{
			static_assert(Member < sizeof...(Columns), "member index out of range");

			return type_at_index<static_cast<size_t>(Member), detail::column_ref<Soa, Columns>...>::get_ref();
		}

		/// @brief Returns a reference to the specified member's value (const overload).
		/// @note `Member` corresponds to the column at the specified position of the `Columns` template argument.
		template <auto Member>
		SOAGEN_PURE_INLINE_GETTER
		constexpr decltype(auto) get() const noexcept
		{
			static_assert(Member < sizeof...(Columns), "member index out of range");

			return type_at_index<static_cast<size_t>(Member), detail::column_ref<Soa, Columns>...>::get_ref();
		}

		/// @}

		/// @name Equality
		/// @availability These operators are only available when all the column types are equality-comparable.
		/// @{

		/// @brief Returns true if all of the elements in two rows are equal.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_equality_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator==(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_equality_comparable)
		{
			return ((lhs.template column<Columns>() == rhs.template column<Columns>()) && ...);
		}

		/// @brief Returns true if not all of the elements in two rows are equal.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_equality_comparable),
									typename T)
		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_equality_comparable)
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
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
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
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) < 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically less-than-or-equal-to the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator<=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) <= 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically greater-than the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) > 0;
		}

		/// @brief Returns true if the RHS row is ordered lexicographically greater-than-or-equal-to the RHS row.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T> && table_traits_type<Soa>::all_less_than_comparable),
									typename T)
		SOAGEN_NODISCARD
		friend constexpr bool operator>=(const row& lhs, const row<T, Columns...>& rhs) //
			noexcept(table_traits_type<Soa>::all_nothrow_less_than_comparable)
		{
			return row_compare_impl<0>(lhs, rhs) >= 0;
		}

		/// @}

		/// @name Conversion
		/// @{

		/// @brief Converts between different rows for the same SoA type.
		///
		/// @details This operator allows the following conversions, only some of which are implicit: <table>
		/// <tr><th> From			<th> To 			<th> `explicit`	<th> Note
		/// <tr><td> `T&`			<td> `const T&`		<td>			<td> gains `const`
		/// <tr><td> `T&&`			<td> `T&` 			<td>			<td> `&&` &rarr; `&`
		/// <tr><td> `T&&`			<td> `const T&` 	<td>			<td> `&&` &rarr; `&`, gains `const`
		/// <tr><td> `T&&`			<td> `const T&&`	<td>			<td> gains `const`
		/// <tr><td> `const T&&`	<td> `const T&` 	<td>			<td> `&&` &rarr; `&`
		/// <tr><td> `T&`			<td> `T&&` 			<td> Yes		<td> Equivalent to `std::move()`
		/// <tr><td> `T&`			<td> `const T&&`	<td> Yes		<td> Equivalent to `std::move()`
		/// <tr><td> `const T&`		<td> `const T&&`	<td> Yes		<td> Equivalent to `std::move()`
		/// </table>
		///
		///	@note	Any of these conversions can also reduce or re-order the columns viewed by the row.
		///
		/// @attention 	There are no conversions which offer the equivalent of a `const_cast`-by-proxy.
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
}

/// @cond
namespace std
{
	template <typename Soa, size_t... Columns>
	struct tuple_size<soagen::row<Soa, Columns...>> //
		: std::integral_constant<size_t, sizeof...(Columns)>
	{};

	template <size_t Member, typename Soa, size_t... Columns>
	struct tuple_element<Member, soagen::row<Soa, Columns...>>
	{
		using type = decltype(std::declval<soagen::row<Soa, Columns...>>().template get<Member>());
	};
}
/// @endcond

#include "header_end.hpp"
