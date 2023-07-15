//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "row.hpp"
SOAGEN_DISABLE_WARNINGS;
#include <iterator>
SOAGEN_ENABLE_WARNINGS;
#include "header_start.hpp"

namespace soagen
{
	/// @cond
	template <typename, size_t...>
	class iterator;

	namespace detail
	{
		// iterator implicit conversions are allowed when:
		// - changing columns
		// - losing rvalue (Table&& -> Table&), (const Table&& -> const Table&)
		// - gaining const (Table& -> const Table&, Table&& -> const Table&&)
		// - any combination of the three

		template <typename From, typename To>
		inline constexpr bool iterator_implicit_conversion_ok = false;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_implicit_conversion_ok<iterator<Table, ColumnsA...>, //
															  iterator<Table, ColumnsB...>> = true;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_implicit_conversion_ok<iterator<Table&&, ColumnsA...>, //
															  iterator<Table&, ColumnsB...>> = true;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_implicit_conversion_ok<iterator<const Table&&, ColumnsA...>, //
															  iterator<const Table&, ColumnsB...>> = true;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_implicit_conversion_ok<iterator<Table&, ColumnsA...>, //
															  iterator<const Table&, ColumnsB...>> = true;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_implicit_conversion_ok<iterator<Table&&, ColumnsA...>, //
															  iterator<const Table&&, ColumnsB...>> = true;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_implicit_conversion_ok<iterator<Table&&, ColumnsA...>, //
															  iterator<const Table&, ColumnsB...>> = true;

		// iterator explicit conversions are allowed when gaining rvalue and optionally gaining const
		// (note that we specifically avoid providing anything that would be the moral equivalent of
		// a const_cast because that armory is filled with very large and powerful footguns)

		template <typename From, typename To>
		inline constexpr bool iterator_explicit_conversion_ok = false;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_explicit_conversion_ok<iterator<Table&, ColumnsA...>, //
															  iterator<Table&&, ColumnsB...>> = true;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_explicit_conversion_ok<iterator<const Table&, ColumnsA...>, //
															  iterator<const Table&&, ColumnsB...>> = true;

		template <typename Table, size_t... ColumnsA, size_t... ColumnsB>
		inline constexpr bool iterator_explicit_conversion_ok<iterator<Table&, ColumnsA...>, //
															  iterator<const Table&&, ColumnsB...>> = true;

		template <typename T>
		struct arrow_proxy
		{
			mutable T value;

			SOAGEN_PURE_INLINE_GETTER
			constexpr T* operator->() const noexcept
			{
				return &value;
			}
		};

		template <typename Table>
		struct iterator_storage
		{
			std::add_const_t<remove_cvref<Table>>* table;
			typename remove_cvref<Table>::difference_type offset;
		};
	}
	/// @endcond

