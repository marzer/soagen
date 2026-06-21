//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "core.hpp"
#include "header_start.hpp"

/// @cond
// these are all just uninteresting single-type trait helpers, not SoA type traits that are a
// part of the public API, so they're hidden from doxygen.

namespace soagen::detail
{
    using std::conjunction;
    using std::disjunction;
    using std::negation;

    template <typename T>
    using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

    template <typename T>
    using coerce_ref = std::conditional_t<std::is_reference_v<T>, T, std::add_lvalue_reference_t<T>>;

    template <typename T>
    using is_cv = negation<std::is_same<std::remove_cv_t<T>, T>>;

    template <typename T>
    using is_cvref = negation<std::is_same<remove_cvref<T>, T>>;

    template <typename T>
    using is_integer = conjunction<std::is_integral<T>, negation<std::is_same<T, bool>>>;

    template <typename... T>
    struct all_integer : conjunction<is_integer<T>...>
    {};
    template <>
    struct all_integer<> : std::false_type
    {};

    template <typename T>
    using is_unsigned = conjunction<is_integer<T>, std::is_unsigned<T>>;

    template <typename T, typename... U>
    struct any_same : disjunction<std::is_same<T, U>...>
    {};
    template <typename T>
    struct any_same<T> : std::false_type
    {};

    template <auto Value, auto... Values>
    inline constexpr bool any_same_value = ((Value == Values) || ...);

    template <typename T, bool Cond>
    using conditionally_add_const = std::conditional_t<Cond, std::add_const_t<T>, T>;

    template <typename T, bool Cond>
    using conditionally_add_volatile = std::conditional_t<Cond, std::add_volatile_t<T>, T>;

    template <typename T, bool Cond>
    using conditionally_remove_cvref = std::conditional_t<Cond, remove_cvref<T>, T>;

    template <typename T>
    using is_implicit_lifetime_type = disjunction<std::is_scalar<T>,
                                                  std::is_array<T>,
                                                  conjunction<std::is_aggregate<T>, //
                                                              std::is_trivially_constructible<T>,
                                                              std::is_trivially_destructible<T>>>;

    // ---- copy cv

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
    template <typename T, typename CopyFrom>
    using copy_cv = typename copy_cv_<std::remove_cv_t<T>, CopyFrom>::type;

    // ---- copy ref

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
    template <typename T, typename CopyFrom>
    using copy_ref = typename copy_ref_<std::remove_reference_t<T>, CopyFrom>::type;

    // ---- copy cvref

    template <typename T, typename CopyFrom>
    using copy_cvref = copy_ref<copy_cv<remove_cvref<T>, std::remove_reference_t<CopyFrom>>, CopyFrom>;

    // ---- remove lvalue ref

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
    template <typename T>
    using remove_lvalue_ref = typename remove_lvalue_ref_<T>::type;

    // ---- forward_type

    template <typename T>
    using forward_type = std::conditional_t<std::is_lvalue_reference_v<T>, T, std::add_rvalue_reference_t<T>>;

    // ---- is_detected

    template <template <typename...> typename Trait, typename Enabler, typename... Args>
    struct is_detected_impl_ : std::false_type
    {};
    template <template <typename...> typename Trait, typename... Args>
    struct is_detected_impl_<Trait, std::void_t<Trait<Args...>>, Args...> : std::true_type
    {};
    template <template <typename...> typename Trait, typename... Args>
    using is_detected = is_detected_impl_<Trait, void, Args...>;

    // ---- has swap()

    template <typename T>
    using has_swap_member_ = decltype(std::declval<T&>().swap(std::declval<T&>()));
    template <typename T>
    using has_swap_member = is_detected<has_swap_member_, T>;

    template <typename T>
    struct has_nothrow_swap_member_ : std::bool_constant<noexcept(std::declval<T&>().swap(std::declval<T&>()))>
    {};
    template <typename T>
    using has_nothrow_swap_member = conjunction<has_swap_member<T>, has_nothrow_swap_member_<T>>;

    // ---- has resize()

    template <typename T, typename Arg>
    using has_resize_member_ = decltype(std::declval<T&>().resize(std::declval<const Arg&>()));
    template <typename T, typename Arg = size_t>
    using has_resize_member = is_detected<has_resize_member_, T, Arg>;

    template <typename T, typename Arg>
    struct has_nothrow_resize_member_
        : std::bool_constant<noexcept(std::declval<T&>().resize(std::declval<const Arg&>()))>
    {};
    template <typename T, typename Arg = size_t>
    using has_nothrow_resize_member = conjunction<has_resize_member<T, Arg>, has_nothrow_resize_member_<T, Arg>>;

    // ---- has erase()

    template <typename T, typename Arg>
    using has_erase_member_ = decltype(std::declval<T&>().erase(std::declval<const Arg&>()));
    template <typename T, typename Arg = size_t>
    using has_erase_member = is_detected<has_erase_member_, T, Arg>;

