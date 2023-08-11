//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "row.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @cond
	namespace detail
	{
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
			remove_cvref<Table>* table;
			typename remove_cvref<Table>::difference_type offset;
		};
	}
	/// @endcond

	/// @brief		Base class for soagen::iterator.
	/// @details	Specialize this to add functionality to all iterators of a particular type via CRTP.
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES iterator_base
	{};

	/// @brief RandomAccessIterator for table types.
	template <typename Table, size_t... Columns>
	class SOAGEN_EMPTY_BASES iterator ///
		SOAGEN_HIDDEN_BASE(protected detail::iterator_storage<soagen::table_type<remove_cvref<Table>>>,
						   public iterator_base<iterator<remove_lvalue_ref<Table>, Columns...>>)
	{
		static_assert(std::is_empty_v<iterator_base<iterator<remove_lvalue_ref<Table>, Columns...>>>,
					  "iterator_base specializations may not have data members");
		static_assert(std::is_trivial_v<iterator_base<iterator<remove_lvalue_ref<Table>, Columns...>>>,
					  "iterator_base specializations must be trivial");

	  public:
		/// @brief Base soagen::table type for this iterator.
		using table_type = soagen::table_type<remove_cvref<Table>>;
		static_assert(is_table<table_type>, "soagen iterators are for use with soagen SoA types.");

		/// @brief Cvref-qualified version of #table_type.
		using table_ref = coerce_ref<copy_cvref<table_type, Table>>;

		using size_type = typename table_type::size_type;

		/// @brief Signed integer type returned by difference operations.
		using difference_type = typename table_type::difference_type;

		/// @brief The row type dereferenced by this iterator.
		using row_type = soagen::row_type<remove_lvalue_ref<Table>, Columns...>;

		/// @brief Alias for #row_type.
		using value_type = row_type;

		/// @brief Alias for #row_type.
		using reference = row_type;

		/// @brief This iterator type is a RandomAccessIterator.
		using iterator_category = std::random_access_iterator_tag;

#if SOAGEN_CPP <= 17
		using pointer = void;
#endif

	  private:
		/// @cond

		using base = detail::iterator_storage<soagen::table_type<remove_cvref<Table>>>;

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
			: base{ const_cast<table_type*>(&tbl), pos }
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
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_GETTER
		constexpr difference_type operator-(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset - rhs.offset;
		}

		/// @}

		/// @name Dereferencing
		/// @{

	  private:
		/// @cond
		template <size_t Column>
		using cv_value_type =
			conditionally_add_volatile<conditionally_add_const<soagen::value_type<remove_cvref<Table>, Column>,
															   std::is_const_v<std::remove_reference_t<Table>>>,
									   std::is_volatile_v<std::remove_reference_t<Table>>>;

		template <size_t Column>
		using cv_value_ref = std::conditional_t<std::is_rvalue_reference_v<Table>,
												std::add_rvalue_reference_t<cv_value_type<Column>>,
												std::add_lvalue_reference_t<cv_value_type<Column>>>;
		/// @endcond

	  public:
		/// @brief Returns the row the iterator refers to.
		SOAGEN_PURE_GETTER
		constexpr reference operator*() const noexcept
		{
			SOAGEN_ASSUME(!!base::table);
			SOAGEN_ASSUME(base::offset >= 0);

			return row_type{ { static_cast<cv_value_ref<Columns>>(
				base::table->template column<Columns>()[base::offset]) }... };
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

			return row_type{ { static_cast<cv_value_ref<Columns>>(
				base::table->template column<Columns>()[base::offset + offset]) }... };
		}

		/// @}

		/// @name Equality
		/// @{

		/// @brief Returns true if two iterators refer to the same row in the same table.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_GETTER
		constexpr bool operator==(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::table == rhs.table && base::offset == rhs.offset;
		}

		/// @brief Returns true if two iterators do not refer to the same row in the same table.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const iterator& lhs, const iterator<T, Cols...>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		/// @}

		/// @name Comparison
		/// @{

		/// @brief Returns true if the LHS iterator refers to a row less-than the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset < rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row less-than-or-equal-to the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<=(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset <= rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row greater-than the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator>(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset > rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row greater-than-or-equal-to the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Table, T>), typename T, size_t... Cols)
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
		SOAGEN_CONSTRAINED_TEMPLATE((detail::implicit_conversion_ok<coerce_ref<Table>, coerce_ref<T>>
									 && !detail::explicit_conversion_ok<coerce_ref<Table>, coerce_ref<T>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator iterator<T, Cols...>() const noexcept
		{
			return iterator<T, Cols...>{ static_cast<const base&>(*this) };
		}

		/// @cond

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::implicit_conversion_ok<coerce_ref<Table>, coerce_ref<T>>
									 && detail::explicit_conversion_ok<coerce_ref<Table>, coerce_ref<T>>),
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
}

#include "header_end.hpp"
