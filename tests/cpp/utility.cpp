// This file is a part of marzer/soagen and is subject to the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
#include <array>
#include <tuple>
#include <utility>

using namespace tests;

//----------------------------------------------------------------------------------------------------------------------
// functions.hpp - min / max / lcm / has_single_bit / assume_aligned / is_constant_evaluated
//----------------------------------------------------------------------------------------------------------------------

static_assert(soagen::min(3, 1) == 1);
static_assert(soagen::min(3, 1, 2) == 1);       // sizeof...(vals) == 1 -> else branch
static_assert(soagen::min(3, 1, 2, 0) == 0);    // sizeof...(vals) == 2 -> balanced branch
static_assert(soagen::min(5, 4, 3, 2, 1) == 1); // sizeof...(vals) == 3 -> else branch
static_assert(soagen::max(3, 1) == 3);
static_assert(soagen::max(3, 1, 2) == 3);
static_assert(soagen::max(3, 1, 9, 0) == 9);
static_assert(soagen::max(5, 4, 3, 2, 7) == 7);

static_assert(soagen::lcm(4, 6) == 12);
static_assert(soagen::lcm(4, 6, 8) == 24);
static_assert(soagen::lcm(2, 3, 4, 5) == 60);

static_assert(soagen::has_single_bit(8u));
static_assert(soagen::has_single_bit(1u));
static_assert(!soagen::has_single_bit(0u));
static_assert(!soagen::has_single_bit(6u));

namespace
{
    enum class bitflags : unsigned
    {
        one  = 1,
        four = 4,
        six  = 6
    };
}
static_assert(soagen::has_single_bit(bitflags::four)); // enum path
static_assert(!soagen::has_single_bit(bitflags::six));

static_assert(soagen::is_constant_evaluated()); // true in a constant-evaluated context

TEST_CASE("utility - is_constant_evaluated at runtime", "[utility]")
{
    CHECK(!soagen::is_constant_evaluated());
}

TEST_CASE("utility - min/max/lcm/has_single_bit at runtime", "[utility]")
{
    // runtime (non-constant) operands so the function bodies are actually executed, not folded
    volatile int a = 3, b = 1, c = 2, d = 0, e = 5;
    CHECK(soagen::min(a, b) == 1);
    CHECK(soagen::min(a, b, c) == 1);
    CHECK(soagen::min(a, b, c, d) == 0);
    CHECK(soagen::min(e, a, b, c, d) == 0);
    CHECK(soagen::max(a, b) == 3);
    CHECK(soagen::max(a, b, c) == 3);
    CHECK(soagen::max(a, b, c, e) == 5);
    CHECK(soagen::max(d, a, b, c, e) == 5);

    volatile int p = 4, q = 6, r = 8, s = 5;
    CHECK(soagen::lcm(p, q) == 12);
    CHECK(soagen::lcm(p, q, r) == 24);
    CHECK(soagen::lcm(q, p, r, s) == 120);

    volatile unsigned u = 8, v = 6;
    CHECK(soagen::has_single_bit(u));
    CHECK(!soagen::has_single_bit(v));
}

TEST_CASE("utility - assume_aligned", "[utility]")
{
    alignas(32) int buffer[8] = {};
    int* p                    = soagen::assume_aligned<32>(+buffer);
    CHECK(p == buffer);
    CHECK(reinterpret_cast<std::uintptr_t>(p) % 32u == 0u);
}

//----------------------------------------------------------------------------------------------------------------------
// tuples.hpp - is_tuple / get_from_tuple (member + ADL) / is_constructible_by_unpacking_tuple
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    // a tuple-like type providing the protocol via a member get<>() (exercises the member branch)
    struct member_tuple
    {
        int a = 11;
        int b = 22;

        template <std::size_t I>
        constexpr int get() const noexcept
        {
            return I == 0 ? a : b;
        }
    };
}

template <>
struct std::tuple_size<member_tuple> : std::integral_constant<std::size_t, 2>
{};
template <std::size_t I>
struct std::tuple_element<I, member_tuple>
{
    using type = int;
};

static_assert(soagen::is_tuple<std::tuple<int, float>>);
static_assert(soagen::is_tuple<std::pair<int, float>>);
static_assert(soagen::is_tuple<std::array<int, 3>>);
static_assert(soagen::is_tuple<member_tuple>);
static_assert(!soagen::is_tuple<int>);

static_assert(soagen::is_constructible_by_unpacking_tuple<std::pair<int, int>, std::tuple<int, int>>);
static_assert(!soagen::is_constructible_by_unpacking_tuple<std::pair<int, int>, int>);

TEST_CASE("utility - get_from_tuple member branch", "[utility]")
{
    member_tuple mt;
    CHECK(soagen::get_from_tuple<0>(mt) == 11);
    CHECK(soagen::get_from_tuple<1>(mt) == 22);
}

TEST_CASE("utility - get_from_tuple ADL branch", "[utility]")
{
    std::tuple<int, std::string> t{ 7, "hi" }; // no member get<>; resolved via std::get (ADL)
    CHECK(soagen::get_from_tuple<0>(t) == 7);
    CHECK(soagen::get_from_tuple<1>(t) == "hi");
}

//----------------------------------------------------------------------------------------------------------------------
// emplacer.hpp
//----------------------------------------------------------------------------------------------------------------------

static_assert(soagen::is_emplacer<soagen::emplacer<int, char>>);
static_assert(soagen::is_emplacer<soagen::emplacer<>>);
static_assert(!soagen::is_emplacer<int>);
static_assert(std::tuple_size<soagen::emplacer<int, char, float>>::value == 3);

TEST_CASE("utility - emplacer holds pointers to its args", "[utility]")
{
    int a    = 5;
    char b   = 'x';
    auto emp = soagen::emplacer{ a, b }; // deduction guide
    static_assert(soagen::is_emplacer<decltype(emp)>);
    CHECK(emp.ptrs[0] == &a);
    CHECK(emp.ptrs[1] == &b);
}

//----------------------------------------------------------------------------------------------------------------------
// compressed_pair.hpp
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    struct empty_t
    {};
}

static_assert(std::tuple_size<soagen::compressed_pair<int, float>>::value == 2);
static_assert(std::is_same_v<std::tuple_element_t<0, soagen::compressed_pair<int, float>>, int>);
static_assert(std::is_same_v<std::tuple_element_t<1, soagen::compressed_pair<int, float>>, float>);
static_assert(sizeof(soagen::compressed_pair<empty_t, int>) == sizeof(int)); // EBO

TEST_CASE("utility - compressed_pair accessors", "[utility]")
{
    soagen::compressed_pair<int, std::string> cp{ 42, "hello" };
    CHECK(cp.first() == 42);
    CHECK(cp.second() == "hello");
    CHECK(cp.get<0>() == 42);
    CHECK(cp.get<1>() == "hello");
    CHECK(std::as_const(cp).first() == 42);
    CHECK(std::move(cp).get<1>() == "hello");
}

TEST_CASE("utility - compressed_pair swap", "[utility]")
{
    soagen::compressed_pair<int, std::string> a{ 1, "a" };
    soagen::compressed_pair<int, std::string> b{ 2, "b" };
    a.swap(b);
    CHECK(a.first() == 2);
    CHECK(b.first() == 1);
    using std::swap;
    swap(a, b);
    CHECK(a.first() == 1);
    CHECK(b.first() == 2);
}
