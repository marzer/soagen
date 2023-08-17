//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../invoke.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES iterators
	{
		static_assert(!is_cvref<Derived>);

		using difference_type = std::ptrdiff_t;
		using iterator		  = soagen::iterator_type<Derived>;
		using rvalue_iterator = soagen::iterator_type<Derived&&>;
		using const_iterator  = soagen::const_iterator_type<Derived>;

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived, Cols...> begin() & noexcept
		{
			return { static_cast<Derived&>(*this), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived, Cols...> end() & noexcept
		{
			return { static_cast<Derived&>(*this),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived&&, Cols...> begin() && noexcept
		{
			return { static_cast<Derived&&>(*this), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr iterator_type<Derived&&, Cols...> end() && noexcept
		{
			return { static_cast<Derived&&>(*this),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> begin() const& noexcept
		{
			return { static_cast<const Derived&>(*this), 0 };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> end() const& noexcept
		{
			return { static_cast<const Derived&>(*this),
					 static_cast<difference_type>(static_cast<const Derived&>(*this).size()) };
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> cbegin() const noexcept
		{
			return begin<Cols...>();
		}

		template <auto... Cols>
		SOAGEN_PURE_INLINE_GETTER
		constexpr const_iterator_type<Derived, Cols...> cend() const noexcept
		{
			return end<Cols...>();
		}
	};
}

#include "../header_end.hpp"
