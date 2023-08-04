//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "generated/preprocessor.hpp"
SOAGEN_DISABLE_WARNINGS;

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <type_traits>
#include <new>
#include <utility>
#include <memory>
#include <optional>

#ifndef SOAGEN_COLUMN_SPAN_TYPE
	#if SOAGEN_CPP >= 20 && SOAGEN_HAS_INCLUDE(<span>)
		#include <span>
		#define SOAGEN_COLUMN_SPAN_TYPE std::span
	#elif SOAGEN_HAS_INCLUDE(<muu/span.h>)
		#include <muu/span.h>
		#define SOAGEN_COLUMN_SPAN_TYPE				  muu::span
		#define SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT 1
	#elif SOAGEN_HAS_INCLUDE(<tcb/span.hpp>)
		#include <tcb/span.hpp>
		#define SOAGEN_COLUMN_SPAN_TYPE TCB_SPAN_NAMESPACE_NAME::span
	#endif
#endif

#ifndef SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT
	#define SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT 0
#endif

#ifndef SOAGEN_OPTIONAL_TYPE
	#include <optional>
	#define SOAGEN_OPTIONAL_TYPE std::optional
#endif

SOAGEN_ENABLE_WARNINGS;
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

#ifndef SOAGEN_MAKE_NAME
	#define SOAGEN_MAKE_NAME(Name)                                                                                     \
                                                                                                                       \
		struct name_constant_##Name                                                                                    \
		{                                                                                                              \
			static constexpr const char value[] = #Name;                                                               \
		};                                                                                                             \
                                                                                                                       \
		template <typename T, template <typename> typename Transformation = soagen::identity_type>                     \
		struct named_member_##Name                                                                                     \
		{                                                                                                              \
			Transformation<T> Name;                                                                                    \
                                                                                                                       \
		  protected:                                                                                                   \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr decltype(auto) get_named_member() const noexcept                                                 \
			{                                                                                                          \
				if constexpr (std::is_reference_v<Transformation<T>>)                                                  \
					return static_cast<Transformation<T>>(Name);                                                       \
				else                                                                                                   \
					return Name;                                                                                       \
			}                                                                                                          \
		}
#endif

#ifndef SOAGEN_MAKE_COLUMN
	#define SOAGEN_MAKE_COLUMN(Table, Column, Name)                                                                    \
		template <>                                                                                                    \
		struct column_name<Table, Column> : name_constant_##Name                                                       \
		{};                                                                                                            \
                                                                                                                       \
		template <>                                                                                                    \
		struct column_ref<Table&, Column>                                                                              \
			: named_member_##Name<std::add_lvalue_reference_t<soagen::value_type<Table, static_cast<size_t>(Column)>>> \
		{};                                                                                                            \
                                                                                                                       \
		template <>                                                                                                    \
		struct column_ref<Table&&, Column>                                                                             \
			: named_member_##Name<std::add_rvalue_reference_t<soagen::value_type<Table, static_cast<size_t>(Column)>>> \
		{};                                                                                                            \
                                                                                                                       \
		template <>                                                                                                    \
		struct column_ref<const Table&, Column>                                                                        \
			: named_member_##Name<std::add_lvalue_reference_t<                                                         \
				  std::add_const_t<soagen::value_type<Table, static_cast<size_t>(Column)>>>>                           \
		{};                                                                                                            \
                                                                                                                       \
		template <>                                                                                                    \
		struct column_ref<const Table&&, Column>                                                                       \
			: named_member_##Name<std::add_rvalue_reference_t<                                                         \
				  std::add_const_t<soagen::value_type<Table, static_cast<size_t>(Column)>>>>                           \
		{}
#endif

/// @brief The root namespace for the library.
namespace soagen
{
	using std::size_t;
	using std::ptrdiff_t;
	using std::intptr_t;
	using std::uintptr_t;
	using std::nullptr_t;

	/// @brief	Equivalent to C+20's std::remove_cvref_t.
	template <typename T>
	using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

	/// @brief	Transforms `T` &rarr; `const T&`.
	template <typename T>
	using make_cref = std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>;

	/// @brief	Makes `T` into `T&` if `T` was not already a reference.
	template <typename T>
	using coerce_ref = std::conditional_t<std::is_reference_v<T>, T, std::add_lvalue_reference_t<T>>;

	/// @brief	The identity type transformation.
	template <typename T>
	using identity_type = T;

	/// @brief	The 'identity' base for CRTP scenarios (adds nothing to the interface).
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES identity_base
	{};

