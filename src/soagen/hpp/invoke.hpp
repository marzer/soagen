//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @cond
	namespace detail
	{
		template <typename Func, typename... Args>
		struct is_invocable_ : nested_trait_<types_<Func, Args...>, std::is_invocable, std::is_nothrow_invocable>
		{
			using base = nested_trait_<types_<Func, Args...>, std::is_invocable, std::is_nothrow_invocable>;

			using is_nothrow = typename base::template nested<0>;
		};
	}
	/// @endcond

	/// @brief	True if `Func` is invocable with `Args`.
	template <typename Func, typename... Args>
	inline constexpr bool is_invocable = POXY_IMPLEMENTATION_DETAIL(detail::is_invocable_<Func, Args...>::value);

	/// @brief	True if `Func` is nothrow-invocable with `Args`.
	template <typename Func, typename... Args>
	inline constexpr bool is_nothrow_invocable =
		POXY_IMPLEMENTATION_DETAIL(detail::is_invocable_<Func, Args...>::is_nothrow::value);

	/// @cond
	namespace detail
	{
		template <size_t I, typename Func, typename Arg>
		struct is_invocable_with_optional_index_							//
			: std::disjunction<is_invocable_<Func, Arg, index_constant<I>>, //
							   is_invocable_<Func, Arg, size_t>,			//
							   is_invocable_<Func, index_constant<I>, Arg>, //
							   is_invocable_<Func, size_t, Arg>,			//
							   is_invocable_<Func, Arg>>
		{};
	}
	/// @endcond

	/// @brief	True if `Func` is invocable with `Arg` and an optional index_constant/size_t.
	template <size_t I, typename Func, typename Arg>
	inline constexpr bool is_invocable_with_optional_index =
		POXY_IMPLEMENTATION_DETAIL(detail::is_invocable_with_optional_index_<I, Func, Arg>::value);

	/// @brief	True if `Func` is nothrow-invocable with `Arg` and an optional index_constant/size_t.
	template <size_t I, typename Func, typename Arg>
	inline constexpr bool is_nothrow_invocable_with_optional_index =
		POXY_IMPLEMENTATION_DETAIL(detail::is_invocable_with_optional_index_<I, Func, Arg>::is_nothrow::value);

	template <size_t I, typename Func, typename Arg>
	SOAGEN_ALWAYS_INLINE
	constexpr decltype(auto) invoke_with_optional_index(Func&& func,
														Arg&& arg) //
		noexcept(is_nothrow_invocable_with_optional_index<I, Func&&, Arg&&>)
	{
		static_assert(is_invocable_with_optional_index<I, Func&&, Arg&&>);

		if constexpr (is_invocable<Func&&, Arg&&, index_constant<I>>)
		{
			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg), index_constant<I>{});
		}
		else if constexpr (is_invocable<Func&&, Arg&&, size_t>)
		{
			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg), I);
		}
		else if constexpr (is_invocable<Func&&, index_constant<I>, Arg&&>)
		{
			return static_cast<Func&&>(func)(index_constant<I>{}, static_cast<Arg&&>(arg));
		}
		else if constexpr (is_invocable<Func&&, size_t, Arg&&>)
		{
			return static_cast<Func&&>(func)(I, static_cast<Arg&&>(arg));
		}
		else
		{
			static_assert(is_invocable<Func&&, Arg&&>);

			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg));
		}
	}
}

#include "header_end.hpp"