	/// @brief LegacyRandomAccessIterator for soagen-generated table types.
	template <typename Table, size_t... Columns>
	class iterator ///
		SOAGEN_HIDDEN_BASE(protected detail::iterator_storage<remove_cvref<Table>>)
	{
	  public:
		/// @brief Base SoA table type for this iterator.
		using table_type = remove_cvref<Table>;
		static_assert(is_soa<table_type>, "soagen iterators are for use with soagen-generated SoA table types.");

		/// @brief Cvref-qualified version of #table_type.
		using table_ref = Table;
		static_assert(std::is_reference_v<table_ref>,
					  "Table must be a reference so row members can derive their reference category");

		using size_type = typename table_type::size_type;

		/// @brief Signed integer type returned by difference operations.
		using difference_type = typename table_type::difference_type;

		/// @brief The row type dereferenced by this iterator.
		using row_type = row<Table, Columns...>;

		/// @brief Alias for #row_type.
		using value_type = row_type;

		/// @brief Alias for #row_type.
		using reference = row_type;

		/// @brief This iterator type is a LegacyRandomAccessIterator.
		using iterator_category = std::random_access_iterator_tag;

#if SOAGEN_CPP == 17
		using pointer = void;
#endif

	  private:
		/// @cond

		using base		= detail::iterator_storage<remove_cvref<Table>>;
		using table_ptr = std::add_pointer_t<std::remove_reference_t<Table>>;

		template <typename, size_t...>
		friend class soagen::iterator;

		SOAGEN_NODISCARD_CTOR
		constexpr iterator(base b) noexcept //
			: base{ b }
		{}

		/// @endcond

	  public:
		/// @brief Default constructor.
		SOAGEN_NODISCARD_CTOR
		constexpr iterator() noexcept = default;

		/// @brief Constructs an iterator to some part of a table.
		SOAGEN_NODISCARD_CTOR
		constexpr iterator(table_ref tbl, difference_type pos) noexcept //
			: base{ &tbl, pos }
		{}

		/// @name Incrementing
		/// @{

		/// @brief Increments the iterator by one row (pre-fix).
		friend constexpr iterator& operator++(iterator& it) noexcept // pre
		{
			++it.offset;
			return it;
		}

		/// @brief Increments the iterator by one row (post-fix).
		friend constexpr iterator operator++(iterator& it, int) noexcept // post
		{
			iterator pre = it;
			++it.offset;
			return pre;
		}

		/// @brief Increments the iterator by some arbitrary number of rows.
		friend constexpr iterator& operator+=(iterator& it, difference_type n) noexcept
		{
			it.offset += n;
			return it;
		}

		/// @brief Returns a copy of an iterator incremented by some arbitrary number of rows.
		SOAGEN_PURE_GETTER
		friend constexpr iterator operator+(const iterator& it, difference_type n) noexcept
		{
			auto it2 = it;
			it2 += n;
			return it2;
		}

		/// @}

		/// @name Decrementing
		/// @{

		/// @brief Decrements the iterator by one row (pre-fix).
		friend constexpr iterator& operator--(iterator& it) noexcept // pre
		{
			--it.offset;
			return it;
		}

		/// @brief Decrements the iterator by one row (post-fix).
		friend constexpr iterator operator--(iterator& it, int) noexcept // post
		{
			iterator pre = it;
			--it.offset;
			return pre;
		}

		/// @brief Decrements the iterator by some arbitrary number of rows.
		friend constexpr iterator& operator-=(iterator& it, difference_type n) noexcept
		{
			return it += (-n);
		}

		/// @brief Returns a copy of an iterator decremented by some arbitrary number of rows.
		SOAGEN_PURE_INLINE_GETTER
		friend constexpr iterator operator-(const iterator& it, difference_type n) noexcept
		{
			return it + (-n);
		}

		/// @}

		/// @name Difference
		/// @{

		/// @brief Returns the difference between two iterators.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_GETTER
		constexpr difference_type operator-(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset - rhs.offset;
		}

		/// @}

		/// @name Dereferencing
		/// @{

		/// @brief Returns the row the iterator refers to.
		SOAGEN_PURE_GETTER
		constexpr reference operator*() const noexcept
		{
			SOAGEN_ASSUME(!!base::table);
			SOAGEN_ASSUME(base::offset >= 0);

			return static_cast<table_ref>(*const_cast<table_ptr>(base::table))
				.template row<Columns...>(static_cast<size_type>(base::offset));
		}

		/// @brief Returns the row the iterator refers to.
		SOAGEN_PURE_INLINE_GETTER
		constexpr detail::arrow_proxy<row_type> operator->() const noexcept
		{
			return { *(*this) };
		}

		/// @brief Returns the row at some arbitrary offset from the one the iterator refers to.
		SOAGEN_PURE_GETTER
		constexpr reference operator[](difference_type offset) const noexcept
		{
			SOAGEN_ASSUME(!!base::table);
			SOAGEN_ASSUME(base::offset + offset >= 0);

			return static_cast<table_ref>(*const_cast<table_ptr>(base::table))
				.template row<Columns...>(static_cast<size_type>(base::offset + offset));
		}

		/// @}

		/// @name Equality
		/// @{

		/// @brief Returns true if two iterators refer to the same row in the same table.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_GETTER
		constexpr bool operator==(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::table == rhs.table && base::offset == rhs.offset;
		}

		/// @brief Returns true if two iterators do not refer to the same row in the same table.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const iterator& lhs, const iterator<T, Cols...>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		/// @}

		/// @name Comparison
		/// @{

		/// @brief Returns true if the LHS iterator refers to a row less-than the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset < rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row less-than-or-equal-to the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<=(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset <= rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row greater-than the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator>(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset > rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row greater-than-or-equal-to the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator>=(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset >= rhs.offset;
		}

