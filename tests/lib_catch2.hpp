// This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "settings.hpp"

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Weverything"
#elif defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wall"
	#pragma GCC diagnostic ignored "-Wextra"
	#pragma GCC diagnostic ignored "-Wpadded"
	#pragma GCC diagnostic ignored "-Wfloat-equal"
#elif defined(_MSC_VER)
	#pragma warning(push, 0)
	#pragma warning(disable : 4619)
	#pragma warning(disable : 4365)
	#pragma warning(disable : 4868)
	#pragma warning(disable : 5105)
	#pragma warning(disable : 5262)
	#pragma warning(disable : 5264)
#endif

#if __has_include(<catch2/catch.hpp>)
	#include <catch2/catch.hpp>
#elif __has_include(<catch2/catch_test_macros.hpp>)
	#include <catch2/catch_test_macros.hpp>
#else
	#error Catch2 is missing!
#endif

#ifdef __clang__
	#pragma clang diagnostic pop
#elif defined(__GNUC__)
	#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
	#pragma warning(pop)
#endif
