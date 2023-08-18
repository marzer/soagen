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
			static_assert(!is_cvref<Table>);

			Table* table;
			ptrdiff_t offset;
		};
	}
	/// @endcond

	/// @brief		Base class for soagen::iterator.
	/// @details	Specialize this to add functionality to all iterators of a particular type via CRTP.
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES iterator_base
	{};

	/// @brief RandomAccessIterator for SoA types.
	template <typename Soa, size_t... Columns>
	class SOAGEN_EMPTY_BASES iterator ///
		SOAGEN_HIDDEN_BASE(protected detail::iterator_storage<table_type<Soa>>,
						   public iterator_base<iterator<Soa, Columns...>>)
	{
		static_assert(is_soa<remove_cvref<Soa>>, "Soa must be a table or soagen-generated SoA type.");
		static_assert(!std::is_lvalue_reference_v<Soa>, "Soa may not be an lvalue reference.");
		static_assert(sizeof...(Columns), "Column index list may not be empty.");
		static_assert(std::is_empty_v<iterator_base<iterator<Soa, Columns...>>>,
					  "iterator_base specializations may not have data members");
		static_assert(std::is_trivial_v<iterator_base<iterator<Soa, Columns...>>>,
					  "iterator_base specializations must be trivial");

	  public:
		/// @brief Base SoA type for this iterator.
		using soa_type = remove_cvref<Soa>;

		/// @brief Cvref-qualified version of #soa_type.
		using soa_ref = coerce_ref<Soa>;
		static_assert(std::is_reference_v<soa_ref>);

		/// @brief Unsigned integer size type used by the corresponding SoA type.
		using size_type = std::size_t;

		/// @brief Signed integer difference type used by the corresponding SoA type.
		using difference_type = std::ptrdiff_t;

		/// @brief The #soagen::row type dereferenced by this iterator.
		using row_type = soagen::row_type<Soa, Columns...>;

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

		template <typename, size_t...>
		friend class iterator;
		template <typename>
		friend class span;

		using base = detail::iterator_storage<table_type<Soa>>;

		using table_ref = copy_cvref<table_type<Soa>, soa_ref>;
		static_assert(std::is_reference_v<table_ref>);

		SOAGEN_NODISCARD_CTOR
		constexpr iterator(base b) noexcept //
			: base{ b }
		{}

		SOAGEN_NODISCARD_CTOR
		constexpr iterator(table_ref tbl, difference_type pos, std::true_type) noexcept //
			: base{ const_cast<table_type<Soa>*>(&static_cast<table_ref>(tbl)), pos }
		{}

		/// @endcond

	  public:
		/// @brief Default constructor.
		SOAGEN_NODISCARD_CTOR
		constexpr iterator() noexcept //
			: base{}
		{}

		/// @brief Copy constructor.
		SOAGEN_NODISCARD_CTOR
		iterator(const iterator&) = default;

		/// @brief Copy-assignment operator.
		iterator& operator=(const iterator&) = default;

		/// @brief Constructs an iterator to some row of a SoA container.
		SOAGEN_NODISCARD_CTOR
		constexpr iterator(soa_ref soa, difference_type pos) noexcept //
			: iterator{ static_cast<table_ref>(static_cast<soa_ref>(soa)), pos, std::true_type{} }
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
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
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

			return row_type{ static_cast<value_ref<Soa, Columns>>(
				base::table->template column<Columns>()[base::offset])... };
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

			return row_type{ static_cast<value_ref<Soa, Columns>>(
				base::table->template column<Columns>()[base::offset + offset])... };
		}

		/// @}

		/// @name Equality
		/// @{

		/// @brief Returns true if two iterators refer to the same row in the same table.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_GETTER
		constexpr bool operator==(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::table == rhs.table && base::offset == rhs.offset;
		}

		/// @brief Returns true if two iterators do not refer to the same row in the same table.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const iterator& lhs, const iterator<T, Cols...>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		/// @}

		/// @name Comparison
		/// @{

		/// @brief Returns true if the LHS iterator refers to a row less-than the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset < rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row less-than-or-equal-to the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator<=(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset <= rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row greater-than the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator>(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset > rhs.offset;
		}

		/// @brief Returns true if the LHS iterator refers to a row greater-than-or-equal-to the RHS iterator.
		SOAGEN_CONSTRAINED_TEMPLATE((same_table_type<Soa, T>), typename T, size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool operator>=(const iterator<T, Cols...>& rhs) const noexcept
		{
			return base::offset >= rhs.offset;
		}

		/// @}

		/// @name Conversion
		/// @{

		/// @brief Converts between different iterators for the same SoA type.
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
		///	@note	Any of these conversions can also change the columns viewed by the iterator - iterators to a table
		///			share the same underlying data structure regardless of the columns they are viewing.
		///
		/// @attention 	There are no conversions which offer the equivalent of a `const_cast`-by-proxy.
		///				This is by design.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && !detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
									typename T,
									size_t... Cols)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator iterator<T, Cols...>() const noexcept
		{
			return iterator<T, Cols...>{ static_cast<const base&>(*this) };
		}

		/// @cond

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
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

	template <typename Soa, size_t... Columns>
	SOAGEN_PURE_INLINE_GETTER
	constexpr iterator<Soa, Columns...> operator+(typename iterator<Soa, Columns...>::difference_type n,
												  const iterator<Soa, Columns...>& it) noexcept
	{
		return it + n;
	}
}

#include "header_end.hpp"
