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

static_assert(soagen::is_soa<employees>);
static_assert(!soagen::is_table<employees>);
static_assert(soagen::is_table<employees::table_type>);
static_assert(soagen::is_table_traits<employees::table_traits>);
static_assert(std::is_same_v<employees::table_type, soagen::table<employees::table_traits>>);

static_assert(std::is_same_v<soagen::table_type<employees>, soagen::table<employees::table_traits>>);
static_assert(std::is_same_v<soagen::table_traits_type<employees>, employees::table_traits>);

static_assert(std::is_same_v<soagen::table_type<soagen::table<employees::table_traits>>, //
							 soagen::table<employees::table_traits>>);
static_assert(std::is_same_v<soagen::table_traits_type<soagen::table<employees::table_traits>>, //
							 employees::table_traits>);

static_assert(std::is_same_v<employees, soagen::soa_type<employees>>);
static_assert(std::is_same_v<employees, soagen::soa_type<employees::row_type>>);
static_assert(std::is_same_v<employees, soagen::soa_type<employees::span_type>>);
static_assert(std::is_same_v<employees, soagen::soa_type<employees::iterator>>);

static_assert(std::is_nothrow_default_constructible_v<employees>);
static_assert(std::is_nothrow_move_constructible_v<employees>);
static_assert(std::is_nothrow_move_assignable_v<employees>);
static_assert(std::is_copy_constructible_v<employees>);
static_assert(std::is_copy_assignable_v<employees>);
static_assert(std::is_nothrow_destructible_v<employees>);
static_assert(std::is_nothrow_swappable_v<employees>);

static_assert(soagen::is_row<employees::row_type>);
static_assert(soagen::is_row<employees::rvalue_row_type>);
static_assert(soagen::is_row<employees::const_row_type>);

static_assert(std::is_same_v<soagen::row<employees, 0, 1, 2, 3, 4>, soagen::row_type<employees>>);
static_assert(std::is_same_v<soagen::row<const employees, 0, 1, 2, 3, 4>, soagen::row_type<const employees>>);
static_assert(std::is_same_v<soagen::row<employees, 0, 1, 2, 3, 4>, soagen::row_type<employees&>>);
static_assert(std::is_same_v<soagen::row<const employees, 0, 1, 2, 3, 4>, soagen::row_type<const employees&>>);
static_assert(std::is_same_v<soagen::row<employees&&, 0, 1, 2, 3, 4>, soagen::row_type<employees&&>>);
static_assert(std::is_same_v<soagen::row<const employees&&, 0, 1, 2, 3, 4>, soagen::row_type<const employees&&>>);

static_assert(std::is_trivially_move_constructible_v<employees::row_type>);
static_assert(std::is_trivially_copy_constructible_v<employees::row_type>);
static_assert(std::is_trivially_destructible_v<employees::row_type>);

static_assert(soagen::is_iterator<employees::iterator>);
static_assert(soagen::is_iterator<employees::rvalue_iterator>);
static_assert(soagen::is_iterator<employees::const_iterator>);

static_assert(std::is_nothrow_default_constructible_v<employees::iterator>);
static_assert(std::is_trivially_move_constructible_v<employees::iterator>);
static_assert(std::is_trivially_move_assignable_v<employees::iterator>);
static_assert(std::is_trivially_copy_constructible_v<employees::iterator>);
static_assert(std::is_trivially_copy_assignable_v<employees::iterator>);
static_assert(std::is_trivially_destructible_v<employees::iterator>);
static_assert(std::is_nothrow_swappable_v<employees::iterator>);

static_assert(soagen::is_span<employees::span_type>);
static_assert(soagen::is_span<employees::rvalue_span_type>);
static_assert(soagen::is_span<employees::const_span_type>);

static_assert(std::is_nothrow_default_constructible_v<employees::span_type>);
static_assert(std::is_trivially_move_constructible_v<employees::span_type>);
static_assert(std::is_trivially_move_assignable_v<employees::span_type>);
static_assert(std::is_trivially_copy_constructible_v<employees::span_type>);
static_assert(std::is_trivially_copy_assignable_v<employees::span_type>);
static_assert(std::is_trivially_destructible_v<employees::span_type>);
static_assert(std::is_nothrow_swappable_v<employees::span_type>);