		/// @}

		/// @name Conversion
		/// @{

		/// @brief Converts between different iterators for the same table type.
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
		///	@note	Any of these conversions can also change the columns viewed by the iterator - iterators to a table
		///			share the same underlying data structure regardless of the columns they are viewing.
		///
		/// @attention 	There are no conversions provided which offer the equivalent of a `const_cast`-by-proxy.
		///				This is by design.
		///
		SOAGEN_CONSTRAINED_TEMPLATE((detail::iterator_implicit_conversion_ok<iterator, iterator<T, Cols...>>
									 && !detail::iterator_explicit_conversion_ok<iterator, iterator<T, Cols...>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator iterator<T, Cols...>() const noexcept
		{
			return iterator<T, Cols...>{ static_cast<const base&>(*this) };
		}

		/// @cond

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::iterator_implicit_conversion_ok<iterator, iterator<T, Cols...>>
									 && detail::iterator_explicit_conversion_ok<iterator, iterator<T, Cols...>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator iterator<T, Cols...>() const noexcept
		{
			return iterator<T, Cols...>{ static_cast<const base&>(*this) };
		}

		/// @endcond

		/// @brief Converts this iterator to it's underlying #difference_type value.
		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator difference_type() const noexcept
		{
			return base::offset;
		}

		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator size_type() const noexcept
		{
			SOAGEN_ASSUME(base::offset >= 0);

			return static_cast<size_type>(base::offset);
		}

		/// @}
	};

	template <typename Table, size_t... Columns>
	SOAGEN_PURE_INLINE_GETTER
	constexpr iterator<Table, Columns...> operator+(typename iterator<Table, Columns...>::difference_type n,
													const iterator<Table, Columns...>& it) noexcept
	{
		return it + n;
	}

	/// @brief True if `T` is an instance of #soagen::iterator.
	template <typename T>
	inline constexpr bool is_iterator = POXY_IMPLEMENTATION_DETAIL(false);
	/// @cond
	template <typename Table, size_t... Columns>
	inline constexpr bool is_iterator<iterator<Table, Columns...>> = true;
	template <typename T>
	inline constexpr bool is_iterator<const T> = is_row<T>;
	template <typename T>
	inline constexpr bool is_iterator<volatile T> = is_row<T>;
	template <typename T>
	inline constexpr bool is_iterator<const volatile T> = is_row<T>;
	/// @endcond

	/// @cond
	namespace detail
	{
		template <typename Table, size_t... Columns>
		struct table_type_<iterator<Table, Columns...>>
		{
			using type = remove_cvref<Table>;
		};
		template <typename Table, size_t... Columns>
		struct table_traits_type_<iterator<Table, Columns...>>
		{
			using type = table_traits_type<remove_cvref<Table>>;
		};

		template <typename Table, typename IndexSequence>
		struct iterator_type_;
		template <typename Table, size_t... Columns>
		struct iterator_type_<Table, std::index_sequence<Columns...>>
		{
			using type = iterator<Table, Columns...>;
		};
		template <typename Table>
		struct iterator_type_<Table, std::index_sequence<>>
			: iterator_type_<Table, std::make_index_sequence<table_traits_type<remove_cvref<Table>>::column_count>>
		{};
	}
	/// @endcond

	/// @brief		The #soagen::iterator for a given SoA type and (some subset of) its columns.
	template <typename Table, size_t... Columns>
	using iterator_type = POXY_IMPLEMENTATION_DETAIL(
		typename detail::iterator_type_<coerce_ref<Table>, std::index_sequence<Columns...>>::type);
}

#include "header_end.hpp"