	/// @brief	True if `T` is `const` or `volatile` qualified.
	template <typename T>
	inline constexpr bool is_cv = !std::is_same_v<std::remove_cv_t<T>, T>;

	/// @brief	True if `T` is (or is a reference to something that is) `const` or `volatile` qualified.
	template <typename T>
	inline constexpr bool is_cvref = !std::is_same_v<remove_cvref<T>, T>;

	/// @brief	True if `T` is a (non-bool) integer.
	template <typename T>
	inline constexpr bool is_integer = std::is_integral_v<T> && !std::is_same_v<T, bool>;

	/// @brief	True if all `T` are (non-bool) integers.
	template <typename... T>
	inline constexpr bool all_integer = (!!sizeof...(T) && ... && is_integer<T>);

	/// @brief	True if `T` is a (non-bool) unsigned integer.
	template <typename T>
	inline constexpr bool is_unsigned = is_integer<T> && std::is_unsigned_v<T>;

	/// @brief	True if all `U` are the same as `T`.
	template <typename T, typename... U>
	inline constexpr bool any_same = (false || ... || std::is_same_v<T, U>);

	/// @brief	True if `T` is a soagen-generated SoA table type.
	template <typename T>
	inline constexpr bool is_soa = POXY_IMPLEMENTATION_DETAIL(false); // specialized in generated code
	/// @cond
	template <typename T>
	inline constexpr bool is_soa<const T> = is_soa<T>;
	template <typename T>
	inline constexpr bool is_soa<volatile T> = is_soa<T>;
	template <typename T>
	inline constexpr bool is_soa<const volatile T> = is_soa<T>;
	/// @endcond

	/// @brief  True if `T` meets the `ImplicitLifetimeType` named requirement.
	template <typename T>
	inline constexpr bool is_implicit_lifetime_type =
		std::is_scalar_v<T> || std::is_array_v<T>
		|| (std::is_aggregate_v<T> && std::is_trivially_constructible_v<T> && std::is_trivially_destructible_v<T>);

	/// @brief  Alias for `std::integral_constant<std::size_t, Value>`
	template <auto Value>
	using index_constant = std::integral_constant<size_t, static_cast<size_t>(Value)>;

	/// @brief  The type that would result from `std::forward<T>()`.
	template <typename T>
	using forward_type = POXY_IMPLEMENTATION_DETAIL(
		std::conditional_t<std::is_lvalue_reference_v<T>, T, std::add_rvalue_reference_t<T>>);

#ifdef SOAGEN_COLUMN_SPAN_TYPE

	#if SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT

	template <typename T, size_t Align = alignof(T)>
	using column_span = SOAGEN_COLUMN_SPAN_TYPE<T, static_cast<size_t>(-1), Align>;

	#else

	template <typename T, size_t Align = alignof(T)>
	using column_span = SOAGEN_COLUMN_SPAN_TYPE<T>;

	#endif

	template <typename T, size_t Align = alignof(T)>
	using const_column_span = column_span<std::add_const_t<T>, Align>;

#endif

#if SOAGEN_DOXYGEN
	/// @brief The optional type used by the library.
	/// @note This can be overridden by redefining `SOAGEN_OPTIONAL_TYPE`, e.g.: @cpp
	///
	///	#define SOAGEN_OPTIONAL_TYPE tl::optional
	///
	///	@ecpp
	template <typename T>
	using optional = std::optional<T>;
#else
	using SOAGEN_OPTIONAL_TYPE;
#endif

	/// @cond
	namespace detail
	{
		template <template <typename...> typename Trait, typename Enabler, typename... Args>
		struct is_detected_impl : std::false_type
		{};
		template <template <typename...> typename Trait, typename... Args>
		struct is_detected_impl<Trait, std::void_t<Trait<Args...>>, Args...> : std::true_type
		{};
		template <template <typename...> typename Trait, typename... Args>
		inline constexpr auto is_detected_ = is_detected_impl<Trait, void, Args...>::value;
	}
	/// @endcond

	/// @brief Detects if a Trait can be applied to a set of Args.
	template <template <typename...> typename Trait, typename... Args>
	inline constexpr auto is_detected = detail::is_detected_<Trait, Args...>;

	/// @cond
	namespace detail
	{
		template <typename T>
		using has_swap_member_ = decltype(std::declval<T&>().swap(std::declval<T&>()));

		template <typename T, typename Arg>
		using has_resize_member_ = decltype(std::declval<T&>().resize(std::declval<const Arg&>()));

