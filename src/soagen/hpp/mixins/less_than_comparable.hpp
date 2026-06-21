//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../core.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
    template <typename Derived, bool = detail::is_less_than_comparable<const table_type<Derived>>::value>
    struct SOAGEN_EMPTY_BASES less_than_comparable
    {
        static_assert(!detail::is_cvref<Derived>::value);

        SOAGEN_CONSTRAINED_TEMPLATE((comparable_soa<Derived, Rhs>), typename Rhs)
        SOAGEN_NODISCARD
        friend constexpr bool operator<(const Derived& lhs, const Rhs& rhs) //
            noexcept(detail::is_nothrow_less_than_comparable<const table_type<Derived>>::value)
        {
            return detail::soa_views_compare(lhs, rhs, column_indices<Derived>{}) < 0;
        }

        SOAGEN_CONSTRAINED_TEMPLATE((comparable_soa<Derived, Rhs>), typename Rhs)
        SOAGEN_NODISCARD
        friend constexpr bool operator<=(const Derived& lhs, const Rhs& rhs) //
            noexcept(detail::is_nothrow_less_than_comparable<const table_type<Derived>>::value)
        {
            return detail::soa_views_compare(lhs, rhs, column_indices<Derived>{}) <= 0;
        }

        SOAGEN_CONSTRAINED_TEMPLATE((comparable_soa<Derived, Rhs>), typename Rhs)
        SOAGEN_NODISCARD
        friend constexpr bool operator>(const Derived& lhs, const Rhs& rhs) //
            noexcept(detail::is_nothrow_less_than_comparable<const table_type<Derived>>::value)
        {
            return detail::soa_views_compare(lhs, rhs, column_indices<Derived>{}) > 0;
        }

        SOAGEN_CONSTRAINED_TEMPLATE((comparable_soa<Derived, Rhs>), typename Rhs)
        SOAGEN_NODISCARD
        friend constexpr bool operator>=(const Derived& lhs, const Rhs& rhs) //
            noexcept(detail::is_nothrow_less_than_comparable<const table_type<Derived>>::value)
        {
            return detail::soa_views_compare(lhs, rhs, column_indices<Derived>{}) >= 0;
        }
    };

    template <typename Derived>
    struct SOAGEN_EMPTY_BASES less_than_comparable<Derived, false>
    {};
}

#include "../header_end.hpp"
