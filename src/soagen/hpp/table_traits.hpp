//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "column_traits.hpp"
#include "row.hpp"
#include "header_start.hpp"
#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

/// @cond
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

		template <typename IndexConstant>
		using column_from_ic = type_at_index<IndexConstant::value, Columns...>;

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

		// trivial-copyability (memcpy + memmove)

		static constexpr bool all_trivially_copyable = (Columns::is_trivially_copyable && ...);

		static constexpr bool any_trivially_copyable = (false || ... || Columns::is_trivially_copyable);

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

		static constexpr bool all_swappable = (Columns::is_swappable && ...);

		static constexpr bool all_nothrow_swappable = (Columns::is_nothrow_swappable && ...);

		// equality comparability

		static constexpr bool all_equality_comparable = (is_equality_comparable<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_equality_comparable =
			(is_nothrow_equality_comparable<typename Columns::storage_type> && ...);

		// less-than comparability

		static constexpr bool all_less_than_comparable =
			(is_less_than_comparable<typename Columns::storage_type> && ...);

		static constexpr bool all_nothrow_less_than_comparable =
			(is_nothrow_less_than_comparable<typename Columns::storage_type> && ...);

		// row constructibility

	  private:
		template <size_t, typename...>
		struct row_constructible_from_row_ : std::false_type
		{};
		template <typename Table, size_t... RowCols, size_t... RowMembers>
		struct row_constructible_from_row_<column_count, row<Table, RowCols...>, std::index_sequence<RowMembers...>>
			: std::bool_constant<(Columns::template is_constructible<
									  decltype(std::declval<row<Table, RowCols...>>().template get<RowMembers>())>
								  && ...)>
		{
			static_assert(sizeof...(RowCols) == column_count);
			static_assert(sizeof...(RowMembers) == column_count);
			static_assert(std::is_same_v<std::index_sequence<RowMembers...>, std::make_index_sequence<column_count>>);
		};

		template <bool, size_t, typename... Args>
		struct row_constructible_from_ : std::false_type
		{};
		template <typename... Args>
		struct row_constructible_from_<false, column_count, Args...>
			: std::bool_constant<(Columns::template is_constructible<Args> && ...)>
		{
			static_assert(sizeof...(Args) == column_count);
		};
		template <typename Row>
		struct row_constructible_from_<true, 1, Row>
			: row_constructible_from_row_<std::tuple_size_v<remove_cvref<Row>>,
										  remove_cvref<Row>,
										  std::make_index_sequence<std::tuple_size_v<remove_cvref<Row>>>>
		{};

	  public:
		template <typename... Args>
		static constexpr bool row_constructible_from =
			row_constructible_from_<(is_row<remove_cvref<Args>> && ...), sizeof...(Args), Args...>::value;

		// row constructibility (nothrow)

	  private:
		template <size_t, typename...>
		struct row_nothrow_constructible_from_row_ : std::false_type
		{};
		template <typename Table, size_t... RowCols, size_t... RowMembers>
		struct row_nothrow_constructible_from_row_<column_count,
												   row<Table, RowCols...>,
												   std::index_sequence<RowMembers...>>
			: std::bool_constant<(Columns::template is_nothrow_constructible<
									  decltype(std::declval<row<Table, RowCols...>>().template get<RowMembers>())>
								  && ...)>
		{
			static_assert(sizeof...(RowCols) == column_count);
			static_assert(sizeof...(RowMembers) == column_count);
			static_assert(std::is_same_v<std::index_sequence<RowMembers...>, std::make_index_sequence<column_count>>);
		};

		template <bool, size_t, typename... Args>
		struct row_nothrow_constructible_from_ : std::false_type
		{};
		template <typename... Args>
		struct row_nothrow_constructible_from_<false, column_count, Args...>
			: std::bool_constant<(Columns::template is_nothrow_constructible<Args> && ...)>
		{
			static_assert(sizeof...(Args) == column_count);
		};
		template <typename Row>
		struct row_nothrow_constructible_from_<true, 1, Row>
			: row_nothrow_constructible_from_row_<std::tuple_size_v<remove_cvref<Row>>,
												  remove_cvref<Row>,
												  std::make_index_sequence<std::tuple_size_v<remove_cvref<Row>>>>
		{};

	  public:
		template <typename... Args>
		static constexpr bool row_nothrow_constructible_from =
			row_nothrow_constructible_from_<(is_row<remove_cvref<Args>> && ...), sizeof...(Args), Args...>::value;

		//--- memmove --------------------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void memmove(column_pointers& dest_columns,
							size_t dest_start,
							column_pointers& source_columns,
							size_t source_start,
							size_t count,
							std::index_sequence<Cols...>) //
			noexcept
		{
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			(column<Cols>::memmove(dest_columns[Cols], dest_start, source_columns[Cols], source_start, count), ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_trivially_copyable)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void memmove(column_pointers& dest_columns,
							size_t dest_start,
							column_pointers& source_columns,
							size_t source_start,
							size_t count) //
			noexcept
		{
			memmove(dest_columns,
					dest_start,
					source_columns,
					source_start,
					count,
					std::make_index_sequence<column_count>{});
		}

		//--- destruction ----------------------------------------------------------------------------------------------

	  private:
		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void destruct_row([[maybe_unused]] column_pointers& columns,
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

					if constexpr (!std::is_trivially_destructible_v<typename column<column_index>::storage_type>)
					{
						SOAGEN_ASSUME(columns[column_index]);
						SOAGEN_ASSUME(leftmost_column <= rightmost_column);
						SOAGEN_ASSUME(leftmost_column < column_count);
						SOAGEN_ASSUME(rightmost_column < column_count);

						if (column_index >= leftmost_column && column_index <= rightmost_column)
							column<column_index>::destruct(columns[column_index], index);
					}
				};

				(destructor(index_constant<Cols>{}), ...);
			}
		}

		template <size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void destruct_row([[maybe_unused]] column_pointers& columns,
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

					if constexpr (!std::is_trivially_destructible_v<typename column<column_index>::storage_type>)
					{
						SOAGEN_ASSUME(columns[column_index]);

						column<column_index>::destruct(columns[column_index], index);
					}
				};

				(destructor(index_constant<Cols>{}), ...);
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
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			if constexpr (all_nothrow_default_constructible || all_trivially_destructible)
			{
				(column<Cols>::default_construct(columns[Cols], index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor = [&](auto ic) //
					noexcept(
						std::is_nothrow_default_constructible_v<typename column<decltype(ic)::value>::storage_type>)
				{
					column_from_ic<decltype(ic)>::default_construct(columns[decltype(ic)::value], index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<Cols>{}), ...);
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
			if constexpr (all_nothrow_default_constructible || all_trivially_destructible)
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
		template <typename Row, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row_from_row(column_pointers& columns,
										   size_t index,
										   Row&& row,
										   std::index_sequence<Cols...>) //
			noexcept(row_nothrow_constructible_from<Row&&>)
		{
			static_assert(is_row<remove_cvref<Row>>);
			static_assert(row_constructible_from<Row&&>);
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);
			static_assert(std::tuple_size_v<remove_cvref<Row>> == column_count);

			construct_row(columns,
						  index,
						  std::index_sequence<Cols...>{},
						  static_cast<Row&&>(row).template get<Cols>()...);
		}

		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row(column_pointers& columns,
								  size_t index,
								  std::index_sequence<Cols...>,
								  Args&&... args) //
			noexcept(row_nothrow_constructible_from<Args&&...>)
		{
			static_assert(row_constructible_from<Args&&...>);
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			if constexpr (sizeof...(Args) == 1 && (is_row<remove_cvref<Args>> && ...))
			{
				construct_row_from_row(columns, index, static_cast<Args&&>(args)..., std::index_sequence<Cols...>{});
			}
			else
			{
				static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);
				static_assert(sizeof...(Args) == sizeof...(Cols));

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
							std::is_nothrow_constructible_v<typename column_from_ic<decltype(ic)>::storage_type,
															decltype(arg)&&>)
					{
						column_from_ic<decltype(ic)>::construct_at(columns[decltype(ic)::value],
																   index,
																   static_cast<decltype(arg)&&>(arg));

						constructed_columns++;
					};

					try
					{
						(constructor(index_constant<Cols>{}, static_cast<Args&&>(args)), ...);
					}
					catch (...)
					{
						if (constructed_columns)
							destruct_row(columns, index, 0u, constructed_columns - 1u);

						throw;
					}
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
			if constexpr (sizeof...(Args) == 0)
			{
				default_construct_row(columns,
									  index,
									  std::make_index_sequence<column_count>{},
									  static_cast<Args&&>(args)...);
			}
			else
			{
				construct_row(columns, index, std::make_index_sequence<column_count>{}, static_cast<Args&&>(args)...);
			}
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
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_index != source_index);

			if constexpr (all_nothrow_move_constructible || all_trivially_destructible)
			{
				(column<Cols>::move_construct(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(
						std::is_nothrow_move_constructible_v<typename column_from_ic<decltype(ic)>::storage_type>)
				{
					column_from_ic<decltype(ic)>::move_construct(dest_columns[decltype(ic)::value],
																 dest_index,
																 source_columns[decltype(ic)::value],
																 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<Cols>{}), ...);
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
			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest_columns, dest_start, source_columns, source_start, count);
			}
			else if constexpr (all_nothrow_move_constructible)
			{
				if (&dest_columns == &source_columns && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					if (&dest_columns == &source_columns && dest_start > source_start)
					{
						for (; i-- > 0u;)
							move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							move_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
					}
				}
				catch (...)
				{
					if (&dest_columns == &source_columns && dest_start > source_start)
					{
						for (; i < count; i++)
							destruct_row(dest_columns, dest_start + i);
					}
					else
					{
						for (; i-- > 0u;)
							destruct_row(dest_columns, dest_start + i);
					}
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
									   const const_column_pointers& source_columns,
									   size_t source_index,
									   std::index_sequence<Cols...>) //
			noexcept(all_nothrow_copy_constructible)
		{
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_index != source_index);

			if constexpr (all_nothrow_copy_constructible || all_trivially_destructible)
			{
				(column<Cols>::copy_construct(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(
						std::is_nothrow_copy_constructible_v<typename column_from_ic<decltype(ic)>::storage_type>)
				{
					column_from_ic<decltype(ic)>::copy_construct(dest_columns[decltype(ic)::value],
																 dest_index,
																 source_columns[decltype(ic)::value],
																 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<Cols>{}), ...);
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
									   const const_column_pointers& source_columns,
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
										const const_column_pointers& source_columns,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest_columns, dest_start, source_columns, source_start, count);
			}
			else if constexpr (all_nothrow_copy_constructible)
			{
				if (&dest_columns == &source_columns && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					if (&dest_columns == &source_columns && dest_start > source_start)
					{
						for (; i-- > 0u;)
							copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							copy_construct_row(dest_columns, dest_start + i, source_columns, source_start + i);
					}
				}
				catch (...)
				{
					if (&dest_columns == &source_columns && dest_start > source_start)
					{
						for (; i < count; i++)
							destruct_row(dest_columns, dest_start + i);
					}
					else
					{
						for (; i-- > 0u;)
							destruct_row(dest_columns, dest_start + i);
					}
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
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<Cols>::move_assign(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
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
									 column_pointers& source_columns,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest_columns, dest_start, source_columns, source_start, count);
			}
			else
			{
				if (&dest_columns == &source_columns && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
			}
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									const const_column_pointers& source_columns,
									size_t source_index,
									std::index_sequence<Cols...>) //
			noexcept(all_nothrow_copy_assignable)
		{
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<Cols>::copy_assign(dest_columns[Cols], dest_index, source_columns[Cols], source_index), ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest_columns,
									size_t dest_index,
									const const_column_pointers& source_columns,
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
									 const const_column_pointers& source_columns,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest_columns != &source_columns || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest_columns, dest_start, source_columns, source_start, count);
			}
			else
			{
				if (&dest_columns == &source_columns && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						copy_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						copy_assign_row(dest_columns, dest_start + i, source_columns, source_start + i);
				}
			}
		}

		//--- swap -----------------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_CPP20_CONSTEXPR
		static void swap_rows(column_pointers& lhs_columns,
							  size_t lhs_index,
							  column_pointers& rhs_columns,
							  size_t rhs_index,
							  std::index_sequence<Cols...>) //
			noexcept(all_nothrow_swappable)
		{
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			(column<Cols>::swap(lhs_columns[Cols], lhs_index, rhs_columns[Cols], rhs_index), ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_swappable)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void swap_rows(column_pointers& lhs_columns,
							  size_t lhs_index,
							  column_pointers& rhs_columns,
							  size_t rhs_index) //
			noexcept(all_nothrow_swappable)
		{
			swap_rows(lhs_columns, lhs_index, rhs_columns, rhs_index, std::make_index_sequence<column_count>{});
		}

		//--- equality -------------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_NODISCARD
		constexpr static bool equal(const const_column_pointers& lhs_columns,
									size_t lhs_start_index,
									const const_column_pointers& rhs_columns,
									size_t rhs_start_index,
									size_t count,
									std::index_sequence<Cols...>) //
			noexcept(all_nothrow_equality_comparable)
		{
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			// note that the equality-comparison is done column-major for speed (cache-locality) because
			// equality-comparison does not need to know anything about the order of the columns,
			// just that they are all equal (there is no lexicographic constraint)

			return (column<Cols>::equal(lhs_columns[Cols], lhs_start_index, rhs_columns[Cols], rhs_start_index, count)
					&& ...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_equality_comparable)
		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		constexpr static bool equal(const const_column_pointers& lhs_columns,
									size_t lhs_start,
									const const_column_pointers& rhs_columns,
									size_t rhs_start,
									size_t count) //
			noexcept(all_nothrow_equality_comparable)
		{
			return equal(lhs_columns,
						 lhs_start,
						 rhs_columns,
						 rhs_start,
						 count,
						 std::make_index_sequence<column_count>{});
		}

		//--- compare --------------------------------------------------------------------------------------------------

	  private:
		template <typename... Args, size_t... Cols>
		SOAGEN_NODISCARD
		constexpr static int compare(const const_column_pointers& lhs_columns,
									 size_t lhs_start_index,
									 const const_column_pointers& rhs_columns,
									 size_t rhs_start_index,
									 size_t count,
									 std::index_sequence<Cols...>) //
			noexcept(all_nothrow_less_than_comparable)
		{
			static_assert(std::is_same_v<std::index_sequence<Cols...>, std::make_index_sequence<column_count>>);

			for (size_t i = 0; i < count; i++)
			{
				if ((false || ...
					 || column<Cols>::less_than(lhs_columns[Cols],
												lhs_start_index + i,
												rhs_columns[Cols],
												rhs_start_index + i)))
					return -1;

				if ((false || ...
					 || column<Cols>::less_than(rhs_columns[Cols],
												rhs_start_index + i,
												lhs_columns[Cols],
												lhs_start_index + i)))
					return 1;
			}

			return 0;
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_less_than_comparable)
		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		constexpr static int compare(const const_column_pointers& lhs_columns,
									 size_t lhs_start,
									 const const_column_pointers& rhs_columns,
									 size_t rhs_start,
									 size_t count) //
			noexcept(all_nothrow_less_than_comparable)
		{
			return compare(lhs_columns,
						   lhs_start,
						   rhs_columns,
						   rhs_start,
						   count,
						   std::make_index_sequence<column_count>{});
		}
	};
}
/// @endcond

namespace soagen
{
	/// @brief	Traits for a table.
	/// @tparam	Columns		The #soagen::column_traits for the columns of the table.
	///
	///	@attention	This class is an implementation detail for the soagen-generated Structure-of-arrays classes.
	///				You don't need to know anything about it unless you are implementing your own SoA machinery
	///				without using the soagen generator.
	template <typename... Columns>
	struct SOAGEN_EMPTY_BASES table_traits : public detail::table_traits_base<detail::to_base_traits<Columns>...>
	{
		/// @brief The number of columns in the table.
		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		/// @brief	The number of rows to advance to maintain the requested `alignment` for every column.
		///
		/// @details	The stride size you need to use when iterating through rows of this table such that
		///				the starting element for each batch in each column would have the same memory alignment as the
		///				value specified for the column-specific `alignment`.
		///
		/// @note		Typically you can ignore this; column elements are always aligned correctly according to their
		///				type. This is for over-alignment scenarios where you need to do things in batches (e.g. SIMD).
		static constexpr size_t aligned_stride = lcm(size_t{ 1 }, Columns::aligned_stride...);

		// columns
		// (note that these hide the base class typedefs - this is intentional)

		/// @brief	Returns the #soagen::column_traits for the column at the specified index.
		template <size_t Index>
		using column = type_at_index<Index, Columns...>;

		/// @brief	Same as #column but takes an #index_constant.
		template <typename IndexConstant>
		using column_from_ic = type_at_index<IndexConstant::value, Columns...>;

		/// @brief Array containing the `alignment` for each column.
		static constexpr size_t column_alignments[column_count] = { Columns::alignment... };

		/// @brief The max `alignment` of all columns in the table.
		static constexpr size_t largest_alignment = max(size_t{ 1 }, Columns::alignment...);

		/// @brief True if the arguments passed to the rvalue overloads of `push_back()`
		/// and `insert()` would be distinct from the regular const lvalue  overload.
		static constexpr bool rvalue_type_list_is_distinct = POXY_IMPLEMENTATION_DETAIL(
			!(std::is_same_v<typename Columns::param_type, typename Columns::rvalue_type> && ...));

		/// @brief True if a generated class's `emplace_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Args The args being passed to #soagen::table::emplace_back().
		template <typename BackingTable, typename... Args>
		static constexpr bool emplace_back_is_nothrow =
			noexcept(std::declval<BackingTable>().emplace_back(std::declval<Args>()...));

		/// @brief True if a generated class's lvalue `push_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool push_back_is_nothrow =
			emplace_back_is_nothrow<BackingTable, typename Columns::param_forward_type...>;

		/// @brief True if a generated class's rvalue `push_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool rvalue_push_back_is_nothrow =
			emplace_back_is_nothrow<BackingTable, typename Columns::rvalue_forward_type...>;

		/// @brief True if a generated class's row `push_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Row The row type.
		template <typename BackingTable, typename Row>
		static constexpr bool row_push_back_is_nothrow = emplace_back_is_nothrow<BackingTable, Row>;

		/// @brief True if a generated class's `emplace()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Args The args being passed to #soagen::table::emplace().
		template <typename BackingTable, typename... Args>
		static constexpr bool emplace_is_nothrow =
			noexcept(std::declval<BackingTable>().emplace(typename remove_cvref<BackingTable>::size_type{},
														  std::declval<Args>()...));

		/// @brief True if a generated class's lvalue `insert()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool insert_is_nothrow =
			emplace_is_nothrow<BackingTable, typename Columns::param_forward_type...>;

		/// @brief True if a generated class's rvalue `insert()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool rvalue_insert_is_nothrow =
			emplace_is_nothrow<BackingTable, typename Columns::rvalue_forward_type...>;

		/// @brief True if a generated class's row `insert()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Row The row type.
		template <typename BackingTable, typename Row>
		static constexpr bool row_insert_is_nothrow = emplace_is_nothrow<BackingTable, Row>;

		/// @brief True if a generated class's `for_each_column()` would be invocable with a callable.
		/// @tparam Func The callable type being invoked.
		/// @tparam Const True for `const` column data pointers.
		template <typename Func, bool Const = false>
		static constexpr bool for_each_column_ptr_invocable = POXY_IMPLEMENTATION_DETAIL( //
			(is_invocable_with_optarg<Func,
									  std::conditional_t<Const,
														 std::add_const_t<typename Columns::value_type>,
														 typename Columns::value_type>*,
									  size_t>
			 && ...));

		/// @brief True if a generated class's `for_each_column()` would be nothrow-invocable with a callable.
		/// @tparam Func The callable type being invoked.
		/// @tparam Const True for `const` column data pointers.
		template <typename Func, bool Const = false>
		static constexpr bool for_each_column_ptr_nothrow_invocable = POXY_IMPLEMENTATION_DETAIL( //
			(is_nothrow_invocable_with_optarg<Func,
											  std::conditional_t<Const,
																 std::add_const_t<typename Columns::value_type>,
																 typename Columns::value_type>*,
											  size_t>
			 && ...));
	};

	/// @brief True if `T` is an instance of #soagen::table_traits.
	template <typename T>
	inline constexpr bool is_table_traits = POXY_IMPLEMENTATION_DETAIL(false);
	/// @cond
	template <typename... Columns>
	inline constexpr bool is_table_traits<table_traits<Columns...>> = true;
	template <typename... Columns>
	inline constexpr bool is_table_traits<detail::table_traits_base<Columns...>> = true;
	template <typename T>
	inline constexpr bool is_table_traits<const T> = is_table_traits<T>;
	template <typename T>
	inline constexpr bool is_table_traits<volatile T> = is_table_traits<T>;
	template <typename T>
	inline constexpr bool is_table_traits<const volatile T> = is_table_traits<T>;
	/// @endcond
}

/// @cond
namespace soagen::detail
{
	template <typename... Columns>
	struct to_base_traits_<table_traits<Columns...>>
	{
		using type = table_traits_base<to_base_traits<Columns>...>;

		static_assert(std::is_base_of_v<type, table_traits<Columns...>>);
	};

	template <typename... Columns>
	struct table_traits_type_<table_traits<Columns...>>
	{
		using type = table_traits<Columns...>;
	};
}
/// @endcond

#include "header_end.hpp"
