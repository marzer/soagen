// This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	namespace foo
	{
		// the order here matches the resolution order in get_source_offset

		struct by_static_func
		{
			SOAGEN_PURE_INLINE_GETTER
			static constexpr unsigned source_offset() noexcept
			{
				return 1;
			}
		};

		struct by_member_func
		{
			enum class an_enum
			{
				value = 2
			};

			SOAGEN_PURE_INLINE_GETTER
			constexpr an_enum source_offset() const noexcept
			{
				return an_enum::value;
			}
		};

		struct by_member_var
		{
			short source_offset = 3;
		};

		struct by_static_var
		{
			static constexpr int source_offset = 4;
		};

		struct by_adl
		{};

		SOAGEN_PURE_INLINE_GETTER
		constexpr std::size_t source_offset(const by_adl&) noexcept
		{
			return 5;
		}
	}
}

TEST_CASE("source_offset")
{
	SECTION("by static function")
	{
		CHECK(soagen::get_source_offset(foo::by_static_func{}) == 1u);
	}

	SECTION("by member function")
	{
		CHECK(soagen::get_source_offset(foo::by_member_func{}) == 2u);
	}

	SECTION("by member var")
	{
		CHECK(soagen::get_source_offset(foo::by_member_var{}) == 3u);
	}

	SECTION("by static var")
	{
		CHECK(soagen::get_source_offset(foo::by_static_var{}) == 4u);
	}

	SECTION("by ADL")
	{
		CHECK(soagen::get_source_offset(foo::by_adl{}) == 5u);
	}
}
