//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "column_traits.hpp"
#include "row.hpp"
#include "invoke.hpp"
#include "header_start.hpp"

/// @cond
namespace soagen::detail
{
	// a base class for the table traits that handles all the non-alignment-dependent stuff -
	// the 'Columns' argument seen by this class should be the column_base_traits, NOT soagen::column_traits
	// (to minimize template instantiation explosion)
	template <typename...>
	struct table_traits_base;

	template <size_t... I, typename... Columns>
	struct table_traits_base<std::index_sequence<I...>, Columns...>
	{
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		static_assert(std::is_same_v<std::index_sequence<I...>, std::make_index_sequence<sizeof...(Columns)>>,
					  "index sequence must match columns");

		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		// columns

		template <auto Index>
		using column = type_at_index<static_cast<size_t>(Index), Columns...>;

		template <typename IndexConstant>
		using column_from_ic = type_at_index<static_cast<size_t>(remove_cvref<IndexConstant>::value), Columns...>;

		template <auto Index>
		using storage_type = typename column<static_cast<size_t>(Index)>::storage_type;

		using column_pointers		= std::byte* [column_count];
		using const_column_pointers = std::byte* const[column_count];

		static constexpr size_t column_sizes[column_count] = { sizeof(typename Columns::storage_type)... };

		// default constructibility

		static constexpr bool all_default_constructible =
			std::conjunction<std::is_default_constructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_default_constructible =
			std::conjunction<std::is_nothrow_default_constructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_trivially_default_constructible =
			std::conjunction<std::is_trivially_default_constructible<typename Columns::storage_type>...>::value;

		// trivial-copyability (memcpy + memmove)

		static constexpr bool all_trivially_copyable = (Columns::is_trivially_copyable && ...);

		static constexpr bool any_trivially_copyable = (false || ... || Columns::is_trivially_copyable);

		// move-constructibility

		static constexpr bool all_move_constructible = (Columns::is_move_constructible && ...);

		static constexpr bool all_nothrow_move_constructible = (Columns::is_nothrow_move_constructible && ...);

		static constexpr bool all_trivially_move_constructible = (Columns::is_trivially_move_constructible && ...);

		// move-assignability

		static constexpr bool all_move_assignable = (Columns::is_move_assignable && ...);

		static constexpr bool all_nothrow_move_assignable = (Columns::is_nothrow_move_assignable && ...);

		static constexpr bool all_trivially_move_assignable = (Columns::is_trivially_move_assignable && ...);

		// copy-constructibility

		static constexpr bool all_copy_constructible = (Columns::is_copy_constructible && ...);

		static constexpr bool all_nothrow_copy_constructible = (Columns::is_nothrow_copy_constructible && ...);

		static constexpr bool all_trivially_copy_constructible = (Columns::is_trivially_copy_constructible && ...);

		// copy-assignability

		static constexpr bool all_copy_assignable = (Columns::is_copy_assignable && ...);

		static constexpr bool all_nothrow_copy_assignable = (Columns::is_nothrow_copy_assignable && ...);

		static constexpr bool all_trivially_copy_assignable = (Columns::is_trivially_copy_assignable && ...);

		// move-or-copy-constructibility

		static constexpr bool all_move_or_copy_constructible = (Columns::is_move_or_copy_constructible && ...);

		static constexpr bool all_nothrow_move_or_copy_constructible =
			(Columns::is_nothrow_move_or_copy_constructible && ...);

		static constexpr bool all_trivially_move_or_copy_constructible =
			(Columns::is_trivially_move_or_copy_constructible && ...);

		// move-or-copy-assignability

		static constexpr bool all_move_or_copy_assignable = (Columns::is_move_or_copy_assignable && ...);

		static constexpr bool all_nothrow_move_or_copy_assignable =
			(Columns::is_nothrow_move_or_copy_assignable && ...);

