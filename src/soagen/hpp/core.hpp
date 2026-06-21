//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.hpp"
#include "single_type_traits.hpp"
#include "functions.hpp"
SOAGEN_DISABLE_WARNINGS;
#ifndef SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT
    #define SOAGEN_COLUMN_SPAN_SUPPORTS_ALIGNMENT 0
#endif
#ifndef SOAGEN_OPTIONAL_TYPE
    #include <optional>
    #define SOAGEN_OPTIONAL_TYPE std::optional
#endif
SOAGEN_ENABLE_WARNINGS;
#include "header_start.hpp"

namespace soagen
{
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

        template <typename T, bool = is_soa<detail::remove_cvref<T>>>
        struct soa_type_cvref_
        {
            using type = T;
        };
        template <typename T>
        struct soa_type_cvref_<T, false> //
            : soa_type_cvref_unwrap_<detail::remove_cvref<T>>
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
    using soa_type = POXY_IMPLEMENTATION_DETAIL(detail::remove_cvref<detail::soa_type_cvref<detail::remove_cvref<T>>>);

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
    using table_traits_type =
        POXY_IMPLEMENTATION_DETAIL(typename detail::table_traits_type_<detail::remove_cvref<T>>::type);

    /// @brief Gets the #soagen::column_traits::value_type for the selected column of a type.
    template <typename T, auto Column>
    using value_type = POXY_IMPLEMENTATION_DETAIL(
        typename table_traits_type<T>::template column<static_cast<size_t>(Column)>::value_type);

