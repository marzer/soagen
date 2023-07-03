//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "compressed_pair.hpp"
#include "allocator.hpp"
#include "table_traits.hpp"
#include "header_start.hpp"
#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

/// \cond
namespace soagen::detail
{
	//------------------------------------------------------------------------------------------------------------------
	// generic allocation class for tracking the column pointers and the actual size in bytes
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount>
	struct table_allocation
	{
		using column_pointers		= std::byte* [ColumnCount];
		using const_column_pointers = std::byte* const[ColumnCount];

		column_pointers columns;
		size_t size_in_bytes;

		SOAGEN_PURE_INLINE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return !!columns[0];
		}

		SOAGEN_PURE_INLINE_GETTER
		/* implicit */ constexpr operator column_pointers&() noexcept
		{
			return columns;
		}

		SOAGEN_PURE_INLINE_GETTER
		/* implicit */ constexpr operator const_column_pointers&() const noexcept
		{
			return columns;
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// base class for handling most allocation-related boilerplate
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount, typename Allocator>
	class table_base
	{
		static_assert(ColumnCount, "tables must have at least one column");
		static_assert(!is_cvref<Allocator>, "allocators may not be cvref-qualified");

	  public:
		using size_type		   = size_t;
		using difference_type  = ptrdiff_t;
		using allocator_type   = Allocator;
		using allocator_traits = soagen::allocator_traits<allocator_type>;

	  protected:
		using allocation = table_allocation<ColumnCount>;

		allocation alloc_ = {};
		size_t count_	  = {};
		compressed_pair<size_t, Allocator> capacity_;

	  public:
		SOAGEN_NODISCARD_CTOR
		explicit constexpr table_base(const Allocator& alloc) noexcept //
			: capacity_{ size_t{}, alloc }
		{
			static_assert(std::is_nothrow_copy_constructible_v<Allocator>,
						  "allocators must be nothrow copy-constructible");
		}

		SOAGEN_NODISCARD_CTOR
		constexpr table_base(table_base&& other) noexcept //
			: alloc_{ std::exchange(other.alloc_, allocation{}) },
			  count_{ std::exchange(other.count_, size_t{}) },
			  capacity_{ std::exchange(other.capacity_.first(), size_t{}), static_cast<Allocator&&>(other.allocator()) }
		{
			static_assert(std::is_nothrow_move_constructible_v<Allocator>,
						  "allocators must be nothrow move-constructible");
		}

		// conditionally-implemented in specialized child classes:
		table_base()							 = delete;
		table_base& operator=(table_base&&)		 = delete;
		table_base(const table_base&)			 = delete;
		table_base& operator=(const table_base&) = delete;

		~table_base() noexcept
		{
			static_assert(std::is_nothrow_destructible_v<Allocator>, "allocators must be nothrow destructible");

			// element destructors are run in a more specialized child class

			if (alloc_)
				deallocate(alloc_);
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t size() const noexcept
		{
			return count_;
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr bool empty() const noexcept
		{
			return !count_;
		}

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t capacity() const noexcept
		{
			return capacity_.first();
		}

		SOAGEN_INLINE_GETTER
		constexpr Allocator get_allocator() const noexcept
		{
			return capacity_.second();
		}

	  protected:
		SOAGEN_INLINE_GETTER
		constexpr Allocator& allocator() noexcept
		{
			return capacity_.second();
		}

		SOAGEN_INLINE_GETTER
		constexpr const Allocator& allocator() const noexcept
		{
			return capacity_.second();
		}

		static constexpr bool allocate_is_nothrow =
			noexcept(allocator_traits::allocate(std::declval<Allocator&>(), size_t{}, std::align_val_t{}));

		constexpr allocation allocate(size_t n_bytes, size_t alignment) noexcept(allocate_is_nothrow)
		{
			SOAGEN_ASSUME(n_bytes);
			SOAGEN_ASSUME(has_single_bit(alignment));

			const auto do_alloc = [&]() noexcept(allocate_is_nothrow) -> auto*
			{
				return allocator_traits::allocate(allocator(),
												  n_bytes,
												  std::align_val_t{ max(alignment, allocator_traits::min_alignment) });
			};

#if SOAGEN_CPP >= 20
			// guard against allocators with incorrect pointer typedefs where possible
			using alloc_type = std::remove_reference_t<decltype(do_alloc())>;
#else
			using alloc_type = typename allocator_traits::pointer;
#endif

			// avoid doing reinterpret_cast where unnecessary (for constexpr)
			if constexpr (std::is_same_v<alloc_type, std::byte*>)
				return { { do_alloc() }, n_bytes };
			else
				return { { reinterpret_cast<std::byte*>(do_alloc()) }, n_bytes };
		}

		constexpr void deallocate(const allocation& al) noexcept
		{
			SOAGEN_ASSUME(al.columns[0]);
			SOAGEN_ASSUME(al.size_in_bytes);

			allocator_traits::deallocate(allocator(),
										 static_cast<typename allocator_traits::pointer>(al.columns[0]),
										 al.size_in_bytes);
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructibility
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount,
			  typename Allocator,
			  typename Base = table_base<ColumnCount, Allocator>,
			  bool			= std::is_default_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_default_construct //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FOUR(table_default_construct);

		SOAGEN_NODISCARD_CTOR
		constexpr table_default_construct() noexcept //
			: Base{ Allocator{} }
		{
			static_assert(std::is_nothrow_default_constructible_v<Allocator>,
						  "allocators must be nothrow default-constructible, or not default-constructible at all");
		}
	};

	template <size_t ColumnCount, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_default_construct<ColumnCount, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (un-typed base parts only)
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount,
			  typename Allocator,
			  typename Base = table_default_construct<ColumnCount, Allocator>,
			  bool			= !allocator_traits<Allocator>::propagate_on_container_move_assignment::value
				  || std::is_move_assignable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_move_assignable_base //
		: public Base
	{
	  protected:
		using allocation	   = table_allocation<ColumnCount>;
		using allocator_traits = allocator_traits<Allocator>;

		constexpr bool move_from_by_taking_ownership(table_move_assignable_base&& rhs) noexcept
		{
			SOAGEN_ASSUME(&rhs != this);

			const auto take_ownership = [&]() noexcept
			{
				if (Base::alloc_)
					Base::deallocate(Base::alloc_);

				Base::alloc_			= std::exchange(rhs.alloc_, allocation{});
				Base::count_			= std::exchange(rhs.count_, size_t{});
				Base::capacity_.first() = std::exchange(rhs.capacity_.first(), size_t{});

				if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
				{
					static_assert(std::is_nothrow_move_assignable_v<Allocator>,
								  "move-propagated allocators must be nothrow move-assignable");

					Base::allocator() = static_cast<Allocator&&>(rhs.allocator());
				}
			};

			if constexpr (allocator_traits::container_move_assign_always_takes_ownership)
			{
				take_ownership();
				return true;
			}
			else
			{
				if (allocator_traits::equal(Base::allocator(), rhs.allocator()))
				{
					take_ownership();
					return true;
				}

				// if they compare non-equal then we need to do elementwise move-assignment,
				// which can't be implemented at this untyped level - must be implemented as part of a more specialized
				// child class.

				return false;
			}
		}

	  public:
		using Base::Base;

		table_move_assignable_base()											 = default;
		table_move_assignable_base(table_move_assignable_base&&)				 = default;
		table_move_assignable_base(const table_move_assignable_base&)			 = default;
		table_move_assignable_base& operator=(const table_move_assignable_base&) = default;

		constexpr table_move_assignable_base& operator=(table_move_assignable_base&& rhs) noexcept
		{
			if SOAGEN_LIKELY(&rhs != this)
				move_from_by_taking_ownership(static_cast<table_move_assignable_base&&>(rhs));
			return *this;
		}
	};

	template <size_t ColumnCount, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_move_assignable_base<ColumnCount, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assignable_base);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: swap
	//------------------------------------------------------------------------------------------------------------------

	template <size_t ColumnCount,
			  typename Allocator,
			  typename Base = table_move_assignable_base<ColumnCount, Allocator>,
			  bool = !allocator_traits<Allocator>::propagate_on_container_swap::value || std::is_swappable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_swap //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);

		constexpr void swap(table_swap& other) noexcept
		{
			if SOAGEN_UNLIKELY(&other == this)
				return;

			using allocator_traits = soagen::allocator_traits<Allocator>;

			if constexpr (!allocator_traits::propagate_on_container_swap::value
						  && !allocator_traits::is_always_equal::value)
			{
				SOAGEN_ASSERT(Base::allocator() == other.allocator()
							  && "allocators must compare equal in a non-propagating swap");
			}

			using std::swap;
			swap(Base::alloc_, other.alloc_);
			swap(Base::count_, other.count_);
			swap(Base::capacity_.first(), other.capacity_.first());

			if constexpr (allocator_traits::propagate_on_container_swap::value)
			{
				static_assert(std::is_nothrow_swappable_v<Allocator>,
							  "swap-propagated allocators must be nothrow-swappable");

				swap(Base::allocator(), other.allocator());
			}
		}
	};

	template <size_t ColumnCount, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_swap<ColumnCount, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: typed base
	//
	// note: specializations from here down take the full TableTraits, not just the ColumnCount
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits,
			  typename Allocator,
			  typename Base = table_swap<TableTraits::column_count, Allocator>>
	class SOAGEN_EMPTY_BASES table_typed_base //
		: public Base
	{
	  private:
		using allocation	 = table_allocation<TableTraits::column_count>;
		using column_offsets = size_t[TableTraits::column_count + 1u]; // +1 == end of the buffer (total size)

		constexpr allocation allocate(const column_offsets& offsets) noexcept(Base::allocate_is_nothrow)
		{
			auto alloc = Base::allocate(offsets[TableTraits::column_count], TableTraits::largest_alignment);
			SOAGEN_ASSUME(alloc.columns[0]);
			SOAGEN_ASSUME(alloc.size_in_bytes == offsets[TableTraits::column_count]);

			for (size_t i = 1; i < TableTraits::column_count; i++)
			{
				alloc.columns[i] = alloc.columns[0] + offsets[i];
				SOAGEN_ASSUME(reinterpret_cast<uintptr_t>(alloc.columns[i]) % TableTraits::column_alignments[i] == 0u);
			}

			return alloc;
		}

		static constexpr void calc_column_offsets(column_offsets& offsets, size_t capacity) noexcept
		{
			SOAGEN_ASSUME(capacity % TableTraits::aligned_stride == 0u);

			// aligned offsets:
			offsets[0] = {};
			for (size_t i = 1; i < TableTraits::column_count; i++)
			{
				offsets[i] = offsets[i - 1u] + TableTraits::column_sizes[i - 1u] * capacity;
				offsets[i] = (offsets[i] + TableTraits::column_alignments[i] - 1u) //
						   & ~(TableTraits::column_alignments[i] - 1u);

				SOAGEN_ASSUME(offsets[i] % TableTraits::column_alignments[i] == 0u);
			}

			// end of buffer:
			offsets[TableTraits::column_count] = offsets[TableTraits::column_count - 1u]
											   + TableTraits::column_sizes[TableTraits::column_count - 1u] * capacity;
		}

		SOAGEN_CPP20_CONSTEXPR
		void adjust_capacity(size_t new_capacity) noexcept(
			Base::allocate_is_nothrow
			&& (TableTraits::all_nothrow_move_constructible
				|| (TableTraits::all_nothrow_default_constructible && TableTraits::all_nothrow_move_assignable)
				|| TableTraits::all_nothrow_copy_constructible
				|| (TableTraits::all_nothrow_default_constructible && TableTraits::all_nothrow_copy_assignable)))
		{
			SOAGEN_ASSUME(new_capacity);
			SOAGEN_ASSUME(new_capacity >= Base::count_);
			SOAGEN_ASSUME(new_capacity % TableTraits::aligned_stride == 0u);

			if (new_capacity == Base::capacity())
				return;

			// get new offsets
			column_offsets new_offsets{};
			calc_column_offsets(new_offsets, new_capacity);

			// alloc + move
			auto new_alloc = allocate(new_offsets);
			if (Base::count_)
			{
				SOAGEN_ASSERT(Base::alloc_);

				// this looks a bit convoluted and is probably worth an explainer:
				// we really, really, _really_ don't want reserve() and shrink_to_fit() to throw if we can avoid it,
				// since it potentially leaves the container in a sliced state (i.e. some elements were moved already
				// when an exception is raised), so we're moving or copying the elements according to the 'most nothrow'
				// path that still fufills the brief.

				if constexpr (TableTraits::all_nothrow_move_constructible)
				{
					TableTraits::move_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else if constexpr (TableTraits::all_nothrow_default_constructible
								   && TableTraits::all_nothrow_move_assignable)
				{
					TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
					TableTraits::move_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else if constexpr (TableTraits::all_nothrow_copy_constructible)
				{
					TableTraits::copy_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else if constexpr (TableTraits::all_nothrow_default_constructible
								   && TableTraits::all_nothrow_copy_assignable)
				{
					TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
					TableTraits::copy_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
				}
				else
				{
					try
					{
						if constexpr (TableTraits::all_move_constructible)
						{
							TableTraits::move_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
						else if constexpr (TableTraits::all_default_constructible && TableTraits::all_move_assignable)
						{
							TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
							TableTraits::move_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
						else if constexpr (TableTraits::all_copy_constructible)
						{
							TableTraits::copy_construct_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
						else if constexpr (TableTraits::all_default_constructible && TableTraits::all_copy_assignable)
						{
							TableTraits::default_construct_rows(new_alloc, {}, Base::count_);
							TableTraits::copy_assign_rows(new_alloc, {}, Base::alloc_, {}, Base::count_);
						}
					}
					catch (...)
					{
						Base::deallocate(new_alloc);
						throw;
					}
				}
				TableTraits::destruct_rows(Base::alloc_, {}, Base::count_);
				Base::deallocate(Base::alloc_);
			}
			Base::alloc_			= new_alloc;
			Base::capacity_.first() = new_capacity;
		}

	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_typed_base);

		~table_typed_base() noexcept
		{
			clear();
		}

		constexpr void clear() noexcept
		{
			if constexpr (!TableTraits::all_trivially_destructible)
			{
				static_assert(TableTraits::all_nothrow_destructible,
							  "column storage_types must be nothrow-destructible");

				if (Base::count_)
					TableTraits::destruct_rows(Base::alloc_, {}, Base::count_);
			}
			Base::count_ = {};
		}

		SOAGEN_CPP20_CONSTEXPR
		void reserve(size_t new_capacity) noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if (!new_capacity)
				return;

			if (const size_t rem = new_capacity % TableTraits::aligned_stride; rem > 0u)
				new_capacity += TableTraits::aligned_stride - rem;
			if (new_capacity <= Base::capacity())
				return;

			adjust_capacity(new_capacity);
		}

		SOAGEN_CPP20_CONSTEXPR
		void shrink_to_fit() noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if (!Base::count_)
			{
				if (Base::alloc_)
				{
					Base::deallocate(Base::alloc_);
					Base::alloc_			= {};
					Base::capacity_.first() = {};
				}
				return;
			}

			auto new_capacity = Base::count_;
			if (const size_t rem = new_capacity % TableTraits::aligned_stride; rem > 0u)
				new_capacity += TableTraits::aligned_stride - rem;
			if (new_capacity == Base::capacity())
				return;
			SOAGEN_ASSERT(new_capacity < Base::capacity());
			SOAGEN_ASSERT(new_capacity >= Base::count_);

			adjust_capacity(new_capacity);
		}

		SOAGEN_CPP20_CONSTEXPR
		void pop_back(size_t num = 1) noexcept
		{
			static_assert(TableTraits::all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			num = min(Base::count_, num);
			TableTraits::destruct_rows(Base::alloc_, Base::count_ - num, num);
			Base::count_ -= num;
		}

	  public:
		static constexpr size_t max_capacity = []() -> size_t
		{
			if constexpr (TableTraits::column_count == 1)
			{
				return static_cast<size_t>(-1) / TableTraits::column_sizes[0];
			}
			else
			{
				// todo: i'm sure there's a smarter way to do this

				constexpr auto capacity_ok = [](size_t cap) noexcept
				{
					constexpr auto add_without_overflowing = [](size_t lhs, size_t rhs, size_t& result) noexcept -> bool
					{
						if (lhs > static_cast<size_t>(-1) - rhs)
							return false;

						result = lhs + rhs;
						return true;
					};

					size_t buf_end = {};
					for (size_t i = 0u; i < TableTraits::column_count; i++)
					{
						if (i)
						{
							if (const size_t rem = buf_end % TableTraits::column_alignments[i - 1u]; rem > 0u)
							{
								if (!add_without_overflowing(buf_end,
															 TableTraits::column_alignments[i - 1u] - rem,
															 buf_end))
									return false;
							}
						}
						if (!add_without_overflowing(buf_end, TableTraits::column_sizes[i] * cap, buf_end))
							return false;
					}
					return true;
				};

				size_t lower  = static_cast<size_t>(-1);
				size_t higher = lower;
				while (lower && !capacity_ok(lower))
				{
					higher = lower;
					lower /= 16u;
				}
				if (!lower)
					return 0;

				while (true)
				{
					const auto midpoint = lower + (higher - lower) / 2u;
					if (capacity_ok(midpoint))
						lower = midpoint;
					else
						higher = midpoint;

					if ((higher - lower) <= 1u)
						return lower;
				}

				return 0;
			}
		}();

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t max_size() const noexcept
		{
			return max_capacity;
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructible column types
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits, //
			  typename Allocator,
			  typename Base = table_typed_base<TableTraits, Allocator>,
			  bool			= TableTraits::all_default_constructible>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);

		SOAGEN_CPP20_CONSTEXPR
		void resize(size_t num) noexcept(				   //
			TableTraits::all_nothrow_default_constructible //
				&& noexcept(this->pop_back(size_t{}))	   //
					&& noexcept(this->reserve(size_t{})))
		{
			if (Base::size() > num)
			{
				Base::pop_back(Base::size() - num);
			}
			else if (Base::size() < num)
			{
				Base::reserve(num);
				for (size_t i = Base::size(); i < num; i++)
				{
					TableTraits::default_construct_row(Base::alloc_, i);
					Base::count_++;
				}
			}
		}
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns<TableTraits, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (typed)
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits, //
			  typename Allocator,
			  typename Base = table_default_constructible_columns<TableTraits, Allocator>,
			  bool UseBase	= !std::is_move_assignable_v<Base>
						  || allocator_traits<Allocator>::container_move_assign_always_takes_ownership,
			  bool Movable = (TableTraits::all_move_assignable && TableTraits::all_move_constructible)>
	class SOAGEN_EMPTY_BASES table_move_assign //
		: public Base
	{
	  protected:
		using allocator_traits = allocator_traits<Allocator>;

	  public:
		using Base::Base;

		table_move_assign()									   = default;
		table_move_assign(table_move_assign&&)				   = default;
		table_move_assign(const table_move_assign& other)	   = default;
		table_move_assign& operator=(const table_move_assign&) = default;

		// note that this is only specialized when allocator_traits::container_move_assign_always_takes_ownership
		// is false - if it's true then the untyped base move-assign operator is already complete and no elementwise
		// moving will ever need to take place
		static_assert(!UseBase);
		static_assert(Movable);

		table_move_assign& operator=(table_move_assign&& rhs) noexcept( //
			TableTraits::all_nothrow_move_assignable					//
				&& TableTraits::all_nothrow_move_constructible			//
					&& noexcept(Base::clear())							//
						&& noexcept(Base::pop_back())					//
							&& noexcept(Base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			static_assert(!allocator_traits::propagate_on_container_move_assignment::value);
			static_assert(!allocator_traits::is_always_equal::value);

			if (Base::move_from_by_taking_ownership(static_cast<table_move_assignable_base&&>(rhs)))
				return *this;

			SOAGEN_ASSERT(!allocator_traits::equal(Base::allocator(), rhs.allocator()));

			if (rhs.empty())
			{
				Base::clear();
				return *this;
			}

			if (rhs.size() < Base::size())
				Base::pop_back(Base::size() - rhs.size());

			Base::reserve(rhs.size());

			const auto assigned_end = min(Base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				TableTraits::move_assign_row(Base::alloc_, i, rhs.alloc_, i);

			const auto constructed_end = max(Base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				TableTraits::move_construct_row(Base::alloc_, i, rhs.alloc_, i);
				Base::count_++;
			}

			return *this;
		}
	};

	template <typename TableTraits, typename Allocator, typename Base, bool Movable>
	class SOAGEN_EMPTY_BASES table_move_assign<TableTraits, Allocator, Base, /*UseBase*/ true, Movable> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assign);
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_move_assign<TableTraits, Allocator, Base, /*UseBase*/ false, /*Movable*/ false> //
		: public Base
	{
	  public:
		using Base::Base;

		table_move_assign()									   = default;
		table_move_assign(table_move_assign&&)				   = default;
		table_move_assign(const table_move_assign&)			   = default;
		table_move_assign& operator=(const table_move_assign&) = default;

		// base operator is insufficient but the elements aren't movable so no choice but to explicitly delete
		table_move_assign& operator=(table_move_assign&&) = delete;
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: copy-construct
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits, //
			  typename Allocator,
			  typename Base = table_move_assign<TableTraits, Allocator>,
			  bool			= TableTraits::all_copy_constructible&& std::is_copy_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_copy_construct //
		: public Base
	{
	  protected:
		using allocator_traits = allocator_traits<Allocator>;

	  public:
		using Base::Base;

		table_copy_construct()										 = default;
		table_copy_construct(table_copy_construct&&)				 = default;
		table_copy_construct& operator=(table_copy_construct&&)		 = default;
		table_copy_construct& operator=(const table_copy_construct&) = default;

		table_copy_construct(const table_copy_construct& other) noexcept(TableTraits::all_nothrow_copy_constructible //
																			 && noexcept(Base::reserve(size_t{})))
			: Base{ allocator_traits::select_on_container_copy_construction(Base::allocator()) }
		{
			static_assert(std::is_nothrow_copy_constructible_v<Allocator>,
						  "allocators must be nothrow copy-constructible");

			Base::reserve(other.size());
			TableTraits::copy_construct_rows(Base::alloc_, {}, other.alloc_, {}, other.size());
			Base::count_ = other.size();
		}
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_copy_construct<TableTraits, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: copy-assign
	//------------------------------------------------------------------------------------------------------------------

	template <typename TableTraits,
			  typename Allocator,
			  typename Base = table_copy_construct<TableTraits, Allocator>,
			  bool			= TableTraits::all_copy_assignable	//
						 && TableTraits::all_copy_constructible //
				  && (!allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
					  || std::is_copy_assignable_v<Allocator>)>
	class SOAGEN_EMPTY_BASES table_copy_assign //
		: public Base
	{
	  protected:
		using allocator_traits = allocator_traits<Allocator>;

	  public:
		using Base::Base;

		table_copy_assign()								  = default;
		table_copy_assign(table_copy_assign&&)			  = default;
		table_copy_assign& operator=(table_copy_assign&&) = default;
		table_copy_assign(const table_copy_assign&)		  = default;

		constexpr table_copy_assign& operator=(const table_copy_assign& rhs) noexcept( // wtb: noexcept(auto)
			TableTraits::all_nothrow_copy_assignable								   //
				&& TableTraits::all_nothrow_copy_constructible						   //
					&& noexcept(Base::clear())										   //
						&& noexcept(Base::pop_back())								   //
							&& noexcept(Base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			if constexpr (allocator_traits::propagate_on_container_copy_assignment::value
						  || !allocator_traits::is_always_equal::value)
			{
				if (!allocator_traits::equal(Base::allocator(), rhs.allocator()))
				{
					Base::clear();
					Base::shink_to_fit();
					SOAGEN_ASSERT(Base::empty());
					SOAGEN_ASSERT(!Base::alloc_);
				}
			}

			if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
			{
				static_assert(std::is_nothrow_copy_assignable_v<Allocator>,
							  "copy-propagated allocators must be nothrow copy-assignable");

				Base::allocator() = rhs.allocator();
			}

			if (rhs.empty())
			{
				Base::clear();
				return *this;
			}

			if (rhs.size() < Base::size())
				Base::pop_back(Base::size() - rhs.size());

			Base::reserve(rhs.size());

			const auto assigned_end = min(Base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				TableTraits::copy_assign_row(Base::alloc_, i, rhs.alloc_, i);

			const auto constructed_end = max(Base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				TableTraits::copy_construct_row(Base::alloc_, i, rhs.alloc_, i);
				Base::count_++;
			}

			return *this;
		}
	};

	template <typename TableTraits, typename Allocator, typename Base>
	class SOAGEN_EMPTY_BASES table_copy_assign<TableTraits, Allocator, Base, false> //
		: public Base
	{
	  public:
		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_assign);
	};

	//------------------------------------------------------------------------------------------------------------------
	// misc
	//------------------------------------------------------------------------------------------------------------------

	// trait for determining the _actual_ alignment of a table column, taking the allocator and table-allocation
	// semantics into account (since the full allocation for a table is always has alignment ==
	// table_traits::largest_alignment).
	//
	// note that this has absolutely nothing to do with the aligned_stride; that is still calculated according to
	// the user's specified alignment requirements. this trait is _only_ used to help the compiler via assume_aligned.
	template <typename TableTraits, typename Allocator, size_t ColumnIndex>
	inline constexpr size_t actual_column_alignment = TableTraits::template column<ColumnIndex>::alignment;

	template <typename TableTraits, typename Allocator>
	inline constexpr size_t actual_column_alignment<TableTraits, Allocator, 0> =
		max(TableTraits::template column<0>::alignment,
			allocator_traits<Allocator>::min_alignment,
			TableTraits::largest_alignment);
}
/// \endcond

namespace soagen
{
	template <typename TableTraits,
			  typename Allocator = allocator,
			  typename Base		 = detail::table_copy_assign<TableTraits, Allocator>>
	class SOAGEN_EMPTY_BASES table //
		: public Base
	{
		static_assert(is_table_traits<TableTraits>, "TableTraits must be an instance of soagen::table_traits");
		static_assert(!is_cvref<TableTraits>, "table traits may not be cvref-qualified");
		static_assert(!is_cvref<Allocator>, "allocators may not be cvref-qualified");

	  public:
		using size_type		   = size_t;
		using difference_type  = ptrdiff_t;
		using allocator_type   = Allocator;
		using allocator_traits = allocator_traits<Allocator>;
		using table_traits	   = TableTraits;

		template <size_type I>
		using column_traits = typename table_traits::template column<I>;

		template <size_type I>
		using column_type = typename column_traits<I>::value_type;

		using Base::Base;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table);

		template <size_t I>
		SOAGEN_ALIGNED_COLUMN(I)
		column_type<I>* column() noexcept
		{
			static_assert(I < table_traits::column_count, "column index out of range");

			return soagen::assume_aligned<detail::actual_column_alignment<table_traits, allocator_type, I>>(
				SOAGEN_LAUNDER(reinterpret_cast<column_type<I>*>(Base::alloc_.columns[I])));
		}

		template <size_t I>
		SOAGEN_ALIGNED_COLUMN(I)
		std::add_const_t<column_type<I>>* column() const noexcept
		{
			static_assert(I < table_traits::column_count, "column index out of range");

			return soagen::assume_aligned<detail::actual_column_alignment<table_traits, allocator_type, I>>(
				SOAGEN_LAUNDER(reinterpret_cast<column_type<I>*>(Base::alloc_.columns[I])));
		}
	};

	template <typename>
	inline constexpr bool is_table = false;

	template <typename... Args>
	inline constexpr bool is_table<table<Args...>> = true;

	template <typename... Args>
	inline constexpr bool is_soa<table<Args...>> = true;

	SOAGEN_CONSTRAINED_TEMPLATE((has_swap_member<table<Traits, Alloc>>), typename Traits, typename Alloc)
	SOAGEN_ALWAYS_INLINE
	constexpr void swap(table<Traits, Alloc>& lhs, table<Traits, Alloc>& rhs) //
		noexcept(noexcept(std::declval<table<Traits, Alloc>&>().swap(std::declval<table<Traits, Alloc>&>())))
	{
		lhs.swap(rhs);
	}
}

#include "header_end.hpp"
