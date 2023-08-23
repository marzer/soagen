// This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "settings.hpp"
#include <soagen.hpp>
SOAGEN_DISABLE_WARNINGS;
#include <string>
#include <string_view>
#include "lib_catch2.hpp"
SOAGEN_ENABLE_WARNINGS;

#ifndef NDEBUG
	#undef SOAGEN_ASSUME
	#define SOAGEN_ASSUME(cond) SOAGEN_CONSTEXPR_SAFE_ASSERT(cond)
#endif

namespace tests
{
	SOAGEN_DISABLE_WARNINGS;
	using namespace std::string_view_literals;
	using namespace std::string_literals;
	SOAGEN_ENABLE_WARNINGS;

	using std::size_t;
	using std::ptrdiff_t;
	using std::uintptr_t;

	using soagen::emplacer;

	template <typename T, size_t... Cols>
	using row = soagen::row_type<T, Cols...>;

	template <typename T, size_t... Cols>
	using const_row = soagen::const_row_type<T, Cols...>;

	template <typename From, typename To>
	inline constexpr bool is_implicitly_convertible = std::is_convertible_v<From, To>;

	template <typename From, typename To>
	inline constexpr bool is_explicitly_convertible =
		!std::is_convertible_v<From, To> && std::is_constructible_v<To, From>;
}

SOAGEN_DISABLE_SPAM_WARNINGS;
