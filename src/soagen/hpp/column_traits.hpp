//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "emplacer.hpp"
#include "tuples.hpp"
#include "header_start.hpp"

#ifndef SOAGEN_LAUNDER
	#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
		#define SOAGEN_LAUNDER(...) std::launder(__VA_ARGS__)
	#elif SOAGEN_CLANG >= 8 || SOAGEN_GCC >= 7 || SOAGEN_ICC >= 1910 || SOAGEN_MSVC >= 1914                            \
		|| SOAGEN_HAS_BUILTIN(__builtin_launder)
		#define SOAGEN_LAUNDER(...) __builtin_launder(__VA_ARGS__)
	#else
		#define SOAGEN_LAUNDER(...) __VA_ARGS__
	#endif
#endif

#ifndef SOAGEN_COLUMN
	#define SOAGEN_COLUMN(Table, Column)                                                                               \
		SOAGEN_PURE_INLINE_GETTER                                                                                      \
		SOAGEN_ATTR(returns_nonnull)                                                                                   \
		SOAGEN_ATTR(                                                                                                   \
			assume_aligned(soagen::actual_alignment<soagen::remove_cvref<Table>, static_cast<std::size_t>(Column)>))
#endif

#define soagen_aligned_storage(...) soagen::assume_aligned<alignof(storage_type)>(__VA_ARGS__)

