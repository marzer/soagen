//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"

#ifndef SOAGEN_MAKE_NAME
	#define SOAGEN_MAKE_NAME(Name)                                                                                     \
                                                                                                                       \
		struct name_tag_##Name;                                                                                        \
                                                                                                                       \
		template <>                                                                                                    \
		struct SOAGEN_EMPTY_BASES name_constant<name_tag_##Name>                                                       \
		{                                                                                                              \
			static constexpr const char value[] = #Name;                                                               \
		};                                                                                                             \
                                                                                                                       \
		template <typename T>                                                                                          \
		struct SOAGEN_EMPTY_BASES named_ref<name_tag_##Name, T>                                                        \
		{                                                                                                              \
			static_assert(std::is_reference_v<T>);                                                                     \
                                                                                                                       \
			T Name;                                                                                                    \
                                                                                                                       \
		  protected:                                                                                                   \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr T get_ref() const noexcept                                                                       \
			{                                                                                                          \
				return static_cast<T>(Name);                                                                           \
			}                                                                                                          \
		};                                                                                                             \
                                                                                                                       \
		template <typename Derived, size_t Column>                                                                     \
		struct SOAGEN_EMPTY_BASES named_func<name_tag_##Name, Derived, Column>                                         \
		{                                                                                                              \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr decltype(auto) Name() noexcept                                                                   \
			{                                                                                                          \
				using return_type = decltype(static_cast<Derived&>(*this).template column<Column>());                  \
				if constexpr (std::is_reference_v<return_type>)                                                        \
					return static_cast<return_type>(static_cast<Derived&>(*this).template column<Column>());           \
				else                                                                                                   \
					return static_cast<Derived&>(*this).template column<Column>();                                     \
			}                                                                                                          \
                                                                                                                       \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr decltype(auto) Name() const noexcept                                                             \
			{                                                                                                          \
				using return_type = decltype(static_cast<const Derived&>(*this).template column<Column>());            \
				if constexpr (std::is_reference_v<return_type>)                                                        \
					return static_cast<return_type>(static_cast<const Derived&>(*this).template column<Column>());     \
				else                                                                                                   \
					return static_cast<const Derived&>(*this).template column<Column>();                               \
			}                                                                                                          \
		}
#endif

#ifndef SOAGEN_MAKE_NAMED_COLUMN
	#define SOAGEN_MAKE_NAMED_COLUMN(Soa, Column, Name)                                                                \
                                                                                                                       \
		template <>                                                                                                    \
		struct SOAGEN_EMPTY_BASES column_name_tag_<Soa, Column>                                                        \
		{                                                                                                              \
			using type = name_tag_##Name;                                                                              \
		}
#endif

#ifndef SOAGEN_MAKE_GENERIC_NAMED_COLUMN
	#define SOAGEN_MAKE_GENERIC_NAMED_COLUMN(Column, Name)                                                             \
                                                                                                                       \
		SOAGEN_MAKE_NAME(Name);                                                                                        \
                                                                                                                       \
		template <typename Soa>                                                                                        \
		struct SOAGEN_EMPTY_BASES column_name_tag_<Soa, Column>                                                        \
		{                                                                                                              \
			using type = name_tag_##Name;                                                                              \
		}
#endif

/// @cond
namespace soagen::detail
{
	//--- name constants ---------

	template <typename Tag>
	struct name_constant;

	template <>
	struct name_constant<void>
	{
		static constexpr const char value[] = "";
	};

	//--- named ref type ---------

	template <typename Tag, typename>
	struct named_ref;

	template <typename T>
	struct SOAGEN_EMPTY_BASES named_ref<void, T> // unnamed columns
	{
		static_assert(std::is_reference_v<T>);

	  protected:
		T val_;

		SOAGEN_PURE_INLINE_GETTER
		constexpr T get_ref() const noexcept
		{
			return static_cast<T>(val_);
		}

	  public:
		template <typename Val>
		SOAGEN_NODISCARD_CTOR
		constexpr named_ref(Val&& val) noexcept //
			: val_{ static_cast<Val&&>(val) }
		{}

		SOAGEN_DEFAULT_RULE_OF_FIVE(named_ref);
	};

	//--- named func type ---------

	template <typename Tag, typename Derived, size_t>
	struct named_func;

	template <typename Derived, size_t Column>
	struct SOAGEN_EMPTY_BASES named_func<void, Derived, Column>
	{
		// unnamed columns do not contribute any functions
	};

	//--- column names ---------

	template <typename Soa, size_t Column>
	struct column_name_tag_
	{
		using type = void; // unnamed columns
	};
	template <typename Soa, auto Column>
	using column_name_tag = typename column_name_tag_<soa_type<Soa>, static_cast<size_t>(Column)>::type;

	template <typename Soa, auto Column>
	using column_name = name_constant<column_name_tag<Soa, Column>>;

	//--- generic fallback names ---------

	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(0, first);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(1, second);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(2, third);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(3, fourth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(4, fifth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(5, sixth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(6, seventh);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(7, eighth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(8, ninth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(9, tenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(10, eleventh);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(11, twelfth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(12, thirteenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(13, fourteenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(14, fifteenth);
	SOAGEN_MAKE_GENERIC_NAMED_COLUMN(15, sixteenth);

	//--- column references ---------

	template <typename Soa, size_t Column>
	struct SOAGEN_EMPTY_BASES column_ref //
		: named_ref<column_name_tag<Soa, Column>, value_ref<Soa, Column>>
	{
		static_assert(!std::is_lvalue_reference_v<Soa>);
	};

	//--- column functions ---------

	template <typename Soa, size_t Column>
	struct SOAGEN_EMPTY_BASES column_func //
		: named_func<column_name_tag<Soa, Column>, Soa, Column>
	{
		static_assert(!is_cvref<Soa>);
	};
}
/// @endcond

#include "header_end.hpp"
