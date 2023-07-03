//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"
#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

/// \cond
namespace soagen::detail
{
	// a base class for the column traits that handles all the non-alignment-dependent stuff
	// (to minimize template instantiation explosion)
	template <typename StorageType>
	struct column_traits_base
	{
		using storage_type = StorageType;
		static_assert(!is_cvref<storage_type>, "column storage_type may not be cvref-qualified");
		static_assert(!std::is_void_v<storage_type>, "column storage_type may not be void");
		static_assert(std::is_destructible_v<storage_type>, "column storage_type must be destructible");

		//--- dereferencing --------------------------------------------------------------------------------------------

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& get(std::byte* ptr) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);

			return *SOAGEN_LAUNDER(reinterpret_cast<storage_type*>(soagen::assume_aligned<alignof(storage_type)>(ptr)));
		}

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr const storage_type& get(const std::byte* ptr) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);

			return *SOAGEN_LAUNDER(
				reinterpret_cast<const storage_type*>(soagen::assume_aligned<alignof(storage_type)>(ptr)));
		}

		//--- construction ---------------------------------------------------------------------------------------------

		template <typename... Args>
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct(std::byte* destination, Args&&... args) //
			noexcept(std::is_nothrow_constructible_v<storage_type, Args&&...>)
		{
			SOAGEN_ASSUME(destination != nullptr);

			if constexpr (std::is_aggregate_v<storage_type>)
			{
				return *(::new (static_cast<void*>(soagen::assume_aligned<alignof(storage_type)>(destination)))
							 storage_type{ static_cast<Args&&>(args)... });
			}
			else
			{
				return *(::new (static_cast<void*>(soagen::assume_aligned<alignof(storage_type)>(destination)))
							 storage_type(static_cast<Args&&>(args)...));
			}
		}

		template <typename... Args>
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct_at(std::byte* buffer, size_t element_index, Args&&... args) //
			noexcept(std::is_nothrow_constructible_v<storage_type, Args&&...>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			return construct(buffer + element_index * sizeof(storage_type), static_cast<Args&&>(args)...);
		}

		//--- move-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_move_constructible =
			std::is_move_constructible_v<storage_type>
			|| (std::is_default_constructible_v<storage_type> && std::is_move_assignable_v<storage_type>);

		static constexpr bool is_nothrow_move_constructible = std::is_move_constructible_v<storage_type>
																? std::is_nothrow_move_constructible_v<storage_type>
																: (std::is_nothrow_default_constructible_v<storage_type>
																   && std::is_nothrow_move_assignable_v<storage_type>);

		static constexpr bool is_trivially_move_constructible =
			std::is_move_constructible_v<storage_type> ? std::is_trivially_move_constructible_v<storage_type>
													   : (std::is_trivially_default_constructible_v<storage_type>
														  && std::is_trivially_move_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_move_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<storage_type&&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_move_assignable_v<storage_type>);

				construct(destination);

				if constexpr (std::is_nothrow_move_assignable_v<storage_type>)
				{
					return move_assign(destination, source);
				}
				else
				{
					try
					{
						return move_assign(destination, source);
					}
					catch (...)
					{
						destruct(destination);
						throw;
					}
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_move_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& move_construct_at(std::byte* dest_buffer,
														 size_t dest_element_index,
														 std::byte* source_buffer,
														 size_t source_element_index) //
			noexcept(std::is_nothrow_move_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_construct(dest_buffer + dest_element_index * sizeof(storage_type),
								  source_buffer + source_element_index * sizeof(storage_type));
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_copy_constructible =
			std::is_copy_constructible_v<storage_type>
			|| (std::is_default_constructible_v<storage_type> && std::is_copy_assignable_v<storage_type>);

		static constexpr bool is_nothrow_copy_constructible = std::is_copy_constructible_v<storage_type>
																? std::is_nothrow_copy_constructible_v<storage_type>
																: (std::is_nothrow_default_constructible_v<storage_type>
																   && std::is_nothrow_copy_assignable_v<storage_type>);

		static constexpr bool is_trivially_copy_constructible =
			std::is_copy_constructible_v<storage_type> ? std::is_trivially_copy_constructible_v<storage_type>
													   : (std::is_trivially_default_constructible_v<storage_type>
														  && std::is_trivially_copy_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_copy_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<const storage_type&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_copy_assignable_v<storage_type>);

				construct(destination);

				if constexpr (std::is_nothrow_copy_assignable_v<storage_type>)
				{
					return copy_assign(destination, source);
				}
				else
				{
					try
					{
						return copy_assign(destination, source);
					}
					catch (...)
					{
						destruct(destination);
						throw;
					}
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_constructible)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& copy_construct_at(std::byte* dest_buffer,
														 size_t dest_element_index,
														 std::byte* source_buffer,
														 size_t source_element_index) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_construct(dest_buffer + dest_element_index * sizeof(storage_type),
								  source_buffer + source_element_index * sizeof(storage_type));
		}

		//--- destruction ----------------------------------------------------------------------------------------------

		SOAGEN_ATTR(nonnull)
		static constexpr void destruct([[maybe_unused]] std::byte* target) //
			noexcept(std::is_nothrow_destructible_v<storage_type>)
		{
			SOAGEN_ASSUME(target != nullptr);

			if constexpr (!std::is_trivially_destructible_v<storage_type>)
			{
				get(target).~storage_type();
			}
		}

		SOAGEN_ATTR(nonnull)
		static constexpr void destruct_at([[maybe_unused]] std::byte* buffer,	 //
										  [[maybe_unused]] size_t element_index) //
			noexcept(std::is_nothrow_destructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			if constexpr (!std::is_trivially_destructible_v<storage_type>)
			{
				destruct(buffer + element_index * sizeof(storage_type));
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_move_assignable =
			std::is_move_assignable_v<storage_type>
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_move_constructible_v<storage_type>);

		static constexpr bool is_nothrow_move_assignable =
			std::is_move_assignable_v<storage_type>
				? std::is_nothrow_move_assignable_v<storage_type>
				: (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_move_constructible_v<storage_type>);

		static constexpr bool is_trivially_move_assignable =
			std::is_move_assignable_v<storage_type> ? std::is_trivially_move_assignable_v<storage_type>
													: (std::is_trivially_destructible_v<storage_type>
													   && std::is_trivially_move_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& move_assign(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_move_assignable_v<storage_type>)
			{
				return get(destination) = static_cast<storage_type&&>(get(source));
			}
			else
			{
				// note we only fallback to this if they're nothrow because we don't want to leave the destination
				// in a half-constructed state (it existed before the assignment, it should still exist after)
				static_assert(std::is_nothrow_destructible_v<storage_type>);
				static_assert(std::is_nothrow_move_constructible_v<storage_type>);

				destruct(destination);
				return move_construct(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& move_assign_at(std::byte* dest_buffer,
													  size_t dest_element_index,
													  std::byte* source_buffer,
													  size_t source_element_index) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_assign(dest_buffer + dest_element_index * sizeof(storage_type),
							   source_buffer + source_element_index * sizeof(storage_type));
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_copy_assignable =
			std::is_copy_assignable_v<storage_type>
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_copy_constructible_v<storage_type>);

		static constexpr bool is_nothrow_copy_assignable =
			std::is_copy_assignable_v<storage_type>
				? std::is_nothrow_copy_assignable_v<storage_type>
				: (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_copy_constructible_v<storage_type>);

		static constexpr bool is_trivially_copy_assignable =
			std::is_copy_assignable_v<storage_type> ? std::is_trivially_copy_assignable_v<storage_type>
													: (std::is_trivially_destructible_v<storage_type>
													   && std::is_trivially_copy_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& copy_assign(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);

			if constexpr (std::is_copy_assignable_v<storage_type>)
			{
				return get(destination) = static_cast<const storage_type&>(get(source));
			}
			else
			{
				// note we only fallback to this if they're nothrow because we don't want to leave the destination
				// in a half-constructed state (it existed before the assignment, it should still exist after)
				static_assert(std::is_nothrow_destructible_v<storage_type>);
				static_assert(std::is_nothrow_move_constructible_v<storage_type>);

				destruct(destination);
				return copy_construct(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_assignable)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& copy_assign_at(std::byte* dest_buffer,
													  size_t dest_element_index,
													  std::byte* source_buffer,
													  size_t source_element_index) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_assign(dest_buffer + dest_element_index * sizeof(storage_type),
							   source_buffer + source_element_index * sizeof(storage_type));
		}
	};

	// trait for determining the actual storage type for a column.
	// we can strip off const/volatile and coerce all pointers to be void* to reduce template instantiation burden
	template <typename ValueType>
	struct storage_type_
	{
		using type = ValueType;
	};
	template <typename T>
	struct storage_type_<T*>
	{
		using type = void*;
	};
	template <typename T>
	struct storage_type_<const T*> : public storage_type_<T*>
	{};
	template <typename T>
	struct storage_type_<volatile T*> : public storage_type_<T*>
	{};
	template <typename T>
	struct storage_type_<const volatile T*> : public storage_type_<T*>
	{};
	template <typename T>
	struct storage_type_<const T> : public storage_type_<T>
	{};
	template <typename T>
	struct storage_type_<volatile T> : public storage_type_<T>
	{};
	template <typename T>
	struct storage_type_<const volatile T> : public storage_type_<T>
	{};
	template <typename ValueType>
	using storage_type = typename detail::storage_type_<ValueType>::type;

	// trait for determining the default parameter type for a column.
	// ideally we want to pass small+fast things by value, move-only things by rvalue,
	// and everything else as const lvalue.
	template <typename ValueType,
			  bool Value = std::is_scalar_v<ValueType>		//
						|| std::is_fundamental_v<ValueType> //
						|| (std::is_trivially_copyable_v<ValueType> && sizeof(ValueType) <= sizeof(void*) * 2),
			  bool Move = !std::is_copy_constructible_v<ValueType> && std::is_move_constructible_v<ValueType>>
	struct param_type_
	{
		using type = ValueType;
	};
	template <typename ValueType>
	struct param_type_<ValueType, false, true>
	{
		using type = std::add_rvalue_reference_t<ValueType>;
	};
	template <typename ValueType>
	struct param_type_<ValueType, false, false>
	{
		using type = std::add_lvalue_reference_t<std::add_const_t<ValueType>>;
	};
}
/// \endcond

namespace soagen
{
	template <typename ValueType>
	using param_type = typename detail::param_type_<ValueType>::type;

	template <typename ValueType,
			  typename ParamType = param_type<ValueType>,
			  size_t Align		 = alignof(ValueType),
			  typename Base		 = detail::column_traits_base<detail::storage_type<ValueType>>>
	struct column_traits : public Base
	{
		using value_type = ValueType;
		static_assert(!std::is_reference_v<value_type>, "column value_type may not be a reference");
		static_assert(!std::is_void_v<value_type>, "column value_type may not be void");
		static_assert(alignof(value_type) == alignof(typename Base::storage_type));
		static_assert(sizeof(value_type) == sizeof(typename Base::storage_type));

		using param_type = ParamType;
		static_assert(!std::is_void_v<param_type>, "column param_type may not be void");
		static_assert(std::is_convertible_v<param_type, value_type> || std::is_constructible_v<value_type, param_type>
						  || (std::is_pointer_v<param_type> && std::is_same_v<typename Base::storage_type, void*>),
					  "column value_type must be constructible or convertible from param_type");

		static constexpr size_t alignment = max(Align, alignof(value_type));
		static_assert(has_single_bit(alignment), "column alignment must be a power of two");

		static constexpr size_t max_capacity = static_cast<size_t>(-1) / sizeof(value_type);

		static constexpr size_t aligned_stride = lcm(alignment, sizeof(value_type)) / sizeof(value_type);
	};

	template <typename>
	inline constexpr bool is_column_traits = false;

	template <typename StorageType>
	inline constexpr bool is_column_traits<detail::column_traits_base<StorageType>> = true;

	template <typename ValueType, typename ParamType, size_t Align, typename Base>
	inline constexpr bool is_column_traits<column_traits<ValueType, ParamType, Align, Base>> = is_column_traits<Base>;

}

/// \cond
namespace soagen::detail
{
	template <typename T>
	struct to_base_traits_;

	template <typename ValueType, typename ParamType, size_t Align>
	struct to_base_traits_<column_traits<ValueType, ParamType, Align>>
	{
		using type = column_traits_base<ValueType>;
	};

	template <typename T>
	using to_base_traits = typename to_base_traits_<T>::type;
}
/// \endcond

#include "header_end.hpp"
