//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "../span.hpp"
#include "../header_start.hpp"

namespace soagen::mixins
{
	template <typename Derived>
	struct SOAGEN_EMPTY_BASES spans
	{
		static_assert(!is_cvref<Derived>);

		using size_type		   = std::size_t;
		using span_type		   = soagen::span_type<Derived>;
		using rvalue_span_type = soagen::span_type<Derived&&>;
		using const_span_type  = soagen::const_span_type<Derived>;

		SOAGEN_PURE_INLINE_GETTER
		span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) & noexcept
		{
			return { static_cast<Derived&>(*this), start, count };
		}

		SOAGEN_PURE_INLINE_GETTER
		rvalue_span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) && noexcept
		{
			return { static_cast<Derived&&>(*this), start, count };
		}

		SOAGEN_PURE_INLINE_GETTER
		const_span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) const& noexcept
		{
			return { static_cast<const Derived&>(*this), start, count };
		}

		SOAGEN_PURE_INLINE_GETTER
		const_span_type const_subspan(size_type start, size_type count = static_cast<size_type>(-1)) const noexcept
		{
			return subspan(start, count);
		}
	};
}

#include "../header_end.hpp"
