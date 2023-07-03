#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re

from trieregex import TrieRegEx



def _make_regex_trie(*type_names):
	return re.compile(rf'\b(?:{TrieRegEx(*type_names).regex()})\b')



def _make_std_regex_trie(*type_names):
	return _make_regex_trie(*[rf'std::{n}' for n in type_names])



# yapf: disable

HEADER_MATCHERS = (
	(r'cstddef', _make_std_regex_trie(r'size_t', r'ptrdiff_t', r'max_align_t', r'byte')),
	(r'cstdlib', _make_std_regex_trie(r'div_t', r'ldiv_t', r'lldiv_t')),
	(r'limits', _make_std_regex_trie(r'numeric_limits', r'float_round_style', r'float_denorm_style')),

	(r'climits', _make_regex_trie(r'CHAR_BIT',
	r'MB_LEN_MAX',
	r'CHAR_MIN',
	r'CHAR_MAX',
	r'SCHAR_MIN',
	r'SHRT_MIN',
	r'INT_MIN',
	r'LONG_MIN',
	r'LLONG_MIN',
	r'SCHAR_MAX',
	r'SHRT_MAX',
	r'INT_MAX',
	r'LONG_MAX',
	r'LLONG_MAX',
	r'UCHAR_MAX',
	r'USHRT_MAX',
	r'UINT_MAX',
	r'ULONG_MAX',
	r'ULLONG_MAX')),

	(r'cfloat', _make_regex_trie(r'FLT_ROUNDS',
	r'FLT_EVAL_METHOD',
	r'FLT_HAS_SUBNORM',
	r'DBL_HAS_SUBNORM',
	r'LDBL_HAS_SUBNORM',
	r'FLT_RADIX',
	r'FLT_MANT_DIG',
	r'DBL_MANT_DIG',
	r'LDBL_MANT_DIG',
	r'FLT_DECIMAL_DIG',
	r'DBL_DECIMAL_DIG',
	r'LDBL_DECIMAL_DIG',
	r'DECIMAL_DIG',
	r'FLT_DIG',
	r'DBL_DIG',
	r'LDBL_DIG',
	r'FLT_MIN_EXP',
	r'DBL_MIN_EXP',
	r'LDBL_MIN_EXP',
	r'FLT_MIN_10_EXP',
	r'DBL_MIN_10_EXP',
	r'LDBL_MIN_10_EXP',
	r'FLT_MAX_EXP',
	r'DBL_MAX_EXP',
	r'LDBL_MAX_EXP',
	r'FLT_MAX_10_EXP',
	r'DBL_MAX_10_EXP',
	r'LDBL_MAX_10_EXP',
	r'FLT_MAX',
	r'DBL_MAX',
	r'LDBL_MAX',
	r'FLT_EPSILON',
	r'DBL_EPSILON',
	r'LDBL_EPSILON',
	r'FLT_MIN',
	r'DBL_MIN',
	r'LDBL_MIN',
	r'FLT_TRUE_MIN',
	r'DBL_TRUE_MIN',
	r'LDBL_TRUE_MIN')),

	(r'cstdint', _make_std_regex_trie(
	r'int8_t',
	r'int16_t',
	r'int32_t',
	r'int64_t',
	r'int_fast8_t',
	r'int_fast16_t',
	r'int_fast32_t',
	r'int_fast64_t',
	r'int_fastN_t',
	r'int_least8_t',
	r'int_least16_t',
	r'int_least32_t',
	r'int_least64_t',
	r'intmax_t',
	r'intptr_t',
	r'uint8_t',
	r'uint16_t',
	r'uint32_t',
	r'uint64_t',
	r'uint_fast8_t',
	r'uint_fast16_t',
	r'uint_fast32_t',
	r'uint_fast64_t',
	r'uint_least8_t',
	r'uint_least16_t',
	r'uint_least32_t',
	r'uint_least64_t',
	r'uintmax_t',
	r'uintptr_t'
	)),

	(r'cstdint', _make_regex_trie(
	r'INTN_MIN',
	r'INTN_MAX',
	r'UINTN_MAX',
	r'INT_FASTN_MIN',
	r'INT_FASTN_MAX',
	r'UINT_FASTN_MAX',
	r'INT_LEASTN_MIN',
	r'INT_LEASTN_MAX',
	r'UINT_LEASTN_MAX',
	r'INTMAX_MIN',
	r'INTMAX_MAX',
	r'UINTMAX_MAX',
	r'INTPTR_MIN',
	r'INTPTR_MAX',
	r'UINTPTR_MAX',
	r'PTRDIFF_MIN',
	r'PTRDIFF_MAX',
	r'SIZE_MAX',
	r'SIG_ATOMIC_MIN',
	r'SIG_ATOMIC_MAX',
	r'WCHAR_MIN',
	r'WCHAR_MAX',
	r'WINT_MIN',
	r'WINT_MAX',
	r'INTN_C',
	r'UINTN_C',
	r'INTMAX_C',
	r'UINTMAX_C'
	)),

	(r'stdfloat', _make_std_regex_trie(
	r'float16_t',
	r'float32_t',
	r'float64_t',
	r'float128_t',
	r'bfloat16_t'
	)),

	(r'new', _make_std_regex_trie(
	r'bad_alloc',
	r'bad_array_new_length',
	r'nothrow_t',
	r'align_val_t',
	r'destroying_delete_t',
	r'new_handler',
	r'nothrow',
	r'destroying_delete',
	r'hardware_destructive_interference_size',
	r'hardware_constructive_interference_size'
	)),

	(r'typeinfo', _make_std_regex_trie(
	r'type_info',
	r'bad_typeid',
	r'bad_cast'
	)),

	(r'source_location', _make_std_regex_trie(
	r'source_location'
	)),

	(r'exception', _make_std_regex_trie(
	r'exception',
	r'exception_ptr',
	r'bad_exception',
	r'nested_exception',
	r'terminate_handler'
	)),

	(r'initializer_list', _make_std_regex_trie(
	r'initializer_list'
	)),

	(r'compare', _make_std_regex_trie(
	r'three_way_comparable',
	r'three_way_comparable_with',
	r'partial_ordering',
	r'weak_ordering',
	r'strong_ordering',
	r'common_comparison_category',
	r'common_comparison_category_t',
	r'compare_three_way_result',
	r'compare_three_way_result_t',
	r'compare_three_way',
	r'strong_order',
	r'weak_order',
	r'partial_order',
	r'compare_strong_order_fallback',
	r'compare_weak_order_fallback',
	r'compare_partial_order_fallback',
	)),

	(r'coroutine', _make_std_regex_trie(
	r'coroutine_traits',
	r'coroutine_handle',
	r'noop_coroutine_promise',
	r'noop_coroutine_handle',
	r'suspend_never',
	r'suspend_always',
	r'noop_coroutine'
	)),

	(r'csignal', _make_std_regex_trie(
	r'sig_atomic_t'
	)),

	(r'csignal', _make_regex_trie(
	r'SIG_DFL',
	r'SIG_ERR',
	r'SIG_IGN',
	r'SIGABRT',
	r'SIGFPE',
	r'SIGILL',
	r'SIGINT',
	r'SIGSEGV',
	r'SIGTERM'
	)),

	(r'concepts', _make_std_regex_trie(
	r'same_as',
	r'derived_from',
	r'convertible_to',
	r'common_reference_with',
	r'common_with',
	r'integral',
	r'signed_integral',
	r'unsigned_integral',
	r'floating_point',
	r'assignable_from',
	r'swappable',
	r'swappable_with',
	r'destructible',
	r'constructible_from',
	r'default_initializable',
	r'move_constructible',
	r'copy_constructible',
	r'equality_comparable',
	r'equality_comparable_with',
	r'totally_ordered',
	r'totally_ordered_with',
	r'movable',
	r'copyable',
	r'semiregular',
	r'regular',
	r'invocable',
	r'regular_invocable',
	r'predicate',
	r'relation',
	r'equivalence_relation',
	r'strict_weak_order'
	)),

	(r'utility', _make_std_regex_trie(
	r'pair',
	r'integer_sequence',
	r'swap',
	r'exchange',
	r'forward',
	r'forward_like',
	r'move',
	r'move_if_noexcept',
	r'as_const',
	r'declval',
	r'to_underlying',
	r'cmp_equal',
	r'cmp_not_equal',
	r'cmp_less',
	r'cmp_greater',
	r'cmp_less_equal',
	r'cmp_greater_equal',
	r'in_range',
	r'unreachable',
	r'make_pair',
	r'piecewise_construct_t',
	r'piecewise_construct',
	r'in_place',
	r'in_place_type',
	r'in_place_index',
	r'in_place_t',
	r'in_place_type_t',
	r'in_place_index_t'
	)),

	(r'memory', _make_std_regex_trie(
	r'allocate_shared_for_overwrite',
	r'allocate_shared',
	r'allocation_result',
	r'allocator_arg_t',
	r'allocator_arg',
	r'allocator_traits',
	r'allocator',
	r'assume_aligned',
	r'bad_weak_ptr',
	r'construct_at',
	r'default_delete',
	r'destroy_at',
	r'destroy_n',
	r'destroy',
	r'enable_shared_from_this',
	r'inout_ptr_t',
	r'make_shared_for_overwrite',
	r'make_shared',
	r'make_unique_for_overwrite',
	r'make_unique',
	r'out_ptr_t',
	r'owner_less',
	r'pointer_safety',
	r'pointer_traits',
	r'raw_storage_iterator',
	r'shared_ptr',
	r'start_lifetime_as_array',
	r'start_lifetime_as',
	r'to_address',
	r'uninitialized_copy_n',
	r'uninitialized_copy',
	r'uninitialized_default_construct_n',
	r'uninitialized_default_construct',
	r'uninitialized_fill_n',
	r'uninitialized_fill',
	r'uninitialized_move_n',
	r'uninitialized_move',
	r'uninitialized_value_construct_n',
	r'uninitialized_value_construct',
	r'unique_ptr',
	r'uses_allocator_v',
	r'uses_allocator',
	r'weak_ptr'
	)),

	(r'tuple', _make_std_regex_trie(
	r'tuple',
	r'ignore',
	r'make_tuple',
	r'tie',
	r'forward_as_tuple',
	r'apply',
	r'make_from_tuple'
	)),


	(r'optional', _make_std_regex_trie(
	r'optional',
	r'bad_optional_access',
	r'nullopt_t',
	r'nullopt',
	r'make_optional'
	)),

	(r'variant', _make_std_regex_trie(
	r'variant',
	r'monostate',
	r'bad_variant_access',
	r'variant_size',
	r'variant_size_v',
	r'variant_alternative',
	r'variant_alternative_t',
	r'variant_npos',
	r'visit',
	r'holds_alternative',
	r'get_if'
	)),

	(r'array', _make_std_regex_trie(
	r'array',
	r'to_array'
	)),

	(r'deque', _make_std_regex_trie(
	r'deque'
	)),

	(r'forward_list', _make_std_regex_trie(
	r'forward_list'
	)),

	(r'list', _make_std_regex_trie(
	r'list'
	)),

	(r'vector', _make_std_regex_trie(
	r'vector'
	)),

	(r'map', _make_std_regex_trie(
	r'map',
	r'multimap'
	)),

	(r'unordered_map', _make_std_regex_trie(
	r'unordered_map',
	r'unordered_multimap'
	)),

	(r'flat_map', _make_std_regex_trie(
	r'flat_map',
	r'flat_multimap'
	)),

	(r'set', _make_std_regex_trie(
	r'set',
	r'multiset'
	)),

	(r'unordered_set', _make_std_regex_trie(
	r'unordered_set',
	r'unordered_multiset'
	)),

	(r'flat_set', _make_std_regex_trie(
	r'flat_set',
	r'flat_multiset'
	)),

	(r'queue', _make_std_regex_trie(
	r'queue',
	r'priority_queue'
	)),

	(r'stack', _make_std_regex_trie(
	r'stack'
	)),

	(r'span', _make_std_regex_trie(
	r'span',
	r'dynamic_extent',
	r'as_bytes',
	r'as_writable_bytes'
	)),

	(r'mdspan', _make_std_regex_trie(
	r'mdspan',
	r'extents',
	r'layout_left',
	r'layout_right',
	r'layout_stride',
	r'default_accessor'
	)),

	(r'string', _make_std_regex_trie(
	r'basic_string',
	r'string',
	r'wstring',
	r'u8string',
	r'u16string',
	r'u32string',
	r'to_string',
	r'to_wstring'
	)),

	(r'string_view', _make_std_regex_trie(
	r'basic_string_view',
	r'string_view',
	r'wstring_view',
	r'u8string_view',
	r'u16string_view',
	r'u32string_view'
	)),

)

# yapf: enable



def detect_includes(text: str) -> list[str]:
	out = list()
	for header, regex in HEADER_MATCHERS:
		if regex.search(text) is not None:
			out.append(header)
	return out



__all__ = [r'detect_includes']
