//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
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
}

#include "../header_end.hpp"
