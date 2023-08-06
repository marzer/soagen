//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @brief Helper class for #table::emplace() and #table::emplace_back().
	template <typename... Args>
	struct emplacer
	{
		static_assert(sizeof...(Args));
		static_assert((std::is_reference_v<Args> && ...));

		void* ptrs[sizeof...(Args)];

		SOAGEN_DEFAULT_RULE_OF_FIVE(emplacer);

		SOAGEN_NODISCARD_CTOR
		constexpr emplacer(Args&&... args) noexcept //
			: ptrs{ const_cast<void*>(static_cast<const volatile void*>(&args))... }
		{}
	};

	/// @cond
	template <>
	struct emplacer<>
	{};

	template <typename... Args>
	emplacer(Args&&...) -> emplacer<Args&&...>;
	/// @endcond

	/// @brief True if `T` is an instance of #soagen::emplacer.
	template <typename T>
	inline constexpr bool is_emplacer = POXY_IMPLEMENTATION_DETAIL(false);

	/// @cond
	template <typename... T>
	inline constexpr bool is_emplacer<emplacer<T...>> = true;
	template <typename T>
	inline constexpr bool is_emplacer<const T> = is_emplacer<T>;
	template <typename T>
	inline constexpr bool is_emplacer<volatile T> = is_emplacer<T>;
	template <typename T>
	inline constexpr bool is_emplacer<const volatile T> = is_emplacer<T>;
	/// @endcond
}

/// @cond
namespace std
{
	template <typename... Args>
	struct tuple_size<soagen::emplacer<Args...>> //
		: std::integral_constant<size_t, sizeof...(Args)>
	{};
}
/// @endcond

#include "header_end.hpp"
