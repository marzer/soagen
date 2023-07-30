//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "generated/compressed_pair.hpp"
#include "allocator.hpp"
#include "table_traits.hpp"
#include "header_start.hpp"

#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

/// @cond
namespace soagen::detail
{
	SOAGEN_CONSTRAINED_TEMPLATE(is_unsigned<T>, typename T)
	[[nodiscard]]
	constexpr bool add_without_overflowing(T lhs, T rhs, T& result) noexcept
	{
		if (lhs > static_cast<T>(-1) - rhs)
			return false;

		result = lhs + rhs;
		return true;
	};

	// trait for determining the _actual_ alignment of a table column, taking the allocator and
	// table-allocation semantics into account (since the full allocation for a table always has
	// alignment == table_traits::largest_alignment).
	//
	// note that this has absolutely nothing to do with the aligned_stride; that is still calculated
	// according to the user's specified alignment requirements. this trait is _only_ used
	// to help the compiler via assume_aligned.
	template <typename Traits, typename Allocator, size_t ColumnIndex>
	inline constexpr size_t actual_column_alignment = Traits::template column<ColumnIndex>::alignment;

	template <typename Traits, typename Allocator>
	inline constexpr size_t actual_column_alignment<Traits, Allocator, 0> =
		max(Traits::template column<0>::alignment,
			allocator_traits<Allocator>::min_alignment,
			Traits::largest_alignment);

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
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;
		using allocator_type  = Allocator;

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
		explicit constexpr table_base(Allocator&& alloc) noexcept //
			: capacity_{ size_t{}, static_cast<Allocator&&>(alloc) }
		{
			static_assert(std::is_nothrow_move_constructible_v<Allocator>,
						  "allocators must be nothrow move-constructible");
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

		SOAGEN_PURE_INLINE_GETTER
		constexpr size_t allocation_size() const noexcept
		{
			return alloc_.size_in_bytes;
		}

		SOAGEN_INLINE_GETTER
		SOAGEN_CPP20_CONSTEXPR
		Allocator get_allocator() const noexcept
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
			noexcept(allocator_traits<Allocator>::allocate(std::declval<Allocator&>(), size_t{}, std::align_val_t{}));

		// guard against allocators with incorrect pointer typedefs where possible
		using allocator_pointer_type = std::remove_reference_t<
			decltype(allocator_traits<Allocator>::allocate(std::declval<Allocator&>(), size_t{}, std::align_val_t{}))>;

		SOAGEN_NODISCARD
		constexpr allocation allocate(size_t n_bytes, size_t alignment) noexcept(allocate_is_nothrow)
		{
			SOAGEN_ASSUME(n_bytes);
			SOAGEN_ASSUME((static_cast<size_t>(alignment) & (static_cast<size_t>(alignment) - 1u)) == 0u);

			const auto ptr = soagen::assume_aligned<allocator_traits<Allocator>::min_alignment>(
				allocator_traits<Allocator>::allocate(
					allocator(),
					n_bytes,
					std::align_val_t{ max(alignment, allocator_traits<Allocator>::min_alignment) }));
			SOAGEN_ASSUME(ptr != nullptr);
			std::memset(ptr, 0, n_bytes);

			if constexpr (std::is_same_v<allocator_pointer_type, std::byte*>)
				return { { ptr }, n_bytes };
			else
				return { { reinterpret_cast<std::byte*>(ptr) }, n_bytes };
		}

		constexpr void deallocate(const allocation& al) noexcept
		{
			SOAGEN_ASSUME(al.columns[0]);
			SOAGEN_ASSUME(al.size_in_bytes);

			if constexpr (std::is_same_v<allocator_pointer_type, std::byte*>)
				allocator_traits<Allocator>::deallocate(allocator(), al.columns[0], al.size_in_bytes);
			else
				allocator_traits<Allocator>::deallocate(allocator(),
														reinterpret_cast<allocator_pointer_type>(al.columns[0]),
														al.size_in_bytes);
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructibility
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_base
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE SOAGEN_BASE_NAME<ColumnCount, Allocator>

	template <size_t ColumnCount,
			  typename Allocator,
			  bool = std::is_default_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_default_construct //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FOUR(table_default_construct);

		SOAGEN_NODISCARD_CTOR
		constexpr table_default_construct() noexcept //
			: SOAGEN_BASE_TYPE{ Allocator{} }
		{
			static_assert(std::is_nothrow_default_constructible_v<Allocator>,
						  "allocators must be nothrow default-constructible, or not default-constructible at all");
		}
	};

	template <size_t ColumnCount, typename Allocator>
	class SOAGEN_EMPTY_BASES table_default_construct<ColumnCount, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (un-typed base parts only)
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_default_construct

	template <size_t ColumnCount,
			  typename Allocator,
			  bool = !allocator_traits<Allocator>::propagate_on_container_move_assignment::value
				  || std::is_move_assignable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_move_assignable_base //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  protected:
		using allocation = table_allocation<ColumnCount>;

		constexpr bool move_from_by_taking_ownership(table_move_assignable_base&& rhs) noexcept
		{
			SOAGEN_ASSUME(&rhs != this);

			const auto take_ownership = [&]() noexcept
			{
				if (base::alloc_)
					base::deallocate(base::alloc_);

				base::alloc_			= std::exchange(rhs.alloc_, allocation{});
				base::count_			= std::exchange(rhs.count_, size_t{});
				base::capacity_.first() = std::exchange(rhs.capacity_.first(), size_t{});

				if constexpr (allocator_traits<Allocator>::propagate_on_container_move_assignment::value)
				{
					static_assert(std::is_nothrow_move_assignable_v<Allocator>,
								  "move-propagated allocators must be nothrow move-assignable");

					base::allocator() = static_cast<Allocator&&>(rhs.allocator());
				}
			};

			if constexpr (allocator_traits<Allocator>::container_move_assign_always_takes_ownership)
			{
				take_ownership();
				return true;
			}
			else
			{
				if (allocator_traits<Allocator>::equal(base::allocator(), rhs.allocator()))
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

		template <typename T>
		constexpr bool move_from_by_taking_ownership(T&& rhs, std::true_type) noexcept
		{
			return move_from_by_taking_ownership(static_cast<table_move_assignable_base&&>(rhs));
		}

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

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

	template <size_t ColumnCount, typename Allocator>
	class SOAGEN_EMPTY_BASES table_move_assignable_base<ColumnCount, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assignable_base);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: swap
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_move_assignable_base

	template <size_t ColumnCount,
			  typename Allocator,
			  bool = !allocator_traits<Allocator>::propagate_on_container_swap::value || std::is_swappable_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_swap //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);

		constexpr void swap(table_swap& other) noexcept
		{
			if SOAGEN_UNLIKELY(&other == this)
				return;

			if constexpr (!allocator_traits<Allocator>::propagate_on_container_swap::value
						  && !allocator_traits<Allocator>::is_always_equal::value)
			{
				SOAGEN_ASSERT(base::allocator() == other.allocator()
							  && "allocators must compare equal in a non-propagating swap");
			}

			using std::swap;
			swap(base::alloc_, other.alloc_);
			swap(base::count_, other.count_);
			swap(base::capacity_.first(), other.capacity_.first());

			if constexpr (allocator_traits<Allocator>::propagate_on_container_swap::value)
			{
				static_assert(std::is_nothrow_swappable_v<Allocator>,
							  "swap-propagated allocators must be nothrow-swappable");

				swap(base::allocator(), other.allocator());
			}
		}
	};

	template <size_t ColumnCount, typename Allocator>
	class SOAGEN_EMPTY_BASES table_swap<ColumnCount, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_swap);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: typed base
	//
	// note: specializations from here down take the full Traits, not just the ColumnCount
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_swap
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE SOAGEN_BASE_NAME<Traits::column_count, Allocator>

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_typed_base //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using allocation  = table_allocation<Traits::column_count>;
		using column_ends = size_t[Traits::column_count];
		using base		  = SOAGEN_BASE_TYPE;

