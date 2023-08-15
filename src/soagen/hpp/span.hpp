//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "row.hpp"
#include "iterator.hpp"
#include "mixins/columns.hpp"
#include "header_start.hpp"
SOAGEN_DISABLE_SHADOW_WARNINGS;

namespace soagen
{
	/// @cond
	namespace detail
	{
		template <typename Table>
		struct span_storage
		{
			static_assert(!is_cvref<Table>);

			Table* table;
			size_t start;
			size_t count;
		};

		SOAGEN_CONST_GETTER
		constexpr size_t calc_span_count(size_t start, size_t src_count, size_t desired_count) noexcept
		{
			return desired_count == static_cast<size_t>(-1) ? src_count - min(start, src_count) : desired_count;
		}
	}
	/// @endcond

	/// @brief		Base class for soagen::span.
	/// @details	Specialize this to add functionality to all spans of a particular type via CRTP.
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES span_base
	{};

	/// @brief A span type for representing some subset of a SoA container's rows.
	template <typename Soa>
	class SOAGEN_EMPTY_BASES span //
		SOAGEN_HIDDEN_BASE(protected detail::span_storage<table_type<Soa>>,
						   public span_base<span<Soa>>,
						   public mixins::columns<span<Soa>>)
	{
		static_assert(is_soa<remove_cvref<Soa>>, "Soa must be a table or soagen-generated SoA type.");
		static_assert(!std::is_lvalue_reference_v<Soa>, "Soa may not be an lvalue reference.");
		static_assert(std::is_empty_v<span_base<span<Soa>>>, "span_base specializations may not have data members");
		static_assert(std::is_trivial_v<span_base<span<Soa>>>, "span_base specializations must be trivial");

	  public:
		/// @brief Base SoA type for this span.
		using soa_type = remove_cvref<Soa>;

		/// @brief Cvref-qualified version of #soa_type.
		using soa_ref = coerce_ref<Soa>;
		static_assert(std::is_reference_v<soa_ref>);

		/// @brief Unsigned integer size type used by the corresponding SoA type.
		using size_type = std::size_t;

		/// @brief Signed integer difference type used by the corresponding SoA type.
		using difference_type = std::ptrdiff_t;

		/// @brief #soagen::row type used by this span.
		using row_type = soagen::row_type<Soa>;

		/// @brief Row iterators returned by iterator functions.
		using iterator = soagen::iterator_type<Soa>;

		/// @brief Row iterators returned by "c"-prefixed iterator functions.
		using const_iterator = soagen::const_iterator_type<Soa>;

		/// @brief Alias for this span.
		using span_type = span;

		/// @brief Const-qualified span type.
		using const_span_type = soagen::const_span_type<Soa>;

	  private:
		/// @cond

		template <typename>
		friend class span;

		using base = detail::span_storage<table_type<Soa>>;

		using table_ref = copy_cvref<table_type<Soa>, soa_ref>;
		static_assert(std::is_reference_v<table_ref>);

		using const_table_ref =
			POXY_IMPLEMENTATION_DETAIL(copy_ref<std::add_const_t<std::remove_reference_t<table_ref>>, soa_ref>);
		static_assert(std::is_reference_v<const_table_ref>);

		SOAGEN_NODISCARD_CTOR
		constexpr span(base b) noexcept //
			: base{ b }
		{}

		SOAGEN_NODISCARD_CTOR
		constexpr span(table_ref tbl, size_type start, size_type count, std::true_type) noexcept //
			: base{ const_cast<table_type<Soa>*>(&static_cast<table_ref>(tbl)), start, count }
		{
			SOAGEN_CONSTEXPR_SAFE_ASSERT(start < tbl.size());
			SOAGEN_CONSTEXPR_SAFE_ASSERT(start + count <= tbl.size());
		}

		/// @endcond

	  public:
		/// @brief Default constructor.
		SOAGEN_NODISCARD_CTOR
		constexpr span() noexcept //
			: base{}
		{}

		/// @brief Copy constructor.
		SOAGEN_NODISCARD_CTOR
		span(const span&) = default;

		/// @brief Copy-assignment operator.
		span& operator=(const span&) = default;

		/// @brief Constructs a span for some part of a SoA container.
		SOAGEN_NODISCARD_CTOR
		constexpr span(soa_ref soa, size_type start, size_type count = static_cast<size_type>(-1)) noexcept //
			: span{ static_cast<table_ref>(static_cast<soa_ref>(soa)),
					start,
					detail::calc_span_count(start, soa.size(), count),
					std::true_type{} }
		{}

		/// @brief Constructs a span for an entire SoA container.
		SOAGEN_NODISCARD_CTOR
		explicit constexpr span(soa_ref soa) noexcept //
			: span{ static_cast<table_ref>(static_cast<soa_ref>(soa)), 0u, soa.size(), std::true_type{} }
		{}

		/// @name Size
		/// @{

		/// @brief Returns the number of rows viewed by the span.
		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type size() const noexcept
		{
			return base::count;
		}

		/// @brief Returns true if the number of rows viewed by the span is zero.
		SOAGEN_PURE_INLINE_GETTER
		constexpr bool empty() const noexcept
		{
			return !size();
		}

		/// @}

		/// @name Columns
		/// @{

		/// @brief Returns a pointer to the elements of a specific column.
		template <auto Column>
		SOAGEN_COLUMN(span, Column)
		constexpr auto* column() const noexcept
		{
			return static_cast<table_ref>(*base::table).template column<Column>() + base::start;
		}

#if SOAGEN_DOXYGEN
		/// @brief Invokes a function once for each column data pointer (const overload).
		///
		/// @tparam Func A callable type compatible with one of the following signatures:<ul>
		/// <li> `void(auto*, std::integral_constant<size_type, N>)`
		/// <li> `void(auto*, size_type)`
		/// <li> `void(std::integral_constant<size_type, N>, auto*)`
		/// <li> `void(size_type, auto*)`
		/// <li> `void(auto*)`
		/// </ul>
		/// Overload resolution is performed in the order listed above.
		///
		/// @param func The callable to invoke.
		template <typename Func>
		constexpr void for_each_column(Func&& func) const noexcept(...);
#endif

		/// @}

		/// @name Rows
		/// @{

	  private:
		/// @cond
		template <size_t... Cols, size_t... Columns>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Soa, Cols...> row(size_type index, std::index_sequence<Columns...>) const noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { { static_cast<value_ref<Soa, Cols>>(
					static_cast<table_ref>(*base::table).template column<Cols>()[base::start + index]) }... };
			}
			else
			{
				return { { static_cast<value_ref<Soa, Columns>>(
					static_cast<table_ref>(*base::table).template column<Columns>()[base::start + index]) }... };
			}
		}
		/// @endcond

	  public:
		/// @brief Returns the row at the given index.
		///
		/// @tparam Cols Indices of the columns to include in the row. Leave the list empty for all columns.
		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Soa, Cols...> row(size_type index) const noexcept
		{
			return row<static_cast<size_t>(Cols)...>(index, column_indices<Soa>{});
		}

		/// @brief Returns the row at the given index.
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type operator[](size_type index) const noexcept
		{
			return row(index);
		}

		/// @brief Returns the row at the given index.
		///
		/// @throws std::out_of_range
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type at(size_type index) const
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= size())
				throw std::out_of_range{ "bad element access" };
