//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "generated/preprocessor.hpp"
#include "generated/functions.hpp"
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
	template <typename>
	struct allocator_traits;
	struct allocator;
	template <typename...>
	struct emplacer;
	template <typename, size_t...>
	struct row;
	template <typename, size_t...>
	class iterator;
	template <typename>
	class span;
	template <typename...>
	struct table_traits;
	template <typename, typename>
	class table;
	/// @endcond

	/// @brief	Equivalent to C+20's std::remove_cvref_t.
	template <typename T>
	using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

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

	/// @brief	Conditionally adds `const` to a type.
	template <typename T, bool Cond>
	using conditionally_add_const = std::conditional_t<Cond, std::add_const_t<T>, T>;

	/// @brief	Conditionally adds `volatile` to a type.
	template <typename T, bool Cond>
	using conditionally_add_volatile = std::conditional_t<Cond, std::add_volatile_t<T>, T>;

	/// @brief	Conditionally removes all cvref-qualifiers from a type.
	template <typename T, bool Cond>
	using conditionally_remove_cvref = std::conditional_t<Cond, remove_cvref<T>, T>;

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
		struct copy_cv_
		{
			using type = T;
		};
		template <typename T, typename CopyFrom>
		struct copy_cv_<T, const CopyFrom> : std::add_const<T>
		{};
		template <typename T, typename CopyFrom>
		struct copy_cv_<T, volatile CopyFrom> : std::add_volatile<T>
		{};
		template <typename T, typename CopyFrom>
		struct copy_cv_<T, const volatile CopyFrom> : std::add_cv<T>
		{};

		template <typename T, typename>
		struct copy_ref_
		{
			using type = T;
		};
		template <typename T, typename CopyFrom>
		struct copy_ref_<T, CopyFrom&> : std::add_lvalue_reference<T>
		{};
		template <typename T, typename CopyFrom>
		struct copy_ref_<T, CopyFrom&&> : std::add_rvalue_reference<T>
		{};

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

	/// @brief  Copies the cv-qualifiers from one type onto another, replacing the existing ones.
	template <typename T, typename CopyFrom>
	using copy_cv = POXY_IMPLEMENTATION_DETAIL(typename detail::copy_cv_<std::remove_cv_t<T>, CopyFrom>::type);

	/// @brief  Copies the ref-qualifiers from one type onto another, replacing the existing ones.
	template <typename T, typename CopyFrom>
	using copy_ref = POXY_IMPLEMENTATION_DETAIL(typename detail::copy_ref_<std::remove_reference_t<T>, CopyFrom>::type);

	/// @brief  Copies the cvref-qualifiers from one type onto another, replacing the existing ones.
	template <typename T, typename CopyFrom>
	using copy_cvref =
		POXY_IMPLEMENTATION_DETAIL(copy_ref<copy_cv<remove_cvref<T>, std::remove_reference_t<CopyFrom>>, CopyFrom>);

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

		template <typename T, typename Pos, typename... Args>
		using has_emplace_member_impl_ =
			decltype(std::declval<T&>().emplace(std::declval<Pos>(), std::declval<Args>()...));
		template <typename T, typename Pos, typename... Args>
		using has_emplace_member_ = is_detected_<has_emplace_member_impl_, Pos, Args...>;

		template <typename T, typename... Args>
		using has_emplace_back_member_impl_ = decltype(std::declval<T&>().emplace_back(std::declval<Args>()...));
		template <typename T, typename... Args>
		using has_emplace_back_member_ = is_detected_<has_emplace_back_member_impl_, Args...>;

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

	/// @brief	True if `T` has an `emplace(Pos, Args...)` member.
	template <typename T, typename Pos, typename... Args>
	inline constexpr bool has_emplace_member =
		POXY_IMPLEMENTATION_DETAIL(detail::has_emplace_member_<T, Pos, Args...>::value);

	/// @brief	True if `T` has an `emplace_back(Args...)` member.
	template <typename T, typename... Args>
	inline constexpr bool has_emplace_back_member =
		POXY_IMPLEMENTATION_DETAIL(detail::has_emplace_back_member_<T, Args...>::value);

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

		template <bool /* has_emplace_member<T, Pos, Args> */, typename T, typename Pos, typename... Args>
		struct has_nothrow_emplace_member_
			: std::bool_constant<noexcept(std::declval<T&>().emplace(std::declval<Pos>(), std::declval<Args>()...))>
		{};
		template <typename T, typename Pos, typename... Args>
		struct has_nothrow_emplace_member_<false, T, Pos, Args...> : std::false_type
		{};

		template <bool /* has_emplace_back_member<T, Args> */, typename T, typename... Args>
		struct has_nothrow_emplace_back_member_
			: std::bool_constant<noexcept(std::declval<T&>().emplace_back(std::declval<Args>()...))>
		{};
		template <typename T, typename... Args>
		struct has_nothrow_emplace_back_member_<false, T, Args...> : std::false_type
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

	/// @brief	True if `T` has a non-throwing `emplace(Pos, Args...)` member.
	template <typename T, typename Pos, typename... Args>
	inline constexpr bool has_nothrow_emplace_member = POXY_IMPLEMENTATION_DETAIL(
		detail::has_nothrow_emplace_member_<has_emplace_member<T, Pos, Args...>, T, Pos, Args...>::value);

	/// @brief	True if `T` has a non-throwing `emplace_back(Args...)` member.
	template <typename T, typename... Args>
	inline constexpr bool has_nothrow_emplace_back_member = POXY_IMPLEMENTATION_DETAIL(
		detail::has_nothrow_emplace_back_member_<has_emplace_back_member<T, Args...>, T, Args...>::value);

	/// @brief	True if `T` and `U` meet the `EqualityComparable` named requirement without throwing.
	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_equality_comparable =
		POXY_IMPLEMENTATION_DETAIL(detail::is_nothrow_equality_comparable_<T, U>::value);

	/// @brief	True if `T` and `U` meet the `LessThanComparable` named requirement without throwing.
	template <typename T, typename U = T>
	inline constexpr bool is_nothrow_less_than_comparable =
		POXY_IMPLEMENTATION_DETAIL(detail::is_nothrow_less_than_comparable_<T, U>::value);

