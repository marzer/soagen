//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "column_traits.hpp"
#include "names.hpp"
#include "row.hpp"
#include "iterator.hpp"
#include "invoke.hpp"
#include "header_start.hpp"

namespace soagen::mixins
{
	//--- size and capacity --------------------------------------------------------------------------------------------

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES size_and_capacity
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;
		using size_type	 = std::size_t;

		SOAGEN_PURE_INLINE_GETTER
		constexpr bool empty() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).empty();
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type size() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).size();
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type max_size() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).max_size();
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type allocation_size() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).allocation_size();
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& reserve(size_type new_cap) //
			noexcept(noexcept(std::declval<table_type&>().reserve(size_type{})))
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).reserve(new_cap);
			return static_cast<Derived&>(*this);
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_type capacity() const noexcept
		{
			return static_cast<const table_type&>(static_cast<const Derived&>(*this)).capacity();
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& shrink_to_fit() //
			noexcept(noexcept(std::declval<table_type&>().shrink_to_fit()))
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).shrink_to_fit();
			return static_cast<Derived&>(*this);
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& pop_back(size_type num = 1) //
			noexcept(noexcept(std::declval<table_type&>().pop_back(size_type{})))
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).pop_back(num);
			return static_cast<Derived&>(*this);
		}

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		Derived& clear() noexcept
		{
			static_cast<table_type&>(static_cast<Derived&>(*this)).clear();
			return static_cast<Derived&>(*this);
		}
	};

	//--- resize() -----------------------------------------------------------------------------------------------------

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

	//--- swap() -------------------------------------------------------------------------------------------------------

	template <typename Derived, bool = has_swap_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES swappable
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		void swap(Derived& other) //
			noexcept(has_nothrow_swap_member<table_type>)
		{
			return static_cast<table_type&>(static_cast<Derived&>(*this)).swap(static_cast<table_type&>(other));
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES swappable<Derived, false>
	{};

	//--- equality-comparable-------------------------------------------------------------------------------------------

	template <typename Derived, bool = is_equality_comparable<const table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES equality_comparable
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator==(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_equality_comparable<table_type>)
		{
			return static_cast<const table_type&>(lhs) == static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_equality_comparable<table_type>)
		{
			return static_cast<const table_type&>(lhs) != static_cast<const table_type&>(rhs);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES equality_comparable<Derived, false>
	{};

	//--- less-than-comparable-------------------------------------------------------------------------------------------

	template <typename Derived, bool = is_less_than_comparable<const table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES less_than_comparable
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<table_type>)
		{
			return static_cast<const table_type&>(lhs) < static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<=(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<table_type>)
		{
			return static_cast<const table_type&>(lhs) <= static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<table_type>)
		{
			return static_cast<const table_type&>(lhs) > static_cast<const table_type&>(rhs);
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>=(const Derived& lhs, const Derived& rhs) //
			noexcept(is_nothrow_less_than_comparable<table_type>)
		{
			return static_cast<const table_type&>(lhs) >= static_cast<const table_type&>(rhs);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES less_than_comparable<Derived, false>
	{};

	//--- data() -------------------------------------------------------------------------------------------------------

	template <typename Derived, bool = has_data_member<table_type<Derived>>>
	struct SOAGEN_EMPTY_BASES data_ptr
	{
		static_assert(!is_cvref<Derived>);

		using table_type = soagen::table_type<Derived>;

		SOAGEN_COLUMN(table_type, 0)
		constexpr std::byte* data() //
			noexcept(has_nothrow_data_member<table_type>)
		{
			return soagen::assume_aligned<actual_alignment<table_type, 0>>(
				static_cast<table_type&>(static_cast<Derived&>(*this)).data());
		}

		SOAGEN_COLUMN(table_type, 0)
		constexpr const std::byte* data() const //
			noexcept(has_nothrow_data_member<const table_type>)
		{
			return soagen::assume_aligned<actual_alignment<table_type, 0>>(
				static_cast<const table_type&>(static_cast<const Derived&>(*this)).data());
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES data_ptr<Derived, false>
	{};

	//--- columns ------------------------------------------------------------------------------------------------------

	template <typename Derived, typename...>
	struct SOAGEN_EMPTY_BASES columns;

	template <typename Derived, size_t... Columns>
	struct SOAGEN_EMPTY_BASES columns<Derived, std::index_sequence<Columns...>>
		: detail::column_ptr<remove_cvref<Derived>, Columns>...
	{
		static_assert(!is_cvref<Derived>);

		using table_type   = soagen::table_type<Derived>;
		using table_traits = soagen::table_traits_type<Derived>;
		using size_type	   = std::size_t;

		template <typename Func>
		constexpr void for_each_column(Func&& func) //
			noexcept(table_traits::template for_each_column_nothrow_invocable<Func&&>)
		{
			(invoke_with_optional_index<Columns>(static_cast<Func&&>(func),
												 static_cast<Derived&>(*this).template column<Columns>()),
			 ...);
		}

		template <typename Func>
		constexpr void for_each_column(Func&& func) const //
			noexcept(table_traits::template for_each_column_nothrow_invocable<Func&&, true>)
		{
			(invoke_with_optional_index<Columns>(static_cast<Func&&>(func),
												 static_cast<const Derived&>(*this).template column<Columns>()),
			 ...);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES columns<Derived>
		: columns<remove_cvref<Derived>,
				  std::make_index_sequence<table_traits_type<remove_cvref<Derived>>::column_count>>
	{};

	//--- rows ---------------------------------------------------------------------------------------------------------

	template <typename Derived, typename...>
	struct SOAGEN_EMPTY_BASES rows;

	template <typename Derived, size_t... Columns>
	struct SOAGEN_EMPTY_BASES rows<Derived, std::index_sequence<Columns...>>
	{
		static_assert(!is_cvref<Derived>);

		using table_type	  = soagen::table_type<Derived>;
		using size_type		  = std::size_t;
		using row_type		  = soagen::row_type<Derived>;
		using const_row_type  = soagen::row_type<const Derived>;
		using rvalue_row_type = soagen::row_type<Derived&&>;

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived, Cols...> row(size_type index) & noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { { static_cast<Derived&>(*this).template column<static_cast<size_type>(Cols)>()[index] }... };
			}
			else
			{
				return {
					{ static_cast<Derived&>(*this).template column<static_cast<size_type>(Columns)>()[index] }...
				};
			}
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived&&, Cols...> row(size_type index) && noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { { std::move(
					static_cast<Derived&>(*this).template column<static_cast<size_type>(Cols)>()[index]) }... };
			}
			else
			{
				return { { std::move(
					static_cast<Derived&>(*this).template column<static_cast<size_type>(Columns)>()[index]) }... };
			}
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<const Derived, Cols...> row(size_type index) const& noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return {
					{ static_cast<const Derived&>(*this).template column<static_cast<size_type>(Cols)>()[index] }...
				};
			}
			else
			{
				return {
					{ static_cast<const Derived&>(*this).template column<static_cast<size_type>(Columns)>()[index] }...
				};
			}
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type operator[](size_type index) & noexcept
		{
			return row(index);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		rvalue_row_type operator[](size_type index) && noexcept
		{
			return static_cast<rows&&>(*this).row(index);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		const_row_type operator[](size_type index) const& noexcept
		{
			return row(index);
		}

		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type at(size_type index) &
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= static_cast<const Derived&>(*this).size())
				throw std::out_of_range{ "bad element access" };
#endif
			return row(index);
		}

		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		rvalue_row_type at(size_type index) &&
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= static_cast<const Derived&>(*this).size())
				throw std::out_of_range{ "bad element access" };
#endif
			return static_cast<rows&&>(*this).row(index);
		}

		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		const_row_type at(size_type index) const&
		{
#if SOAGEN_HAS_EXCEPTIONS
			if (index >= static_cast<const Derived&>(*this).size())
				throw std::out_of_range{ "bad element access" };
#endif
			return row(index);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type front() & noexcept
		{
			return row(0u);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		row_type back() & noexcept
		{
			return row(static_cast<const Derived&>(*this).size() - 1u);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		rvalue_row_type front() && noexcept
		{
			return static_cast<rows&&>(*this).row(0u);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		rvalue_row_type back() && noexcept
		{
			return static_cast<rows&&>(*this).row(static_cast<const Derived&>(*this).size() - 1u);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		const_row_type front() const& noexcept
		{
			return row(0u);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		const_row_type back() const& noexcept
		{
			return row(static_cast<const Derived&>(*this).size() - 1u);
		}
	};

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES rows<Derived>
		: rows<remove_cvref<Derived>, std::make_index_sequence<table_traits_type<remove_cvref<Derived>>::column_count>>
	{};

	//--- iterators ----------------------------------------------------------------------------------------------------

	template <typename Derived>
	struct SOAGEN_EMPTY_BASES iterators
	{
		static_assert(!is_cvref<Derived>);

		using table_type	  = soagen::table_type<Derived>;
		using difference_type = std::ptrdiff_t;
		using iterator		  = iterator_type<Derived>;
		using const_iterator  = iterator_type<const Derived>;
		using rvalue_iterator = iterator_type<Derived&&>;

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived, Cols...> begin() & noexcept
		{
			return { static_cast<table_type&>(static_cast<Derived&>(*this)), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived, Cols...> end() & noexcept
		{
			return { static_cast<table_type&>(static_cast<Derived&>(*this)),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived&&, Cols...> begin() && noexcept
		{
			return { static_cast<table_type&&>(static_cast<Derived&&>(*this)), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived&&, Cols...> end() && noexcept
		{
			return { static_cast<table_type&&>(static_cast<Derived&&>(*this)),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<const Derived, Cols...> begin() const& noexcept
		{
			return { static_cast<const table_type&>(static_cast<const Derived&>(*this)), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<const Derived, Cols...> end() const& noexcept
		{
			return { static_cast<const table_type&>(static_cast<const Derived&>(*this)),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<const Derived, Cols...> cbegin() const noexcept
		{
			return begin<Cols...>();
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<const Derived, Cols...> cend() const noexcept
		{
			return end<Cols...>();
		}
	};
}

#include "header_end.hpp"