#endif
			return row(index);
		}

		/// @brief Returns the first row viewed by the span.
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type front() const noexcept
		{
			return row(0u);
		}

		/// @brief Returns the last row viewed by the span.
		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type back() const noexcept
		{
			return row(size() - 1u);
		}

		/// @}

		/// @name Iterators
		/// @{

		/// @brief Returns an iterator to the first row viewed by the span.
		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::iterator_type<Soa, Cols...> begin() const noexcept
		{
			return { static_cast<table_ref>(*base::table),
					 static_cast<difference_type>(base::start),
					 std::true_type{} };
		}

		/// @brief Returns an iterator to one-past-the-last row viewed by the span.
		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::iterator_type<Soa, Cols...> end() const noexcept
		{
			return { static_cast<table_ref>(*base::table),
					 static_cast<difference_type>(base::start + base::count),
					 std::true_type{} };
		}

		/// @brief Returns a const iterator to the first row viewed by the span.
		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::const_iterator_type<Soa, Cols...> cbegin() const noexcept
		{
			return { static_cast<table_ref>(*base::table),
					 static_cast<difference_type>(base::start),
					 std::true_type{} };
		}

		/// @brief Returns a const iterator to one-past-the-last row viewed by the span.
		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr soagen::const_iterator_type<Soa, Cols...> cend() const noexcept
		{
			return { static_cast<table_ref>(*base::table),
					 static_cast<difference_type>(base::start + base::count),
					 std::true_type{} };
		}

		/// @}

		/// @name Spans
		/// @{

		/// @brief Returns a subspan of this span.
		SOAGEN_PURE_INLINE_GETTER
		span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) const noexcept
		{
			return { static_cast<table_ref>(*base::table),
					 base::start + start,
					 detail::calc_span_count(start, base::count, count),
					 std::true_type{} };
		}

		/// @brief Returns a const-qualified subspan of this span.
		SOAGEN_PURE_INLINE_GETTER
		const_span_type const_subspan(size_type start, size_type count = static_cast<size_type>(-1)) const noexcept
		{
			return { static_cast<table_ref>(*base::table),
					 base::start + start,
					 detail::calc_span_count(start, base::count, count),
					 std::true_type{} };
		}

		/// @}

		/// @name Conversion
		/// @{

		/// @brief Converts between different spans for the same SoA type.
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
		/// @attention 	There are no conversions which offer the equivalent of a `const_cast`-by-proxy.
		///				This is by design.
		SOAGEN_CONSTRAINED_TEMPLATE((detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && !detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
									typename T)
		SOAGEN_PURE_INLINE_GETTER
		constexpr operator span<T>() const noexcept
		{
			return span<T>{ static_cast<const base&>(*this) };
		}

		/// @cond

		SOAGEN_CONSTRAINED_TEMPLATE((!detail::implicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>
									 && detail::explicit_conversion_ok<coerce_ref<Soa>, coerce_ref<T>>),
									typename T)
		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator span<T>() const noexcept
		{
			return span<T>{ static_cast<const base&>(*this) };
		}

		/// @endcond

		/// @}
	};
}

#include "header_end.hpp"
