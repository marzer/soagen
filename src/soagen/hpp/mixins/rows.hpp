//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../row.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
	template <typename Derived, typename...>
	struct SOAGEN_EMPTY_BASES rows;

	template <typename Derived, size_t... Columns>
	struct SOAGEN_EMPTY_BASES rows<Derived, std::index_sequence<Columns...>>
	{
		static_assert(!is_cvref<Derived>);
		static_assert(std::is_same_v<column_indices<Derived>, std::index_sequence<Columns...>>);

		using size_type		  = std::size_t;
		using row_type		  = soagen::row_type<Derived>;
		using rvalue_row_type = soagen::row_type<Derived&&>;
		using const_row_type  = soagen::const_row_type<Derived>;

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived, Cols...> row(size_type index) & noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { static_cast<value_ref<Derived, Cols>>(
					static_cast<Derived&>(*this).template column<static_cast<size_type>(Cols)>()[index])... };
			}
			else
			{
				return { static_cast<value_ref<Derived, Columns>>(
					static_cast<Derived&>(*this).template column<static_cast<size_type>(Columns)>()[index])... };
			}
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::row_type<Derived&&, Cols...> row(size_type index) && noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { static_cast<value_ref<Derived&&, Cols>>(
					static_cast<Derived&&>(*this).template column<static_cast<size_type>(Cols)>()[index])... };
			}
			else
			{
				return { static_cast<value_ref<Derived&&, Columns>>(
					static_cast<Derived&&>(*this).template column<static_cast<size_type>(Columns)>()[index])... };
			}
		}

		template <auto... Cols>
		SOAGEN_PURE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		soagen::const_row_type<Derived, Cols...> row(size_type index) const& noexcept
		{
			if constexpr (sizeof...(Cols))
			{
				return { static_cast<value_ref<const Derived, Cols>>(
					static_cast<const Derived&>(*this).template column<static_cast<size_type>(Cols)>()[index])... };
			}
			else
			{
				return { static_cast<value_ref<const Derived, Columns>>(
					static_cast<const Derived&>(*this)
						.template column<static_cast<size_type>(Columns)>()[index])... };
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
	struct SOAGEN_EMPTY_BASES rows<Derived> : rows<Derived, column_indices<Derived>>
	{};
}

#include "../header_end.hpp"
