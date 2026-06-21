//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
    template <typename Derived, bool = detail::has_swap_member<table_type<Derived>>::value>
    struct SOAGEN_EMPTY_BASES swappable
    {
        static_assert(!detail::is_cvref<Derived>::value);

        using table_type = soagen::table_type<Derived>;

        SOAGEN_ALWAYS_INLINE
        SOAGEN_CONSTEXPR_20
        void swap(Derived& other) //
            noexcept(detail::has_nothrow_swap_member<table_type>::value)
        {
            return static_cast<table_type&>(static_cast<Derived&>(*this)).swap(static_cast<table_type&>(other));
        }
    };

    template <typename Derived>
    struct SOAGEN_EMPTY_BASES swappable<Derived, false>
    {};
}

#include "../header_end.hpp"