/// @cond
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

		//--- pointers -------------------------------------------------------------------------------------------------

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type* ptr(std::byte* p) noexcept
		{
			SOAGEN_ASSUME(p != nullptr);

			if constexpr (std::is_same_v<storage_type, std::byte>)
				return p;
			else
				return SOAGEN_LAUNDER(reinterpret_cast<storage_type*>(p));
		}

		SOAGEN_PURE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr const storage_type* ptr(const std::byte* p) noexcept
		{
			SOAGEN_ASSUME(p != nullptr);

			if constexpr (std::is_same_v<storage_type, std::byte>)
				return p;
			else
				return SOAGEN_LAUNDER(reinterpret_cast<const storage_type*>(p));
		}

		//--- dereferencing --------------------------------------------------------------------------------------------

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& get(std::byte* p) noexcept
		{
			return *ptr(p);
		}

		SOAGEN_PURE_INLINE_GETTER
		SOAGEN_ATTR(nonnull)
		static constexpr const storage_type& get(const std::byte* p) noexcept
		{
			SOAGEN_ASSUME(p != nullptr);

			return *ptr(p);
		}

		//--- default construction -------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_default_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& default_construct(std::byte* destination) //
			noexcept(std::is_nothrow_default_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(destination != nullptr);

#if defined(__cpp_lib_start_lifetime_as) && __cpp_lib_start_lifetime_as >= 202207
			if constexpr (is_implicit_lifetime_type<storage_type>)
			{
				return *(std::start_lifetime_as<storage_type>(destination));
			}
			else
			{
#endif
				return *(::new (static_cast<void*>(destination)) storage_type);

#if defined(__cpp_lib_start_lifetime_as) && __cpp_lib_start_lifetime_as >= 202207
			}
#endif
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_default_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& default_construct(std::byte* buffer, size_t index) //
			noexcept(std::is_nothrow_default_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			return default_construct(buffer + index * sizeof(storage_type));
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = std::is_default_constructible_v<storage_type>)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static void default_construct(std::byte* buffer, size_t index, size_t count) //
			noexcept(std::is_nothrow_default_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);
			SOAGEN_ASSUME(count);

#if defined(__cpp_lib_start_lifetime_as) && __cpp_lib_start_lifetime_as >= 2022071
			if constexpr (is_implicit_lifetime_type<storage_type>)
			{
				std::start_lifetime_as_array<storage_type>(destination, count);
			}
			else
#endif
				if constexpr (std::is_nothrow_default_constructible_v<storage_type>
							  || std::is_trivially_destructible_v<storage_type>)
			{
				for (const size_t e = index + count; index < e; index++)
					default_construct(buffer, index);
			}
			else
			{
				// machinery to provide strong-exception guarantee

				size_t i = index;

				try
				{
					for (const size_t e = index + count; i < e; i++)
						default_construct(buffer, i);
				}
				catch (...)
				{
					for (; i-- > index;)
						destruct(buffer, i);
					throw;
				}
			}
		}

		//--- construction ---------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_copyable = std::is_trivially_copyable_v<storage_type>;

		// constructibility using memcpy

	  private:
		template <typename Arg>
		struct is_constructible_with_memcpy_
			: std::conjunction<
				  std::bool_constant<sizeof(storage_type) == sizeof(remove_cvref<Arg>)>, //
				  std::is_trivially_copyable<storage_type>,								 //
				  is_implicit_lifetime_type_<storage_type>,								 //
				  std::is_trivially_copyable<remove_cvref<Arg>>,						 //
				  std::disjunction<
					  std::is_same<storage_type, remove_cvref<Arg>>,
					  std::bool_constant<any_same<storage_type, char, unsigned char, std::byte>
										 && any_same<remove_cvref<Arg>, char, unsigned char, std::byte>>,
					  std::conjunction<std::is_same<storage_type, void*>, std::is_pointer<remove_cvref<Arg>>>>>
		{};

	  public:
		template <typename Arg>
		static constexpr bool is_constructible_with_memcpy = is_constructible_with_memcpy_<Arg>::value;

		// constructibility by unpacking a tuple

	  private:
		template <typename Arg>
		struct is_constructible_by_unpacking_tuple_ //
			: detail::is_constructible_by_unpacking_tuple_<storage_type, Arg>
		{};

	  public:
		template <typename Arg>
		static constexpr bool is_constructible_by_unpacking_tuple = is_constructible_by_unpacking_tuple_<Arg>::value;

		// trivial-constructibility

	  private:
		template <typename... Args>
		struct is_trivially_constructible_ : std::is_trivially_constructible<storage_type, remove_cvref<Args>...>
		{};

		template <typename Arg>
		struct is_trivially_constructible_<Arg>
			: std::disjunction<std::is_trivially_constructible<storage_type, remove_cvref<Arg>>,
							   is_constructible_with_memcpy_<remove_cvref<Arg>>,
							   typename is_constructible_by_unpacking_tuple_<Arg>::is_trivial>
		{};

	  public:
		template <typename... Args>
		static constexpr bool is_trivially_constructible = is_trivially_constructible_<Args...>::value;

		// constructibility from arbitrary args

		template <typename... Args>
		struct is_constructible_trait : std::disjunction<is_trivially_constructible_<Args...>, //
														 std::is_constructible<storage_type, Args...>>
		{};
		template <typename Arg>
		struct is_constructible_trait<Arg> : std::disjunction<is_trivially_constructible_<Arg>, //
															  std::is_constructible<storage_type, Arg>,
															  is_constructible_by_unpacking_tuple_<Arg>>
		{};
		template <typename T>
		struct is_constructible_trait<T*> : std::disjunction<std::is_same<storage_type, void*>, //
															 is_trivially_constructible_<T*>,	//
															 std::is_constructible<storage_type, T*>>
		{};
		template <typename T>
		struct is_constructible_trait<T*&> : std::disjunction<std::is_same<storage_type, void*>, //
															  is_trivially_constructible_<T*&>,	 //
															  std::is_constructible<storage_type, T*&>>
		{};
		template <typename T>
		struct is_constructible_trait<T*&&> : std::disjunction<std::is_same<storage_type, void*>, //
															   is_trivially_constructible_<T*&&>, //
															   std::is_constructible<storage_type, T*&&>>
		{};
		template <typename... Args>
		struct is_constructible_trait<emplacer<Args...>&&> : is_constructible_trait<Args...>
		{};

		template <typename... Args>
		static constexpr bool is_constructible = is_constructible_trait<Args...>::value;

		// nothrow-constructibility from arbitrary args

		template <typename... Args>
		struct is_nothrow_constructible_trait : std::disjunction<is_trivially_constructible_<Args...>,
																 std::is_nothrow_constructible<storage_type, Args...>>
		{};
		template <typename Arg>
		struct is_nothrow_constructible_trait<Arg>
			: std::disjunction<is_trivially_constructible_<Arg>,
							   std::is_nothrow_constructible<storage_type, Arg>,
							   typename is_constructible_by_unpacking_tuple_<Arg>::is_nothrow>
		{};
		template <typename T>
		struct is_nothrow_constructible_trait<T*> : std::disjunction<std::is_same<storage_type, void*>, //
																	 is_trivially_constructible_<T*>,	//
																	 std::is_nothrow_constructible<storage_type, T*>>
		{};
		template <typename T>
		struct is_nothrow_constructible_trait<T*&> : std::disjunction<std::is_same<storage_type, void*>, //
																	  is_trivially_constructible_<T*&>,	 //
																	  std::is_nothrow_constructible<storage_type, T*&>>
		{};
		template <typename T>
		struct is_nothrow_constructible_trait<T*&&>
			: std::disjunction<std::is_same<storage_type, void*>, //
							   is_trivially_constructible_<T*&&>, //
							   std::is_nothrow_constructible<storage_type, T*&&>>
		{};
		template <typename... Args>
		struct is_nothrow_constructible_trait<emplacer<Args...>&&> : is_nothrow_constructible_trait<Args...>
		{};

		template <typename... Args>
		static constexpr bool is_nothrow_constructible = is_nothrow_constructible_trait<Args...>::value;

	  private:
		template <typename... Args, size_t... Indices>
		SOAGEN_ATTR(nonnull)
		SOAGEN_ALWAYS_INLINE
		static constexpr storage_type& construct_from_emplacer(std::byte* destination,
															   emplacer<Args...>&& args,
															   std::index_sequence<Indices...>) //
			noexcept(is_nothrow_constructible<Args...>)
		{
			static_assert((std::is_reference_v<Args> && ...));
			static_assert(sizeof...(Args) == sizeof...(Indices));
			SOAGEN_ASSUME(destination != nullptr);

			return construct(destination,
							 static_cast<Args>(*static_cast<std::add_pointer_t<std::remove_reference_t<Args>>>(
								 args.ptrs[Indices]))...);
		}

		template <typename Tuple, size_t... Indices>
		SOAGEN_ATTR(nonnull)
		SOAGEN_ALWAYS_INLINE
		static constexpr storage_type& construct_from_tuple(std::byte* destination,
															Tuple&& tuple,
															std::index_sequence<Indices...>) //
			noexcept(is_constructible_by_unpacking_tuple_<Tuple&&>::is_nothrow::value)
		{
			SOAGEN_ASSUME(destination != nullptr);

			return construct(destination, get_from_tuple<Indices>(static_cast<Tuple&&>(tuple))...);
		}

	  public:
		SOAGEN_CONSTRAINED_TEMPLATE(is_constructible<Args&&...>, typename... Args)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct(std::byte* destination, Args&&... args) //
			noexcept(is_nothrow_constructible<Args&&...>)
		{
			SOAGEN_ASSUME(destination != nullptr);

			if constexpr (sizeof...(Args) == 0)
			{
				return default_construct(destination);
			}
			else
			{
				if constexpr (sizeof...(Args) == 1 && (is_emplacer<std::remove_reference_t<Args>> && ...))
				{
					return construct_from_emplacer(
						destination,
						static_cast<Args&&>(args)...,
						std::make_index_sequence<std::tuple_size_v<remove_cvref<Args>>>{}...);
				}
				else if constexpr (sizeof...(Args) == 1 && (is_constructible_with_memcpy<Args&&> && ...))
				{
					std::memcpy(soagen_aligned_storage(destination), &args..., sizeof(storage_type));

					return get(destination);
				}
				else if constexpr (std::is_constructible_v<storage_type, Args&&...>)
				{
					if constexpr (std::is_aggregate_v<storage_type>)
					{
						return *(::new (static_cast<void*>(destination)) storage_type{ static_cast<Args&&>(args)... });
					}
					else
					{
						return *(::new (static_cast<void*>(destination)) storage_type(static_cast<Args&&>(args)...));
					}
				}
				else if constexpr (sizeof...(Args) == 1 && (is_constructible_by_unpacking_tuple<Args&&> && ...))
				{
					return construct_from_tuple(destination,
												static_cast<Args&&>(args)...,
												std::make_index_sequence<std::tuple_size_v<remove_cvref<Args>>>{}...);
				}
			}
		}

		SOAGEN_CONSTRAINED_TEMPLATE(is_constructible<Args&&...>, typename... Args)
		SOAGEN_ATTR(nonnull)
		static constexpr storage_type& construct_at(std::byte* buffer, size_t index, Args&&... args) //
			noexcept(is_nothrow_constructible<Args&&...>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			if constexpr (sizeof...(Args) == 0)
			{
				return default_construct(buffer + index * sizeof(storage_type));
			}
			else
			{
				return construct(buffer + index * sizeof(storage_type), static_cast<Args&&>(args)...);
			}
		}

		//--- move-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_trivially_move_constructible = is_trivially_constructible<storage_type&&>;

		static constexpr bool is_move_constructible =
			is_trivially_move_constructible				  //
			|| std::is_move_constructible_v<storage_type> //
			|| (std::is_default_constructible_v<storage_type> && std::is_move_assignable_v<storage_type>);

		static constexpr bool is_nothrow_move_constructible =
			is_trivially_move_constructible
				? true
				: (std::is_move_constructible_v<storage_type> ? std::is_nothrow_move_constructible_v<storage_type>
															  : std::is_nothrow_default_constructible_v<storage_type>
																	&& std::is_nothrow_move_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_trivially_move_constructible || std::is_move_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<storage_type&&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_move_assignable_v<storage_type>);

				default_construct(destination);

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
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_construct(std::byte* dest_buffer,
											size_t dest_index,
											std::byte* source_buffer,
											size_t source_index) //
			noexcept(std::is_nothrow_move_constructible_v<storage_type>)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_construct(dest_buffer + dest_index * sizeof(storage_type),
								  source_buffer + source_index * sizeof(storage_type));
		}

		//--- copy-construction ----------------------------------------------------------------------------------------

		static constexpr bool is_trivially_copy_constructible = is_trivially_constructible<const storage_type&>;

		static constexpr bool is_copy_constructible =
			is_trivially_copy_constructible				  //
			|| std::is_copy_constructible_v<storage_type> //
			|| (std::is_default_constructible_v<storage_type> && std::is_copy_assignable_v<storage_type>);

		static constexpr bool is_nothrow_copy_constructible =
			is_trivially_copy_constructible
				? true
				: (std::is_copy_constructible_v<storage_type> ? std::is_nothrow_copy_constructible_v<storage_type>
															  : std::is_nothrow_default_constructible_v<storage_type>
																	&& std::is_nothrow_copy_assignable_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_construct(std::byte* destination, const std::byte* source) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_trivially_copy_constructible || std::is_copy_constructible_v<storage_type>)
			{
				return construct(destination, static_cast<const storage_type&>(get(source)));
			}
			else
			{
				static_assert(std::is_default_constructible_v<storage_type>);
				static_assert(std::is_copy_assignable_v<storage_type>);

				default_construct(destination);

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
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_construct(std::byte* dest_buffer,
											size_t dest_index,
											const std::byte* source_buffer,
											size_t source_index) //
			noexcept(is_nothrow_copy_constructible)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_construct(dest_buffer + dest_index * sizeof(storage_type),
								  source_buffer + source_index * sizeof(storage_type));
		}

		//--- move-or-copy-construction (whichever is possible) --------------------------------------------------------

		static constexpr bool is_move_or_copy_constructible = is_move_constructible || is_copy_constructible;

		static constexpr bool is_nothrow_move_or_copy_constructible =
			is_move_constructible ? is_nothrow_move_constructible : is_nothrow_copy_constructible;

		static constexpr bool is_trivially_move_or_copy_constructible =
			is_move_constructible ? is_trivially_move_constructible : is_trivially_copy_constructible;

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_construct(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_move_constructible)
			{
				return move_construct(destination, source);
			}
			else
			{
				return copy_construct(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_constructible)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_construct(std::byte* dest_buffer,
													size_t dest_index,
													std::byte* source_buffer,
													size_t source_index) //
			noexcept(is_nothrow_move_or_copy_constructible)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_or_copy_construct(dest_buffer + dest_index * sizeof(storage_type),
										  source_buffer + source_index * sizeof(storage_type));
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
		static constexpr void destruct([[maybe_unused]] std::byte* buffer, //
									   [[maybe_unused]] size_t index)	   //
			noexcept(std::is_nothrow_destructible_v<storage_type>)
		{
			SOAGEN_ASSUME(buffer != nullptr);

			if constexpr (!std::is_trivially_destructible_v<storage_type>)
			{
				destruct(buffer + index * sizeof(storage_type));
			}
		}

		//--- move-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_move_assignable =
			is_constructible_with_memcpy<storage_type&&> || std::is_trivially_move_assignable_v<storage_type>;

		static constexpr bool is_move_assignable =
			is_trivially_move_assignable			   //
			|| std::is_move_assignable_v<storage_type> //
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_move_constructible_v<storage_type>);

		static constexpr bool is_nothrow_move_assignable =
			is_trivially_move_assignable
				? true
				: (std::is_move_assignable_v<storage_type> ? std::is_nothrow_move_assignable_v<storage_type>
														   : std::is_nothrow_destructible_v<storage_type>
																 && std::is_nothrow_move_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_assign(std::byte* destination, void* source) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_constructible_with_memcpy<storage_type&&>)
			{
				std::memcpy(soagen_aligned_storage(destination),
							soagen_aligned_storage(static_cast<std::byte*>(source)),
							sizeof(storage_type));

				return get(destination);
			}
			else if constexpr (is_trivially_move_assignable || std::is_move_assignable_v<storage_type>)
			{
				return get(destination) = static_cast<storage_type&&>(get(static_cast<std::byte*>(source)));
			}
			else
			{
				// note we only fall back to this if they're nothrow because we don't want to leave the destination
				// in a half-constructed state (it existed before the assignment, it should still exist after)
				static_assert(std::is_nothrow_destructible_v<storage_type>);
				static_assert(std::is_nothrow_move_constructible_v<storage_type>);

				destruct(destination);
				return move_construct(destination, static_cast<std::byte*>(source));
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_assign(std::byte* dest_buffer,
										 size_t dest_index,
										 std::byte* source_buffer,
										 size_t source_index) //
			noexcept(is_nothrow_move_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_assign(dest_buffer + dest_index * sizeof(storage_type),
							   source_buffer + source_index * sizeof(storage_type));
		}

		//--- copy-assignment ------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_copy_assignable =
			is_constructible_with_memcpy<const storage_type&> || std::is_trivially_copy_assignable_v<storage_type>;

		static constexpr bool is_copy_assignable =
			is_trivially_copy_assignable			   //
			|| std::is_copy_assignable_v<storage_type> //
			|| (std::is_nothrow_destructible_v<storage_type> && std::is_nothrow_copy_constructible_v<storage_type>);

		static constexpr bool is_nothrow_copy_assignable =
			is_trivially_copy_assignable
				? true
				: (std::is_copy_assignable_v<storage_type> ? std::is_nothrow_copy_assignable_v<storage_type>
														   : std::is_nothrow_destructible_v<storage_type>
																 && std::is_nothrow_copy_constructible_v<storage_type>);

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_copy_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_assign(std::byte* destination, const std::byte* source) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_constructible_with_memcpy<storage_type&&>)
			{
				std::memcpy(soagen_aligned_storage(destination),
							soagen_aligned_storage(static_cast<const std::byte*>(source)),
							sizeof(storage_type));

				return get(destination);
			}
			else if constexpr (is_trivially_copy_assignable || std::is_copy_assignable_v<storage_type>)
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
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& copy_assign(std::byte* dest_buffer,
										 size_t dest_index,
										 const std::byte* source_buffer,
										 size_t source_index) //
			noexcept(is_nothrow_copy_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return copy_assign(dest_buffer + dest_index * sizeof(storage_type),
							   source_buffer + source_index * sizeof(storage_type));
		}

		//--- move-or-copy-assignment (whichever is possible) --------------------------------------------------------

		static constexpr bool is_move_or_copy_assignable = is_move_assignable || is_copy_assignable;

		static constexpr bool is_nothrow_move_or_copy_assignable =
			is_move_assignable ? is_nothrow_move_assignable : is_nothrow_copy_assignable;

		static constexpr bool is_trivially_move_or_copy_assignable =
			is_move_assignable ? is_trivially_move_assignable : is_trivially_copy_assignable;

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_assign(std::byte* destination, std::byte* source) //
			noexcept(is_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(destination != nullptr);
			SOAGEN_ASSUME(source != nullptr);
			SOAGEN_ASSUME(destination != source);

			if constexpr (is_move_assignable)
			{
				return move_assign(destination, source);
			}
			else
			{
				return copy_assign(destination, source);
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_move_or_copy_assignable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static storage_type& move_or_copy_assign(std::byte* dest_buffer,
												 size_t dest_index,
												 std::byte* source_buffer,
												 size_t source_index) //
			noexcept(is_nothrow_move_or_copy_assignable)
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			return move_or_copy_assign(dest_buffer + dest_index * sizeof(storage_type),
									   source_buffer + source_index * sizeof(storage_type));
		}

		//--- swap -----------------------------------------------------------------------------------------------------

		static constexpr bool is_trivially_swappable =
			is_trivially_copyable
			&& (std::is_scalar_v<storage_type> || std::is_fundamental_v<storage_type>
				|| !std::is_swappable_v<storage_type> // we don't want to usurp user-defined swap() functions
			);

		static constexpr bool is_swappable = is_trivially_swappable //
										  || std::is_swappable_v<storage_type>
										  || (is_move_or_copy_constructible && is_move_or_copy_assignable);

		static constexpr bool is_nothrow_swappable =
			is_trivially_swappable
			|| (std::is_swappable_v<storage_type>
					? std::is_nothrow_swappable_v<storage_type>
					: (is_nothrow_move_or_copy_constructible && is_nothrow_move_or_copy_assignable));

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_swappable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static void swap(std::byte* lhs, std::byte* rhs) //
			noexcept(is_nothrow_swappable)
		{
			SOAGEN_ASSUME(lhs != nullptr);
			SOAGEN_ASSUME(rhs != nullptr);
			SOAGEN_ASSUME(lhs != rhs);

			if constexpr (is_trivially_swappable)
			{
				alignas(storage_type) std::byte temp[sizeof(storage_type)];
				std::memcpy(soagen_aligned_storage(temp), soagen_aligned_storage(lhs), sizeof(storage_type));
				std::memcpy(soagen_aligned_storage(lhs), soagen_aligned_storage(rhs), sizeof(storage_type));
				std::memcpy(soagen_aligned_storage(rhs), soagen_aligned_storage(temp), sizeof(storage_type));
			}
			else if constexpr (std::is_swappable_v<storage_type>)
			{
				using std::swap;
				swap(get(lhs), get(rhs));
			}
			else
			{
				static_assert(is_move_or_copy_constructible && is_move_or_copy_assignable);

				alignas(storage_type) std::byte temp[sizeof(storage_type)];

				if constexpr ((is_nothrow_move_or_copy_constructible && is_nothrow_move_or_copy_assignable)
							  || std::is_trivially_destructible_v<storage_type>)
				{
					move_or_copy_construct(soagen_aligned_storage(temp), lhs);
					move_or_copy_assign(lhs, rhs);
					move_or_copy_assign(rhs, soagen_aligned_storage(temp));
					destruct(temp);
				}
				else
				{
					bool needs_destruct = false;
					try
					{
						move_or_copy_construct(soagen_aligned_storage(temp), lhs);
						needs_destruct = true;
						move_or_copy_assign(lhs, rhs);
						move_or_copy_assign(rhs, soagen_aligned_storage(temp));
					}
					catch (...)
					{
						if (needs_destruct)
							destruct(temp);
						throw;
					}
				}
			}
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_swappable)
		SOAGEN_ATTR(nonnull)
		SOAGEN_CPP20_CONSTEXPR
		static void swap(std::byte* lhs_buffer,
						 size_t lhs_index,
						 std::byte* rhs_buffer,
						 size_t rhs_index,
						 size_t count = 1) //
			noexcept(is_nothrow_swappable)
		{
			SOAGEN_ASSUME(lhs_buffer != nullptr);
			SOAGEN_ASSUME(rhs_buffer != nullptr);

			if SOAGEN_UNLIKELY(!count)
				return;

			lhs_buffer = soagen_aligned_storage(lhs_buffer + lhs_index * sizeof(storage_type));
			rhs_buffer = soagen_aligned_storage(rhs_buffer + rhs_index * sizeof(storage_type));

			[[maybe_unused]] SOAGEN_CPP23_STATIC_CONSTEXPR size_t trivial_buf_size = 2048u / sizeof(storage_type);

			if constexpr (is_trivially_swappable && trivial_buf_size > 1)
			{
				for (size_t i = 0; i < count; i += trivial_buf_size)
				{
					const auto num = soagen::min(trivial_buf_size, count - i);

					alignas(storage_type) std::byte buf[sizeof(storage_type) * trivial_buf_size];
					std::memcpy(buf, lhs_buffer, sizeof(storage_type) * num);
					std::memcpy(lhs_buffer, rhs_buffer, sizeof(storage_type) * num);
					std::memcpy(rhs_buffer, buf, sizeof(storage_type) * num);

					lhs_buffer = soagen_aligned_storage(lhs_buffer + trivial_buf_size * sizeof(storage_type));
					rhs_buffer = soagen_aligned_storage(rhs_buffer + trivial_buf_size * sizeof(storage_type));
				}
			}
			else
			{
				const auto end = lhs_buffer + count * sizeof(storage_type);
				for (; lhs_buffer < end; lhs_buffer += sizeof(storage_type), rhs_buffer += sizeof(storage_type))
				{
					swap(lhs_buffer, rhs_buffer);
				}
			}
		}

		//--- memmove ---------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_trivially_copyable)
		static constexpr void memmove(std::byte* dest_buffer,
									  size_t dest_index,
									  const std::byte* source_buffer,
									  size_t source_index,
									  size_t count = 1) noexcept
		{
			SOAGEN_ASSUME(dest_buffer != nullptr);
			SOAGEN_ASSUME(source_buffer != nullptr);

			std::memmove(soagen_aligned_storage(dest_buffer + dest_index * sizeof(storage_type)),
						 soagen_aligned_storage(source_buffer + source_index * sizeof(storage_type)),
						 count * sizeof(storage_type));
		}

		//--- equality -------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_equality_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool equal(const std::byte* lhs, const std::byte* rhs) //
			noexcept(is_nothrow_equality_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs != nullptr);
			SOAGEN_ASSUME(rhs != nullptr);

			return get(lhs) == get(rhs);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_equality_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool equal(const std::byte* lhs_buffer,
									size_t lhs_index,
									const std::byte* rhs_buffer,
									size_t rhs_index,
									size_t count = 1) //
			noexcept(is_nothrow_equality_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs_buffer != nullptr);
			SOAGEN_ASSUME(rhs_buffer != nullptr);

			lhs_buffer += lhs_index * sizeof(storage_type);
			rhs_buffer += rhs_index * sizeof(storage_type);
			const auto end = lhs_buffer + count * sizeof(storage_type);

			for (; lhs_buffer < end; lhs_buffer += sizeof(storage_type), rhs_buffer += sizeof(storage_type))
			{
				if (!equal(lhs_buffer, rhs_buffer))
					return false;
			}

			return true;
		}

		//--- less-than ------------------------------------------------------------------------------------------------

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_less_than_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool less_than(const std::byte* lhs, const std::byte* rhs) //
			noexcept(is_nothrow_less_than_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs != nullptr);
			SOAGEN_ASSUME(rhs != nullptr);

			return get(lhs) < get(rhs);
		}

		SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = is_less_than_comparable<storage_type>)
		SOAGEN_NODISCARD
		SOAGEN_ATTR(nonnull)
		static constexpr bool less_than(const std::byte* lhs_buffer,
										size_t lhs_index,
										const std::byte* rhs_buffer,
										size_t rhs_index) //
			noexcept(is_nothrow_less_than_comparable<storage_type>)
		{
			SOAGEN_ASSUME(lhs_buffer != nullptr);
			SOAGEN_ASSUME(rhs_buffer != nullptr);

			return less_than(lhs_buffer + lhs_index * sizeof(storage_type),
							 rhs_buffer + rhs_index * sizeof(storage_type));
		}
	};
}
/// @endcond

