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
		struct SOAGEN_EMPTY_BASES named_ptr<name_tag_##Name, Derived, Column>                                          \
		{                                                                                                              \
			SOAGEN_COLUMN(Derived, Column)                                                                             \
			constexpr auto* Name() noexcept                                                                            \
			{                                                                                                          \
				return static_cast<Derived&>(*this).template column<Column>();                                         \
			}                                                                                                          \
                                                                                                                       \
			SOAGEN_COLUMN(Derived, Column)                                                                             \
			constexpr const auto* Name() const noexcept                                                                \
			{                                                                                                          \
				return static_cast<const Derived&>(*this).template column<Column>();                                   \
			}                                                                                                          \
		}
#endif

#ifndef SOAGEN_MAKE_NAMED_COLUMN
	#define SOAGEN_MAKE_NAMED_COLUMN(Table, Column, Name)                                                              \
                                                                                                                       \
		template <>                                                                                                    \
		struct SOAGEN_EMPTY_BASES column_name_tag_<Table, Column>                                                      \
		{                                                                                                              \
			using type = name_tag_##Name;                                                                              \
		}

#endif

/// @cond
namespace soagen::detail
{
	template <typename Tag>
	struct name_constant;

	template <typename Tag, typename>
	struct named_ref;

	template <typename Tag, typename, size_t>
	struct SOAGEN_EMPTY_BASES named_ptr
	{};

	//--- column names ---------

	template <typename Table, size_t Column>
	struct column_name_tag_
	{
		using type = void; // unnamed columns
	};
	template <typename Table, auto Column>
	using column_name_tag = typename column_name_tag_<remove_cvref<Table>, static_cast<size_t>(Column)>::type;

	template <typename Table, auto Column>
	using column_name = name_constant<column_name_tag<Table, Column>>;

	//--- named column references ---------

	template <typename Table, size_t Column>
	struct SOAGEN_EMPTY_BASES column_ref
		: named_ref<column_name_tag<Table, Column>, std::add_lvalue_reference_t<value_type<Table, Column>>>
	{};

	template <typename Table, size_t Column>
	struct SOAGEN_EMPTY_BASES column_ref<Table&&, Column>
		: named_ref<column_name_tag<Table, Column>, std::add_rvalue_reference_t<value_type<Table, Column>>>
	{};

	template <typename Table, size_t Column>
	struct SOAGEN_EMPTY_BASES column_ref<const Table, Column>
		: named_ref<column_name_tag<Table, Column>,
					std::add_lvalue_reference_t<std::add_const_t<value_type<Table, Column>>>>
	{};

	template <typename Table, size_t Column>
	struct SOAGEN_EMPTY_BASES column_ref<const Table&&, Column>
		: named_ref<column_name_tag<Table, Column>,
					std::add_rvalue_reference_t<std::add_const_t<value_type<Table, Column>>>>
	{};

	//--- named column pointer functions ---------

	template <typename Table, size_t Column>
	struct SOAGEN_EMPTY_BASES column_ptr //
		: named_ptr<column_name_tag<Table, Column>, remove_cvref<Table>, Column>
	{};

}
/// @endcond

#include "header_end.hpp"
