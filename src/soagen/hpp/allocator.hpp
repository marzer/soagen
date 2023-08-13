//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"

namespace soagen
{
	/// @brief The default allocator used by soagen tables.
	///
	/// @note Use of this allocator is not required; you can use any allocator compatible with the
	/// 		 std::allocator protocol, so long as its `value_type` is `char`, `unsigned char` or `std::byte`.
	///
	/// @section customizing_allocators Customizing allocators for soagen
	/// 	Custom allocators may implement two soagen-specific extensions
	/// 	relating to memory alignment to assist the compiler with generating performant code.
	/// 	These are detailed below.
	///
	/// @subsection customizing_allocators_min_alignment Specifying a min_alignment
	/// 	If you know that your allocator will never align allocations on a boundary smaller than a
	/// 	particular value (say, 64 bytes), you can make soagen aware of this by adding a constexpr
	/// 	constant called `min_alignment`. Example:
	///
	/// 	@cpp
	/// 	struct my_allocator
	/// 	{
	/// 		inline constexpr std::size_t min_alignment = 64;
	/// 	}
	/// 	@ecpp
	///
	/// 	Soagen will then propagate this information to the tables where it may be of use to the compiler.
	///
	/// @subsection customizing_allocators_aligned_allocate Providing an alignment-aware allocate()
	/// 	If your environment has some aligned-allocation facility you wish to make soagen aware of, you
	/// 	can do so by providing an alignment-aware overload of `allocate()` taking a `std::align_val_t`
	/// 	for alignment:
	///
	/// 	@cpp
	/// 	struct my_allocator
	/// 	{
	/// 		value_type* allocate(std::size_t size, std::align_val_t alignment)
	/// 		{
	/// 			return my_aligned_alloc_function(size, static_cast<std::size_t>(alignment));
	/// 		}
	/// 	}
	/// 	@ecpp
	///
	/// 	Soagen will choose this overload over any others if it is present.
	struct allocator
	{
		/// @brief The value type allocated by this allocator.
		using value_type		 = std::byte;
		using pointer			 = value_type*;
		using const_pointer		 = const value_type*;
		using void_pointer		 = std::byte*;
		using const_void_pointer = const std::byte*;
		using size_type			 = std::size_t;
		using difference_type	 = std::ptrdiff_t;

		/// @brief Instances of this allocator are always equal.
		using is_always_equal = std::true_type;

		/// @brief Instances of this allocator don't propagate on copy-assignment.
		using propagate_on_container_copy_assignment = std::false_type;

		/// @brief Instances of this allocator don't propagate on move-assignment.
		using propagate_on_container_move_assignment = std::false_type;

		/// @brief Instances of this allocator don't propagate on swap.
		using propagate_on_container_swap = std::false_type;

		/// @brief The minimum alignment of any allocations created by this allocator.
		static constexpr size_type min_alignment =
			max(size_t{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ }, alignof(std::max_align_t), alignof(value_type));
		static_assert(has_single_bit(min_alignment));

		/// @brief Alignment-aware allocation.
		/// @param size The size of the allocation, in bytes.
		/// @param alignment The minimum alignment, in bytes. Must be a power-of-two.
		/// @return A new allocation.
		/// @throws std::bad_alloc If the system aligned-allocation function failed.
		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		value_type* allocate(size_type size, std::align_val_t alignment = std::align_val_t{ min_alignment })
		{
			SOAGEN_ASSUME(size);
			SOAGEN_ASSUME(static_cast<size_t>(alignment));

			size	  = (size + static_cast<size_t>(alignment) - 1u) & ~(static_cast<size_t>(alignment) - 1u);
			alignment = std::align_val_t{ max(static_cast<size_t>(alignment), min_alignment) };

			SOAGEN_ASSUME((static_cast<size_t>(alignment) & (static_cast<size_t>(alignment) - 1u)) == 0u);

#if SOAGEN_WINDOWS
			auto ptr = static_cast<pointer>(_aligned_malloc(size, static_cast<size_t>(alignment)));
#else
			auto ptr = static_cast<pointer>(std::aligned_alloc(static_cast<size_t>(alignment), size));
#endif
			if SOAGEN_UNLIKELY(!ptr)
				throw std::bad_alloc{};

			return soagen::assume_aligned<min_alignment>(ptr);
		}

		/// @brief Deallocation.
		/// @param ptr The pointer to the memory being deallocated. Must have been acquired via #allocate().
		/// @param size The size of the allocation, in bytes. Must be the value used for previous call to #allocate().
		SOAGEN_ATTR(nonnull)
		void deallocate(value_type* ptr, [[maybe_unused]] size_type size) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);
			SOAGEN_ASSUME(size);

#if SOAGEN_WINDOWS
			_aligned_free(ptr);
#else
			std::free(ptr);
#endif
		}

		/// @brief Equality operator.
		SOAGEN_CONST_INLINE_GETTER
		friend bool operator==(const allocator&, const allocator&) noexcept
		{
			return true;
		}

		/// @brief Inequality operator.
		SOAGEN_CONST_INLINE_GETTER
		friend bool operator!=(const allocator&, const allocator&) noexcept
		{
			return false;
		}
	};
	static_assert(std::is_trivially_default_constructible_v<allocator>);
	static_assert(std::is_trivially_move_constructible_v<allocator>);
	static_assert(std::is_trivially_copy_constructible_v<allocator>);
	static_assert(std::is_trivially_move_assignable_v<allocator>);
	static_assert(std::is_trivially_copy_assignable_v<allocator>);
	static_assert(std::is_trivially_destructible_v<allocator>);
	static_assert(std::is_nothrow_swappable_v<allocator>);

}

