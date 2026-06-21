// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "fixtures.hpp"

using namespace tests;

//----------------------------------------------------------------------------------------------------------------------
// soa / table / row / span / iterator detection + type extraction
//----------------------------------------------------------------------------------------------------------------------

static_assert(soagen::is_soa<rich>);
static_assert(soagen::is_soa<trivial>);
static_assert(soagen::is_soa<move_only>);
static_assert(!soagen::is_soa<int>);
static_assert(!soagen::is_table<rich>);
static_assert(soagen::is_table<rich::table_type>);
static_assert(soagen::is_table_traits<rich::table_traits>);
static_assert(std::is_same_v<rich::table_type, soagen::table<rich::table_traits>>);

static_assert(std::is_same_v<soagen::table_type<rich>, soagen::table<rich::table_traits>>);
static_assert(std::is_same_v<soagen::table_traits_type<rich>, rich::table_traits>);
static_assert(std::is_same_v<soagen::table_type<soagen::table<rich::table_traits>>, soagen::table<rich::table_traits>>);
static_assert(std::is_same_v<soagen::table_traits_type<soagen::table<rich::table_traits>>, rich::table_traits>);

static_assert(std::is_same_v<rich, soagen::soa_type<rich>>);
static_assert(std::is_same_v<rich, soagen::soa_type<rich::row_type>>);
static_assert(std::is_same_v<rich, soagen::soa_type<rich::span_type>>);
static_assert(std::is_same_v<rich, soagen::soa_type<rich::iterator>>);

static_assert(soagen::is_row<rich::row_type>);
static_assert(soagen::is_row<rich::rvalue_row_type>);
static_assert(soagen::is_row<rich::const_row_type>);
static_assert(!soagen::is_row<rich>);

static_assert(std::is_same_v<soagen::row<rich, 0, 1, 2, 3, 4>, soagen::row_type<rich>>);
static_assert(std::is_same_v<soagen::row<const rich, 0, 1, 2, 3, 4>, soagen::row_type<const rich>>);
static_assert(std::is_same_v<soagen::row<rich, 0, 1, 2, 3, 4>, soagen::row_type<rich&>>);
static_assert(std::is_same_v<soagen::row<const rich, 0, 1, 2, 3, 4>, soagen::row_type<const rich&>>);
static_assert(std::is_same_v<soagen::row<rich&&, 0, 1, 2, 3, 4>, soagen::row_type<rich&&>>);
static_assert(std::is_same_v<soagen::row<const rich&&, 0, 1, 2, 3, 4>, soagen::row_type<const rich&&>>);

static_assert(soagen::is_iterator<rich::iterator>);
static_assert(soagen::is_iterator<rich::rvalue_iterator>);
static_assert(soagen::is_iterator<rich::const_iterator>);

static_assert(soagen::is_span<rich::span_type>);
static_assert(soagen::is_span<rich::rvalue_span_type>);
static_assert(soagen::is_span<rich::const_span_type>);

//----------------------------------------------------------------------------------------------------------------------
// trivially-copyable value semantics of the lightweight view types
//----------------------------------------------------------------------------------------------------------------------

static_assert(std::is_trivially_copy_constructible_v<rich::row_type>);
static_assert(std::is_trivially_destructible_v<rich::row_type>);

static_assert(std::is_nothrow_default_constructible_v<rich::iterator>);
static_assert(std::is_trivially_copy_constructible_v<rich::iterator>);
static_assert(std::is_trivially_copy_assignable_v<rich::iterator>);
static_assert(std::is_trivially_destructible_v<rich::iterator>);
static_assert(std::is_nothrow_swappable_v<rich::iterator>);

static_assert(std::is_nothrow_default_constructible_v<rich::span_type>);
static_assert(std::is_trivially_copy_constructible_v<rich::span_type>);
static_assert(std::is_trivially_copy_assignable_v<rich::span_type>);
static_assert(std::is_trivially_destructible_v<rich::span_type>);
static_assert(std::is_nothrow_swappable_v<rich::span_type>);

//----------------------------------------------------------------------------------------------------------------------
// columns enum / names / count
//----------------------------------------------------------------------------------------------------------------------

static_assert(rich::column_count == 5);
static_assert(static_cast<size_t>(rich::columns::name) == 0);
static_assert(static_cast<size_t>(rich::columns::id) == 1);
static_assert(static_cast<size_t>(rich::columns::date_of_birth) == 2);
static_assert(static_cast<size_t>(rich::columns::salary) == 3);
static_assert(static_cast<size_t>(rich::columns::tag) == 4);
static_assert(rich::column_name<0> == "name"sv);
static_assert(rich::column_name<rich::columns::date_of_birth> == "date_of_birth"sv);
static_assert(rich::aligned_stride >= 1);

