#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re

from trieregex import TrieRegEx


def make_regex(*symbols):
    return re.compile(rf'\b(?:{TrieRegEx(*[s.strip() for s in symbols]).regex()})\b')


# yapf: disable

HEADER_MATCHERS = (
    (r'cstddef', make_regex(
    r'std::byte',
    r'std::max_align_t',
    r'std::ptrdiff_t',
    r'std::size_t',
    )),

    (r'cstdlib', make_regex(
    r'std::div_t',
    r'std::ldiv_t',
    r'std::lldiv_t',
    )),

    (r'limits', make_regex(
    r'std::float_denorm_style',
    r'std::float_round_style',
    r'std::numeric_limits',
    )),

    (r'climits', make_regex(
    r'CHAR_BIT',
    r'CHAR_MAX',
    r'CHAR_MIN',
    r'INT_MAX',
    r'INT_MIN',
    r'LLONG_MAX',
    r'LLONG_MIN',
    r'LONG_MAX',
    r'LONG_MIN',
    r'MB_LEN_MAX',
    r'SCHAR_MAX',
    r'SCHAR_MIN',
    r'SHRT_MAX',
    r'SHRT_MIN',
    r'UCHAR_MAX',
    r'UINT_MAX',
    r'ULLONG_MAX',
    r'ULONG_MAX',
    r'USHRT_MAX',
    )),

    (r'cfloat', make_regex(
    r'DBL_DECIMAL_DIG',
    r'DBL_DIG',
    r'DBL_EPSILON',
    r'DBL_HAS_SUBNORM',
    r'DBL_MANT_DIG',
    r'DBL_MAX_10_EXP',
    r'DBL_MAX_EXP',
    r'DBL_MAX',
    r'DBL_MIN_10_EXP',
    r'DBL_MIN_EXP',
    r'DBL_MIN',
    r'DBL_TRUE_MIN',
    r'DECIMAL_DIG',
    r'FLT_DECIMAL_DIG',
    r'FLT_DIG',
    r'FLT_EPSILON',
    r'FLT_EVAL_METHOD',
    r'FLT_HAS_SUBNORM',
    r'FLT_MANT_DIG',
    r'FLT_MAX_10_EXP',
    r'FLT_MAX_EXP',
    r'FLT_MAX',
    r'FLT_MIN_10_EXP',
    r'FLT_MIN_EXP',
    r'FLT_MIN',
    r'FLT_RADIX',
    r'FLT_ROUNDS',
    r'FLT_TRUE_MIN',
    r'LDBL_DECIMAL_DIG',
    r'LDBL_DIG',
    r'LDBL_EPSILON',
    r'LDBL_HAS_SUBNORM',
    r'LDBL_MANT_DIG',
    r'LDBL_MAX_10_EXP',
    r'LDBL_MAX_EXP',
    r'LDBL_MAX',
    r'LDBL_MIN_10_EXP',
    r'LDBL_MIN_EXP',
    r'LDBL_MIN',
    r'LDBL_TRUE_MIN',
    )),

    (r'cstdint', make_regex(
    r'std::int_fast16_t',
    r'std::int_fast32_t',
    r'std::int_fast64_t',
    r'std::int_fast8_t',
    r'std::int_fastN_t',
    r'std::int_least16_t',
    r'std::int_least32_t',
    r'std::int_least64_t',
    r'std::int_least8_t',
    r'std::int16_t',
    r'std::int32_t',
    r'std::int64_t',
    r'std::int8_t',
    r'std::intmax_t',
    r'std::intptr_t',
    r'std::uint_fast16_t',
    r'std::uint_fast32_t',
    r'std::uint_fast64_t',
    r'std::uint_fast8_t',
    r'std::uint_least16_t',
    r'std::uint_least32_t',
    r'std::uint_least64_t',
    r'std::uint_least8_t',
    r'std::uint16_t',
    r'std::uint32_t',
    r'std::uint64_t',
    r'std::uint8_t',
    r'std::uintmax_t',
    r'std::uintptr_t',
    r'INT_FASTN_MAX',
    r'INT_FASTN_MIN',
    r'INT_LEASTN_MAX',
    r'INT_LEASTN_MIN',
    r'INTMAX_C',
    r'INTMAX_MAX',
    r'INTMAX_MIN',
    r'INTN_C',
    r'INTN_MAX',
    r'INTN_MIN',
    r'INTPTR_MAX',
    r'INTPTR_MIN',
    r'PTRDIFF_MAX',
    r'PTRDIFF_MIN',
    r'SIG_ATOMIC_MAX',
    r'SIG_ATOMIC_MIN',
    r'SIZE_MAX',
    r'UINT_FASTN_MAX',
    r'UINT_LEASTN_MAX',
    r'UINTMAX_C',
    r'UINTMAX_MAX',
    r'UINTN_C',
    r'UINTN_MAX',
    r'UINTPTR_MAX',
    r'WCHAR_MAX',
    r'WCHAR_MIN',
    r'WINT_MAX',
    r'WINT_MIN',
    )),

    (r'stdfloat', make_regex(
    r'std::bfloat16_t',
    r'std::float128_t',
    r'std::float16_t',
    r'std::float32_t',
    r'std::float64_t',
    )),

    (r'new', make_regex(
    r'std::align_val_t',
    r'std::bad_alloc',
    r'std::bad_array_new_length',
    r'std::destroying_delete_t',
    r'std::destroying_delete',
    r'std::hardware_constructive_interference_size',
    r'std::hardware_destructive_interference_size',
    r'std::new_handler',
    r'std::nothrow_t',
    r'std::nothrow',
    )),

    (r'typeinfo', make_regex(
    r'std::bad_cast',
    r'std::bad_typeid',
    r'std::type_info',
    )),

    (r'source_location', make_regex(
    r'std::source_location',
    )),

    (r'exception', make_regex(
    r'std::bad_exception',
    r'std::exception_ptr',
    r'std::exception',
    r'std::nested_exception',
    r'std::terminate_handler',
    )),

    (r'initializer_list', make_regex(
    r'std::initializer_list',
    )),

    (r'compare', make_regex(
    r'std::common_comparison_category_t',
    r'std::common_comparison_category',
    r'std::compare_partial_order_fallback',
    r'std::compare_strong_order_fallback',
    r'std::compare_three_way_result_t',
    r'std::compare_three_way_result',
    r'std::compare_three_way',
    r'std::compare_weak_order_fallback',
    r'std::partial_order',
    r'std::partial_ordering',
    r'std::strong_order',
    r'std::strong_ordering',
    r'std::three_way_comparable_with',
    r'std::three_way_comparable',
    r'std::weak_order',
    r'std::weak_ordering',
    )),

    (r'coroutine', make_regex(
    r'std::coroutine_handle',
    r'std::coroutine_traits',
    r'std::noop_coroutine_handle',
    r'std::noop_coroutine_promise',
    r'std::noop_coroutine',
    r'std::suspend_always',
    r'std::suspend_never',
    )),

    (r'csignal', make_regex(
    r'std::sig_atomic_t',
    r'SIG_DFL',
    r'SIG_ERR',
    r'SIG_IGN',
    r'SIGABRT',
    r'SIGFPE',
    r'SIGILL',
    r'SIGINT',
    r'SIGSEGV',
    r'SIGTERM',
    )),

    (r'concepts', make_regex(
    r'std::assignable_from',
    r'std::common_reference_with',
    r'std::common_with',
    r'std::constructible_from',
    r'std::convertible_to',
    r'std::copy_constructible',
    r'std::copyable',
    r'std::default_initializable',
    r'std::derived_from',
    r'std::destructible',
    r'std::equality_comparable_with',
    r'std::equality_comparable',
    r'std::equivalence_relation',
    r'std::floating_point',
    r'std::integral',
    r'std::invocable',
    r'std::movable',
    r'std::move_constructible',
    r'std::predicate',
    r'std::regular_invocable',
    r'std::regular',
    r'std::relation',
    r'std::same_as',
    r'std::semiregular',
    r'std::signed_integral',
    r'std::strict_weak_order',
    r'std::swappable_with',
    r'std::swappable',
    r'std::totally_ordered_with',
    r'std::totally_ordered',
    r'std::unsigned_integral',
    )),

    (r'utility', make_regex(
    r'std::as_const',
    r'std::cmp_equal',
    r'std::cmp_greater_equal',
    r'std::cmp_greater',
    r'std::cmp_less_equal',
    r'std::cmp_less',
    r'std::cmp_not_equal',
    r'std::declval',
    r'std::exchange',
    r'std::forward_like',
    r'std::forward',
    r'std::in_place_index_t',
    r'std::in_place_index',
    r'std::in_place_t',
    r'std::in_place_type_t',
    r'std::in_place_type',
    r'std::in_place',
    r'std::in_range',
    r'std::integer_sequence',
    r'std::make_pair',
    r'std::move_if_noexcept',
    r'std::move',
    r'std::pair',
    r'std::piecewise_construct_t',
    r'std::piecewise_construct',
    r'std::swap',
    r'std::to_underlying',
    r'std::unreachable',
    )),

    (r'memory', make_regex(
    r'std::allocate_shared_for_overwrite',
    r'std::allocate_shared',
    r'std::allocation_result',
    r'std::allocator_arg_t',
    r'std::allocator_arg',
    r'std::allocator_traits',
    r'std::allocator',
    r'std::assume_aligned',
    r'std::bad_weak_ptr',
    r'std::construct_at',
    r'std::default_delete',
    r'std::destroy_at',
    r'std::destroy_n',
    r'std::destroy',
    r'std::enable_shared_from_this',
    r'std::inout_ptr_t',
    r'std::make_shared_for_overwrite',
    r'std::make_shared',
    r'std::make_unique_for_overwrite',
    r'std::make_unique',
    r'std::out_ptr_t',
    r'std::owner_less',
    r'std::pointer_safety',
    r'std::pointer_traits',
    r'std::raw_storage_iterator',
    r'std::shared_ptr',
    r'std::start_lifetime_as_array',
    r'std::start_lifetime_as',
    r'std::to_address',
    r'std::uninitialized_copy_n',
    r'std::uninitialized_copy',
    r'std::uninitialized_default_construct_n',
    r'std::uninitialized_default_construct',
    r'std::uninitialized_fill_n',
    r'std::uninitialized_fill',
    r'std::uninitialized_move_n',
    r'std::uninitialized_move',
    r'std::uninitialized_value_construct_n',
    r'std::uninitialized_value_construct',
    r'std::unique_ptr',
    r'std::uses_allocator_v',
    r'std::uses_allocator',
    r'std::weak_ptr',
    )),

    (r'tuple', make_regex(
    r'std::apply',
    r'std::forward_as_tuple',
    r'std::ignore',
    r'std::make_from_tuple',
    r'std::make_tuple',
    r'std::tie',
    r'std::tuple',
    )),


    (r'optional', make_regex(
    r'std::bad_optional_access',
    r'std::make_optional',
    r'std::nullopt_t',
    r'std::nullopt',
    r'std::optional',
    )),

    (r'variant', make_regex(
    r'std::bad_variant_access',
    r'std::get_if',
    r'std::holds_alternative',
    r'std::monostate',
    r'std::variant_alternative_t',
    r'std::variant_alternative',
    r'std::variant_npos',
    r'std::variant_size_v',
    r'std::variant_size',
    r'std::variant',
    r'std::visit',
    )),

    (r'array', make_regex(
    r'std::array',
    r'std::to_array',
    )),

    (r'deque', make_regex(
    r'std::deque',
    )),

    (r'forward_list', make_regex(
    r'std::forward_list',
    )),

    (r'list', make_regex(
    r'std::list',
    )),

    (r'vector', make_regex(
    r'std::vector',
    )),

    (r'map', make_regex(
    r'std::map',
    r'std::multimap',
    )),

    (r'unordered_map', make_regex(
    r'std::unordered_map',
    r'std::unordered_multimap',
    )),

    (r'flat_map', make_regex(
    r'std::flat_map',
    r'std::flat_multimap',
    )),

    (r'set', make_regex(
    r'std::set',
    r'std::multiset',
    )),

    (r'unordered_set', make_regex(
    r'std::unordered_set',
    r'std::unordered_multiset',
    )),

    (r'flat_set', make_regex(
    r'std::flat_set',
    r'std::flat_multiset',
    )),

    (r'queue', make_regex(
    r'std::queue',
    r'std::priority_queue',
    )),

    (r'stack', make_regex(
    r'std::stack',
    )),

    (r'span', make_regex(
    r'std::as_bytes',
    r'std::as_writable_bytes',
    r'std::dynamic_extent',
    r'std::span',
    )),

    (r'mdspan', make_regex(
    r'std::default_accessor',
    r'std::extents',
    r'std::layout_left',
    r'std::layout_right',
    r'std::layout_stride',
    r'std::mdspan',
    )),

    (r'string', make_regex(
    r'std::basic_string',
    r'std::literals::string_literals',
    r'std::string_literals',
    r'std::string',
    r'std::to_string',
    r'std::to_wstring',
    r'std::u16string',
    r'std::u32string',
    r'std::u8string',
    r'std::wstring',
    r'"s'
    )),

    (r'string_view', make_regex(
    r'std::basic_string_view',
    r'std::literals::string_view_literals',
    r'std::string_view_literals',
    r'std::string_view',
    r'std::u16string_view',
    r'std::u32string_view',
    r'std::u8string_view',
    r'std::wstring_view',
    r'"sv'
    )),

    (r'iterator', make_regex(
    r'std::back_insert_iterator',
    r'std::bidirectional_iterator_tag',
    r'std::common_iterator',
    r'std::contiguous_iterator_tag',
    r'std::counted_iterator',
    r'std::forward_iterator_tag',
    r'std::front_insert_iterator',
    r'std::input_iterator_tag',
    r'std::insert_iterator',
    r'std::iterator',
    r'std::move_iterator',
    r'std::move_sentinel',
    r'std::output_iterator_tag',
    r'std::random_access_iterator_tag',
    r'std::reverse_iterator',
    )),

    (r'chrono', make_regex(
    r'std::chrono::ambiguous_local_time',
    r'std::chrono::choose',
    r'std::chrono::clock_time_conversion',
    r'std::chrono::day',
    r'std::chrono::days',
    r'std::chrono::duration_values',
    r'std::chrono::duration',
    r'std::chrono::file_clock',
    r'std::chrono::gps_clock',
    r'std::chrono::hh_mm_ss',
    r'std::chrono::high_resolution_clock',
    r'std::chrono::hours',
    r'std::chrono::is_clock_v',
    r'std::chrono::is_clock',
    r'std::chrono::last_spec',
    r'std::chrono::leap_second_info',
    r'std::chrono::leap_second',
    r'std::chrono::local_info',
    r'std::chrono::local_t',
    r'std::chrono::microseconds',
    r'std::chrono::milliseconds',
    r'std::chrono::minutes',
    r'std::chrono::month_day_last',
    r'std::chrono::month_day',
    r'std::chrono::month_weekday_last',
    r'std::chrono::month_weekday',
    r'std::chrono::month',
    r'std::chrono::months',
    r'std::chrono::nanoseconds',
    r'std::chrono::nonexistent_local_time',
    r'std::chrono::seconds',
    r'std::chrono::steady_clock',
    r'std::chrono::sys_info',
    r'std::chrono::system_clock',
    r'std::chrono::tai_clock',
    r'std::chrono::time_point',
    r'std::chrono::time_zone_link',
    r'std::chrono::time_zone',
    r'std::chrono::treat_as_floating_point',
    r'std::chrono::tzdb_list',
    r'std::chrono::tzdb',
    r'std::chrono::utc_clock',
    r'std::chrono::weekday_indexed',
    r'std::chrono::weekday_last',
    r'std::chrono::weekday',
    r'std::chrono::weeks',
    r'std::chrono::year_month_day_last',
    r'std::chrono::year_month_day',
    r'std::chrono::year_month_weekday_last',
    r'std::chrono::year_month_weekday',
    r'std::chrono::year_month',
    r'std::chrono::year',
    r'std::chrono::years',
    r'std::chrono::zoned_time',
    r'std::chrono::zoned_traits',
    )),

    (r'mz/type_list.hpp', make_regex(
    r'mz::type_list',
    )),

    (r'mz/spin_mutex.hpp', make_regex(
    r'mz::spin_mutex',
    )),

    (r'mz/tagged_ptr.hpp', make_regex(
    r'mz::tagged_ptr',
    )),

    (r'mz/noexcept.hpp', make_regex(
    r'mz::add_noexcept',
    r'mz::add_noexcept_t',
    r'mz::remove_noexcept',
    r'mz::remove_noexcept_t',
    r'mz::is_noexcept',
    r'mz::is_noexcept_v',
    )),

    (r'toml++/toml.h', make_regex(
    r'toml::array_iterator',
    r'toml::array',
    r'toml::const_array_iterator',
    r'toml::const_table_iterator',
    r'toml::date_time',
    r'toml::date',
    r'toml::default_formatter',
    r'toml::format_flags',
    r'toml::inserted_type_of',
    r'toml::inserter',
    r'toml::json_formatter',
    r'toml::key',
    r'toml::node_type',
    r'toml::node_view',
    r'toml::node',
    r'toml::optional',
    r'toml::parse_error',
    r'toml::parse_result',
    r'toml::path_component_type',
    r'toml::path_component',
    r'toml::path',
    r'toml::source_index',
    r'toml::source_path_ptr',
    r'toml::source_position',
    r'toml::source_region',
    r'toml::table_iterator',
    r'toml::table',
    r'toml::time_offset',
    r'toml::time',
    r'toml::time',
    r'toml::toml_formatter',
    r'toml::value_flags',
    r'toml::value',
    r'toml::yaml_formatter',
    )),

    (r'muu/span.h', make_regex(
    r'muu::span',
    r'muu::const_span',
    )),

    (r'tcb/span.hpp', make_regex(
    r'tcb::span'
    )),

    (r'tl/optional.hpp', make_regex(
    r'tl::optional'
    )),

)