    template <typename T, typename Arg>
    struct has_nothrow_erase_member_
        : std::bool_constant<noexcept(std::declval<T&>().erase(std::declval<const Arg&>()))>
    {};
    template <typename T, typename Arg = size_t>
    using has_nothrow_erase_member = conjunction<has_erase_member<T, Arg>, has_nothrow_erase_member_<T, Arg>>;

    // ---- has unordered_erase()

    template <typename T, typename Arg>
    using has_unordered_erase_member_ = decltype(std::declval<T&>().unordered_erase(std::declval<const Arg&>()));
    template <typename T, typename Arg = size_t>
    using has_unordered_erase_member = is_detected<has_unordered_erase_member_, T, Arg>;

    template <typename T, typename Arg>
    struct has_nothrow_unordered_erase_member_
        : std::bool_constant<noexcept(std::declval<T&>().unordered_erase(std::declval<const Arg&>()))>
    {};
    template <typename T, typename Arg = size_t>
    using has_nothrow_unordered_erase_member =
        conjunction<has_unordered_erase_member<T, Arg>, has_nothrow_unordered_erase_member_<T, Arg>>;

    // ---- has data()

    template <typename T>
    using has_data_member_ = decltype(std::declval<T&>().data());
    template <typename T>
    using has_data_member = is_detected<has_data_member_, T>;

    template <typename T>
    struct has_nothrow_data_member_ : std::bool_constant<noexcept(std::declval<T&>().data())>
    {};
    template <typename T>
    using has_nothrow_data_member = conjunction<has_data_member<T>, has_nothrow_data_member_<T>>;

    // ---- has emplace()

    template <typename T, typename Pos, typename... Args>
    using has_emplace_member_ = decltype(std::declval<T&>().emplace(std::declval<Pos>(), std::declval<Args>()...));
    template <typename T, typename Pos, typename... Args>
    using has_emplace_member = is_detected<has_emplace_member_, T, Pos, Args...>;

    template <typename T, typename Pos, typename... Args>
    struct has_nothrow_emplace_member_
        : std::bool_constant<noexcept(std::declval<T&>().emplace(std::declval<Pos>(), std::declval<Args>()...))>
    {};
    template <typename T, typename Pos, typename... Args>
    using has_nothrow_emplace_member =
        conjunction<has_emplace_member<T, Pos, Args...>, has_nothrow_emplace_member_<T, Pos, Args...>>;

    // ---- has emplace_back()

    template <typename T, typename... Args>
    using has_emplace_back_member_ = decltype(std::declval<T&>().emplace_back(std::declval<Args>()...));
    template <typename T, typename... Args>
    using has_emplace_back_member = is_detected<has_emplace_back_member_, T, Args...>;

    template <typename T, typename... Args>
    struct has_nothrow_emplace_back_member_
        : std::bool_constant<noexcept(std::declval<T&>().emplace_back(std::declval<Args>()...))>
    {};
    template <typename T, typename... Args>
    using has_nothrow_emplace_back_member =
        conjunction<has_emplace_back_member<T, Args...>, has_nothrow_emplace_back_member_<T, Args...>>;

    // ---- has operator ==

    template <typename T, typename U>
    using is_equality_comparable_ = decltype(std::declval<const std::remove_reference_t<T>&>()
                                             == std::declval<const std::remove_reference_t<U>&>());
    template <typename T, typename U = T>
    using is_equality_comparable = is_detected<is_equality_comparable_, T, U>;

    template <typename T, typename U>
    struct is_nothrow_equality_comparable_
        : std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
                                      == std::declval<const std::remove_reference_t<U>&>())>
    {};
    template <typename T, typename U = T>
    using is_nothrow_equality_comparable =
        conjunction<is_equality_comparable<T, U>, is_nothrow_equality_comparable_<T, U>>;

    // ---- has operator <

    template <typename T, typename U>
    using is_less_than_comparable_ =
        decltype(std::declval<const std::remove_reference_t<T>&>() < std::declval<const std::remove_reference_t<U>&>());
    template <typename T, typename U = T>
    using is_less_than_comparable = is_detected<is_less_than_comparable_, T, U>;

    template <typename T, typename U>
    struct is_nothrow_less_than_comparable_
        : std::bool_constant<noexcept(std::declval<const std::remove_reference_t<T>&>()
                                      < std::declval<const std::remove_reference_t<U>&>())>
    {};
    template <typename T, typename U = T>
    using is_nothrow_less_than_comparable =
        conjunction<is_less_than_comparable<T, U>, is_nothrow_less_than_comparable_<T, U>>;

    // ---- type_at_index

#if SOAGEN_HAS_BUILTIN(__type_pack_element)

    template <auto I, typename... T>
    using type_at_index = __type_pack_element<static_cast<size_t>(I), T...>;

#else

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
    template <auto I, typename... T>
    using type_at_index = typename type_at_index_<static_cast<size_t>(I), T...>::type;

#endif

}

/// @endcond

#include "header_end.hpp"