		static constexpr void calc_column_ends(column_ends& ends, size_t capacity) noexcept
		{
			// pad ends so the next column starts at the right alignment for the storage type
			size_t prev = {};
			for (size_t i = 0; i < Traits::column_count - 1u; i++)
			{
				ends[i] = prev + Traits::column_sizes[i] * capacity;
				ends[i] = (ends[i] + Traits::column_alignments[i + 1u] - 1u) //
						& ~(Traits::column_alignments[i + 1u] - 1u);

				SOAGEN_ASSUME(ends[i] % Traits::column_alignments[i + 1u] == 0u);
				prev = ends[i];
			}

			// last end doesn't need to be aligned (it's just the total buffer size)
			ends[Traits::column_count - 1u] = prev + Traits::column_sizes[Traits::column_count - 1u] * capacity;
		}

		constexpr allocation allocate(const column_ends& ends) noexcept(base::allocate_is_nothrow)
		{
			SOAGEN_ASSUME(ends[Traits::column_count - 1u]);

			auto alloc = base::allocate(ends[Traits::column_count - 1u], Traits::largest_alignment);
			SOAGEN_ASSUME(alloc.columns[0]);
			SOAGEN_ASSUME(alloc.size_in_bytes == ends[Traits::column_count - 1u]);

			for (size_t i = 1; i < Traits::column_count; i++)
			{
				alloc.columns[i] = alloc.columns[0] + ends[i - 1u];
				SOAGEN_ASSUME(reinterpret_cast<uintptr_t>(alloc.columns[i]) % Traits::column_alignments[i] == 0u);
			}

			return alloc;
		}