namespace soagen
{
	/// @brief	Traits for a single column of a table.
	/// @tparam	ValueType	The column's value type.
	/// @tparam	Align		The minimum memory alignment of the first element in the column.
	/// @tparam	ParamType	The type used for the column in function parameter contexts (e.g. push_back()).
	///
	///	@attention	This class is an implementation detail for the soagen-generated Structure-of-arrays classes.
	///				You don't need to know anything about it unless you are implementing your own SoA machinery
	///				without using the soagen generator.
	template <typename ValueType,
			  size_t Align		 = alignof(ValueType),
			  typename ParamType = soagen::param_type<ValueType>>
	struct SOAGEN_EMPTY_BASES column_traits //
		SOAGEN_HIDDEN_BASE(public detail::column_traits_base<storage_type<ValueType>>)
	{
#if SOAGEN_DOXYGEN

		/// @copydoc soagen::storage_type
		using storage_type = POXY_IMPLEMENTATION_DETAIL(dummy);
#else

		using base_traits = detail::column_traits_base<storage_type<ValueType>>;
#endif

		/// @brief	The column's value type.
		using value_type = ValueType;
		static_assert(!std::is_reference_v<value_type>, "column value_type may not be a reference");
		static_assert(!std::is_void_v<value_type>, "column value_type may not be void");
		static_assert(alignof(value_type) == alignof(typename base_traits::storage_type));
		static_assert(sizeof(value_type) == sizeof(typename base_traits::storage_type));

		/// @brief	The minimum memory alignment of the first element in the column.
		///
		/// @note	This is *not* the alignment of each individual element! That is always `alignof(value_type)`.
		///			This value is referring to the alignment of the allocation for the entire column's buffer region.
		static constexpr size_t alignment = max(Align, alignof(value_type));
		static_assert(has_single_bit(alignment), "column alignment must be a power of two");

		/// @brief	The amount of elements to advance to maintain the requested #alignment for this column.
		///
		/// @details	The stride size you need to use when iterating through elements of this column such that
		///				the starting element for each batch would have the same memory alignment as the value specified
		/// 			for #alignment.
		///
		/// @note		Typically you can ignore this; column elements are always aligned correctly according to their
		///				type. This is for over-alignment scenarios where you need to do things in batches (e.g. SIMD).
		static constexpr size_t aligned_stride = lcm(alignment, sizeof(value_type)) / sizeof(value_type);

		/// @brief	The type used for the column in lvalue function parameter contexts (e.g. `push_back(const &)`).
		using param_type = ParamType;
		static_assert(!std::is_void_v<param_type>, "column param_type may not be void");
		static_assert(std::is_reference_v<param_type> || !is_cv<param_type>,
					  "value parameters may not be cv-qualified");
		static_assert(base_traits::template is_constructible<param_type>);

		/// @brief	The type used when forwarding #param_type to the backing container (e.g. `table.emplace_back()`)
		using param_forward_type = forward_type<param_type>;

		/// @brief	The type used for the column in rvalue function parameter contexts (e.g. `push_back(&&)`).
		using rvalue_type = soagen::rvalue_type<param_type>;
		static_assert(base_traits::template is_constructible<rvalue_type>);

		/// @brief	The type used when forwarding #rvalue_type to the backing container (e.g. `table.emplace_back()`)
		using rvalue_forward_type = forward_type<rvalue_type>;

		/// @brief	The default type for `emplace()` and `emplace_back()` for columns that have a `default` value.
		using default_emplace_type = POXY_IMPLEMENTATION_DETAIL(
			std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<rvalue_type>>>);
	};

