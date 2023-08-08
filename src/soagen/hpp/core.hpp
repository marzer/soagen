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
#include <iterator>
#if SOAGEN_HAS_EXCEPTIONS
	#include <stdexcept>
#endif

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

#ifndef SOAGEN_MAKE_NAME
	#define SOAGEN_MAKE_NAME(Name)                                                                                     \
                                                                                                                       \
		struct name_constant_##Name                                                                                    \
		{                                                                                                              \
			static constexpr const char value[] = #Name;                                                               \
		};                                                                                                             \
                                                                                                                       \
		template <typename T>                                                                                          \
		struct named_ref_##Name                                                                                        \
		{                                                                                                              \
			T Name;                                                                                                    \
                                                                                                                       \
		  protected:                                                                                                   \
			SOAGEN_PURE_INLINE_GETTER                                                                                  \
			constexpr decltype(auto) get_ref() const noexcept                                                          \
			{                                                                                                          \
				if constexpr (std::is_reference_v<T>)                                                                  \
					return static_cast<T>(Name);                                                                       \
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
		struct column_ref<Table, Column>                                                                               \
			: named_ref_##Name<std::add_lvalue_reference_t<soagen::value_type<Table, static_cast<size_t>(Column)>>>    \
		{};                                                                                                            \
                                                                                                                       \
		template <>                                                                                                    \
		struct column_ref<Table&&, Column>                                                                             \
			: named_ref_##Name<std::add_rvalue_reference_t<soagen::value_type<Table, static_cast<size_t>(Column)>>>    \
		{};                                                                                                            \
                                                                                                                       \
		template <>                                                                                                    \
		struct column_ref<const Table, Column>                                                                         \
			: named_ref_##Name<std::add_lvalue_reference_t<                                                            \
				  std::add_const_t<soagen::value_type<Table, static_cast<size_t>(Column)>>>>                           \
		{};                                                                                                            \
                                                                                                                       \
		template <>                                                                                                    \
		struct column_ref<const Table&&, Column>                                                                       \
			: named_ref_##Name<std::add_rvalue_reference_t<                                                            \
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

	/// @cond
	// forward declarations
	template <typename... Args>
	struct emplacer;
	template <typename, size_t...>
	struct row;
	template <typename Table, size_t... Columns>
	class iterator;
	template <typename... Columns>
	struct table_traits;
	template <typename Traits, typename Allocator>
	class table;
	/// @endcond

	/// @brief	Equivalent to C+20's std::remove_cvref_t.
	template <typename T>
	using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

	/// @brief	Transforms `T` &rarr; `const T&`.
	template <typename T>
	using make_cref = std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>;

	/// @brief	Makes `T` into `T&` if `T` was not already a reference.
	template <typename T>
	using coerce_ref = std::conditional_t<std::is_reference_v<T>, T, std::add_lvalue_reference_t<T>>;

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

	/// @brief	True if any `U` are the same as `T`.
	template <typename T, typename... U>
	inline constexpr bool any_same = (false || ... || std::is_same_v<T, U>);

	/// @brief	True if `Value` is in the list `Values`.
	template <auto Value, auto... Values>
	inline constexpr bool any_same_value = ((Value == Values) || ...);

	/// @cond
	namespace detail
	{
		template <typename>
		struct is_table_ : std::false_type
		{};
		template <typename... Args>
		struct is_table_<table<Args...>> : std::true_type
		{};
	}
	/// @endcond
	/// @brief True if `T` is a #soagen::table.
	template <typename T>
	inline constexpr bool is_table = POXY_IMPLEMENTATION_DETAIL(detail::is_table_<std::remove_cv_t<T>>::value);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct is_soa_ : is_table_<T> // specialized in the generated code
		{};
	}
	/// @endcond
	/// @brief	True if `T` is a #soagen::table or a soagen-generated SoA class.
	template <typename T>
	inline constexpr bool is_soa = POXY_IMPLEMENTATION_DETAIL(detail::is_soa_<std::remove_cv_t<T>>::value);

	/// @brief	Conditionally adds `const` to a type.
	template <typename T, bool Cond>
	using conditionally_add_const = std::conditional_t<Cond, std::add_const_t<T>, T>;

	/// @brief	Conditionally adds `volatile` to a type.
	template <typename T, bool Cond>
	using conditionally_add_volatile = std::conditional_t<Cond, std::add_volatile_t<T>, T>;

	/// @cond
	namespace detail
	{
		template <typename T>
		using is_implicit_lifetime_type_ = std::disjunction<std::is_scalar<T>,
															std::is_array<T>,
															std::conjunction<std::is_aggregate<T>,
																			 std::is_trivially_constructible<T>,
																			 std::is_trivially_destructible<T>>>;

		template <typename T, typename>
		struct copy_cvref_
		{
			using type = T;
		};
		template <typename T, typename CopyFrom>
		struct copy_cvref_<T, CopyFrom&>
		{
			using type = std::add_lvalue_reference_t<typename copy_cvref_<T, CopyFrom>::type>;
		};
		template <typename T, typename CopyFrom>
		struct copy_cvref_<T, CopyFrom&&>
		{
			using type = std::add_rvalue_reference_t<typename copy_cvref_<T, CopyFrom>::type>;
		};
		template <typename T, typename CopyFrom>
		struct copy_cvref_<T, const CopyFrom>
		{
			using type = std::add_const_t<typename copy_cvref_<T, CopyFrom>::type>;
		};
		template <typename T, typename CopyFrom>
		struct copy_cvref_<T, volatile CopyFrom>
		{
			using type = std::add_volatile_t<typename copy_cvref_<T, CopyFrom>::type>;
		};
		template <typename T, typename CopyFrom>
		struct copy_cvref_<T, const volatile CopyFrom>
		{
			using type = std::add_cv_t<typename copy_cvref_<T, CopyFrom>::type>;
		};

		template <typename T>
		struct remove_lvalue_ref_
		{
			using type = T;
		};
		template <typename T>
		struct remove_lvalue_ref_<T&>
		{
			using type = T;
		};
	}
	/// @endcond

	/// @brief  True if `T` meets the `ImplicitLifetimeType` named requirement.
	template <typename T>
	inline constexpr bool is_implicit_lifetime_type =
		POXY_IMPLEMENTATION_DETAIL(detail::is_implicit_lifetime_type_<T>::value);

	/// @brief  Copies the cvref-qualifiers from one type onto another, replacing the existing ones.
	template <typename T, typename CopyFrom>
	using copy_cvref = POXY_IMPLEMENTATION_DETAIL(typename detail::copy_cvref_<remove_cvref<T>, CopyFrom>::type);

	/// @brief	Removes lvalue reference qualifiers. Rvalues references are preserved as-is.
	template <typename T>
	using remove_lvalue_ref = POXY_IMPLEMENTATION_DETAIL(typename detail::remove_lvalue_ref_<T>::type);

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
		using is_detected_ = is_detected_impl<Trait, void, Args...>;
	}
	/// @endcond

	/// @brief Detects if `Trait` can be applied to a set of `Args`.
	template <template <typename...> typename Trait, typename... Args>
	inline constexpr auto is_detected = POXY_IMPLEMENTATION_DETAIL(detail::is_detected_<Trait, Args...>::value);

	/// @cond
	namespace detail
	{
		template <typename T>
		using has_swap_member_impl_ = decltype(std::declval<T&>().swap(std::declval<T&>()));
		template <typename T>
		using has_swap_member_ = is_detected_<has_swap_member_impl_, T>;

		template <typename T, typename Arg>
		using has_resize_member_impl_ = decltype(std::declval<T&>().resize(std::declval<const Arg&>()));
		template <typename T, typename Arg = size_t>
		using has_resize_member_ = is_detected_<has_resize_member_impl_, T, Arg>;

		template <typename T, typename Arg>
		using has_erase_member_impl_ = decltype(std::declval<T&>().erase(std::declval<const Arg&>()));
		template <typename T, typename Arg = size_t>
		using has_erase_member_ = is_detected_<has_erase_member_impl_, T, Arg>;

		template <typename T, typename Arg>
		using has_unordered_erase_member_impl_ =
			decltype(std::declval<T&>().unordered_erase(std::declval<const Arg&>()));
		template <typename T, typename Arg = size_t>
		using has_unordered_erase_member_ = is_detected_<has_unordered_erase_member_impl_, T, Arg>;

		template <typename T>
		using has_data_member_impl_ = decltype(std::declval<T&>().data());
		template <typename T>
		using has_data_member_ = is_detected_<has_data_member_impl_, T>;

		template <typename T, typename U>
		using is_equality_comparable_impl_ = decltype(std::declval<const std::remove_reference_t<T>&>()
													  == std::declval<const std::remove_reference_t<U>&>());
		template <typename T, typename U = T>
		using is_equality_comparable_ = is_detected_<is_equality_comparable_impl_, T, U>;

		template <typename T, typename U>
		using is_less_than_comparable_impl_ = decltype(std::declval<const std::remove_reference_t<T>&>()
													   < std::declval<const std::remove_reference_t<U>&>());
		template <typename T, typename U = T>
		using is_less_than_comparable_ = is_detected_<is_less_than_comparable_impl_, T, U>;
	}
	/// @endcond

	/// @brief	True if all `T` have a `swap(T&)` member.
	template <typename... T>
	inline constexpr bool has_swap_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_swap_member_<T>...>::value);

	/// @brief	True if all `T` have a `resize(size_t)` member.
	template <typename... T>
	inline constexpr bool has_resize_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_resize_member_<T, size_t>...>::value);

	/// @brief	True if all `T` have an `erase(size_t)` member.
	template <typename... T>
	inline constexpr bool has_erase_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_erase_member_<T, size_t>...>::value);

	/// @brief	True if all `T` have an `unordered_erase(size_t)` member.
	template <typename... T>
	inline constexpr bool has_unordered_erase_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_unordered_erase_member_<T, size_t>...>::value);

	/// @brief	True if all `T` have a `data()` member.
	template <typename... T>
	inline constexpr bool has_data_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_data_member_<T>...>::value);

	/// @brief	True if `T` and `U` meet the `EqualityComparable` named requirement.
	template <typename T, typename U = T>
	inline constexpr bool is_equality_comparable =
		POXY_IMPLEMENTATION_DETAIL(detail::is_equality_comparable_<T, U>::value);

	/// @brief	True if `T` and `U` meet the `LessThanComparable` named requirement.
	template <typename T, typename U = T>
	inline constexpr bool is_less_than_comparable =
		POXY_IMPLEMENTATION_DETAIL(detail::is_less_than_comparable_<T, U>::value);

	/// @cond
	namespace detail
	{
		template <typename T, bool = has_swap_member<T>>
		struct has_nothrow_swap_member_ : std::bool_constant<noexcept(std::declval<T&>().swap(std::declval<T&>()))>
		{};
		template <typename T>
		struct has_nothrow_swap_member_<T, false> : std::false_type
		{};

		template <typename T, typename Arg = size_t, bool = has_resize_member<T, Arg>>
		struct has_nothrow_resize_member_
			: std::bool_constant<noexcept(std::declval<T&>().resize(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_resize_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, typename Arg = size_t, bool = has_erase_member<T, Arg>>
		struct has_nothrow_erase_member_
			: std::bool_constant<noexcept(std::declval<T&>().erase(std::declval<const Arg&>()))>
		{};
		template <typename T, typename Arg>
		struct has_nothrow_erase_member_<T, Arg, false> : std::false_type
		{};

		template <typename T, typename Arg = size_t, bool = has_unordered_erase_member<T, Arg>>
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

		template <typename T, typename U = T, bool = is_equality_comparable<T, U>>
		struct is_nothrow_equality_comparable_
			: std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
										  == std::declval<const std::remove_reference_t<U>&>())>
		{};
		template <typename T, typename U>
		struct is_nothrow_equality_comparable_<T, U, false> : std::false_type
		{};

		template <typename T, typename U = T, bool = is_less_than_comparable<T, U>>
		struct is_nothrow_less_than_comparable_
			: std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
										  < std::declval<const std::remove_reference_t<U>&>())>
		{};
		template <typename T, typename U>
		struct is_nothrow_less_than_comparable_<T, U, false> : std::false_type
		{};
	}
	/// @endcond

	/// @brief	True if all `T` have a non-throwing `swap(T&)` member.
	template <typename... T>
	inline constexpr bool has_nothrow_swap_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_nothrow_swap_member_<T>...>::value);

	/// @brief	True if all `T` have a non-throwing `resize(size_t)` member.
	template <typename... T>
	inline constexpr bool has_nothrow_resize_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_nothrow_resize_member_<T, size_t>...>::value);

	/// @brief	True if all `T` have a non-throwing `erase(size_t)` member.
	template <typename... T>
	inline constexpr bool has_nothrow_erase_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_nothrow_erase_member_<T, size_t>...>::value);

	/// @brief	True if all `T` have a non-throwing `unordered_erase(size_t)` member.
	template <typename... T>
	inline constexpr bool has_nothrow_unordered_erase_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_nothrow_unordered_erase_member_<T, size_t>...>::value);

	/// @brief	True if all `T` have a non-throwing `data()` member.
	template <typename... T>
	inline constexpr bool has_nothrow_data_member =
		POXY_IMPLEMENTATION_DETAIL(std::conjunction<detail::has_nothrow_data_member_<T>...>::value);

	/// @brief	True if `T` and `U` meet the `EqualityComparable` named requirement without throwing.
	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_equality_comparable =
		POXY_IMPLEMENTATION_DETAIL(detail::is_nothrow_equality_comparable_<T, U>::value);

	/// @brief	True if `T` and `U` meet the `LessThanComparable` named requirement without throwing.
	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_less_than_comparable =
		POXY_IMPLEMENTATION_DETAIL(detail::is_nothrow_less_than_comparable_<T, U>::value);

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
	using table_type = POXY_IMPLEMENTATION_DETAIL(typename detail::table_type_<remove_cvref<T>>::type);

	/// @brief True if two types have the same underlying #soagen::table type.
	template <typename A, typename B>
	inline constexpr bool same_table_type =
		POXY_IMPLEMENTATION_DETAIL(std::is_same_v<table_type<remove_cvref<A>>, table_type<remove_cvref<B>>>);

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
	using table_traits_type = POXY_IMPLEMENTATION_DETAIL(typename detail::table_traits_type_<remove_cvref<T>>::type);

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
	using allocator_type = POXY_IMPLEMENTATION_DETAIL(typename detail::allocator_type_<remove_cvref<T>>::type);

	/// @brief Gets the #soagen::column_traits::value_type for the selected column of an SoA type.
	template <typename T, auto Column>
	using value_type = POXY_IMPLEMENTATION_DETAIL(
		typename table_traits_type<remove_cvref<T>>::template column<static_cast<size_t>(Column)>::value_type);

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

	/// @cond
	namespace detail
	{
		// machinery to only instantiate one or more secondary traits when the primary one is true
		// e.g. for traits that come in pairs like is_invocable / is_nothrow_invocable

		template <typename...>
		struct types_;

		template <bool, template <typename...> typename, typename...>
		struct nested_trait_indirect_ : std::false_type
		{};

		template <template <typename...> typename Trait, typename... Args>
		struct nested_trait_indirect_<true, Trait, Args...> : Trait<Args...>
		{};

		template <typename, template <typename...> typename, template <typename...> typename...>
		struct nested_trait_;

		template <template <typename...> typename Trait,
				  template <typename...>
				  typename... NestedTraits,
				  typename... Args>
		struct nested_trait_<types_<Args...>, Trait, NestedTraits...> : Trait<Args...>
		{
			template <size_t Index>
			using nested =
				type_at_index<Index, nested_trait_indirect_<Trait<Args...>::value, NestedTraits, Args...>...>;
		};
	}
	/// @endcond

	/// @cond
	namespace detail
	{
		template <typename Table, size_t ColumnIndex>
		struct column_name;
		template <typename Table, size_t ColumnIndex>
		struct column_ref;
	}
	/// @endcond
}

#include "header_end.hpp"