		static constexpr bool all_trivially_move_or_copy_assignable =
			(Columns::is_trivially_move_or_copy_assignable && ...);

		// destructibility

		static constexpr bool all_destructible =
			std::conjunction<std::is_destructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_destructible =
			std::conjunction<std::is_nothrow_destructible<typename Columns::storage_type>...>::value;

		static constexpr bool all_trivially_destructible =
			std::conjunction<std::is_trivially_destructible<typename Columns::storage_type>...>::value;

		// swappability

		static constexpr bool all_swappable = (Columns::is_swappable && ...);

		static constexpr bool all_nothrow_swappable = (Columns::is_nothrow_swappable && ...);

		// equality comparability

		static constexpr bool all_equality_comparable =
			std::conjunction<is_equality_comparable_<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_equality_comparable =
			std::conjunction<is_nothrow_equality_comparable_<typename Columns::storage_type>...>::value;

		// less-than comparability

		static constexpr bool all_less_than_comparable =
			std::conjunction<is_less_than_comparable_<typename Columns::storage_type>...>::value;

		static constexpr bool all_nothrow_less_than_comparable =
			std::conjunction<is_nothrow_less_than_comparable_<typename Columns::storage_type>...>::value;

		// row constructibility

	  private:
		template <size_t, typename...>
		struct row_constructible_from_tuple_ : std::false_type
		{};
		template <typename Tuple, size_t... Members>
		struct row_constructible_from_tuple_<column_count, Tuple, std::index_sequence<Members...>>
			: std::conjunction<typename Columns::template is_constructible_trait<decltype(get_from_tuple<Members>(
				  std::declval<Tuple>()))>...>
		{
			static_assert(std::is_same_v<std::index_sequence<Members...>, std::make_index_sequence<column_count>>);
		};

		template <bool, size_t, typename... Args>
		struct row_constructible_from_ : std::false_type
		{};
		template <typename... Args>
		struct row_constructible_from_<false, column_count, Args...>
			: std::conjunction<typename Columns::template is_constructible_trait<Args>...>
		{
			static_assert(sizeof...(Args) == column_count);
		};
		template <typename Tuple>
		struct row_constructible_from_<true, 1, Tuple>
			: row_constructible_from_tuple_<std::tuple_size_v<remove_cvref<Tuple>>,
											remove_cvref<Tuple>,
											std::make_index_sequence<std::tuple_size_v<remove_cvref<Tuple>>>>
		{};

	  public:
		template <typename... Args>
		static constexpr bool row_constructible_from =
			row_constructible_from_<(is_tuple<remove_cvref<Args>> && ...), sizeof...(Args), Args...>::value;

		// row constructibility (nothrow)

	  private:
		template <size_t, typename...>
		struct row_nothrow_constructible_from_tuple_ : std::false_type
		{};
		template <typename Tuple, size_t... Members>
		struct row_nothrow_constructible_from_tuple_<column_count, Tuple, std::index_sequence<Members...>>
			: std::conjunction<typename Columns::template is_nothrow_constructible_trait<
				  decltype(get_from_tuple<Members>(std::declval<Tuple>()))>...>
		{
			static_assert(std::is_same_v<std::index_sequence<Members...>, std::make_index_sequence<column_count>>);
		};

		template <bool, size_t, typename... Args>
		struct row_nothrow_constructible_from_ : std::false_type
		{};
		template <typename... Args>
		struct row_nothrow_constructible_from_<false, column_count, Args...>
			: std::conjunction<typename Columns::template is_nothrow_constructible_trait<Args>...>
		{
			static_assert(sizeof...(Args) == column_count);
		};
		template <typename Tuple>
		struct row_nothrow_constructible_from_<true, 1, Tuple>
			: row_nothrow_constructible_from_tuple_<std::tuple_size_v<remove_cvref<Tuple>>,
													remove_cvref<Tuple>,
													std::make_index_sequence<std::tuple_size_v<remove_cvref<Tuple>>>>
		{};