		template <typename T, typename Arg>
		using has_erase_member_ = decltype(std::declval<T&>().erase(std::declval<const Arg&>()));

		template <typename T, typename Arg>
		using has_unordered_erase_member_ = decltype(std::declval<T&>().unordered_erase(std::declval<const Arg&>()));

		template <typename T>
		using has_data_member_ = decltype(std::declval<T&>().data());

		template <typename T, typename U>
		using is_equality_comparable_ = decltype(std::declval<const std::remove_reference_t<T>&>()
												 == std::declval<const std::remove_reference_t<U>&>());

		template <typename T, typename U>
		using is_less_than_comparable_ = decltype(std::declval<const std::remove_reference_t<T>&>()
												  < std::declval<const std::remove_reference_t<U>&>());
	}
	/// @endcond

	/// @brief	True if `T` has a `swap(T&)` member.
	template <typename T>
	inline constexpr bool has_swap_member = is_detected<detail::has_swap_member_, T>;

	/// @brief	True if `T` has a `resize(size_t)` member.
	template <typename T, typename Arg = size_t>
	inline constexpr bool has_resize_member = is_detected<detail::has_resize_member_, T, Arg>;

	/// @brief	True if `T` has an `erase(size_t)` member.
	template <typename T, typename Arg = size_t>
	inline constexpr bool has_erase_member = is_detected<detail::has_erase_member_, T, Arg>;

	/// @brief	True if `T` has an `unordered_erase(size_t)` member.
	template <typename T, typename Arg = size_t>
	inline constexpr bool has_unordered_erase_member = is_detected<detail::has_unordered_erase_member_, T, Arg>;

	/// @brief	True if `T` has a `data()` member.
	template <typename T>
	inline constexpr bool has_data_member = is_detected<detail::has_data_member_, T>;

	/// @brief	True if `T` and `U` meet the `EqualityComparable` named requirement.
	template <typename T, typename U = T>
	inline constexpr bool is_equality_comparable = is_detected<detail::is_equality_comparable_, T, U>;

	/// @brief	True if `T` and `U` meet the `LessThanComparable` named requirement.
	template <typename T, typename U = T>
	inline constexpr bool is_less_than_comparable = is_detected<detail::is_less_than_comparable_, T, U>;

	/// @cond
	namespace detail
	{
		template <typename T, bool = has_swap_member<T>>
		struct has_nothrow_swap_member_ : std::bool_constant<noexcept(std::declval<T&>().swap(std::declval<T&>()))>
		{};
		template <typename T>
		struct has_nothrow_swap_member_<T, false> : std::false_type
		{};