# yapf: enable


def detect_includes(text: str) -> list[str]:
    out = []
    if text is not None:
        for header, regex in HEADER_MATCHERS:
            if regex.search(text) is not None:
                out.append(header)
    return out


IMPLICIT_INCLUDES = (
    r'version',
    r'cstdint',
    r'cstddef',
    r'cstdlib',
    r'numeric',
    r'type_traits',
    r'new',
    r'utility',
    r'memory',
    r'stdexcept',
    r'iterator',
    r'soagen.hpp',
)


def remove_implicit_includes(includes: list[str]) -> list[str]:
    return [inc for inc in includes if inc not in IMPLICIT_INCLUDES]


RESERVED_CPP_KEYWORDS = (
    r'alignas',
    r'alignof',
    r'and',
    r'and_eq',
    r'asm',
    r'atomic_cancel',
    r'atomic_commit',
    r'atomic_noexcept',
    r'auto',
    r'bitand',
    r'bitor',
    r'bool',
    r'break',
    r'case',
    r'catch',
    r'char',
    r'char8_t',
    r'char16_t',
    r'char32_t',
    r'class',
    r'compl',
    r'concept',
    r'const',
    r'consteval',
    r'constexpr',
    r'constinit',
    r'const_cast',
    r'continue',
    r'co_await',
    r'co_return',
    r'co_yield',
    r'decltype',
    r'default',
    r'delete',
    r'do',
    r'double',
    r'dynamic_cast',
    r'else',
    r'enum',
    r'explicit',
    r'export',
    r'extern',
    r'false',
    r'float',
    r'for',
    r'friend',
    r'goto',
    r'if',
    r'inline',
    r'int',
    r'long',
    r'mutable',
    r'namespace',
    r'new',
    r'noexcept',
    r'not',
    r'not_eq',
    r'nullptr',
    r'operator',
    r'or',
    r'or_eq',
    r'private',
    r'protected',
    r'public',
    r'reflexpr',
    r'register',
    r'reinterpret_cast',
    r'requires',
    r'return',
    r'short',
    r'signed',
    r'sizeof',
    r'static',
    r'static_assert',
    r'static_cast',
    r'struct',
    r'switch',
    r'template',
    r'this',
    r'thread_local',
    r'throw',
    r'true',
    r'try',
    r'typedef',
    r'typeid',
    r'typename',
    r'union',
    r'unsigned',
    r'using',
    r'virtual',
    r'void',
    r'volatile',
    r'wchar_t',
    r'while',
    r'xor',
    r'xor_eq',
)
RESERVED_CPP_KEYWORDS = set(RESERVED_CPP_KEYWORDS)