	  public:
		template <typename... Args>
		static constexpr bool row_nothrow_constructible_from =
			row_nothrow_constructible_from_<(is_tuple<remove_cvref<Args>> && ...), sizeof...(Args), Args...>::value;

		//--- memmove --------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_trivially_copyable)
		SOAGEN_CPP20_CONSTEXPR
		static void memmove(column_pointers& dest,
							size_t dest_start,
							column_pointers& source,
							size_t source_start,
							size_t count) //
			noexcept
		{
			(column<I>::memmove(dest[I], dest_start, source[I], source_start, count), ...);
		}

		//--- destruction ----------------------------------------------------------------------------------------------

		static constexpr void destruct_row(column_pointers& columns,
										   size_t index,
										   size_t leftmost_column,
										   size_t rightmost_column) noexcept
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
					static constexpr size_t column_index = decltype(ic)::value;

					if constexpr (!std::is_trivially_destructible_v<storage_type<column_index>>)
					{
						if (column_index >= leftmost_column && column_index <= rightmost_column)
							column<column_index>::destruct(columns[column_index], index);
					}
				};

				(destructor(index_constant<column_count - I - 1u>{}), ...);
			}
		}

		static constexpr void destruct_row(column_pointers& columns, size_t index) noexcept
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
					static constexpr size_t column_index = decltype(ic)::value;

					if constexpr (!std::is_trivially_destructible_v<storage_type<column_index>>)
					{
						column<column_index>::destruct(columns[column_index], index);
					}
				};

				(destructor(index_constant<column_count - I - 1u>{}), ...);
			}
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

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_default_constructible)
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct_row(column_pointers& columns,
										  size_t index) //
			noexcept(all_nothrow_default_constructible) //
		{
			static_assert(all_default_constructible);

			if constexpr (all_nothrow_default_constructible || all_trivially_destructible)
			{
				(column<I>::default_construct(columns[I], index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor = [&](auto ic) //
					noexcept(std::is_nothrow_default_constructible_v<storage_type<decltype(ic)::value>>)
				{
					column_from_ic<decltype(ic)>::default_construct(columns[decltype(ic)::value], index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(columns, index, 0u, constructed_columns - 1u);

					throw;
				}
			}
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
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, typename Tuple, auto sfinae = row_constructible_from<Tuple&&>)
		SOAGEN_ALWAYS_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row_from_tuple(column_pointers& columns,
											 size_t index,
											 Tuple&& tuple) //
			noexcept(row_nothrow_constructible_from<Tuple&&>)
		{
			static_assert(std::tuple_size_v<remove_cvref<Tuple>> == column_count);

			construct_row(columns, index, get_from_tuple<I>(static_cast<Tuple&&>(tuple))...);
		}

	  public:
		SOAGEN_HIDDEN_CONSTRAINT(sfinae, typename... Args, auto sfinae = row_constructible_from<Args&&...>)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void construct_row(column_pointers& columns, size_t index, Args&&... args) //
			noexcept(row_nothrow_constructible_from<Args&&...>)
		{
			if constexpr (sizeof...(Args) == 0)
			{
				default_construct_row(columns, index, static_cast<Args&&>(args)...);
			}
			else if constexpr (sizeof...(Args) == 1 && (is_tuple<remove_cvref<Args>> && ...))
			{
				construct_row_from_tuple(columns, index, static_cast<Args&&>(args)...);
			}
			else
			{
				static_assert(sizeof...(Args) == sizeof...(I));

				if constexpr (row_nothrow_constructible_from<Args&&...> || all_trivially_destructible)
				{
					(column<I>::construct_at(columns[I], index, static_cast<Args&&>(args)), ...);
				}
				else
				{
					// machinery to provide strong-exception guarantee

					size_t constructed_columns = {};

					const auto constructor =
						[&](auto ic, auto&& arg) noexcept(
							column_from_ic<decltype(ic)>::template is_nothrow_constructible<decltype(arg)>)
					{
						static_assert(std::is_reference_v<decltype(arg)>);

						column_from_ic<decltype(ic)>::construct_at(columns[decltype(ic)::value],
																   index,
																   static_cast<decltype(arg)>(arg));

						constructed_columns++;
					};

					try
					{
						(constructor(index_constant<I>{}, static_cast<Args&&>(args)), ...);
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

		//--- move-construction ----------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_row(column_pointers& dest,
									   size_t dest_index,
									   column_pointers& source,
									   size_t source_index) //
			noexcept(all_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			if constexpr (all_nothrow_move_constructible || all_trivially_destructible)
			{
				(column<I>::move_construct(dest[I], dest_index, source[I], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(column_from_ic<decltype(ic)>::is_nothrow_move_constructible)
				{
					column_from_ic<decltype(ic)>::move_construct(dest[decltype(ic)::value],
																 dest_index,
																 source[decltype(ic)::value],
																 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_construct_rows(column_pointers& dest,
										size_t dest_start,
										column_pointers& source,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else if constexpr (all_nothrow_move_constructible)
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_construct_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_construct_row(dest, dest_start + i, source, source_start + i);
				}
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i-- > 0u;)
							move_construct_row(dest, dest_start + i, source, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							move_construct_row(dest, dest_start + i, source, source_start + i);
					}
				}
				catch (...)
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i < count; i++)
							destruct_row(dest, dest_start + i);
					}
					else
					{
						for (; i-- > 0u;)
							destruct_row(dest, dest_start + i);
					}
					throw;
				}
			}
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_row(column_pointers& dest,
									   size_t dest_index,
									   const const_column_pointers& source,
									   size_t source_index) //
			noexcept(all_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			if constexpr (all_nothrow_copy_constructible || all_trivially_destructible)
			{
				(column<I>::copy_construct(dest[I], dest_index, source[I], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(column_from_ic<decltype(ic)>::is_nothrow_copy_constructible)
				{
					column_from_ic<decltype(ic)>::copy_construct(dest[decltype(ic)::value],
																 dest_index,
																 source[decltype(ic)::value],
																 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_construct_rows(column_pointers& dest,
										size_t dest_start,
										const const_column_pointers& source,
										size_t source_start,
										size_t count) //
			noexcept(all_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else if constexpr (all_nothrow_copy_constructible)
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						copy_construct_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						copy_construct_row(dest, dest_start + i, source, source_start + i);
				}
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i-- > 0u;)
							copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
				}
				catch (...)
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i < count; i++)
							destruct_row(dest, dest_start + i);
					}
					else
					{
						for (; i-- > 0u;)
							destruct_row(dest, dest_start + i);
					}
					throw;
				}
			}
		}

		//--- move-or-copy-construction (whichever is possible per-column) ---------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_construct_row(column_pointers& dest,
											   size_t dest_index,
											   column_pointers& source,
											   size_t source_index) //
			noexcept(all_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			if constexpr (all_nothrow_move_or_copy_constructible || all_trivially_destructible)
			{
				(column<I>::move_or_copy_construct(dest[I], dest_index, source[I], source_index), ...);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t constructed_columns = {};

				const auto constructor =
					[&](auto ic) noexcept(column_from_ic<decltype(ic)>::is_nothrow_move_or_copy_constructible)
				{
					column_from_ic<decltype(ic)>::move_or_copy_construct(dest[decltype(ic)::value],
																		 dest_index,
																		 source[decltype(ic)::value],
																		 source_index);

					constructed_columns++;
				};

				try
				{
					(constructor(index_constant<I>{}), ...);
				}
				catch (...)
				{
					if (constructed_columns)
						destruct_row(dest, dest_index, 0u, constructed_columns - 1u);

					throw;
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_constructible)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_construct_rows(column_pointers& dest,
												size_t dest_start,
												column_pointers& source,
												size_t source_start,
												size_t count) //
			noexcept(all_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else if constexpr (all_nothrow_move_or_copy_constructible)
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_or_copy_construct_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_or_copy_construct_row(dest, dest_start + i, source, source_start + i);
				}
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = 0;

				try
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i-- > 0u;)
							move_or_copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
					else
					{
						for (; i < count; i++)
							move_or_copy_construct_row(dest, dest_start + i, source, source_start + i);
					}
				}
				catch (...)
				{
					if (&dest == &source && dest_start > source_start)
					{
						for (; i < count; i++)
							move_or_copy_construct_row(dest, dest_start + i);
					}
					else
					{
						for (; i-- > 0u;)
							move_or_copy_construct_row(dest, dest_start + i);
					}
					throw;
				}
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_row(column_pointers& dest,
									size_t dest_index,
									column_pointers& source,
									size_t source_index) //
			noexcept(all_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<I>::move_assign(dest[I], dest_index, source[I], source_index), ...);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_assign_rows(column_pointers& dest,
									 size_t dest_start,
									 column_pointers& source,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_assign_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_assign_row(dest, dest_start + i, source, source_start + i);
				}
			}
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_row(column_pointers& dest,
									size_t dest_index,
									const const_column_pointers& source,
									size_t source_index) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<I>::copy_assign(dest[I], dest_index, source[I], source_index), ...);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void copy_assign_rows(column_pointers& dest,
									 size_t dest_start,
									 const const_column_pointers& source,
									 size_t source_start,
									 size_t count) //
			noexcept(all_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
			}
		}

		//--- move-or-copy-assignment (whichever is possible per-column) -----------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_assign_row(column_pointers& dest,
											size_t dest_index,
											column_pointers& source,
											size_t source_index) //
			noexcept(all_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_index != source_index);

			// todo: how to provide a strong-exception guarantee here?

			(column<I>::move_or_copy_assign(dest[I], dest_index, source[I], source_index), ...);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_move_or_copy_assignable)
		SOAGEN_NEVER_INLINE
		SOAGEN_CPP20_CONSTEXPR
		static void move_or_copy_assign_rows(column_pointers& dest,
											 size_t dest_start,
											 column_pointers& source,
											 size_t source_start,
											 size_t count) //
			noexcept(all_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(&dest != &source || dest_start != source_start);

			if constexpr (all_trivially_copyable)
			{
				memmove(dest, dest_start, source, source_start, count);
			}
			else
			{
				if (&dest == &source && dest_start > source_start)
				{
					for (size_t i = count; i-- > 0u;)
						move_or_copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
				else
				{
					for (size_t i = 0; i < count; i++)
						move_or_copy_assign_row(dest, dest_start + i, source, source_start + i);
				}
			}
		}

		//--- swap rows ------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_swappable)
		SOAGEN_CPP20_CONSTEXPR
		static void swap_rows(column_pointers& lhs,
							  size_t lhs_index,
							  column_pointers& rhs,
							  size_t rhs_index) //
			noexcept(all_nothrow_swappable)
		{
			(column<I>::swap(lhs[I], lhs_index, rhs[I], rhs_index), ...);
		}

		//--- swap columns ---------------------------------------------------------------------------------------------

		template <auto A, auto B>
		static constexpr bool can_swap_columns =
			static_cast<size_t>(A) == static_cast<size_t>(B)
			|| (std::is_same_v<storage_type<A>, storage_type<B>> && column<A>::is_swappable);

		template <auto A, auto B>
		static constexpr bool can_nothrow_swap_columns =
			static_cast<size_t>(A) == static_cast<size_t>(B)
			|| (std::is_same_v<storage_type<A>, storage_type<B>> && column<A>::is_nothrow_swappable);

		SOAGEN_HIDDEN_CONSTRAINT((can_swap_columns<A, B>), auto A, auto B)
		SOAGEN_CPP20_CONSTEXPR
		static void swap_columns([[maybe_unused]] column_pointers& columns,
								 [[maybe_unused]] size_t start,
								 [[maybe_unused]] size_t count) //
			noexcept(can_nothrow_swap_columns<A, B>)
		{
			if constexpr (static_cast<size_t>(A) != static_cast<size_t>(B))
			{
				static_assert(std::is_same_v<storage_type<A>, storage_type<B>>);
				static_assert(column<A>::is_swappable);
				static_assert(column<B>::is_swappable);

				column<A>::swap(columns[A], start, columns[B], start, count);
			}
		}

		//--- equality -------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_equality_comparable)
		SOAGEN_NODISCARD
		static constexpr bool equal(const const_column_pointers& lhs,
									size_t lhs_start,
									const const_column_pointers& rhs,
									size_t rhs_start,
									size_t count) //
			noexcept(all_nothrow_equality_comparable)
		{
			// note that the equality-comparison is done column-major for speed (cache-locality) because
			// equality-comparison does not need to know anything about the order of the columns,
			// just that they are all equal (there is no lexicographic constraint)

			return (column<I>::equal(lhs[I], lhs_start, rhs[I], rhs_start, count) && ...);
		}

		//--- compare --------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = all_less_than_comparable)
		SOAGEN_NODISCARD
		static constexpr int compare(const const_column_pointers& lhs,
									 size_t lhs_start,
									 const const_column_pointers& rhs,
									 size_t rhs_start,
									 size_t count) //
			noexcept(all_nothrow_less_than_comparable)
		{
			for (size_t i = 0; i < count; i++)
			{
				if ((false || ... || column<I>::less_than(lhs[I], lhs_start + i, rhs[I], rhs_start + i)))
					return -1;

				if ((false || ... || column<I>::less_than(rhs[I], rhs_start + i, lhs[I], lhs_start + i)))
					return 1;
			}

			return 0;
		}
	};

	// a more specialzed base that sees the full column traits, not just the base version.
	// (this is just for things that need the specialized information _and_ the column indices)
	template <typename...>
	struct table_traits_base_specialized;

	template <size_t... I, typename... Columns>
	struct table_traits_base_specialized<std::index_sequence<I...>, Columns...> //
		: public table_traits_base<std::index_sequence<I...>, to_base_traits<Columns>...>
	{
		static_assert((... && is_column_traits<Columns>), "columns must be instances of soagen::column_traits");

		//
	};
}
/// @endcond