		template <typename T, typename Arg, bool = has_resize_member<T, Arg>>
		struct has_nothrow_resize_member_
			: std::bool_constant<noexcept(std::declval<T&>().resize(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_resize_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, typename Arg, bool = has_erase_member<T, Arg>>
		struct has_nothrow_erase_member_
			: std::bool_constant<noexcept(std::declval<T&>().erase(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_erase_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, typename Arg, bool = has_unordered_erase_member<T, Arg>>
		struct has_nothrow_unordered_erase_member_
			: std::bool_constant<noexcept(std::declval<T&>().unordered_erase(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_unordered_erase_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, bool = has_data_member<T>>
		struct has_nothrow_data_member_ : std::bool_constant<noexcept(std::declval<T&>().data())>
		{};
		template <typename T>
		struct has_nothrow_data_member_<T, false> : std::false_type
		{};

		template <typename T, typename U, bool = is_equality_comparable<T, U>>
		struct is_nothrow_equality_comparable_
			: std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
										  == std::declval<const std::remove_reference_t<U>&>())>
		{};
		template <typename T, typename U>
		struct is_nothrow_equality_comparable_<T, U, false> : std::false_type
		{};

		template <typename T, typename U, bool = is_less_than_comparable<T, U>>
		struct is_nothrow_less_than_comparable_
			: std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
										  < std::declval<const std::remove_reference_t<U>&>())>
		{};
		template <typename T, typename U>
		struct is_nothrow_less_than_comparable_<T, U, false> : std::false_type
		{};
	}
	/// @endcond

	/// @brief	True if `T` has a non-throwing `swap(T&)` member.
	template <typename T>
	inline constexpr bool has_nothrow_swap_member = detail::has_nothrow_swap_member_<T>::value;

	/// @brief	True if `T` has a non-throwing `resize(size_t)` member.
	template <typename T, typename Arg = size_t>
	inline constexpr bool has_nothrow_resize_member = detail::has_nothrow_resize_member_<T, Arg>::value;

	/// @brief	True if `T` has a non-throwing `erase(size_t)` member.
	template <typename T, typename Arg = size_t>
	inline constexpr bool has_nothrow_erase_member = detail::has_nothrow_erase_member_<T, Arg>::value;

	/// @brief	True if `T` has a non-throwing `unordered_erase(size_t)` member.
	template <typename T, typename Arg = size_t>
	inline constexpr bool has_nothrow_unordered_erase_member =
		detail::has_nothrow_unordered_erase_member_<T, Arg>::value;

	/// @brief	True if `T` has a non-throwing `data()` member.
	template <typename T>
	inline constexpr bool has_nothrow_data_member = detail::has_nothrow_data_member_<T>::value;

	/// @brief	True if `T` and `U` meet the `EqualityComparable` named requirement without throwing.
	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_equality_comparable = detail::is_nothrow_equality_comparable_<T, U>::value;

	/// @brief	True if `T` and `U` meet the `LessThanComparable` named requirement without throwing.
	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_less_than_comparable = detail::is_nothrow_less_than_comparable_<T, U>::value;

	/// @cond
	namespace detail
	{
		template <typename T>
		using has_tuple_size_ = decltype(std::tuple_size<remove_cvref<T>>::value);
		template <typename T>
		using has_tuple_element_ = decltype(std::declval<typename std::tuple_element<0, remove_cvref<T>>::type>());
	}
	/// @endcond

	/// @brief	True if `T` implements std::tuple_size and std::tuple_element.
	template <typename T>
	inline constexpr bool is_tuple_like =
		is_detected<detail::has_tuple_size_, T> && is_detected<detail::has_tuple_element_, T>;

#if !SOAGEN_DOXYGEN && SOAGEN_HAS_BUILTIN(__type_pack_element)

	template <size_t I, typename... T>
	using type_at_index = __type_pack_element<I, T...>;

#else

	/// @cond
	namespace detail
	{
		template <size_t I, typename T, typename... U>
		struct type_at_index_impl
		{
			using type = typename type_at_index_impl<I - 1, U...>::type;
		};

		template <typename T, typename... U>
		struct type_at_index_impl<0, T, U...>
		{
			using type = T;
		};
	}
	/// @endcond

	/// @brief Gets the type `T` at index `I` in the parameter pack.
	template <size_t I, typename... T>
	using type_at_index = POXY_IMPLEMENTATION_DETAIL(typename detail::type_at_index_impl<I, T...>::type);

#endif

	/// @cond
	namespace detail
	{
		template <typename T>
		struct table_type_
		{
			using type = typename T::table_type;
		};
	}
	/// @endcond

	/// @brief Gets the underlying #soagen::table of an SoA type.
	template <typename T>
	using table_type = POXY_IMPLEMENTATION_DETAIL(typename detail::table_type_<std::remove_cv_t<T>>::type);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct table_traits_type_
		{
			using type = typename T::table_traits;
		};
	}
	/// @endcond

	/// @brief Gets the #soagen::table_traits for the underlying #soagen::table of an SoA type.
	template <typename T>
	using table_traits_type =
		POXY_IMPLEMENTATION_DETAIL(typename detail::table_traits_type_<std::remove_cv_t<T>>::type);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct allocator_type_
		{
			using type = typename T::allocator_type;
		};
	}
	/// @endcond

	/// @brief Gets the allocator being used by the #soagen::table of an SoA type.
	template <typename T>
	using allocator_type = POXY_IMPLEMENTATION_DETAIL(typename detail::allocator_type_<std::remove_cv_t<T>>::type);

	/// @brief Gets the #soagen::column_traits::value_type for the selected column of an SoA type.
	template <typename T, auto Column>
	using value_type = POXY_IMPLEMENTATION_DETAIL(
		typename table_traits_type<T>::template column<static_cast<size_t>(Column)>::value_type);

	/// @cond
	namespace detail
	{
		template <typename ValueType>
		struct storage_type_
		{
			using type = ValueType;
		};
		// store all pointers as void*
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
		// strip off const and volatile
		template <typename T>
		struct storage_type_<const T> : public storage_type_<T>
		{};
		template <typename T>
		struct storage_type_<volatile T> : public storage_type_<T>
		{};
		template <typename T>
		struct storage_type_<const volatile T> : public storage_type_<T>
		{};
		// store byte-like types as std::byte (since these pointers can legally alias each other)
		template <>
		struct storage_type_<char> : public storage_type_<std::byte>
		{};
		template <>
		struct storage_type_<unsigned char> : public storage_type_<std::byte>
		{};
	}
	/// @endcond

	/// @brief		The internal storage type soagen will use to store a column.
	///
	/// @details	In most cases it will be the same as the `ValueType`, but in some circumstances soagen is able to
	///				reduce the number of template instantiations (and thus binary size) by applying simple and safe type
	///				transformations (e.g. removing `const` and `volatile`, storing all pointer types as `void*`,
	///				et cetera.)
	///
	/// @attention	<b>This has no impact on the interfaces of soagen-generated tables!</b> This is an internal detail
	///				that only has meaning to advanced users who wish to build their own SoA types around soagen's
	///				table machinery, rather than use the generator.
	template <typename ValueType>
	using storage_type = POXY_IMPLEMENTATION_DETAIL(typename detail::storage_type_<ValueType>::type);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct param_type_
		{
			static_assert(!std::is_reference_v<T>);

			using type = std::conditional_t<
				// move-only types
				!std::is_copy_constructible_v<T> && std::is_move_constructible_v<T>,
				std::add_rvalue_reference_t<std::remove_cv_t<T>>,
				std::conditional_t<
					// small + trivial types
					std::is_scalar_v<T> || std::is_fundamental_v<T>
						|| (std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(void*) * 2),
					std::remove_cv_t<T>,
					// everything else
					std::add_lvalue_reference_t<std::add_const_t<T>>>>;
		};
		// references are kept as-is
		template <typename T>
		struct param_type_<T&>
		{
			using type = T&;
		};
		template <typename T>
		struct param_type_<T&&>
		{
			using type = T&&;
		};
		// ... except const rvalues, which are converted to const lvalues (because const T&& is nonsense)
		template <typename T>
		struct param_type_<const T&&> : param_type_<const T&>
		{};
		template <typename T>
		struct param_type_<const volatile T&&> : param_type_<const volatile T&>
		{};

	}
	/// @endcond

