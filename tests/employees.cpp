// This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
#include "employees.hpp"

SOAGEN_DISABLE_ARITHMETIC_WARNINGS;

using namespace tests;
using table_traits = employees::table_traits;

/*

	tests::employees models this struct:

	 struct employees
	 {
		const std::string name;
		unsigned long long id;
		std::tuple<int, int, int> date_of_birth;
		int salary;
		int* tag;
	 };
*/

static_assert(std::is_nothrow_default_constructible_v<employees>);
static_assert(std::is_nothrow_move_constructible_v<employees>);
static_assert(std::is_nothrow_move_assignable_v<employees>);
static_assert(std::is_copy_constructible_v<employees>);
static_assert(std::is_copy_assignable_v<employees>);
static_assert(std::is_nothrow_destructible_v<employees>);
static_assert(std::is_nothrow_swappable_v<employees>);

static_assert(employees::column_count == 5);
static_assert(employees::column_name<0> == "name"sv);
static_assert(employees::column_name<1> == "id"sv);
static_assert(employees::column_name<2> == "date_of_birth"sv);
static_assert(employees::column_name<3> == "salary"sv);
static_assert(employees::column_name<4> == "tag"sv);
static_assert(employees::column_indices::name == 0);
static_assert(employees::column_indices::id == 1);
static_assert(employees::column_indices::date_of_birth == 2);
static_assert(employees::column_indices::salary == 3);
static_assert(employees::column_indices::tag == 4);
static_assert(employees::aligned_stride >= 1);

static_assert(table_traits::all_default_constructible);
static_assert(table_traits::all_nothrow_default_constructible);
static_assert(!table_traits::all_trivially_default_constructible); // std::string
static_assert(!table_traits::all_trivially_copyable);
static_assert(table_traits::any_trivially_copyable);

static_assert(table_traits::all_copy_constructible);
static_assert(!table_traits::all_nothrow_copy_constructible);
static_assert(!table_traits::all_trivially_copy_constructible);

static_assert(table_traits::all_move_constructible);
static_assert(table_traits::all_nothrow_move_constructible);
static_assert(!table_traits::all_trivially_move_constructible);

static_assert(table_traits::all_copy_assignable);
static_assert(!table_traits::all_nothrow_copy_assignable);
static_assert(!table_traits::all_trivially_copy_assignable);

static_assert(table_traits::all_move_assignable);
static_assert(table_traits::all_nothrow_move_assignable);
static_assert(!table_traits::all_trivially_move_assignable);

static_assert(table_traits::all_destructible);
static_assert(table_traits::all_nothrow_destructible);
static_assert(!table_traits::all_trivially_destructible);

static_assert(table_traits::all_swappable);
static_assert(table_traits::all_nothrow_swappable);

static_assert(table_traits::all_equality_comparable);
static_assert(table_traits::all_less_than_comparable);

static_assert(table_traits::row_constructible_from<const char*&&, //
												   unsigned int&&,
												   std::tuple<int, int, int>&&,
												   int&&,
												   int*&&>);

static_assert(!table_traits::row_nothrow_constructible_from<const char*&&, //
															unsigned int&&,
															std::tuple<int, int, int>&&,
															int&&,
															int*&&>);

#define CHECK_ROW_EQ(row, Name, Id, Date_of_birth, Salary, Tag)                                                        \
	CHECK((row).name == Name);                                                                                         \
	CHECK((row).id == static_cast<unsigned long long>(Id));                                                            \
	CHECK((row).date_of_birth == std::tuple Date_of_birth);                                                            \
	CHECK((row).salary == static_cast<int>(Salary));                                                                   \
	REQUIRE((row).tag == Tag)