static_assert(employees::column_count == 5);
static_assert(static_cast<size_t>(employees::columns::name) == 0);
static_assert(static_cast<size_t>(employees::columns::id) == 1);
static_assert(static_cast<size_t>(employees::columns::date_of_birth) == 2);
static_assert(static_cast<size_t>(employees::columns::salary) == 3);
static_assert(static_cast<size_t>(employees::columns::tag) == 4);
static_assert(employees::column_name<0> == "name"sv);
static_assert(employees::column_name<1> == "id"sv);
static_assert(employees::column_name<2> == "date_of_birth"sv);
static_assert(employees::column_name<3> == "salary"sv);
static_assert(employees::column_name<4> == "tag"sv);
static_assert(employees::column_name<employees::columns::name> == "name"sv);
static_assert(employees::column_name<employees::columns::id> == "id"sv);
static_assert(employees::column_name<employees::columns::date_of_birth> == "date_of_birth"sv);
static_assert(employees::column_name<employees::columns::salary> == "salary"sv);
static_assert(employees::column_name<employees::columns::tag> == "tag"sv);
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
	CHECK((row).column<employees::columns::name>() == Name);                                                           \
	CHECK((row).column<employees::columns::id>() == static_cast<unsigned long long>(Id));                              \
	CHECK((row).column<employees::columns::date_of_birth>() == std::tuple Date_of_birth);                              \
	CHECK((row).column<employees::columns::salary>() == static_cast<int>(Salary));                                     \
	REQUIRE((row).column<employees::columns::tag>() == Tag)

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
			CHECK(emp.name()[0] == "mark gillard");
			CHECK(emp.id()[0] == 0);
			CHECK(emp.date_of_birth()[0] == std::tuple{ 1987, 03, 16 });
			CHECK(emp.salary()[0] == 999999);
			CHECK(emp.tag()[0] == nullptr);
			CHECK(emp[0] == emp[0]);
			CHECK(emp[0] == std::as_const(emp)[0]);
			CHECK(emp[0] == std::move(emp)[0]);
			CHECK(emp[0] == std::move(std::as_const(emp))[0]);
			CHECK(emp[0] <= emp[0]);
			CHECK(emp[0] >= emp[0]);
			CHECK_FALSE(emp[0] < emp[0]);
			CHECK_FALSE(emp[0] > emp[0]);
			emp[0].for_each_column(		//
				[](auto&& val) noexcept //
				{						//
					static_assert(std::is_lvalue_reference_v<decltype(val)>);
				});
			std::move(emp)[0].for_each_column( //
				[](auto&& val) noexcept		   //
				{							   //
					static_assert(std::is_rvalue_reference_v<decltype(val)>);
				});
			std::as_const(emp)[0].for_each_column( //
				[](auto&& val) noexcept
				{
					static_assert(std::is_lvalue_reference_v<decltype(val)>);
					static_assert(std::is_const_v<std::remove_reference_t<decltype(val)>>);
				});

			{
				INFO("subspan");

				auto span = emp.subspan(0u);
				CHECK(!span.empty());
				CHECK(span.size() == 1);
				span.for_each_column(
					[&](auto* ptr, auto idx_ic)
					{
						static_assert(!std::is_same_v<decltype(idx_ic), size_t>); // should be an integral_constant

						REQUIRE(ptr);
						REQUIRE(ptr == emp.column<decltype(idx_ic)::value>());
					});
				CHECK_ROW_EQ(span.front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
				CHECK_ROW_EQ(span[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
				CHECK_ROW_EQ(span.row(0), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
				CHECK_ROW_EQ(span.at(0), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
				CHECK_ROW_EQ(span.back(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
				CHECK(span.name()[0] == "mark gillard");
				CHECK(span.id()[0] == 0);
				CHECK(span.date_of_birth()[0] == std::tuple{ 1987, 03, 16 });
				CHECK(span.salary()[0] == 999999);
				CHECK(span.tag()[0] == nullptr);
			}
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
				[](auto* ptr, auto idx_ic)
				{
					static_assert(!std::is_same_v<decltype(idx_ic), size_t>); // should be an integral_constant

					REQUIRE(ptr);
					CHECK(reinterpret_cast<uintptr_t>(ptr)
							  % employees::table_traits::column_alignments[decltype(idx_ic)::value]
						  == 0u);
				});
			CHECK_ROW_EQ(emp.front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK_ROW_EQ(emp[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			CHECK_ROW_EQ(emp.back(), "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			CHECK(emp[1] == emp[1]);
			CHECK(emp[1] == std::as_const(emp)[1]);
			CHECK(emp[1] == static_cast<employees::rvalue_row_type>(emp[1]));
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

			{
				INFO("subspan");

				auto span = emp.const_subspan(0u);
				CHECK(!span.empty());
				CHECK(span.size() == 2);
				CHECK_ROW_EQ(span.front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
				CHECK_ROW_EQ(span[1], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
				CHECK_ROW_EQ(span.back(), "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);

				span = span.subspan(1u);
				CHECK(!span.empty());
				CHECK(span.size() == 1);
				CHECK_ROW_EQ(span.front(), "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
				CHECK_ROW_EQ(span[0], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
				CHECK_ROW_EQ(span.back(), "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			}
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

		emp.emplace_back(emplacer{ size_t{ 10 }, 'A' }, 2u, std::tuple{ 1980, 2, 2 }, 40000, &someval);
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

		CHECK_ROW_EQ(emp.table().front(), "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp.table()[0], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
		CHECK_ROW_EQ(emp.table()[1], "hot diggity", 3, (1955, 3, 3), 70000, nullptr);
		CHECK_ROW_EQ(emp.table()[2], "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp.table()[3], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp.table().back(), "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
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
		INFO("emplace_back() - soagen::row");

		emp.emplace_back(emp[2]);
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

		{
			INFO("generated table");

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
			INFO("underlying table");

			std::vector<std::string> names;
			for (auto&& row : emp.table())
				names.push_back(row.column<employees::columns::name>());

			REQUIRE(names.size() == 4u);
			CHECK(names[0] == "BBBBBBBBBB");
			CHECK(names[1] == "mark gillard");
			CHECK(names[2] == "joe bloggs");
			CHECK(names[3] == "AAAAAAAAAA");

			auto it = emp.table().begin();
			CHECK_ROW_EQ(*it, "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
			it++;
			CHECK_ROW_EQ(*it, "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			it++;
			CHECK_ROW_EQ(*it, "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			it++;
			CHECK_ROW_EQ(*it, "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
			it++;
			CHECK(it == emp.table().end());

			it--;
			CHECK_ROW_EQ(*it, "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
			it--;
			CHECK_ROW_EQ(*it, "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			it--;
			CHECK_ROW_EQ(*it, "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			it--;
			CHECK_ROW_EQ(*it, "BBBBBBBBBB", 4, (1990, 3, 3), 30000, nullptr);
			CHECK(it == emp.table().begin());

			CHECK(it->column<employees::columns::name>() == "BBBBBBBBBB");
		}

		{
			INFO("subspan");

			std::vector<std::string> names;
			auto span = emp.subspan(1u, 2u);
			for (auto&& row : span)
				names.push_back(row.column<employees::columns::name>());

			REQUIRE(names.size() == 2u);
			CHECK(names[0] == "mark gillard");
			CHECK(names[1] == "joe bloggs");

			auto it = span.begin();
			CHECK_ROW_EQ(*it, "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			it++;
			CHECK_ROW_EQ(*it, "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			it++;
			CHECK(it == span.end());

			it--;
			CHECK_ROW_EQ(*it, "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
			it--;
			CHECK_ROW_EQ(*it, "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
			CHECK(it == span.begin());

			CHECK(it->column<employees::columns::name>() == "mark gillard");
		}
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
		CHECK_ROW_EQ(emp.row(3), "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
		CHECK_ROW_EQ(emp.at(3), "joe bloggs", 1, (1970, 1, 1), 50000, nullptr);
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

	{
		INFO("emplace_back() - std::tuple");

		emp.emplace_back(std::tuple{ "mark gillard", 0u, std::tuple{ 1987, 03, 16 }, 999999, nullptr });

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

		CHECK_ROW_EQ(emp[0], "AAAAAAAAAA", 2, (1980, 2, 2), 40000, &someval);
		CHECK_ROW_EQ(emp[1], "mark gillard", 0, (1987, 03, 16), 999999, nullptr);
	}

	{
		INFO("swizzles");

		// 3, 1, 0 == salary, id, name

		{
			INFO("row()");

			CHECK(emp.row<3, 1, 0>(1).salary == 999999);
			CHECK(emp.row<3, 1, 0>(1).get<0>() == 999999);
			CHECK(emp.row<3, 1, 0>(1).column<3>() == 999999);

			CHECK(emp.row<3, 1, 0>(1).id == 0);
			CHECK(emp.row<3, 1, 0>(1).get<1>() == 0);
			CHECK(emp.row<3, 1, 0>(1).column<1>() == 0);

			CHECK(emp.row<3, 1, 0>(1).name == "mark gillard");
			CHECK(emp.row<3, 1, 0>(1).get<2>() == "mark gillard");
			CHECK(emp.row<3, 1, 0>(1).column<0>() == "mark gillard");
		}

		{
			INFO("at()");

			CHECK(emp.at<3, 1, 0>(1).salary == 999999);
			CHECK(emp.at<3, 1, 0>(1).get<0>() == 999999);
			CHECK(emp.at<3, 1, 0>(1).column<3>() == 999999);

			CHECK(emp.at<3, 1, 0>(1).id == 0);
			CHECK(emp.at<3, 1, 0>(1).get<1>() == 0);
			CHECK(emp.at<3, 1, 0>(1).column<1>() == 0);

			CHECK(emp.at<3, 1, 0>(1).name == "mark gillard");
			CHECK(emp.at<3, 1, 0>(1).get<2>() == "mark gillard");
			CHECK(emp.at<3, 1, 0>(1).column<0>() == "mark gillard");
		}

		{
			INFO("front()");

			CHECK(emp.front<3, 1, 0>().salary == 40000);
			CHECK(emp.front<3, 1, 0>().get<0>() == 40000);
			CHECK(emp.front<3, 1, 0>().column<3>() == 40000);

			CHECK(emp.front<3, 1, 0>().id == 2);
			CHECK(emp.front<3, 1, 0>().get<1>() == 2);
			CHECK(emp.front<3, 1, 0>().column<1>() == 2);

			CHECK(emp.front<3, 1, 0>().name == "AAAAAAAAAA");
			CHECK(emp.front<3, 1, 0>().get<2>() == "AAAAAAAAAA");
			CHECK(emp.front<3, 1, 0>().column<0>() == "AAAAAAAAAA");
		}

		{
			INFO("back()");

			CHECK(emp.back<3, 1, 0>().salary == 999999);
			CHECK(emp.back<3, 1, 0>().get<0>() == 999999);
			CHECK(emp.back<3, 1, 0>().column<3>() == 999999);

			CHECK(emp.back<3, 1, 0>().id == 0);
			CHECK(emp.back<3, 1, 0>().get<1>() == 0);
			CHECK(emp.back<3, 1, 0>().column<1>() == 0);

			CHECK(emp.back<3, 1, 0>().name == "mark gillard");
			CHECK(emp.back<3, 1, 0>().get<2>() == "mark gillard");
			CHECK(emp.back<3, 1, 0>().column<0>() == "mark gillard");
		}

		{
			INFO("subspan");

			const auto span = emp.subspan(0u);

			{
				INFO("row()");

				CHECK(span.row<3, 1, 0>(1).salary == 999999);
				CHECK(span.row<3, 1, 0>(1).get<0>() == 999999);
				CHECK(span.row<3, 1, 0>(1).column<3>() == 999999);

				CHECK(span.row<3, 1, 0>(1).id == 0);
				CHECK(span.row<3, 1, 0>(1).get<1>() == 0);
				CHECK(span.row<3, 1, 0>(1).column<1>() == 0);

				CHECK(span.row<3, 1, 0>(1).name == "mark gillard");
				CHECK(span.row<3, 1, 0>(1).get<2>() == "mark gillard");
				CHECK(span.row<3, 1, 0>(1).column<0>() == "mark gillard");
			}

			{
				INFO("at()");

				CHECK(span.at<3, 1, 0>(1).salary == 999999);
				CHECK(span.at<3, 1, 0>(1).get<0>() == 999999);
				CHECK(span.at<3, 1, 0>(1).column<3>() == 999999);

				CHECK(span.at<3, 1, 0>(1).id == 0);
				CHECK(span.at<3, 1, 0>(1).get<1>() == 0);
				CHECK(span.at<3, 1, 0>(1).column<1>() == 0);

				CHECK(span.at<3, 1, 0>(1).name == "mark gillard");
				CHECK(span.at<3, 1, 0>(1).get<2>() == "mark gillard");
				CHECK(span.at<3, 1, 0>(1).column<0>() == "mark gillard");
			}

			{
				INFO("front()");

				CHECK(span.front<3, 1, 0>().salary == 40000);
				CHECK(span.front<3, 1, 0>().get<0>() == 40000);
				CHECK(span.front<3, 1, 0>().column<3>() == 40000);

				CHECK(span.front<3, 1, 0>().id == 2);
				CHECK(span.front<3, 1, 0>().get<1>() == 2);
				CHECK(span.front<3, 1, 0>().column<1>() == 2);

				CHECK(span.front<3, 1, 0>().name == "AAAAAAAAAA");
				CHECK(span.front<3, 1, 0>().get<2>() == "AAAAAAAAAA");
				CHECK(span.front<3, 1, 0>().column<0>() == "AAAAAAAAAA");
			}

			{
				INFO("back()");

				CHECK(span.back<3, 1, 0>().salary == 999999);
				CHECK(span.back<3, 1, 0>().get<0>() == 999999);
				CHECK(span.back<3, 1, 0>().column<3>() == 999999);

				CHECK(span.back<3, 1, 0>().id == 0);
				CHECK(span.back<3, 1, 0>().get<1>() == 0);
				CHECK(span.back<3, 1, 0>().column<1>() == 0);

				CHECK(span.back<3, 1, 0>().name == "mark gillard");
				CHECK(span.back<3, 1, 0>().get<2>() == "mark gillard");
				CHECK(span.back<3, 1, 0>().column<0>() == "mark gillard");
			}
		}
	}

	{
		INFO("row conversions");

		// same
		static_assert(is_implicitly_convertible<row<employees>, row<employees>>);
		static_assert(is_implicitly_convertible<const_row<employees>, const_row<employees>>);

		// gaining const, losing rvalue, or both:
		static_assert(is_implicitly_convertible<row<employees>, const_row<employees>>);
		static_assert(is_implicitly_convertible<row<employees&&>, row<employees>>);
		static_assert(is_implicitly_convertible<const_row<employees&&>, const_row<employees>>);
		static_assert(is_implicitly_convertible<row<employees&&>, const_row<employees>>);
		{
			using to = const_row<employees, 3, 1, 0>; // salary, id, name
			to dest	 = std::move(emp)[1];			  // rvalue -> const lvalue
			CHECK(dest.column<3>() == 999999);
			CHECK(dest.column<1>() == 0);
			CHECK(dest.column<0>() == "mark gillard");
			CHECK(dest.get<0>() == 999999);
			CHECK(dest.get<1>() == 0);
			CHECK(dest.get<2>() == "mark gillard");
			CHECK(dest.salary == 999999);
			CHECK(dest.id == 0);
			CHECK(dest.name == "mark gillard");
		}

		// gaining rvalue:
		static_assert(is_explicitly_convertible<row<employees>, row<employees&&>>);
		static_assert(is_explicitly_convertible<row<employees>, const_row<employees&&>>);
		static_assert(is_explicitly_convertible<const_row<employees>, const_row<employees&&>>);
		{
			using to = const_row<employees&&, 3, 1, 0>; // salary, id, name
			to dest	 = static_cast<to>(emp[1]);			// lvalue -> const rvalue
			CHECK(dest.column<3>() == 999999);
			CHECK(dest.column<1>() == 0);
			CHECK(dest.column<0>() == "mark gillard");
			CHECK(dest.get<0>() == 999999);
			CHECK(dest.get<1>() == 0);
			CHECK(dest.get<2>() == "mark gillard");
			CHECK(dest.salary == 999999);
			CHECK(dest.id == 0);
			CHECK(dest.name == "mark gillard");
		}
	}
}
