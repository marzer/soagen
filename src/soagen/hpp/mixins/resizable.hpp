//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
    template <typename Derived, bool = detail::has_resize_member<table_type<Derived>>::value>
    struct SOAGEN_EMPTY_BASES resizable
    {
        static_assert(!detail::is_cvref<Derived>::value);

        using table_type = soagen::table_type<Derived>;
        using size_type  = std::size_t;

        SOAGEN_ALWAYS_INLINE
        SOAGEN_CONSTEXPR_20
        Derived& resize(size_type new_size) //
            noexcept(detail::has_nothrow_resize_member<table_type>::value)
        {
            static_cast<table_type&>(static_cast<Derived&>(*this)).resize(new_size);
            return static_cast<Derived&>(*this);
        }

        SOAGEN_ALWAYS_INLINE
        SOAGEN_CONSTEXPR_20
        Derived& resize_for_overwrite(size_type new_size) //
            noexcept(noexcept(std::declval<table_type&>().resize_for_overwrite(std::declval<size_type>())))
        {
            static_cast<table_type&>(static_cast<Derived&>(*this)).resize_for_overwrite(new_size);
            return static_cast<Derived&>(*this);
        }
    };

    template <typename Derived>
    struct SOAGEN_EMPTY_BASES resizable<Derived, false>
    {};

}

#include "../header_end.hpp"