TEST_CASE("employees - general use")
{
	employees emp;
	{
		INFO("default construction");

		CHECK(emp.empty());
		CHECK(!emp.size());
		CHECK(!emp.capacity());
		CHECK(!emp.allocation_size());
		CHECK(emp.max_size());
		emp.for_each_column([](auto* ptr) { REQUIRE(!ptr); });
	}

	{
		INFO("push_back()");

		{
			INFO("to empty container");

			emp.push_back("mark gillard", 0, { 1987, 03, 16 }, 999999 /* i wish lol */);
			CHECK(!emp.empty());
			CHECK(emp.size() == 1);
			CHECK(emp.capacity() >= 1);
			CHECK(emp.allocation_size());
			CHECK(emp.max_size());
			CHECK(emp.size() <= emp.max_size());
			emp.for_each_column(
				[](auto* ptr, size_t idx)
				{
					REQUIRE(ptr);
					CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
				});
			CHECK_ROW_EQ(emp.front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK_ROW_EQ(emp[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK_ROW_EQ(emp.back(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK(emp[0] == emp[0]);
			CHECK(emp[0] == std::as_const(emp)[0]);
			CHECK(emp[0] == std::move(emp)[0]);
			CHECK(emp[0] == std::move(std::as_const(emp))[0]);
			CHECK(emp[0] <= emp[0]);
			CHECK(emp[0] >= emp[0]);
			CHECK_FALSE(emp[0] < emp[0]);
			CHECK_FALSE(emp[0] > emp[0]);
		}

		{
			INFO("to non-empty container");

			emp.push_back("joe bloggs", 1, { 1970, 01, 01 }, 50000);
			CHECK(!emp.empty());
			CHECK(emp.size() == 2);
			CHECK(emp.capacity() >= 2);
			CHECK(emp.allocation_size());
			CHECK(emp.max_size());
			CHECK(emp.size() <= emp.max_size());
			emp.for_each_column(
				[](auto* ptr, size_t idx)
				{
					REQUIRE(ptr);
					CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
				});
			CHECK_ROW_EQ(emp.front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK_ROW_EQ(emp[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			CHECK_ROW_EQ(emp.back(), "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			CHECK(emp[1] == emp[1]);
			CHECK(emp[1] == std::as_const(emp)[1]);
			CHECK(emp[1] == std::move(emp)[1]);
			CHECK(emp[1] == std::move(std::as_const(emp))[1]);
			CHECK(emp[1] != emp[0]);
			CHECK(emp[1] != std::as_const(emp)[0]);
			CHECK(emp[1] != std::move(emp)[0]);
			CHECK(emp[1] != std::move(std::as_const(emp))[0]);
			CHECK(emp[1] <= emp[1]);
			CHECK(emp[1] >= emp[1]);
			CHECK_FALSE(emp[1] < emp[1]);
			CHECK_FALSE(emp[1] > emp[1]);
			CHECK(emp[1] <= emp[0]);
			CHECK(emp[1] < emp[0]);
			CHECK_FALSE(emp[1] >= emp[0]);
			CHECK_FALSE(emp[1] > emp[0]);
			CHECK(emp[0] >= emp[1]);
			CHECK(emp[0] > emp[1]);
			CHECK_FALSE(emp[0] <= emp[1]);
			CHECK_FALSE(emp[0] < emp[1]);
		}
	}

	employees emp2{ emp };
	{
		INFO("copy construction");

		CHECK(!emp.empty());
		CHECK(emp.size() == 2);
		CHECK(emp.capacity() >= 2);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});
		CHECK(!emp2.empty());
		CHECK(emp2.size() == 2);
		CHECK(emp2.capacity() >= 2);
		CHECK(emp2.allocation_size());
		CHECK(emp2.max_size());
		CHECK(emp2.size() <= emp2.max_size());
		emp2.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});
		CHECK(emp.column<0>() != emp2.column<0>());
		CHECK(emp == emp2);
		CHECK(emp <= emp2);
		CHECK(emp >= emp2);
		CHECK_FALSE(emp != emp2);
		CHECK_FALSE(emp < emp2);
		CHECK_FALSE(emp > emp2);
		CHECK_ROW_EQ(emp2[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp2[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK(emp[0] == emp2[0]);
		CHECK(emp[1] == emp2[1]);
	}

	{
		INFO("shrink_to_fit() - non-empty container");

		emp.shrink_to_fit(); // should leave the container in a valid state still with the two rows we've added
		CHECK(!emp.empty());
		CHECK(emp.size() == 2);
		CHECK(emp.capacity() >= 2);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});
	}

	{
		INFO("clear()");

		emp.clear(); // should leave some capacity
		CHECK(emp.empty());
		CHECK(!emp.size());
		CHECK(emp.capacity());
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});
	}

	{
		INFO("shrink_to_fit() - empty container");

		emp.shrink_to_fit(); // was empty - should free the allocation
		CHECK(emp.empty());
		CHECK(!emp.size());
		CHECK(!emp.capacity());
		CHECK(!emp.allocation_size());
		CHECK(emp.max_size());
		emp.for_each_column([](auto* ptr) { REQUIRE(!ptr); });
	}

	{
		INFO("copy-assignment");

		emp = emp2; // should make emp exactly equal to emp2 again
		CHECK(!emp.empty());
		CHECK(emp.size() == 2);
		CHECK(emp.capacity() >= 2);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});
		CHECK(!emp2.empty());
		CHECK(emp2.size() == 2);
		CHECK(emp2.capacity() >= 2);
		CHECK(emp2.allocation_size());
		CHECK(emp2.max_size());
		CHECK(emp2.size() <= emp2.max_size());
		emp2.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});
		CHECK(emp.column<0>() != emp2.column<0>());
		CHECK(emp == emp2);
		CHECK(emp <= emp2);
		CHECK(emp >= emp2);
		CHECK_FALSE(emp != emp2);
		CHECK_FALSE(emp < emp2);
		CHECK_FALSE(emp > emp2);
		CHECK_ROW_EQ(emp[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK(emp[0] == emp2[0]);
		CHECK(emp[1] == emp2[1]);
	}

	{
		INFO("pop_back()");

		{
			INFO("zero elements (no-op)");

			emp.pop_back(0u);
			CHECK(!emp.empty());
			CHECK(emp.size() == 2);
			CHECK(emp.capacity() >= 2);
			CHECK(emp.allocation_size());
			CHECK(emp.max_size());
			CHECK(emp.size() <= emp.max_size());
			emp.for_each_column(
				[](auto* ptr, size_t idx)
				{
					REQUIRE(ptr);
					CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
				});
			CHECK_ROW_EQ(emp[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK_ROW_EQ(emp[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		}

		{
			INFO("one element");

			emp.pop_back();
			CHECK(!emp.empty());
			CHECK(emp.size() == 1);
			CHECK(emp.capacity() >= 2); // shouldn't have shrunk
			CHECK(emp.allocation_size());
			CHECK(emp.max_size());
			CHECK(emp.size() <= emp.max_size());
			emp.for_each_column(
				[](auto* ptr, size_t idx)
				{
					REQUIRE(ptr);
					CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
				});
			CHECK(emp != emp2);
			CHECK(emp < emp2); // should now be lexicographically less than emp2
			CHECK_FALSE(emp >= emp2);
			CHECK(emp != emp2);
			CHECK(emp < emp2);
			CHECK_FALSE(emp > emp2);
			CHECK_ROW_EQ(emp[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		}

		{
			INFO("multiple elements");

			emp = emp2;
			emp.pop_back(2u);
			CHECK(emp.empty());
			CHECK(!emp.size());
			CHECK(emp.capacity() >= 2); // shouldn't have shrunk
			CHECK(emp.allocation_size());
			CHECK(emp.max_size());
			emp.for_each_column(
				[](auto* ptr, size_t idx)
				{
					REQUIRE(ptr);
					CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
				});
			CHECK(emp != emp2);
			CHECK(emp < emp2);
			CHECK_FALSE(emp >= emp2);
			CHECK(emp != emp2);
			CHECK(emp < emp2);
			CHECK_FALSE(emp > emp2);
		}
	}

	// emp is now empty

	{
		INFO("move-assignment");

		emp = std::move(emp2);
		CHECK(!emp.empty());
		CHECK(emp.size() == 2);
		CHECK(emp.capacity() >= 2);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});
		CHECK(emp2.empty());
		CHECK(!emp2.size());
		CHECK(!emp2.capacity());
		CHECK(!emp2.allocation_size());
		CHECK(emp2.max_size());
		emp2.for_each_column([](auto* ptr) { REQUIRE(!ptr); });
		CHECK(emp.column<0>() != emp2.column<0>());
		CHECK_FALSE(emp == emp2);
		CHECK_FALSE(emp <= emp2);
		CHECK(emp >= emp2);
		CHECK(emp != emp2);
		CHECK_FALSE(emp < emp2);
		CHECK(emp > emp2);
		CHECK_ROW_EQ(emp[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
	}

	// emp is back to having two rows
	// emp2 is now empty with no allocation.

	int someval = 7;

	{
		INFO("emplace_back()");

		emp.emplace_back(emplacer{ size_t{ 10 }, 'A' }, 2u, emplacer{ 1980, 2, 2 }, 40000, &someval);
		CHECK(!emp.empty());
		CHECK(emp.size() == 3);
		CHECK(emp.capacity() >= 3);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		CHECK_ROW_EQ(emp.front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp[2], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp.back(), "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
	}

	{
		INFO("insert()");

		emp.insert(1u, "hot diggity", 3, { 1955, 3, 3 }, 70000);
		CHECK(!emp.empty());
		CHECK(emp.size() == 4);
		CHECK(emp.capacity() >= 4);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		CHECK_ROW_EQ(emp.front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[1], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
		CHECK_ROW_EQ(emp[2], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp[3], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp.back(), "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
	}

	{
		INFO("emplace()");

		emp.emplace(0u, emplacer{ size_t{ 10 }, 'B' }, 4u, emplacer{ 1990, 3, 3 }, 30000);
		CHECK(!emp.empty());
		CHECK(emp.size() == 5);
		CHECK(emp.capacity() >= 5);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		CHECK_ROW_EQ(emp.front(), "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[0], "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[2], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
		CHECK_ROW_EQ(emp[3], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp[4], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp.back(), "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
	}

	{
		INFO("push_back() - many");

		for (size_t i = 0; i < 1000; i++)
		{
			const auto prev_size = emp.size();

			emp.push_back("spam mcspamface", prev_size, { 9876, 54, 32 }, 12345);
			CHECK(!emp.empty());
			CHECK(emp.size() == prev_size + 1u);
			CHECK(emp.capacity() >= prev_size + 1u);
			CHECK(emp.allocation_size());
			CHECK(emp.max_size());
			CHECK(emp.size() <= emp.max_size());
			emp.for_each_column(
				[](auto* ptr, size_t idx)
				{
					REQUIRE(ptr);
					CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
				});

			CHECK_ROW_EQ(emp.front(), "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
			CHECK_ROW_EQ(emp[0], "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
			CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK_ROW_EQ(emp[2], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
			CHECK_ROW_EQ(emp[3], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			CHECK_ROW_EQ(emp[4], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);

			CHECK_ROW_EQ(emp[prev_size], "spam mcspamface", prev_size, (9876, 54, 32), 12345, nullptr);
			CHECK_ROW_EQ(emp.back(), "spam mcspamface", prev_size, (9876, 54, 32), 12345, nullptr);
		}

		INFO("checking aligned_stride");
		for (size_t i = 0; i < emp.size(); i++)
		{
			INFO(i);

			const auto on_stride = (i % employees::aligned_stride) == 0u;
			auto all_aligned	 = true;

			emp.for_each_column(
				[&](auto* ptr, size_t idx)
				{
					all_aligned =
						all_aligned
						&& (reinterpret_cast<uintptr_t>(ptr + i) % employees::table_traits::column_alignments[idx])
							   == 0u;
				});

			CHECK(on_stride == all_aligned);
		}
	}

	{
		INFO("pop_back() - many");

		emp.pop_back(1000u);
		CHECK(!emp.empty());
		CHECK(emp.size() == 5);
		CHECK(emp.capacity() >= 5);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		CHECK_ROW_EQ(emp.front(), "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[0], "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[2], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
		CHECK_ROW_EQ(emp[3], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp[4], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp.back(), "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
	}

	{
		INFO("push_back() - soagen::row");

		emp.push_back(emp[2]);
		CHECK(!emp.empty());
		CHECK(emp.size() == 6);
		CHECK(emp.capacity() >= 6);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		CHECK_ROW_EQ(emp.front(), "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[0], "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[2], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
		CHECK_ROW_EQ(emp[3], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp[4], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp[5], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
		CHECK_ROW_EQ(emp.back(), "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
	}

	{
		INFO("erase(size_type)");

		emp.erase(5); // both instances of "hot diggity"
		emp.erase(2);
		CHECK(!emp.empty());
		CHECK(emp.size() == 4);
		CHECK(emp.capacity() >= 4);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		CHECK_ROW_EQ(emp.front(), "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[0], "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[2], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp[3], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp.back(), "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
	}

	{
		INFO("iterating");

		std::vector<std::string> names;
		for (auto&& row : emp)
			names.push_back(row.name);

		REQUIRE(names.size() == 4u);
		CHECK(names[0] == "BBBBBBBBBB");
		CHECK(names[1] == "mark gillard");
		CHECK(names[2] == "joe bloggs");
		CHECK(names[3] == "AAAAAAAAAA");

		auto it = emp.begin();
		CHECK_ROW_EQ(*it, "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		it++;
		CHECK_ROW_EQ(*it, "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		it++;
		CHECK_ROW_EQ(*it, "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		it++;
		CHECK_ROW_EQ(*it, "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		it++;
		CHECK(it == emp.end());

		it--;
		CHECK_ROW_EQ(*it, "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		it--;
		CHECK_ROW_EQ(*it, "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		it--;
		CHECK_ROW_EQ(*it, "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		it--;
		CHECK_ROW_EQ(*it, "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
		CHECK(it == emp.begin());

		CHECK(it->name == "BBBBBBBBBB");
	}

	{
		INFO("unordered_erase(size_type) - not at end");

		const auto moved_into_place = emp.unordered_erase(0);
		CHECK(!emp.empty());
		CHECK(emp.size() == 3);
		CHECK(emp.capacity() >= 3);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		REQUIRE(moved_into_place.has_value());
		CHECK(*moved_into_place == 3u);

		CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[2], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
	}

	{
		INFO("unordered_erase(size_type) - at end");

		const auto moved_into_place = emp.unordered_erase(2);
		CHECK(!emp.empty());
		CHECK(emp.size() == 2);
		CHECK(emp.capacity() >= 2);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		REQUIRE(!moved_into_place.has_value());

		CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
	}

	emp.emplace_back(emplacer{ size_t{ 10 }, 'B' }, 4u, emplacer{ 1990, 3, 3 }, 30000);
	emp.push_back("hot diggity", 3, { 1955, 3, 3 }, 70000);
	emp.push_back("joe bloggs", 1, { 1970, 01, 01 }, 50000);

	// reset for some more erase() tests
	CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
	CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
	CHECK_ROW_EQ(emp[2], "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
	CHECK_ROW_EQ(emp[3], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
	CHECK_ROW_EQ(emp[4], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);

	{
		INFO("erase(iterator) - not at end");

		const auto pos = emp.erase(emp.begin() + 2); // BBBBBBBBBB

		CHECK(!emp.empty());
		CHECK(emp.size() == 4);
		CHECK(emp.capacity() >= 4);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		REQUIRE(pos == emp.begin() + 2);
		REQUIRE(pos == emp.cbegin() + 2);
		CHECK(pos - emp.begin() == 2);
		CHECK(emp.begin() - pos == -2);
		CHECK(pos > emp.begin());
		CHECK(pos >= emp.begin());
		CHECK_FALSE(pos < emp.begin());
		CHECK_FALSE(pos <= emp.begin());

		CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[2], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
		CHECK_ROW_EQ(emp[3], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
	}

	{
		INFO("erase(iterator) - at end");

		const auto pos = emp.erase(emp.cbegin() + 3); // joe bloggs

		CHECK(!emp.empty());
		CHECK(emp.size() == 3);
		CHECK(emp.capacity() >= 3);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		REQUIRE(pos == emp.end());
		REQUIRE(pos == emp.cend());
		CHECK(pos - emp.begin() == 3);
		CHECK(emp.begin() - pos == -3);
		CHECK(pos > emp.begin());
		CHECK(pos >= emp.begin());
		CHECK_FALSE(pos < emp.begin());
		CHECK_FALSE(pos <= emp.begin());

		CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp[2], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
	}

	{
		INFO("unordered_erase(iterator) - not at end");

		const auto moved_into_place = emp.unordered_erase(emp.begin() + 1);
		CHECK(!emp.empty());
		CHECK(emp.size() == 2);
		CHECK(emp.capacity() >= 2);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		REQUIRE(moved_into_place.has_value());
		CHECK(*moved_into_place - emp.begin() == 2);

		CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp[1], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
	}

	{
		INFO("unordered_erase(iterator) - at end");

		const auto moved_into_place = emp.unordered_erase(emp.cend() - 1);
		CHECK(!emp.empty());
		CHECK(emp.size() == 1);
		CHECK(emp.capacity() >= 1);
		CHECK(emp.allocation_size());
		CHECK(emp.max_size());
		CHECK(emp.size() <= emp.max_size());
		emp.for_each_column(
			[](auto* ptr, size_t idx)
			{
				REQUIRE(ptr);
				CHECK(reinterpret_cast<uintptr_t>(ptr) % employees::table_traits::column_alignments[idx] == 0u);
			});

		REQUIRE(!moved_into_place.has_value());

		CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
	}
}