namespace soagen
{
	/// @brief	Traits for a table.
	/// @tparam	Columns		The types and/or #soagen::column_traits for the columns of the table.
	template <typename... Columns>
	struct SOAGEN_EMPTY_BASES table_traits //
		SOAGEN_HIDDEN_BASE(public detail::table_traits_base_specialized<std::make_index_sequence<sizeof...(Columns)>,
																		detail::as_column<Columns>...>)
	{
		/// @brief The number of columns in the table.
		static constexpr size_t column_count = sizeof...(Columns);
		static_assert(column_count, "tables must have at least one column");

		/// @brief	The number of rows to advance to maintain the requested `alignment` for every column.
		///
		/// @details	The stride size you need to use when iterating through rows of this table such that
		///				the starting element for each batch in each column would have the same memory alignment as the
		///				value specified for the column-specific `alignment`.
		///
		/// @note		Typically you can ignore this; column elements are always aligned correctly according to their
		///				type. This is for over-alignment scenarios where you need to do things in batches (e.g. SIMD).
		static constexpr size_t aligned_stride = lcm(size_t{ 1 }, detail::as_column<Columns>::aligned_stride...);

		// columns
		// (note that these hide the base class typedefs - this is intentional)

		/// @brief	Returns the #soagen::column_traits for the column at the specified index.
		template <auto Index>
		using column = type_at_index<static_cast<size_t>(Index), detail::as_column<Columns>...>;

		/// @brief	Same as #column but takes an #index_constant.
		template <typename IndexConstant>
		using column_from_ic =
			type_at_index<static_cast<size_t>(remove_cvref<IndexConstant>::value), detail::as_column<Columns>...>;

		/// @brief Array containing the `alignment` for each column.
		static constexpr size_t column_alignments[column_count] = { detail::as_column<Columns>::alignment... };

		/// @brief The max `alignment` of all columns in the table.
		static constexpr size_t largest_alignment = max(size_t{ 1 }, detail::as_column<Columns>::alignment...);

		/// @brief True if the arguments passed to the rvalue overloads of `push_back()`
		/// and `insert()` would be distinct from the regular const lvalue overload.
		static constexpr bool rvalues_are_distinct =
			POXY_IMPLEMENTATION_DETAIL(!(std::is_same_v<typename detail::as_column<Columns>::param_type,
														typename detail::as_column<Columns>::rvalue_type>
										 && ...));

		/// @brief True if a generated class's `emplace_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Args The args being passed to #soagen::table::emplace_back().
		template <typename BackingTable, typename... Args>
		static constexpr bool emplace_back_is_nothrow = has_nothrow_emplace_back_member<BackingTable, Args...>;

		/// @brief True if a generated class's lvalue `push_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool push_back_is_nothrow =
			emplace_back_is_nothrow<BackingTable, typename detail::as_column<Columns>::param_forward_type...>;

		/// @brief True if a generated class's rvalue `push_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool rvalue_push_back_is_nothrow =
			emplace_back_is_nothrow<BackingTable, typename detail::as_column<Columns>::rvalue_forward_type...>;

		/// @brief True if a generated class's row `push_back()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Row The row type.
		template <typename BackingTable, typename Row>
		static constexpr bool row_push_back_is_nothrow = emplace_back_is_nothrow<BackingTable, Row>;

		/// @brief True if a generated class's `emplace()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Args The args being passed to #soagen::table::emplace().
		template <typename BackingTable, typename... Args>
		static constexpr bool emplace_is_nothrow = has_nothrow_emplace_member<BackingTable, std::size_t, Args...>;

		/// @brief True if a generated class's lvalue `insert()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool insert_is_nothrow =
			emplace_is_nothrow<BackingTable, typename detail::as_column<Columns>::param_forward_type...>;

		/// @brief True if a generated class's rvalue `insert()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		template <typename BackingTable>
		static constexpr bool rvalue_insert_is_nothrow =
			emplace_is_nothrow<BackingTable, typename detail::as_column<Columns>::rvalue_forward_type...>;

		/// @brief True if a generated class's row `insert()` would be nothrow.
		/// @tparam BackingTable The backing #soagen::table type.
		/// @tparam Row The row type.
		template <typename BackingTable, typename Row>
		static constexpr bool row_insert_is_nothrow = emplace_is_nothrow<BackingTable, Row>;
	};

	/// @cond
	namespace detail
	{
		template <typename>
		struct is_table_traits_ : std::false_type
		{};
		template <typename... Columns>
		struct is_table_traits_<table_traits<Columns...>> : std::true_type
		{};
		template <typename... Columns>
		struct is_table_traits_<detail::table_traits_base<Columns...>> : std::true_type
		{};
	}
	/// @endcond
	/// @brief True if `T` is a #soagen::table_traits.
	template <typename T>
	inline constexpr bool is_table_traits =
		POXY_IMPLEMENTATION_DETAIL(detail::is_table_traits_<std::remove_cv_t<T>>::value);
}

/// @cond
namespace soagen::detail
{
	template <typename... Columns>
	struct to_base_traits_<table_traits<Columns...>>
	{
		using type = table_traits_base<to_base_traits<as_column<Columns>>...>;

		static_assert(std::is_base_of_v<type, table_traits<Columns...>>);
	};
}
/// @endcond

#include "header_end.hpp"