#if !SOAGEN_DOXYGEN && SOAGEN_HAS_BUILTIN(__type_pack_element)

	template <auto I, typename... T>
	using type_at_index = __type_pack_element<static_cast<size_t>(I), T...>;

#else

	/// @cond
	namespace detail
	{
		template <size_t, typename...>
		struct type_at_index_;

		template <size_t I, typename T, typename... U>
		struct type_at_index_<I, T, U...> : type_at_index_<I - 1, U...>
		{};

		template <typename T, typename... U>
		struct type_at_index_<0, T, U...>
		{
			using type = T;
		};
	}
	/// @endcond

	/// @brief Gets the type `T` at index `I` in the parameter pack.
	template <auto I, typename... T>
	using type_at_index =
		POXY_IMPLEMENTATION_DETAIL(typename detail::type_at_index_<static_cast<size_t>(I), T...>::type);

#endif

	/// @cond
	namespace detail
	{
		template <typename>
		struct is_table_ : std::false_type
		{};
		template <typename... Args>
		struct is_table_<table<Args...>> : std::true_type
		{};

		template <typename T>
		struct is_soa_ : is_table_<T> // specialized in the generated code
		{};

		template <typename>
		struct is_row_ : std::false_type
		{};
		template <typename Soa, size_t... Columns>
		struct is_row_<row<Soa, Columns...>> : std::true_type
		{};

		template <typename>
		struct is_span_ : std::false_type
		{};
		template <typename Soa>
		struct is_span_<span<Soa>> : std::true_type
		{};

		template <typename>
		struct is_iterator_ : std::false_type
		{};
		template <typename Soa, size_t... Columns>
		struct is_iterator_<iterator<Soa, Columns...>> : std::true_type
		{};
	}
	/// @endcond

	/// @brief True if `T` is a #soagen::table.
	template <typename T>
	inline constexpr bool is_table = POXY_IMPLEMENTATION_DETAIL(detail::is_table_<std::remove_cv_t<T>>::value);

	/// @brief	True if `T` is a #soagen::table or a soagen-generated SoA class.
	template <typename T>
	inline constexpr bool is_soa = POXY_IMPLEMENTATION_DETAIL(detail::is_soa_<std::remove_cv_t<T>>::value);

	/// @brief True if `T` is a #soagen::row.
	template <typename T>
	inline constexpr bool is_row = POXY_IMPLEMENTATION_DETAIL(detail::is_row_<std::remove_cv_t<T>>::value);

	/// @brief True if `T` is a #soagen::span.
	template <typename T>
	inline constexpr bool is_span = POXY_IMPLEMENTATION_DETAIL(detail::is_span_<std::remove_cv_t<T>>::value);

	/// @brief True if `T` is a #soagen::iterator.
	template <typename T>
	inline constexpr bool is_iterator = POXY_IMPLEMENTATION_DETAIL(detail::is_iterator_<std::remove_cv_t<T>>::value);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct soa_type_cvref_unwrap_;

		template <typename T, bool = is_soa<remove_cvref<T>>>
		struct soa_type_cvref_
		{
			using type = T;
		};
		template <typename T>
		struct soa_type_cvref_<T, false> //
			: soa_type_cvref_unwrap_<remove_cvref<T>>
		{};

		template <template <typename> typename T, typename Soa>
		struct soa_type_cvref_unwrap_<T<Soa>> : soa_type_cvref_<Soa>
		{};

		template <template <typename, size_t...> typename T, typename Soa, size_t... Columns>
		struct soa_type_cvref_unwrap_<T<Soa, Columns...>> : soa_type_cvref_<Soa>
		{};

		template <typename T>
		using soa_type_cvref = typename soa_type_cvref_<T>::type;
	}
	/// @endcond

	/// @brief  Gets the root SoA type from a type (e.g. `T` in `soagen::row<T, Cols...>`),
	///         without any cvref-qualifiers.
	template <typename T>
	using soa_type = POXY_IMPLEMENTATION_DETAIL(remove_cvref<detail::soa_type_cvref<remove_cvref<T>>>);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct table_type_
		{
			using type = typename T::table_type;
		};
		template <typename... Args>
		struct table_type_<table<Args...>>
		{
			using type = table<Args...>;
		};
	}
	/// @endcond

	/// @brief Gets the underlying #soagen::table of a type.
	/// @tparam T		A table, row, span, iterator, or some soagen-generated SoA class.
	template <typename T>
	using table_type = POXY_IMPLEMENTATION_DETAIL(typename detail::table_type_<soa_type<T>>::type);

	/// @brief True if two types have the same underlying #soagen::table type.
	template <typename A, typename B>
	inline constexpr bool same_table_type = POXY_IMPLEMENTATION_DETAIL(std::is_same_v<table_type<A>, table_type<B>>);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct table_traits_type_
		{
			using type = typename table_type<T>::table_traits;
		};
		template <typename... Args>
		struct table_traits_type_<table_traits<Args...>>
		{
			using type = table_traits<Args...>;
		};
		template <typename Traits, typename Allocator>
		struct table_traits_type_<table<Traits, Allocator>>
		{
			using type = Traits;
		};
	}
	/// @endcond

	/// @brief Gets the #soagen::table_traits for the underlying #soagen::table of a type.
	/// @tparam T		A table, table_traits, row, span, iterator, or some soagen-generated SoA class.
	template <typename T>
	using table_traits_type = POXY_IMPLEMENTATION_DETAIL(typename detail::table_traits_type_<remove_cvref<T>>::type);

	/// @brief Gets the #soagen::column_traits::value_type for the selected column of a type.
	template <typename T, auto Column>
	using value_type = POXY_IMPLEMENTATION_DETAIL(
		typename table_traits_type<T>::template column<static_cast<size_t>(Column)>::value_type);

	/// @brief Cvref-qualified version of #soagen::value_type (i.e. having cvref-qualifiers derived from `T`).
	template <typename T, auto Column>
	using value_ref =
		POXY_IMPLEMENTATION_DETAIL(copy_ref<					//
								   conditionally_add_volatile<	//
									   conditionally_add_const< //
										   value_type<T, Column>,
										   std::is_const_v<std::remove_reference_t<detail::soa_type_cvref<T>>>>,
									   std::is_volatile_v<std::remove_reference_t<detail::soa_type_cvref<T>>>>,
								   coerce_ref<detail::soa_type_cvref<T>>>);

	/// @cond
	namespace detail
	{
		template <typename T>
		struct allocator_type_
		{
			using type = typename table_type<T>::allocator_type;
		};
		template <>
		struct allocator_type_<allocator>
		{
			using type = allocator;
		};
		template <typename Traits, typename Allocator>
		struct allocator_type_<table<Traits, Allocator>>
		{
			using type = Allocator;
		};
	}
	/// @endcond

	/// @brief Gets the allocator being used by the #soagen::table of an SoA type.
	/// @tparam T		A table, allocator, row, span, iterator, or some soagen-generated SoA class.
	template <typename T>
	using allocator_type = POXY_IMPLEMENTATION_DETAIL(typename detail::allocator_type_<remove_cvref<T>>::type);

	/// @cond
	namespace detail
	{
		template <typename, typename>
		struct column_indices_;

		// use the override if it is specified
		template <typename T, size_t... Columns>
		struct column_indices_<std::index_sequence<Columns...>, T>
		{
			static_assert(sizeof...(Columns));

			using type = std::index_sequence<Columns...>;
		};

		// otherwise use the column_count for tables, spans and generated types
		template <typename T>
		struct column_indices_<std::index_sequence<>, T>
		{
			using type = std::make_index_sequence<table_traits_type<T>::column_count>;

			static_assert(!std::is_same_v<type, std::index_sequence<>>);
		};

		// and use the viewed columns for rows and iterators
		template <size_t... Indices>
		struct nested_index_sequence
		{
			using type = std::index_sequence<Indices...>;
		};
		template <typename Soa, size_t... Columns>
		struct column_indices_<std::index_sequence<>, row<Soa, Columns...>> : nested_index_sequence<Columns...>
		{};
		template <typename Soa, size_t... Columns>
		struct column_indices_<std::index_sequence<>, iterator<Soa, Columns...>> : nested_index_sequence<Columns...>
		{};
	}
	/// @endcond

	/// @brief Gets the std::index_sequence type corresponding to the columns of a type, optionally overriding them.
	/// @tparam T		A table, table_traits, row, span, iterator, or some soagen-generated SoA class.
	template <typename T, auto... Columns>
	using column_indices = POXY_IMPLEMENTATION_DETAIL(
		typename detail::column_indices_<std::index_sequence<static_cast<size_t>(Columns)...>, remove_cvref<T>>::type);

	/// @cond
	namespace detail
	{
		template <typename>
		struct add_const_to_first_type_arg_;
		template <template <typename> typename Type, typename T>
		struct add_const_to_first_type_arg_<Type<T>>
		{
			using type = Type<copy_ref<std::add_const_t<std::remove_reference_t<T>>, T>>;
		};
		template <template <typename, typename...> typename Type, typename T, typename... Args>
		struct add_const_to_first_type_arg_<Type<T, Args...>>
		{
			using type = Type<copy_ref<std::add_const_t<std::remove_reference_t<T>>, T>, Args...>;
		};
		template <template <typename, size_t...> typename Type, typename T, size_t... Columns>
		struct add_const_to_first_type_arg_<Type<T, Columns...>>
		{
			using type = Type<copy_ref<std::add_const_t<std::remove_reference_t<T>>, T>, Columns...>;
		};
		template <typename T>
		using add_const_to_first_type_arg = typename add_const_to_first_type_arg_<T>::type;
	}
	/// @endcond

	/// @brief			The #soagen::span for a given type.
	/// @tparam T		A table, row, span, iterator, or some soagen-generated SoA class.
	template <typename T>
	using span_type = POXY_IMPLEMENTATION_DETAIL(span<remove_lvalue_ref<detail::soa_type_cvref<remove_lvalue_ref<T>>>>);

	/// @brief			The same as #soagen::span_type but promoting the base SoA type to `const` (if it was not already).
	template <typename T>
	using const_span_type = POXY_IMPLEMENTATION_DETAIL(detail::add_const_to_first_type_arg<span_type<T>>);

	/// @cond
	namespace detail
	{
		template <template <typename, size_t...> typename, typename...>
		struct derive_view_type_;

		template <template <typename, size_t...> typename Type, typename T, size_t... Columns>
		struct derive_view_type_<Type, T, std::index_sequence<Columns...>, std::true_type>
		{
			using type = Type<remove_lvalue_ref<soa_type_cvref<remove_lvalue_ref<T>>>, Columns...>;
		};

		template <template <typename, size_t...> typename Type, typename T, size_t... Columns>
		struct derive_view_type_<Type, T, std::index_sequence<Columns...>>
			: derive_view_type_<Type, T, column_indices<remove_cvref<T>, Columns...>, std::true_type>
		{};
		template <template <typename, size_t...> typename Type, typename... Args>
		using derive_view_type = typename derive_view_type_<Type, Args...>::type;
	}
	/// @endcond

	/// @brief			The #soagen::row for a given type and (some subset of) its columns.
	/// @tparam T		A table, row, span, iterator, or some soagen-generated SoA class.
	/// @tparam Columns	The columns viewed by the row. Leave empty to copy the columns from the source.
	template <typename T, auto... Columns>
	using row_type = POXY_IMPLEMENTATION_DETAIL(
		detail::derive_view_type<row, T, std::index_sequence<static_cast<size_t>(Columns)...>>);

	/// @brief			The same as #soagen::row_type but promoting the base SoA type to `const` (if it was not already).
	template <typename T, auto... Columns>
	using const_row_type =
		POXY_IMPLEMENTATION_DETAIL(detail::add_const_to_first_type_arg<row_type<T, static_cast<size_t>(Columns)...>>);

	/// @brief			The #soagen::iterator for a given type and (some subset of) its columns.
	/// @tparam T		A table, row, span, iterator, or some soagen-generated SoA class.
	/// @tparam Columns	The columns viewed by the iterator. Leave empty to copy the columns from the source.
	template <typename T, auto... Columns>
	using iterator_type = POXY_IMPLEMENTATION_DETAIL(
		detail::derive_view_type<iterator, T, std::index_sequence<static_cast<size_t>(Columns)...>>);

	/// @brief			The same as #soagen::iterator_type but promoting the base SoA type to `const` (if it was not already).
	template <typename T, auto... Columns>
	using const_iterator_type = POXY_IMPLEMENTATION_DETAIL(
		detail::add_const_to_first_type_arg<iterator_type<T, static_cast<size_t>(Columns)...>>);

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
		inline static constexpr size_t min_actual_column_alignment =
			max(size_t{ __STDCPP_DEFAULT_NEW_ALIGNMENT__ }, alignof(std::max_align_t), size_t{ 16 });

		template <typename T>
		struct columns_always_aligned_ : std::bool_constant<is_table<T> || is_soa<T>>
		{};
	}
	/// @endcond

	/// @brief Determines the _actual_ minimum alignment for a table column.
	///
	/// @details This value is based on a number of factors: <ul>
	/// <li> The `alignof` for the column's `value_type`,
	/// <li> The value specified for `column_traits::alignment`,
	/// <li> The value specified for `allocator::min_alignment` (if any),
	/// <li> Internal allocation semantics,
	/// <li> Whether T is a #soagen::table, a soagen-generated SoA type, or some view type (e.g. spans).
	/// </ul>
	///
	/// @note This has absolutely nothing to do with `aligned_stride`; that is still calculated
	/// according to the user's specified alignment requirements. This trait is _only_ used to
	/// help the compiler via `assume_aligned`.
	template <typename T, auto Column>
	inline constexpr size_t actual_alignment =
		max(alignof(value_type<T, Column>),
			detail::columns_always_aligned_<remove_cvref<T>>::value
				? max(table_traits_type<T>::template column<Column>::alignment, detail::min_actual_column_alignment)
				: 0u);

	template <typename T>
	inline constexpr size_t buffer_alignment =
		max(allocator_traits<allocator_type<T>>::min_alignment, table_traits_type<T>::largest_alignment);

	/// @cond
	namespace detail
	{
		// machinery to only instantiate one or more secondary traits when the primary one is true
		// e.g. for traits that come in pairs like is_invocable / is_nothrow_invocable

		template <typename...>
		struct types;

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
		struct nested_trait_<types<Args...>, Trait, NestedTraits...> : Trait<Args...>
		{
			template <size_t Index>
			using nested =
				type_at_index<Index, nested_trait_indirect_<Trait<Args...>::value, NestedTraits, Args...>...>;
		};
	}
	/// @endcond
}

#include "header_end.hpp"