	/// @brief		The default type soagen will use for a column in lvalue parameter contexts (e.g. `push_back(const&)`).
	///
	/// @details	Types chosen by this trait aim to be a good default:
	/// <table>
	/// <tr><td> Move-only types                   <td> `T&&`
	/// <tr><td> Small, trivially-copyable types   <td> `T`
	/// <tr><td> Everything else                   <td> `const T&`
	/// </table>
	template <typename ValueType>
	using param_type = POXY_IMPLEMENTATION_DETAIL(typename detail::param_type_<ValueType>::type);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct rvalue_type_
		{
			using type = std::conditional_t<
				// if the param_type of unreferenced, unqualified T would be a value or an rvalue, use that
				std::is_rvalue_reference_v<param_type<remove_cvref<T>>>
					|| !std::is_reference_v<param_type<remove_cvref<T>>>,
				param_type<remove_cvref<T>>,
				std::conditional_t<
					// copy-only things
					!std::is_move_constructible_v<remove_cvref<T>>,
					std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>,
					// rvalues
					std::add_rvalue_reference_t<remove_cvref<T>>>>;
		};
	}
	/// @endcond

	/// @brief		The type soagen will use for a column in rvalue parameter contexts (e.g. `push_back(&&)`).
	template <typename ParamType>
	using rvalue_type = POXY_IMPLEMENTATION_DETAIL(typename detail::rvalue_type_<ParamType>::type);

	/// @brief Helper class for #table::emplace() and #table::emplace_back().
	template <typename... Args>
	struct emplacer
	{
		static_assert(sizeof...(Args));
		static_assert((std::is_reference_v<Args> && ...));

		void* ptrs[sizeof...(Args)];

		SOAGEN_DEFAULT_RULE_OF_FIVE(emplacer);

		SOAGEN_NODISCARD_CTOR
		constexpr emplacer(Args&&... args) noexcept //
			: ptrs{ const_cast<void*>(static_cast<const volatile void*>(&args))... }
		{}
	};

	/// @cond
	template <>
	struct emplacer<>
	{};
	template <typename... Args>
	emplacer(Args&&...) -> emplacer<Args&&...>;
	/// @endcond