    /// @brief Cvref-qualified version of #soagen::value_type (i.e. having cvref-qualifiers derived from `T`).
    template <typename T, auto Column>
    using value_ref =
        POXY_IMPLEMENTATION_DETAIL(detail::copy_ref<                    //
                                   detail::conditionally_add_volatile<  //
                                       detail::conditionally_add_const< //
                                           value_type<T, Column>,
                                           std::is_const_v<std::remove_reference_t<detail::soa_type_cvref<T>>>>,
                                       std::is_volatile_v<std::remove_reference_t<detail::soa_type_cvref<T>>>>,
                                   detail::coerce_ref<detail::soa_type_cvref<T>>>);

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
    using allocator_type = POXY_IMPLEMENTATION_DETAIL(typename detail::allocator_type_<detail::remove_cvref<T>>::type);

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
        typename detail::column_indices_<std::index_sequence<static_cast<size_t>(Columns)...>,
                                         detail::remove_cvref<T>>::type);

    /// @cond
    namespace detail
    {
        template <typename>
        struct add_const_to_first_type_arg_;
        template <typename Soa>
        struct add_const_to_first_type_arg_<span<Soa>>
        {
            using type = span<copy_ref<std::add_const_t<std::remove_reference_t<Soa>>, Soa>>;
        };
        template <typename Soa, size_t... Columns>
        struct add_const_to_first_type_arg_<row<Soa, Columns...>>
        {
            using type = row<copy_ref<std::add_const_t<std::remove_reference_t<Soa>>, Soa>, Columns...>;
        };
        template <typename Soa, size_t... Columns>
        struct add_const_to_first_type_arg_<iterator<Soa, Columns...>>
        {
            using type = iterator<copy_ref<std::add_const_t<std::remove_reference_t<Soa>>, Soa>, Columns...>;
        };
        template <typename T>
        using add_const_to_first_type_arg = typename add_const_to_first_type_arg_<T>::type;

        template <typename>
        struct add_rvalue_to_first_type_arg_;
        template <typename Soa>
        struct add_rvalue_to_first_type_arg_<span<Soa>>
        {
            using type = span<std::add_rvalue_reference_t<std::remove_reference_t<Soa>>>;
        };
        template <typename Soa, size_t... Columns>
        struct add_rvalue_to_first_type_arg_<row<Soa, Columns...>>
        {
            using type = row<std::add_rvalue_reference_t<std::remove_reference_t<Soa>>, Columns...>;
        };
        template <typename Soa, size_t... Columns>
        struct add_rvalue_to_first_type_arg_<iterator<Soa, Columns...>>
        {
            using type = iterator<std::add_rvalue_reference_t<std::remove_reference_t<Soa>>, Columns...>;
        };
        template <typename T>
        using add_rvalue_to_first_type_arg = typename add_rvalue_to_first_type_arg_<T>::type;
    }
    /// @endcond

    /// @brief			The #soagen::span for a given type.
    /// @tparam T		A table, row, span, iterator, or some soagen-generated SoA class.
    template <typename T>
    using span_type = POXY_IMPLEMENTATION_DETAIL(
        span<detail::remove_lvalue_ref<detail::soa_type_cvref<detail::remove_lvalue_ref<T>>>>);

    /// @brief			The same as #soagen::span_type but promoting the base SoA type to `&&` (if it was not already).
    template <typename T>
    using rvalue_span_type = POXY_IMPLEMENTATION_DETAIL(detail::add_rvalue_to_first_type_arg<span_type<T>>);

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
            : derive_view_type_<Type, T, column_indices<detail::remove_cvref<T>, Columns...>, std::true_type>
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

    /// @brief			The same as #soagen::row_type but promoting the base SoA type to `&&` (if it was not already).
    template <typename T, auto... Columns>
    using rvalue_row_type =
        POXY_IMPLEMENTATION_DETAIL(detail::add_rvalue_to_first_type_arg<row_type<T, static_cast<size_t>(Columns)...>>);

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

    /// @brief			The same as #soagen::iterator_type but promoting the base SoA type to `&&` (if it was not already).
    template <typename T, auto... Columns>
    using rvalue_iterator_type = POXY_IMPLEMENTATION_DETAIL(
        detail::add_rvalue_to_first_type_arg<iterator_type<T, static_cast<size_t>(Columns)...>>);

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
                std::is_rvalue_reference_v<param_type<detail::remove_cvref<T>>>
                    || !std::is_reference_v<param_type<detail::remove_cvref<T>>>,
                param_type<detail::remove_cvref<T>>,
                std::conditional_t<
                    // copy-only things
                    !std::is_move_constructible_v<detail::remove_cvref<T>>,
                    std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>,
                    // rvalues
                    std::add_rvalue_reference_t<detail::remove_cvref<T>>>>;
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
            detail::columns_always_aligned_<detail::remove_cvref<T>>::value
                ? max(table_traits_type<T>::template column<Column>::alignment, detail::min_actual_column_alignment)
                : 0u);

    template <typename T>
    inline constexpr size_t buffer_alignment =
        max(allocator_traits<allocator_type<T>>::min_alignment, table_traits_type<T>::largest_alignment);

    /// @cond
    namespace detail
    {
        template <typename A, typename B, typename = void>
        struct comparable_soa_ : std::false_type
        {};
        template <typename A, typename B>
        struct comparable_soa_<
            A,
            B,
            std::enable_if_t<(is_soa<A> || is_table<A> || is_span<A>) && (is_soa<B> || is_table<B> || is_span<B>)>>
            : std::bool_constant<same_table_type<A, B>>
        {};
    }
    /// @endcond

    /// @brief True if `A` and `B` are both table-shaped (a table, span or generated SoA) over the same table type.
    template <typename A, typename B>
    inline constexpr bool comparable_soa =
        POXY_IMPLEMENTATION_DETAIL(detail::comparable_soa_<detail::remove_cvref<A>, detail::remove_cvref<B>>::value);

    /// @cond
    namespace detail
    {
        template <typename Lhs, typename Rhs, size_t... Columns>
        SOAGEN_NODISCARD
        constexpr bool soa_views_equal(const Lhs& lhs, const Rhs& rhs, std::index_sequence<Columns...>) //
            noexcept(table_traits_type<Lhs>::all_nothrow_equality_comparable)
        {
            using traits = table_traits_type<Lhs>;
            if (lhs.size() != rhs.size())
                return false;
            if (!lhs.size())
                return true;
            const typename traits::const_column_pointers lhs_cols = { const_cast<std::byte*>(
                reinterpret_cast<const std::byte*>(lhs.template column<Columns>()))... };
            const typename traits::const_column_pointers rhs_cols = { const_cast<std::byte*>(
                reinterpret_cast<const std::byte*>(rhs.template column<Columns>()))... };
            return traits::equal(lhs_cols, 0, rhs_cols, 0, lhs.size());
        }

        template <typename Lhs, typename Rhs, size_t... Columns>
        SOAGEN_NODISCARD
        constexpr int soa_views_compare(const Lhs& lhs, const Rhs& rhs, std::index_sequence<Columns...>) //
            noexcept(table_traits_type<Lhs>::all_nothrow_less_than_comparable)
        {
            using traits     = table_traits_type<Lhs>;
            const size_t lsz = lhs.size();
            const size_t rsz = rhs.size();
            if (!lsz)
                return rsz ? -1 : 0;
            if (!rsz)
                return 1;
            const typename traits::const_column_pointers lhs_cols = { const_cast<std::byte*>(
                reinterpret_cast<const std::byte*>(lhs.template column<Columns>()))... };
            const typename traits::const_column_pointers rhs_cols = { const_cast<std::byte*>(
                reinterpret_cast<const std::byte*>(rhs.template column<Columns>()))... };
            const int comp = traits::compare(lhs_cols, 0, rhs_cols, 0, lsz < rsz ? lsz : rsz);
            return comp != 0 ? comp : (lsz == rsz ? 0 : (lsz < rsz ? -1 : 1));
        }
    }
    /// @endcond

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
                  template <typename...> typename... NestedTraits,
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