RESERVED_SOAGEN = (
    # std::vector-like interface:
    r'allocator_type',
    r'assign',
    r'at',
    r'begin',
    r'capacity',
    r'cbegin',
    r'cend',
    r'clear',
    r'data',
    r'difference_type',
    r'emplace_back',
    r'emplace',
    r'empty',
    r'end',
    r'erase',
    r'get_allocator',
    r'insert',
    r'max_size',
    r'push_back',
    r'reserve',
    r'resize',
    r'shrink_to_fit',
    r'size_type',
    r'size',
    r'swap',
    r'unordered_erase',
    # table machinery:
    r'aligned_stride',
    r'column_indices',
    r'column_name',
    r'column_traits',
    r'column_type',
    r'column',
    r'column',
    r'emplacer',
    r'for_each_column',
    r'forward_type',
    r'get',
    r'param_type',
    r'row_type',
    r'row',
    r'rvalue_type',
    r'sfinae',
    r'table_',
    r'table_traits',
    r'table_type',
    r'value_type',
    r'allocation_size',
    r'allocator_traits',
    # future-proofing:
    r'column_span',
    r'const_span',
    r'copy',
    r'find',
    r'index_type',
    r'move',
    r'reset',
    r'row_view',
    r'size_bytes',
    r'span',
    r'swap_columns',
    r'update',
    r'zerofill_column',
    r'zerofill',
    # other
    r'soagen',
    r'std',
)
RESERVED_SOAGEN = set(RESERVED_SOAGEN)

VALID_IDENTIFIER = re.compile(r'^[A-Za-z][A-Za-z_0-9]*$')


def is_valid_identifier(s: str) -> tuple[bool, str]:
    if not s:
        return (False, 'may not be blank')
    if s.startswith('_'):
        return (False, 'may not begin with an underscore')
    if s.find('__') != -1:
        return (False, 'may not contain double-underscores')
    if re.fullmatch(r'^[0-9].*$', s):
        return (False, 'may not begin with a digit')
    if not VALID_IDENTIFIER.fullmatch(s):
        return (False, 'may contain only a-z, A-Z, 0-9, and underscores')
    if s in RESERVED_CPP_KEYWORDS:
        return (False, 'may not be a C++ keyword')
    if s in RESERVED_SOAGEN:
        return (False, 'reserved by soagen')
    return (True,)


__all__ = [r'detect_includes', r'remove_implicit_includes', r'is_valid_identifier']