	/// @brief True if `T` is an instance of #soagen::emplacer.
	template <typename T>
	inline constexpr bool is_emplacer = POXY_IMPLEMENTATION_DETAIL(false);
	/// @cond
	template <typename... T>
	inline constexpr bool is_emplacer<emplacer<T...>> = true;
	template <typename T>
	inline constexpr bool is_emplacer<const T> = is_emplacer<T>;
	template <typename T>
	inline constexpr bool is_emplacer<volatile T> = is_emplacer<T>;
	template <typename T>
	inline constexpr bool is_emplacer<const volatile T> = is_emplacer<T>;
	/// @endcond

	/// @cond
	namespace detail
	{
		// note: this machinery is so that we only instantiate the is_nothrow version
		// when the regular version is true

		template <typename...>
		struct is_nothrow_invocable_indirect_ : std::false_type
		{};

		template <typename Func, typename... Args>
		struct is_nothrow_invocable_indirect_<std::true_type, Func, Args...> : std::is_nothrow_invocable<Func, Args...>
		{};

		template <typename Func, typename... Args>
		struct is_invocable_ : std::is_invocable<Func, Args...>
		{
			using is_nothrow =
				is_nothrow_invocable_indirect_<std::bool_constant<std::is_invocable<Func, Args...>::value>,
											   Func,
											   Args...>;
		};
	}
	/// @endcond

	/// @brief	True if `Func` is invocable with `Args`.
	template <typename Func, typename... Args>
	inline constexpr bool is_invocable = detail::is_invocable_<Func, Args...>::value;

	/// @brief	True if `Func` is nothrow-invocable with `Args`.
	template <typename Func, typename... Args>
	inline constexpr bool is_nothrow_invocable = detail::is_invocable_<Func, Args...>::is_nothrow::value;

	/// @cond
	namespace detail
	{
		template <size_t I, typename Func, typename Arg>
		struct is_invocable_with_optional_index_							//
			: std::disjunction<is_invocable_<Func, Arg, index_constant<I>>, //
							   is_invocable_<Func, Arg, size_t>,			//
							   is_invocable_<Func, index_constant<I>, Arg>, //
							   is_invocable_<Func, size_t, Arg>,			//
							   is_invocable_<Func, Arg>>
		{};
	}
	/// @endcond

	/// @brief	True if `Func` is invocable with `Arg` and an index_constant and/or size_t.
	template <size_t I, typename Func, typename Arg>
	inline constexpr bool is_invocable_with_optional_index =
		detail::is_invocable_with_optional_index_<I, Func, Arg>::value;

	/// @brief	True if `Func` is nothrow-invocable with `Arg` and an index_constant and/or size_t.
	template <size_t I, typename Func, typename Arg>
	inline constexpr bool is_nothrow_invocable_with_optional_index =
		detail::is_invocable_with_optional_index_<I, Func, Arg>::is_nothrow::value;

	template <size_t I, typename Func, typename Arg>
	SOAGEN_ALWAYS_INLINE
	constexpr decltype(auto) invoke_with_optional_index(Func&& func,
														Arg&& arg) //
		noexcept(is_nothrow_invocable_with_optional_index<I, Func&&, Arg&&>)
	{
		static_assert(is_invocable_with_optional_index<I, Func&&, Arg&&>);

		if constexpr (is_invocable<Func&&, Arg&&, index_constant<I>>)
		{
			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg), index_constant<I>{});
		}
		else if constexpr (is_invocable<Func&&, Arg&&, size_t>)
		{
			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg), I);
		}
		else if constexpr (is_invocable<Func&&, index_constant<I>, Arg&&>)
		{
			return static_cast<Func&&>(func)(index_constant<I>{}, static_cast<Arg&&>(arg));
		}
		else if constexpr (is_invocable<Func&&, size_t, Arg&&>)
		{
			return static_cast<Func&&>(func)(I, static_cast<Arg&&>(arg));
		}
		else
		{
			static_assert(is_invocable<Func&&, Arg&&>);

			return static_cast<Func&&>(func)(static_cast<Arg&&>(arg));
		}
	}

	/// @cond
	namespace detail
	{
		template <typename Table, size_t ColumnIndex>
		struct column_name;
		template <typename Table, size_t ColumnIndex>
		struct column_ref;

		template <typename A, typename B>
		inline constexpr bool same_table_type =
			std::is_same_v<table_type<remove_cvref<A>>, table_type<remove_cvref<B>>>;
	}
	/// @endcond
}

/// @cond
namespace std
{
	template <typename... Args>
	struct tuple_size<soagen::emplacer<Args...>> //
		: std::integral_constant<size_t, sizeof...(Args)>
	{};
}
/// @endcond

#include "header_end.hpp"
