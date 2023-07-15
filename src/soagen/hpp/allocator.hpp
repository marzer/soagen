//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "generated/functions.hpp"
#include "header_start.hpp"
#if SOAGEN_CLANG >= 16
	#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

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
		using size_type			 = size_t;
		using difference_type	 = ptrdiff_t;

		/// @brief Instances of this allocator are always equal.
		using is_always_equal = std::true_type;

		/// @brief Instances of this allocator don't propagate on copy-assignment.
		using propagate_on_container_copy_assignment = std::false_type;

		/// @brief Instances of this allocator don't propagate on move-assignment.
		using propagate_on_container_move_assignment = std::false_type;

		/// @brief Instances of this allocator don't propagate on swap.
		using propagate_on_container_swap = std::false_type;

		/// @brief The minimum alignment of any allocations created by this allocator.
		static constexpr size_t min_alignment =
			max(size_t{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ }, alignof(std::max_align_t), alignof(value_type));
		static_assert(has_single_bit(min_alignment));

		/// @brief Alignment-aware allocation.
		/// @param size The size of the desired allocation, in bytes.
		/// @param alignment The minimum desired alignment, in bytes. Must be a power-of-two.
		/// @return A new allocation.
		/// @throws std::bad_alloc If the system aligned-allocation function failed.
		SOAGEN_NODISCARD
		SOAGEN_ALWAYS_INLINE
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		value_type* allocate(size_t size, std::align_val_t alignment = std::align_val_t{ min_alignment })
		{
			SOAGEN_ASSUME(size);
			SOAGEN_ASSUME(static_cast<size_t>(alignment));

			size	  = (size + static_cast<size_t>(alignment) - 1u) & ~(static_cast<size_t>(alignment) - 1u);
			alignment = std::align_val_t{ max(static_cast<size_t>(alignment), min_alignment) };

			SOAGEN_ASSUME((static_cast<size_t>(alignment) & (static_cast<size_t>(alignment) - 1u)) == 0u);

#if SOAGEN_WINDOWS
			auto ptr = _aligned_malloc(size, static_cast<size_t>(alignment));
#else
			auto ptr = std::aligned_alloc(static_cast<size_t>(alignment), size);
#endif
			if SOAGEN_UNLIKELY(!ptr)
				throw std::bad_alloc{};

			return soagen::assume_aligned<min_alignment>(static_cast<pointer>(ptr));
		}

		/// @brief Deallocation.
		/// @param ptr The pointer to the memory being deallocated.
		/// @param size The size requested during the initial call to #allocate().
		/// @attention `ptr` must be a value acquired as the return value of #allocate()!
		SOAGEN_ALWAYS_INLINE
		SOAGEN_ATTR(nonnull)
		void deallocate(value_type* ptr, [[maybe_unused]] size_t size) noexcept
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

	/// @cond
	namespace detail
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

		// internal base type

		template <typename Allocator>
		struct allocator_traits_base : public std::allocator_traits<Allocator>
		{
			static_assert(!is_cvref<Allocator>, "allocators must not be cvref-qualified");
			static_assert(any_same<typename Allocator::value_type, std::byte, char, unsigned char>,
						  "allocators must have either std::byte, char or unsigned char as their value_type");

			using base_traits = std::allocator_traits<Allocator>;

			static constexpr size_t min_alignment = detail::alloc_min_alignment<Allocator>;
			static_assert(has_single_bit(min_alignment), "allocator min_alignment must be a power of two");

			SOAGEN_PURE_INLINE_GETTER
			static constexpr bool equal([[maybe_unused]] const Allocator& a,
										[[maybe_unused]] const Allocator& b) noexcept
			{
				if constexpr (base_traits::is_always_equal::value)
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
				base_traits::propagate_on_container_move_assignment::value || base_traits::is_always_equal::value;
		};
	}

	// primary template - allocator has an aligned-allocation overload
	template <typename Allocator, bool = detail::has_aligned_allocate<Allocator>>
	struct allocator_traits //
		: public detail::allocator_traits_base<Allocator>
	{
		using base_traits = detail::allocator_traits_base<Allocator>;
		using typename base_traits::value_type;
		using typename base_traits::size_type;

		SOAGEN_NODISCARD
		SOAGEN_DECLSPEC(noalias)
		SOAGEN_DECLSPEC(restrict)
		SOAGEN_ATTR(assume_aligned(base_traits::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type num, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{}, std::align_val_t{})))
		{
			return soagen::assume_aligned<base_traits::min_alignment>(
				alloc.allocate(num,
							   std::align_val_t{ max(static_cast<size_t>(alignment), base_traits::min_alignment) }));
		}
	};

	// secondary template - we have to implement the alignment management manually :(:(
	template <typename Allocator>
	struct allocator_traits<Allocator, false> //
		: public detail::allocator_traits_base<Allocator>
	{
		using base_traits = detail::allocator_traits_base<Allocator>;
		using typename base_traits::value_type;
		using typename base_traits::size_type;
		using typename base_traits::pointer;

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
		SOAGEN_ATTR(assume_aligned(base_traits::min_alignment))
		SOAGEN_ATTR(returns_nonnull)
		SOAGEN_ATTR(malloc)
		static constexpr value_type* allocate(Allocator& alloc, size_type n, std::align_val_t alignment) //
			noexcept(noexcept(std::declval<Allocator&>().allocate(size_type{})))
		{
			static_assert(sizeof(typename Allocator::value_type) == 1u);

			alignment = std::align_val_t{ max(static_cast<size_type>(alignment), base_traits::min_alignment) };

			const size_type offset = (static_cast<size_type>(alignment) - 1u) + sizeof(alloc_info);
			pointer ptr			   = alloc.allocate(n + offset);
			SOAGEN_ASSUME(ptr != nullptr);

			alloc_info* info = reinterpret_cast<alloc_info*>((reinterpret_cast<uintptr_t>(ptr) + offset)
															 & ~(static_cast<size_type>(alignment) - 1u));
			info[-1]		 = { n, n + offset, ptr };
			return soagen::assume_aligned<base_traits::min_alignment>(reinterpret_cast<pointer>(info));
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

	/// @endcond
}

#include "header_end.hpp"
