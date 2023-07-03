//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "column_traits.hpp"
#include "header_start.hpp"
#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

/// \cond
namespace soagen::detail
{
	// a base class for the table traits that handles all the non-alignment-dependent stuff
	// (to minimize template instantiation explosion)
	template <typename... Columns>
	struct table_traits_base
	{
		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		// columns

		template <size_t Index>
		using column = type_at_index<Index, Columns...>;

		template <typename IntegralConstant>
		using column_from_ic = type_at_index<IntegralConstant::value, Columns...>;

		template <size_t Index>
		using column_storage_type = typename column<Index>::storage_type;

		template <typename IntegralConstant>
		using column_storage_type_from_ic = typename column_from_ic<IntegralConstant>::storage_type;

		using column_pointers		= std::byte* [column_count];
		using const_column_pointers = std::byte* const[column_count];

		static constexpr size_t column_sizes[column_count] = { sizeof(typename Columns::storage_type)... };

		// default constructibility

		static constexpr bool all_default_constructible =
			(std::is_default_constructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_default_constructible =
			(std::is_nothrow_default_constructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_trivially_default_constructible =
			(std::is_trivially_default_constructible_v<typename Columns::storage_type> && ...);

		// copy-constructibility

		static constexpr bool all_copy_constructible = (Columns::is_copy_constructible && ...);

		static constexpr bool all_nothrow_copy_constructible = (Columns::is_nothrow_copy_constructible && ...);

		static constexpr bool all_trivially_copy_constructible = (Columns::is_trivially_copy_constructible && ...);

		// move-constructibility

		static constexpr bool all_move_constructible = (Columns::is_move_constructible && ...);

		static constexpr bool all_nothrow_move_constructible = (Columns::is_nothrow_move_constructible && ...);

		static constexpr bool all_trivially_move_constructible = (Columns::is_trivially_move_constructible && ...);

		// copy-assignability

		static constexpr bool all_copy_assignable = (Columns::is_copy_assignable && ...);

		static constexpr bool all_nothrow_copy_assignable = (Columns::is_nothrow_copy_assignable && ...);

		static constexpr bool all_trivially_copy_assignable = (Columns::is_trivially_copy_assignable && ...);

		// move-assignability

		static constexpr bool all_move_assignable = (Columns::is_move_assignable && ...);

		static constexpr bool all_nothrow_move_assignable = (Columns::is_nothrow_move_assignable && ...);

		static constexpr bool all_trivially_move_assignable = (Columns::is_trivially_move_assignable && ...);

		// destructibility

		static constexpr bool all_destructible = (std::is_destructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_destructible =
			(std::is_nothrow_destructible_v<typename Columns::storage_type> && ...);

		static constexpr bool all_trivially_destructible =
			(std::is_trivially_destructible_v<typename Columns::storage_type> && ...);

		// swappability

		static constexpr bool all_swappable = (std::is_swappable_v<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_swappable =
			(std::is_nothrow_swappable_v<typename Columns::storage_type> && ...);

		// row constructibility

		template <typename... Args>
		static constexpr bool row_constructible_from =
			sizeof...(Args) == column_count && (std::is_constructible_v<typename Columns::storage_type, Args> && ...);

		template <typename... Args>
		static constexpr bool row_nothrow_constructible_from =
			sizeof...(Args) == column_count && (std::is_constructible_v<typename Columns::storage_type, Args> && ...);

		//--- destruction ----------------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static constexpr void destruct_row([[maybe_unused]] column_pointers& columns,
										   [[maybe_unused]] size_t index,
										   [[maybe_unused]] size_t leftmost_column,
										   [[maybe_unused]] size_t rightmost_column,
										   std::index_sequence<Cols...>) noexcept
		{
			static_assert(all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			if constexpr (all_trivially_destructible)
			{
				// nothing to do in this case :)
			}
			else
			{
				const auto destructor = [&](auto ic) noexcept
				{
					static constexpr size_t column_index = column_count - decltype(ic)::value - 1u;

					if constexpr (!std::is_trivially_destructible_v<column_storage_type<column_index>>)
					{
						SOAGEN_ASSUME(columns[column_index]);
						SOAGEN_ASSUME(leftmost_column <= rightmost_column);
						SOAGEN_ASSUME(leftmost_column < column_count);
						SOAGEN_ASSUME(rightmost_column < column_count);

						if (column_index >= leftmost_column && column_index <= rightmost_column)
							column<column_index>::destruct_at(columns[column_index], index);
					}
				};

				(destructor(index_tag<Cols>{}), ...);
			}
		}

		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static constexpr void destruct_row([[maybe_unused]] column_pointers& columns,
										   [[maybe_unused]] size_t index,
										   std::index_sequence<Cols...>) noexcept
		{
			static_assert(all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			if constexpr (all_trivially_destructible)
			{
				// nothing to do in this case :)
			}
			else
			{
				const auto destructor = [&](auto ic) noexcept
				{
					static constexpr size_t column_index = column_count - decltype(ic)::value - 1u;

					if constexpr (!std::is_trivially_destructible_v<column_storage_type<column_index>>)
					{
						SOAGEN_ASSUME(columns[column_index]);

						column<column_index>::destruct_at(columns[column_index], index);
					}
				};

				(destructor(index_tag<Cols>{}), ...);
			}
		}

	  public:
		SOAGEN_ALWAYS_INLINE
		static constexpr void destruct_row(column_pointers& columns,
										   size_t index,
										   size_t leftmost_column,
										   size_t rightmost_column) noexcept
		{
			destruct_row(columns, index, leftmost_column, rightmost_column, std::make_index_sequence<column_count>{});
		}

		SOAGEN_ALWAYS_INLINE
		static constexpr void destruct_row(column_pointers& columns, size_t index) noexcept
		{
			destruct_row(columns, index, std::make_index_sequence<column_count>{});
		}

		static constexpr void destruct_rows([[maybe_unused]] column_pointers& columns,
											[[maybe_unused]] size_t start,
											[[maybe_unused]] size_t count) noexcept
		{
			static_assert(all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			if constexpr (all_trivially_destructible)
			{
				// nothing to do in this case :)
			}
			else
			{
				for (size_t i = start + count; i-- > start;)
					destruct_row(columns, i);
			}
		}

		//--- default-construction -------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_row(column_pointers& columns,
										  size_t index,
										  std::index_sequence<Cols...>) //
			noexcept(all_nothrow_default_constructible)					//
			SOAGEN_REQUIRES(all_default_constructible)
		{
			static_assert(all_default_constructible);
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (all_nothrow_default_constructible || all_trivially_destructible)
			{
				(column<Cols>::construct_at(columns[Cols], index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor = [&](auto ic) //
					noexcept(std::is_nothrow_default_constructible_v<column_storage_type<decltype(ic)::value>>)
				{
					column_from_ic<decltype(ic)>::construct_at(columns[decltype(ic)::value], index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(columns, index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_default_constructible)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_row(column_pointers& columns,
										  size_t index) //
			noexcept(all_nothrow_default_constructible) //
		{
			default_construct_row(columns, index, std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_default_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_rows([[maybe_unused]] column_pointers& columns,
										   [[maybe_unused]] size_t start,
										   [[maybe_unused]] size_t count) //
			noexcept(all_nothrow_destructible)
		{
			if constexpr (all_nothrow_default_constructible)
			{
				for (size_t i = start, e = start + count; i < e; i++)
					default_construct_row(columns, i);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = start;

				try
				{
					for (const size_t e = start + count; i < e; i++)
						default_construct_row(columns, i);
				}
				catch (...)
				{
					for (; i-- > start;)
						destruct_row(columns, i);
					throw;
				}
			}
		}

		//--- construction ---------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row(column_pointers& columns,
								  size_t index,
								  std::index_sequence<Cols...>,
								  Args&&... args) //
			noexcept(row_nothrow_constructible_from<Args&&...>)
		{
			static_assert(row_constructible_from<Args&&...>);
			static_assert(sizeof...(Cols) == sizeof...(Args));
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (row_nothrow_constructible_from<Args&&...> || all_trivially_destructible)
			{
				(column<Cols>::construct_at(columns[Cols], index, static_cast<Args&&>(args)), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic, auto&& arg) noexcept(
						std::is_nothrow_constructible_v<column_storage_type_from_ic<decltype(ic)>, decltype(arg)&&>)
				{
					column_from_ic<decltype(ic)>::construct_at(columns[decltype(ic)::value],
															   index,
															   static_cast<decltype(arg)&&>(arg));

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}, static_cast<Args&&>(args)), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(columns, index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		template <typename... Args>
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row(column_pointers& columns, size_t index, Args&&... args) //
			noexcept(row_nothrow_constructible_from<Args&&...>)
		{
			construct_row(columns, index, std::make_index_sequence<column_count>{}, static_cast<Args&&>(args)...);
		}

		//--- move-construction ----------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   column_pointers& source_columns,
									   size_t source_index,
									   std::index_sequence<Cols...>) //
			noexcept(all_nothrow_move_constructible)
		{
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (all_nothrow_move_constructible || all_trivially_destructible)
			{
				(column<Cols>::move_construct_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index),
				 ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(
						std::is_nothrow_move_constructible_v<column_storage_type_from_ic<decltype(ic)>>)
				{
					column_from_ic<decltype(ic)>::move_construct_at(dest_columns[decltype(ic)::value],
																	dest_index,
																	source_columns[decltype(ic)::value],
																	source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest_columns, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   column_pointers& source_columns,
									   size_t source_index) //
			noexcept(all_nothrow_move_constructible)
		{
			move_construct_row(dest_columns,
							   dest_index,
							   source_columns,
							   source_index,
							   std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_rows(column_pointers& dest_columns,
										size_t dest_start,
										column_pointers& source_columns,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_move_constructible)
		{
			if constexpr (all_nothrow_move_constructible)
			{
				for (size_t i = 0; i < count; i++)
					move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					for (; i < count; i++)
						move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				catch (...)
				{
					for (; i-- > 0u;)
						destruct_row(dest_columns, dest_start + i);
					throw;
				}
			}
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   const_column_pointers& source_columns,
									   size_t source_index,
									   std::index_sequence<Cols...>) //
			noexcept(all_nothrow_copy_constructible)
		{
			static_assert(sizeof...(Cols) == column_count);

			if constexpr (all_nothrow_copy_constructible || all_trivially_destructible)
			{
				(column<Cols>::copy_construct_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index),
				 ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(
						std::is_nothrow_copy_constructible_v<column_storage_type_from_ic<decltype(ic)>>)
				{
					column_from_ic<decltype(ic)>::copy_construct_at(dest_columns[decltype(ic)::value],
																	dest_index,
																	source_columns[decltype(ic)::value],
																	source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_tag<Cols>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest_columns, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_row(column_pointers& dest_columns,
									   size_t dest_index,
									   const_column_pointers& source_columns,
									   size_t source_index) //
			noexcept(all_nothrow_copy_constructible)
		{
			copy_construct_row(dest_columns,
							   dest_index,
							   source_columns,
							   source_index,
							   std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_rows(column_pointers& dest_columns,
										size_t dest_start,
										const_column_pointers& source_columns,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_copy_constructible)
		{
			if constexpr (all_nothrow_copy_constructible)
			{
				for (size_t i = 0; i < count; i++)
					copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					for (; i < count; i++)
						copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				catch (...)
				{
					for (; i-- > 0u;)
						destruct_row(dest_columns, dest_start + i);
					throw;
				}
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									column_pointers& source_columns,
									size_t source_index,
									std::index_sequence<Cols...>) //
			noexcept(all_nothrow_move_assignable)
		{
			static_assert(sizeof...(Cols) == column_count);

			// todo: how to provide a strong-exception guarantee here?

			(column<Cols>::move_assign_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									column_pointers& source_columns,
									size_t source_index) //
			noexcept(all_nothrow_move_assignable)
		{
			move_assign_row(dest_columns,
							dest_index,
							source_columns,
							source_index,
							std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_rows(column_pointers& dest_columns,
									 size_t dest_start,
									 const_column_pointers& source_columns,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_move_assignable)
		{
			// todo: how to provide a strong-exception guarantee here?

			for (size_t i = 0; i < count; i++)
				move_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									const_column_pointers& source_columns,
									size_t source_index,
									std::index_sequence<Cols...>) //
			noexcept(all_nothrow_copy_assignable)
		{
			static_assert(sizeof...(Cols) == column_count);

			// todo: how to provide a strong-exception guarantee here?

			(column<Cols>::copy_assign_at(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									const_column_pointers& source_columns,
									size_t source_index) //
			noexcept(all_nothrow_copy_assignable)
		{
			copy_assign_row(dest_columns,
							dest_index,
							source_columns,
							source_index,
							std::make_index_sequence<column_count>{});
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_rows(column_pointers& dest_columns,
									 size_t dest_start,
									 const_column_pointers& source_columns,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			// todo: how to provide a strong-exception guarantee here?

			for (size_t i = 0; i < count; i++)
				copy_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
		}
	};
}
/// \endcond

namespace soagen
{
	template <typename... Columns>
	struct table_traits : public detail::table_traits_base<detail::to_base_traits<Columns>...>
	{
		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		static constexpr size_t aligned_stride = lcm(size_t{ 1 }, Columns::aligned_stride...);

		// columns
		// (note that these hide the base class typedefs - this is intentional)

		template <size_t Index>
		using column = type_at_index<Index, Columns...>;

		template <size_t Index>
		using column_type = typename column<Index>::value_type;

		template <size_t Index>
		using column_storage_type = typename column<Index>::storage_type;

		template <typename IntegralConstant>
		using column_from_ic = type_at_index<IntegralConstant::value, Columns...>;

		template <typename IntegralConstant>
		using column_type_from_ic = typename column_from_ic<IntegralConstant>::value_type;

		template <typename IntegralConstant>
		using column_storage_type_from_ic = typename column_from_ic<IntegralConstant>::storage_type;

		static constexpr size_t column_alignments[column_count] = { Columns::alignment... };

		static constexpr size_t largest_alignment = max(Columns::alignment...);
	};

	template <typename>
	inline constexpr bool is_table_traits = false;

	template <typename... Columns>
	inline constexpr bool is_table_traits<table_traits<Columns...>> = true;

	template <typename... Columns>
	inline constexpr bool is_table_traits<detail::table_traits_base<Columns...>> = true;
}

/// \cond
namespace soagen::detail
{
	template <typename... Columns>
	struct to_base_traits_<table_traits<Columns...>>
	{
		using type = table_traits_base<to_base_traits<Columns>...>;
	};
}
/// \endcond

#include "header_end.hpp"