static_assert(trivial::column_count == 4);
static_assert(move_only::column_count == 2);

//----------------------------------------------------------------------------------------------------------------------
// rich (non-trivial) table_traits: copyable, move-and-copy, non-trivial element ops
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    using rich_traits = rich::table_traits;

    static_assert(rich_traits::all_default_constructible);
    static_assert(rich_traits::all_nothrow_default_constructible);
    static_assert(!rich_traits::all_trivially_default_constructible); // std::string
    static_assert(!rich_traits::all_trivially_copyable);
    static_assert(rich_traits::any_trivially_copyable);

    static_assert(rich_traits::all_copy_constructible);
    static_assert(!rich_traits::all_nothrow_copy_constructible);
    static_assert(!rich_traits::all_trivially_copy_constructible);

    static_assert(rich_traits::all_move_constructible);
    static_assert(rich_traits::all_nothrow_move_constructible);
    static_assert(!rich_traits::all_trivially_move_constructible);

    static_assert(rich_traits::all_copy_assignable);
    static_assert(rich_traits::all_move_assignable);
    static_assert(rich_traits::all_nothrow_move_assignable);

    static_assert(rich_traits::all_destructible);
    static_assert(rich_traits::all_nothrow_destructible);
    static_assert(!rich_traits::all_trivially_destructible);

    static_assert(rich_traits::all_swappable);
    static_assert(rich_traits::all_nothrow_swappable);

    static_assert(rich_traits::all_equality_comparable);
    static_assert(rich_traits::all_less_than_comparable);

    static_assert(rich_traits::row_constructible_from<const char*&&, //
                                                      unsigned int&&,
                                                      std::tuple<int, int, int>&&,
                                                      int&&,
                                                      int*&&>);
    static_assert(!rich_traits::row_nothrow_constructible_from<const char*&&, //
                                                               unsigned int&&,
                                                               std::tuple<int, int, int>&&,
                                                               int&&,
                                                               int*&&>);
}

//----------------------------------------------------------------------------------------------------------------------
// trivial table_traits: the all-trivially-copyable fast-path traits
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    using triv_traits = trivial::table_traits;

    static_assert(triv_traits::all_trivially_default_constructible);
    static_assert(triv_traits::all_trivially_copyable);
    static_assert(triv_traits::any_trivially_copyable);
    static_assert(triv_traits::all_trivially_copy_constructible);
    static_assert(triv_traits::all_trivially_move_constructible);
    static_assert(triv_traits::all_trivially_copy_assignable);
    static_assert(triv_traits::all_trivially_move_assignable);
    static_assert(triv_traits::all_trivially_destructible);
    static_assert(triv_traits::all_nothrow_swappable);
    static_assert(triv_traits::all_equality_comparable);
    static_assert(triv_traits::all_less_than_comparable);
}

//----------------------------------------------------------------------------------------------------------------------
// move_only table_traits: copy disabled, move required
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    using mo_traits = move_only::table_traits;

    static_assert(mo_traits::all_move_constructible);
    static_assert(mo_traits::all_nothrow_move_constructible);
    static_assert(mo_traits::all_move_assignable);
    static_assert(!mo_traits::all_copy_constructible);
    static_assert(!mo_traits::all_copy_assignable);
    static_assert(mo_traits::all_destructible);
    static_assert(mo_traits::all_nothrow_destructible);

    static_assert(!std::is_copy_constructible_v<move_only>);
    static_assert(!std::is_copy_assignable_v<move_only>);
    static_assert(std::is_nothrow_move_constructible_v<move_only>);
    static_assert(std::is_nothrow_move_assignable_v<move_only>);
}

//----------------------------------------------------------------------------------------------------------------------
// soa value semantics
//----------------------------------------------------------------------------------------------------------------------

static_assert(std::is_nothrow_default_constructible_v<rich>);
static_assert(std::is_nothrow_move_constructible_v<rich>);
static_assert(std::is_nothrow_move_assignable_v<rich>);
static_assert(std::is_copy_constructible_v<rich>);
static_assert(std::is_copy_assignable_v<rich>);
static_assert(std::is_nothrow_destructible_v<rich>);
static_assert(std::is_nothrow_swappable_v<rich>);

TEST_CASE("traits - compile-time only")
{
    // the meat of this file is static_assert; this exists so the TU contributes a runtime test too
    CHECK(rich::column_count == 5);
    CHECK(trivial::column_count == 4);
    CHECK(move_only::column_count == 2);
}