		SOAGEN_CPP20_CONSTEXPR
		void adjust_capacity(size_t new_capacity) noexcept(
			base::allocate_is_nothrow
			&& (Traits::all_nothrow_move_constructible
				|| (Traits::all_nothrow_default_constructible && Traits::all_nothrow_move_assignable)
				|| Traits::all_nothrow_copy_constructible
				|| (Traits::all_nothrow_default_constructible && Traits::all_nothrow_copy_assignable)))
		{
			SOAGEN_ASSUME(new_capacity);
			SOAGEN_ASSUME(new_capacity >= base::count_);

			if (new_capacity == base::capacity())
				return;

			// get new ends
			column_ends new_ends{};
			calc_column_ends(new_ends, new_capacity);

			// alloc + move
			auto new_alloc = allocate(new_ends);
			SOAGEN_ASSERT(new_alloc);
			if (base::count_)
			{
				SOAGEN_ASSERT(base::alloc_);

				// this looks a bit convoluted and is probably worth an explainer:
				// we really, really, _really_ don't want reserve() and shrink_to_fit() to throw if we can avoid it,
				// since it potentially leaves the container in a sliced state (i.e. some elements were moved already
				// when an exception is raised), so we're moving or copying the elements according to the 'most nothrow'
				// path that still fulfills the brief.

				if constexpr (Traits::all_nothrow_move_constructible)
				{
					Traits::move_construct_rows(new_alloc.columns, {}, base::alloc_.columns, {}, base::count_);
				}
				else if constexpr (Traits::all_nothrow_default_constructible && Traits::all_nothrow_move_assignable)
				{
					Traits::default_construct_rows(new_alloc.columns, {}, base::count_);
					Traits::move_assign_rows(new_alloc.columns, {}, base::alloc_.columns, {}, base::count_);
				}
				else if constexpr (Traits::all_nothrow_copy_constructible)
				{
					Traits::copy_construct_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
				}
				else if constexpr (Traits::all_nothrow_default_constructible && Traits::all_nothrow_copy_assignable)
				{
					Traits::default_construct_rows(new_alloc, {}, base::count_);
					Traits::copy_assign_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
				}
				else
				{
					[[maybe_unused]] bool needs_destruct = false;
					try
					{
						if constexpr (Traits::all_move_constructible)
						{
							Traits::move_construct_rows(new_alloc,
														{},
														base::alloc_.columns,
														{},
														base::count_); // strong
						}
						else if constexpr (Traits::all_default_constructible && Traits::all_move_assignable)
						{
							Traits::default_construct_rows(new_alloc, {}, base::count_); // strong
							needs_destruct = true;
							Traits::move_assign_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
						}
						else if constexpr (Traits::all_copy_constructible)
						{
							Traits::copy_construct_rows(new_alloc,
														{},
														base::alloc_.columns,
														{},
														base::count_); // strong
						}
						else if constexpr (Traits::all_default_constructible && Traits::all_copy_assignable)
						{
							Traits::default_construct_rows(new_alloc, {}, base::count_); // strong
							needs_destruct = true;
							Traits::copy_assign_rows(new_alloc, {}, base::alloc_.columns, {}, base::count_);
						}
					}
					catch (...)
					{
						if (needs_destruct)
							Traits::destruct_rows(new_alloc, {}, base::count_);
						base::deallocate(new_alloc);
						throw;
					}
				}
				Traits::destruct_rows(base::alloc_.columns, {}, base::count_);
				base::deallocate(base::alloc_);
			}
			base::alloc_			= new_alloc;
			base::capacity_.first() = new_capacity;
		}

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_typed_base);

		~table_typed_base() noexcept
		{
			clear();
		}

		constexpr void clear() noexcept
		{
			if constexpr (!Traits::all_trivially_destructible)
			{
				static_assert(Traits::all_nothrow_destructible, "column storage_types must be nothrow-destructible");

				if (base::count_)
					Traits::destruct_rows(base::alloc_.columns, {}, base::count_);
			}
			base::count_ = {};
		}

		SOAGEN_CPP20_CONSTEXPR
		void reserve(size_t new_capacity) noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if (!new_capacity)
				return;

			if constexpr (Traits::aligned_stride > 1)
			{
				if (const size_t rem = new_capacity % Traits::aligned_stride; rem > 0u)
				{
					static_cast<void>(
						add_without_overflowing(new_capacity, Traits::aligned_stride - rem, new_capacity));
				}
			}
			if (new_capacity <= base::capacity())
				return;

			adjust_capacity(new_capacity);
		}

		SOAGEN_CPP20_CONSTEXPR
		void shrink_to_fit() noexcept(noexcept(this->adjust_capacity(size_t{})))
		{
			if (!base::count_)
			{
				if (base::alloc_)
				{
					base::deallocate(base::alloc_);
					base::alloc_			= {};
					base::capacity_.first() = {};
				}
				return;
			}

			auto new_capacity = base::count_;
			if constexpr (Traits::aligned_stride > 1)
			{
				if (const size_t rem = new_capacity % Traits::aligned_stride; rem > 0u)
				{
					static_cast<void>(
						add_without_overflowing(new_capacity, Traits::aligned_stride - rem, new_capacity));
				}
			}
			if (new_capacity >= base::capacity())
				return;

			adjust_capacity(new_capacity);
		}

		SOAGEN_CPP20_CONSTEXPR
		void pop_back(size_t num = 1) noexcept
		{
			static_assert(Traits::all_nothrow_destructible, "column storage_types must be nothrow-destructible");

			num = min(base::count_, num);
			Traits::destruct_rows(base::alloc_.columns, base::count_ - num, num);
			base::count_ -= num;
		}

	  public:
		static constexpr size_t max_capacity = []() -> size_t
		{
			if constexpr (Traits::column_count == 1)
			{
				return static_cast<size_t>(-1) / Traits::column_sizes[0];
			}
			else
			{
				// todo: i'm sure there's a smarter way to do this

				constexpr auto capacity_ok = [](size_t cap) noexcept
				{
					size_t buf_end = {};
					for (size_t i = 0u; i < Traits::column_count; i++)
					{
						if (i)
						{
							if (const size_t rem = buf_end % Traits::column_alignments[i - 1u]; rem > 0u)
							{
								if (!add_without_overflowing(buf_end, Traits::column_alignments[i - 1u] - rem, buf_end))
									return false;
							}
						}
						if (!add_without_overflowing(buf_end, Traits::column_sizes[i] * cap, buf_end))
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

	  private:
		SOAGEN_CPP20_CONSTEXPR
		void grow_if_necessary(size_t new_elements)
		{
			SOAGEN_ASSUME(new_elements);

			// determine the minimum required allocation size to store the new elements.
			// if this calculation would overflow or the result would exceed max_capacity then we've
			// hit system limits and have no choice but to raise std::bad_alloc.
			auto new_size = base::count_;
			if SOAGEN_UNLIKELY(!add_without_overflowing(new_size, new_elements, new_size) || new_size > max_capacity)
				throw std::bad_alloc{};

			// already enough capacity, no work to do.
			if (new_size <= base::capacity())
				return;

			// again we need to check system limits, this time when applying the growth factor.
			// if we overflow or would exceed the max capacity then clamp the result to max_capacity
			// (the user is right on the edge of hitting system limits and will hit them at the next growth)
			auto new_cap = new_size;
			if SOAGEN_UNLIKELY(!add_without_overflowing(new_cap, new_cap, new_cap) || new_cap > max_capacity)
				new_cap = max_capacity;

			reserve(new_cap);
		}

	  public:
		SOAGEN_CONSTRAINED_TEMPLATE(Traits::template row_constructible_from<Args&&...>, typename... Args)
		SOAGEN_CPP20_CONSTEXPR
		void emplace_back(Args&&... args) noexcept(					   //
			Traits::template row_nothrow_constructible_from<Args&&...> //
				&& noexcept(this->grow_if_necessary(size_t{})))
		{
			grow_if_necessary(1u);
			Traits::construct_row(base::alloc_.columns, base::count_, static_cast<Args&&>(args)...);
			base::count_++;
		}

		SOAGEN_CONSTRAINED_TEMPLATE(Traits::all_move_constructible	   //
										&& Traits::all_move_assignable //
											&& Traits::template row_constructible_from<Args&&...>,
									typename... Args)
		SOAGEN_CPP20_CONSTEXPR
		void emplace(size_t position, Args&&... args) noexcept(					  //
			Traits::all_nothrow_move_constructible								  //
				&& Traits::all_nothrow_move_assignable							  //
					&& Traits::template row_nothrow_constructible_from<Args&&...> //
						&& noexcept(this->grow_if_necessary(size_t{})))
		{
			SOAGEN_ASSUME(position <= base::count_);

			// delegating to emplace_back for the end position is ideal
			// because emplace_back never needs to move+destruct elements (modulo re-allocation)
			if (position == base::count_)
			{
				emplace_back(static_cast<Args&&>(args)...);
				return;
			}

			// move construct the last element (since it is new)
			grow_if_necessary(1u);
			Traits::move_construct_row(base::alloc_.columns, base::count_, base::alloc_.columns, base::count_ - 1u);

			// move-assign the rest of the elements (they were already alive)
			if (position + 1u < base::count_)
			{
				Traits::move_assign_rows(base::alloc_.columns,
										 position + 1u,
										 base::alloc_.columns,
										 position,
										 base::count_ - position - 1u);
			}

			// todo: there might be some inputs that allow us to move-assign instead of destruct+construct
			Traits::destruct_row(base::alloc_.columns, position);
			Traits::construct_row(base::alloc_.columns, position, static_cast<Args&&>(args)...);

			base::count_++;
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: default-constructible column types
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_typed_base
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE SOAGEN_BASE_NAME<Traits, Allocator>

	template <typename Traits, //
			  typename Allocator,
			  bool = Traits::all_default_constructible>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);

		SOAGEN_CPP20_CONSTEXPR
		void resize(size_t num) noexcept(			  //
			Traits::all_nothrow_default_constructible //
				&& noexcept(this->pop_back(size_t{})) //
					&& noexcept(this->reserve(size_t{})))
		{
			if (base::size() > num)
			{
				base::pop_back(base::size() - num);
			}
			else if (base::size() < num)
			{
				base::reserve(num);
				for (size_t i = base::size(); i < num; i++)
				{
					Traits::default_construct_row(base::alloc_.columns, i);
					base::count_++;
				}
			}
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_default_constructible_columns<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_default_constructible_columns);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: move-assignment (typed)
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_default_constructible_columns

	template <typename Traits, //
			  typename Allocator,
			  bool UseBase = !std::is_move_assignable_v<SOAGEN_BASE_TYPE>
						  || allocator_traits<Allocator>::container_move_assign_always_takes_ownership,
			  bool Movable = (Traits::all_move_assignable && Traits::all_move_constructible)>
	class SOAGEN_EMPTY_BASES table_move_assign //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

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
			Traits::all_nothrow_move_assignable							//
				&& Traits::all_nothrow_move_constructible				//
					&& noexcept(base::clear())							//
						&& noexcept(base::pop_back())					//
							&& noexcept(base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			static_assert(!allocator_traits<Allocator>::propagate_on_container_move_assignment::value);
			static_assert(!allocator_traits<Allocator>::is_always_equal::value);

			if (base::move_from_by_taking_ownership(rhs, std::true_type{}))
				return *this;

			SOAGEN_ASSERT(!allocator_traits<Allocator>::equal(base::allocator(), rhs.allocator()));

			if (rhs.empty())
			{
				base::clear();
				return *this;
			}

			if (rhs.size() < base::size())
				base::pop_back(base::size() - rhs.size());

			base::reserve(rhs.size());

			const auto assigned_end = min(base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				Traits::move_assign_row(base::alloc_.columns, i, rhs.alloc_.columns, i);

			const auto constructed_end = max(base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				Traits::move_construct_row(base::alloc_.columns, i, rhs.alloc_.columns, i);
				base::count_++;
			}

			return *this;
		}
	};

	template <typename Traits, typename Allocator, bool Movable>
	class SOAGEN_EMPTY_BASES table_move_assign<Traits, Allocator, /*UseBase*/ true, Movable> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_move_assign);
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_move_assign<Traits, Allocator, /*UseBase*/ false, /*Movable*/ false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

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

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_move_assign

	template <typename Traits, //
			  typename Allocator,
			  bool = Traits::all_copy_constructible&& std::is_copy_constructible_v<Allocator>>
	class SOAGEN_EMPTY_BASES table_copy_construct //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		table_copy_construct()										 = default;
		table_copy_construct(table_copy_construct&&)				 = default;
		table_copy_construct& operator=(table_copy_construct&&)		 = default;
		table_copy_construct& operator=(const table_copy_construct&) = default;

		table_copy_construct(const table_copy_construct& other) noexcept(Traits::all_nothrow_copy_constructible //
																			 && noexcept(base::reserve(size_t{})))
			: base{ allocator_traits<Allocator>::select_on_container_copy_construction(base::allocator()) }
		{
			static_assert(std::is_nothrow_copy_constructible_v<Allocator>,
						  "allocators must be nothrow copy-constructible");

			base::reserve(other.size());
			Traits::copy_construct_rows(base::alloc_.columns, {}, other.alloc_.columns, {}, other.size());
			base::count_ = other.size();
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_copy_construct<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_construct);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: copy-assign
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_copy_construct

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_copy_assignable		   //
						 && Traits::all_copy_constructible //
				  && (!allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
					  || std::is_copy_assignable_v<Allocator>)>
	class SOAGEN_EMPTY_BASES table_copy_assign //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		table_copy_assign()								  = default;
		table_copy_assign(table_copy_assign&&)			  = default;
		table_copy_assign& operator=(table_copy_assign&&) = default;
		table_copy_assign(const table_copy_assign&)		  = default;

		constexpr table_copy_assign& operator=(const table_copy_assign& rhs) noexcept( // wtb: noexcept(auto)
			Traits::all_nothrow_copy_assignable										   //
				&& Traits::all_nothrow_copy_constructible							   //
					&& noexcept(base::clear())										   //
						&& noexcept(base::pop_back())								   //
							&& noexcept(base::reserve(size_t{})))
		{
			if SOAGEN_UNLIKELY(&rhs == this)
				return *this;

			if constexpr (allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
						  || !allocator_traits<Allocator>::is_always_equal::value)
			{
				if (!allocator_traits<Allocator>::equal(base::allocator(), rhs.allocator()))
				{
					base::clear();
					base::shink_to_fit();
					SOAGEN_ASSERT(base::empty());
					SOAGEN_ASSERT(!base::alloc_);
				}
			}

			if constexpr (allocator_traits<Allocator>::propagate_on_container_copy_assignment::value)
			{
				static_assert(std::is_nothrow_copy_assignable_v<Allocator>,
							  "copy-propagated allocators must be nothrow copy-assignable");

				base::allocator() = rhs.allocator();
			}

			if (rhs.empty())
			{
				base::clear();
				return *this;
			}

			if (rhs.size() < base::size())
				base::pop_back(base::size() - rhs.size());

			base::reserve(rhs.size());

			const auto assigned_end = min(base::size(), rhs.size());
			for (size_t i = 0; i < assigned_end; i++)
				Traits::copy_assign_row(base::alloc_.columns, i, rhs.alloc_.columns, i);

			const auto constructed_end = max(base::size(), rhs.size());
			for (size_t i = assigned_end; i < constructed_end; i++)
			{
				Traits::copy_construct_row(base::alloc_.columns, i, rhs.alloc_.columns, i);
				base::count_++;
			}

			return *this;
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_copy_assign<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_copy_assign);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: row erasure
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_copy_assign

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_move_assignable>
	class SOAGEN_EMPTY_BASES table_row_erasure //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure);

		void erase(size_t pos)							 //
			noexcept(Traits::all_nothrow_move_assignable //
						 && noexcept(base::pop_back()))
		{
			SOAGEN_ASSUME(pos < base::count_);

			if (pos + 1u < base::count_)
				Traits::move_assign_rows(base::alloc_.columns,
										 pos,
										 base::alloc_.columns,
										 pos + 1u,
										 base::count_ - pos - 1u);

			base::pop_back();
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_row_erasure<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: row erasure (unordered)
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_row_erasure

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_swappable>
	class SOAGEN_EMPTY_BASES table_row_erasure_unordered //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure_unordered);

		soagen::optional<size_t> unordered_erase(size_t pos) //
			noexcept(Traits::all_nothrow_swappable			 //
						 && noexcept(base::pop_back()))
		{
			SOAGEN_ASSUME(pos < base::count_);

			if (pos + 1u == base::count_)
			{
				base::pop_back();
				return {};
			}

			Traits::swap_rows(base::alloc_.columns, pos, base::alloc_.columns, base::count_ - 1u);
			base::pop_back();
			return soagen::optional<size_t>{ base::count_ };
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_row_erasure_unordered<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_row_erasure_unordered);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: EqualityComparable
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_row_erasure_unordered

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_equality_comparable>
	class SOAGEN_EMPTY_BASES table_equality_compare //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_equality_compare);

		SOAGEN_NODISCARD
		friend constexpr bool operator==(const table_equality_compare& lhs,
										 const table_equality_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			if (&lhs == &rhs || (lhs.empty() && rhs.empty()))
				return true;

			if (lhs.size() != rhs.size())
				return false;

			return Traits::equal(lhs.alloc_.columns, {}, rhs.alloc_.columns, {}, lhs.size());
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator!=(const table_equality_compare& lhs,
										 const table_equality_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return !(lhs == rhs);
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_equality_compare<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_equality_compare);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: LessThanComparable
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_equality_compare

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_less_than_comparable>
	class SOAGEN_EMPTY_BASES table_less_than_compare //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_less_than_compare);

		SOAGEN_NODISCARD
		constexpr int compare(const table_less_than_compare& rhs) const //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			if (&rhs == this)
				return 0;
			if (base::empty())
				return rhs.empty() ? 0 : -1;
			if (rhs.empty())
				return 1;

			const auto comp = Traits::compare(base::alloc_.columns, //
											  {},
											  rhs.alloc_.columns,
											  {},
											  min(base::size(), rhs.size()));

			return comp != 0 ? comp : (base::size() == rhs.size() ? 0 : (base::size() < rhs.size() ? -1 : 1));
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<(const table_less_than_compare& lhs,
										const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) < 0;
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator<=(const table_less_than_compare& lhs,
										 const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) <= 0;
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>(const table_less_than_compare& lhs,
										const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) > 0;
		}

		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		friend constexpr bool operator>=(const table_less_than_compare& lhs,
										 const table_less_than_compare& rhs) //
			noexcept(Traits::all_nothrow_equality_comparable)
		{
			return lhs.compare(rhs) >= 0;
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_less_than_compare<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_less_than_compare);
	};

	//------------------------------------------------------------------------------------------------------------------
	// specialization: data()
	//------------------------------------------------------------------------------------------------------------------

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_less_than_compare

	template <typename Traits,
			  typename Allocator,
			  bool = Traits::all_trivially_copyable>
	class SOAGEN_EMPTY_BASES table_data_ptr //
		: public SOAGEN_BASE_TYPE
	{
	  private:
		using base = SOAGEN_BASE_TYPE;

	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_data_ptr);

		using table_traits	 = Traits;
		using allocator_type = Allocator;

		SOAGEN_ALIGNED_COLUMN(0)
		constexpr std::byte* data() noexcept
		{
			return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, 0>>(
				base::alloc_.columns[0]);
		}

		SOAGEN_ALIGNED_COLUMN(0)
		constexpr const std::byte* data() const noexcept
		{
			return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, 0>>(
				base::alloc_.columns[0]);
		}
	};

	template <typename Traits, typename Allocator>
	class SOAGEN_EMPTY_BASES table_data_ptr<Traits, Allocator, false> //
		: public SOAGEN_BASE_TYPE
	{
	  public:
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;

		SOAGEN_DEFAULT_RULE_OF_FIVE(table_data_ptr);
	};
}

#undef SOAGEN_BASE_NAME
#define SOAGEN_BASE_NAME table_data_ptr
#undef SOAGEN_BASE_TYPE
#define SOAGEN_BASE_TYPE detail::SOAGEN_BASE_NAME<Traits, Allocator>

/// @endcond

namespace soagen
{
	/// @brief		A table.
	/// @details	Effectively a multi-column std::vector.
	/// @tparam		Traits		The #soagen::table_traits for the table.
	/// @tparam		Allocator	The allocator used by the table.
	///
	///	@attention	This class is the backing data structure for the soagen-generated Structure-of-arrays classes.
	///				You don't need to know anything about it unless you are implementing your own SoA machinery
	///				without using the soagen generator.
	template <typename Traits,
			  typename Allocator = soagen::allocator>
	class SOAGEN_EMPTY_BASES table //
		SOAGEN_HIDDEN_BASE(public SOAGEN_BASE_TYPE)
	{
		static_assert(is_table_traits<Traits>, "Traits must be an instance of soagen::table_traits");
		static_assert(!is_cvref<Traits>, "table traits may not be cvref-qualified");
		static_assert(!is_cvref<Allocator>, "allocators may not be cvref-qualified");

	  private:
		/// @cond
		using base = SOAGEN_BASE_TYPE;
		/// @endcond

	  public:
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;

		/// @brief	The #soagen::table_traits for the the table.
		using table_traits = Traits;

		/// @brief	The allocator used by the table.
		using allocator_type = Allocator;

		/// @brief	Returns the #soagen::column_traits for the column at the specified index.
		template <size_type Column>
		using column_traits = typename table_traits::template column<Column>;

		/// @brief	Returns the `value_type` for the column at the specified index.
		template <size_type Column>
		using column_type = typename column_traits<Column>::value_type;

		/// @copydoc	table_traits::aligned_stride
		static constexpr size_t aligned_stride = Traits::aligned_stride;

		/// @brief Default constructor.
		SOAGEN_NODISCARD_CTOR
		table() = default;

		/// @brief Move constructor.
		SOAGEN_NODISCARD_CTOR
		table(table&&) = default;

		/// @brief Move-assignment operator.
		table& operator=(table&&) = default;

		/// @brief Copy constructor.
		SOAGEN_NODISCARD_CTOR
		table(const table&) = default;

		/// @brief Copy-assignment operator.
		table& operator=(const table&) = default;

		/// @brief Destructor.
		~table() = default;

#if !SOAGEN_DOXYGEN
		using SOAGEN_BASE_TYPE::SOAGEN_BASE_NAME;
#else
		/// @brief Constructs with the given allocator.
		constexpr explicit table(const allocator_type& alloc) noexcept;

		/// @brief Constructs with the given allocator.
		constexpr explicit table(allocator_type&& alloc) noexcept;

		/// @name Capacity
		/// @{

		/// @brief Returns true if the number of rows is zero.
		constexpr bool empty() const noexcept;

		/// @brief Returns the current number of rows.
		constexpr size_type size() const noexcept;

		/// @brief Returns the maximum possible number of rows.
		constexpr size_type max_size() const noexcept;

		/// @brief Returns the size of the current underlying buffer allocation in bytes.
		/// @warning This value is `capacity() * (sizeof() for every column) + (alignment padding)`.
		///          It is **not** based on `size()`! If you are using the value returned by this function
		///          in conjunction with `data()` to do serialization, hashing, etc, use `shrink_to_fit()` first.
		constexpr size_t allocation_size() const noexcept;

		/// @brief Reserves storage for (at least) the given number of rows.
		void reserve(size_type new_cap) noexcept(...);

		/// @brief Returns the number of rows that can be held in currently allocated storage.
		constexpr size_type capacity() const noexcept;

		/// @brief Frees unused capacity.
		void shrink_to_fit() noexcept(...);

		/// @}

		/// @name Modifiers
		/// @{

		/// @brief Removes all rows from table.
		void clear() noexcept;

		/// @brief Erases the row at the given index.
		///
		/// @availability This method is only available when all the column types are move-assignable.
		void erase(size_type pos) noexcept(...);

		/// @brief	Erases the row at the given index without preserving order.
		///
		/// @details	This is much faster than #erase() because it uses the swap-and-pop idiom:
		/// Instead of shifting all the higher rows downward, the last row is moved into the
		/// position of the erased one and the size of the table is reduced by 1.
		///
		/// @note		If you are tracking row indices in some other place and need to maintain that invariant,
		/// you can use the return value to update your data accordingly.
		///
		/// @returns	The index of the row that was moved into the erased row's position, if any.
		///
		/// @availability This method is only available when all the column types are move-assignable.
		soagen::optional<size_type> unordered_erase(size_type pos) noexcept(...);

		/// @brief Constructs a new row in-place at the end of the table.
		template <typename... Args>
		void emplace_back(Args&&... args) noexcept(...);

		/// @brief Constructs a new row in-place at an arbitrary position in the table.
		template <typename... Args>
		void emplace(size_type index, Args&&... args) noexcept(...);

		/// @brief Removes the last row(s) from the table.
		void pop_back(size_type num = 1) noexcept(...);

		/// @brief Resizes the table to the given number of rows.
		///
		/// @availability This method is only available when all the column types are default-constructible.
		void resize(size_type new_size) noexcept(...);

		/// @brief Swaps the contents of the table with another.
		///
		/// @availability This method is only available when #allocator_type is swappable or non-propagating.
		constexpr void swap(table& other) noexcept(...);

		/// @}

		/// @name Equality
		/// @availability These operators are only available when all the column types are equality-comparable.
		/// @{

		/// @brief Returns true if all of the elements in two tables are equal.
		friend constexpr bool operator==(const table& lhs, const table& rhs) noexcept(...);

		/// @brief Returns true if not all of the elements in two tables are equal.
		friend constexpr bool operator!=(const table& lhs, const table& rhs) noexcept(...);

		/// @}

		/// @name Comparison
		/// @availability These operators are only available when all the column types are less-than-comparable.
		/// @{

		/// @brief Returns true if the LHS table is ordered lexicographically less-than the RHS table.
		friend constexpr bool operator<(const table& lhs, const table& rhs) noexcept(...);

		/// @brief Returns true if the LHS table is ordered lexicographically less-than-or-equal-to the RHS table.
		friend constexpr bool operator<=(const table& lhs, const table& rhs) noexcept(...);

		/// @brief Returns true if the LHS table is ordered lexicographically greater-than the RHS table.
		friend constexpr bool operator>(const table& lhs, const table& rhs) noexcept(...);

		/// @brief Returns true if the LHS table is ordered lexicographically greater-than-or-equal-to the RHS table.
		friend constexpr bool operator>=(const table& lhs, const table& rhs) noexcept(...);

		/// @}

		/// @brief Returns the allocator being used by the table.
		constexpr allocator_type get_allocator() const noexcept;

#endif

		/// @name Column access
		/// @{

#if SOAGEN_DOXYGEN
		/// @brief Returns a pointer to the raw byte backing array.
		///
		/// @availability This method is only available when all the column types are trivially-copyable.
		constexpr std::byte* data() noexcept;

		/// @brief Returns a const pointer to the raw byte backing array.
		///
		/// @availability This method is only available when all the column types are trivially-copyable.
		constexpr const std::byte* data() const noexcept;
#endif

		/// @brief Returns a pointer to the elements of a specific column.
		template <size_t Column>
		SOAGEN_ALIGNED_COLUMN(Column)
		column_type<Column>* column() noexcept
		{
			static_assert(Column < table_traits::column_count, "column index out of range");

			return soagen::assume_aligned<detail::actual_column_alignment<table_traits, allocator_type, Column>>(
				SOAGEN_LAUNDER(reinterpret_cast<column_type<Column>*>(base::alloc_.columns[Column])));
		}

		/// @brief Returns a pointer to the elements of a specific column.
		template <size_t Column>
		SOAGEN_ALIGNED_COLUMN(Column)
		std::add_const_t<column_type<Column>>* column() const noexcept
		{
			static_assert(Column < table_traits::column_count, "column index out of range");

			return soagen::assume_aligned<detail::actual_column_alignment<table_traits, allocator_type, Column>>(
				SOAGEN_LAUNDER(reinterpret_cast<column_type<Column>*>(base::alloc_.columns[Column])));
		}

		/// @}
	};

	/// @brief True if `T` is an instance of #soagen::table.
	template <typename>
	inline constexpr bool is_table = POXY_IMPLEMENTATION_DETAIL(false);
	/// @cond
	template <typename... Args>
	inline constexpr bool is_table<table<Args...>> = true;
	template <typename T>
	inline constexpr bool is_table<const T> = is_table<T>;
	template <typename T>
	inline constexpr bool is_table<volatile T> = is_table<T>;
	template <typename T>
	inline constexpr bool is_table<const volatile T> = is_table<T>;
	namespace detail
	{
		template <typename... Args>
		struct table_type_<table<Args...>>
		{
			using type = table<Args...>;
		};
	}
	/// @endcond

	/// @brief Swaps the contents of two tables.
	///
	/// @availability This method is only available when Allocator is swappable or non-propagating.
	SOAGEN_CONSTRAINED_TEMPLATE((has_swap_member<table<Traits, Allocator>>), typename Traits, typename Allocator)
	SOAGEN_ALWAYS_INLINE
	constexpr void swap(table<Traits, Allocator>& lhs, table<Traits, Allocator>& rhs) //
		noexcept(soagen::has_nothrow_swap_member<table<Traits, Allocator>>)
	{
		lhs.swap(rhs);
	}
}

#undef SOAGEN_BASE_NAME
#undef SOAGEN_BASE_TYPE
#include "header_end.hpp"