	/// @cond
	namespace detail
	{
		template <typename>
		struct is_column_traits_ : std::false_type
		{};
		template <typename ValueType, size_t Align, typename ParamType>
		struct is_column_traits_<column_traits<ValueType, Align, ParamType>> : std::true_type
		{};
		template <typename StorageType>
		struct is_column_traits_<detail::column_traits_base<StorageType>> : std::true_type
		{};
	}
	/// @endcond
	/// @brief True if `T` is a #soagen::column_traits.
	template <typename T>
	inline constexpr bool is_column_traits =
		POXY_IMPLEMENTATION_DETAIL(detail::is_column_traits_<std::remove_cv_t<T>>::value);
}

/// @cond
namespace soagen::detail
{
	template <typename T>
	struct to_base_traits_;
	template <typename ValueType, size_t Align, typename ParamType>
	struct to_base_traits_<column_traits<ValueType, Align, ParamType>>
	{
		using type = column_traits_base<storage_type<ValueType>>;

		static_assert(std::is_base_of_v<type, column_traits<ValueType, Align, ParamType>>);
	};
	template <typename T>
	using to_base_traits = typename to_base_traits_<T>::type;

	template <typename T>
	struct as_column_
	{
		using type = column_traits<T>;
	};
	template <typename ValueType, size_t Align, typename ParamType>
	struct as_column_<column_traits<ValueType, Align, ParamType>>
	{
		using type = column_traits<ValueType, Align, ParamType>;
	};
	template <typename StorageType>
	struct as_column_<detail::column_traits_base<StorageType>>
	{
		using type = detail::column_traits_base<StorageType>;
	};
	template <typename T>
	using as_column = typename as_column_<T>::type;
}
/// @endcond

#undef soagen_aligned_storage

#include "header_end.hpp"