/// @cond
namespace soagen::detail
{
	// can we specify alignment when allocating?

	template <typename Allocator>
	using has_aligned_allocate_ = decltype(std::declval<Allocator&>().allocate(size_t{}, std::align_val_t{}));

	template <typename Allocator>
	inline constexpr bool has_aligned_allocate = is_detected<has_aligned_allocate_, Allocator>;

	// does the allocator know it's minimum possible alignment value?

	template <typename Allocator>
	using has_min_alignment_ = decltype(Allocator::min_alignment);

	template <typename Allocator>
	inline constexpr bool has_min_alignment = is_detected<has_min_alignment_, Allocator>;

	// what is the _actual_ minimum alignment value that makes sense?

	template <typename Allocator, bool = has_min_alignment<Allocator>>
	inline constexpr size_t alloc_min_alignment =
		max(Allocator::min_alignment, alignof(typename Allocator::value_type));
	template <typename Allocator>
	inline constexpr size_t alloc_min_alignment<Allocator, false> = alignof(typename Allocator::value_type);

	//--- base traits --------------------------------------------------------------------------------------------------

	template <typename Allocator>
	struct allocator_traits_base : public std::allocator_traits<Allocator>
	{
	  private:
		using base = std::allocator_traits<Allocator>;

	  public:
		static_assert(!is_cvref<Allocator>, "allocators must not be cvref-qualified");
		static_assert(any_same<typename Allocator::value_type, std::byte, char, unsigned char>,
					  "allocators must have either std::byte, char or unsigned char as their value_type");

		static constexpr size_t min_alignment = detail::alloc_min_alignment<Allocator>;
		static_assert(has_single_bit(min_alignment), "allocator min_alignment must be a power of two");

		SOAGEN_PURE_INLINE_GETTER
		static constexpr bool equal([[maybe_unused]] const Allocator& a, [[maybe_unused]] const Allocator& b) noexcept
		{
			if constexpr (base::is_always_equal::value)
				return true;
			else
				return a == b;
		}

		//                                 always take ownership (de-allocate existing + move allocation)?
		// 1. propagating,     equal       yes
		// 2. propagating,     non-equal   yes
		// 3. non-propagating, equal       yes
		// 4. non-propagating, non-equal   no (need to re-use existing capacity + move elementwise)
		static constexpr bool container_move_assign_always_takes_ownership =
			base::propagate_on_container_move_assignment::value || base::is_always_equal::value;
	};

	//--- alignment-aware allocate() -----------------------------------------------------------------------------------

	template <typename Allocator, bool = has_aligned_allocate<Allocator>>
	struct allocator_traits_alloc : allocator_traits_base<Allocator>
	{
		// primary template - allocator has an aligned-allocation overload

	  private:
		using base = allocator_traits_base<Allocator>;

	  public:
		using typename base::value_type;
		using typename base::size_type;

		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(base::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type size, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{}, std::align_val_t{})))
		{
			return soagen::assume_aligned<base::min_alignment>(
				alloc.allocate(size, std::align_val_t{ max(static_cast<size_t>(alignment), base::min_alignment) }));
		}
	};

	template <typename Allocator>
	struct allocator_traits_alloc<Allocator, false> : allocator_traits_base<Allocator>
	{
		// secondary template - we have to implement the alignment management manually :(:(

	  private:
		using base = allocator_traits_base<Allocator>;

	  public:
		using typename base::value_type;
		using typename base::size_type;
		using typename base::pointer;

	  private:
		struct alloc_info
		{
			size_type requested_size;
			size_type actual_size;
			pointer ptr;
		};

	  public:
		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(base::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type size, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{})))
		{
			static_assert(sizeof(typename Allocator::value_type) == 1u);

			alignment = std::align_val_t{ max(static_cast<size_type>(alignment), base::min_alignment) };

			const size_type offset = (static_cast<size_type>(alignment) - 1u) + sizeof(alloc_info);
			pointer ptr			   = alloc.allocate(size + offset);
			SOAGEN_ASSUME(ptr != nullptr);

			alloc_info* info = reinterpret_cast<alloc_info*>((reinterpret_cast<uintptr_t>(ptr) + offset)
															 & ~(static_cast<size_type>(alignment) - 1u));
			info[-1]		 = { size, size + offset, ptr };
			return soagen::assume_aligned<base::min_alignment>(reinterpret_cast<pointer>(info));
		}

		// note that this hides std::allocator_traits::deallocate - this is intentional
		SOAGEN_ATTR(nonnull)
		static constexpr void deallocate(Allocator& alloc, value_type* ptr, [[maybe_unused]] size_type n) noexcept
		{
			SOAGEN_ASSUME(ptr != nullptr);

			const auto info = reinterpret_cast<alloc_info*>(ptr)[-1];
			SOAGEN_ASSERT(n == info.requested_size);

			alloc.deallocate(info.ptr, info.actual_size);
		}
	};
}
/// @endcond

namespace soagen
{
	template <typename Allocator>
	struct allocator_traits //
		: public detail::allocator_traits_alloc<Allocator>
	{};
}

#include "header_end.hpp"
