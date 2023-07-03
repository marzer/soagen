#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import math
from io import StringIO

from . import utils
from .column import *
from .configurable import Configurable
from .detect_includes import *
from .metavars import *
from .row import *
from .schemas import And, Optional, Or, Schema, Stripped, Use, ValueOrArray
from .span import *
from .stripe import *
from .type_list import *
from .variable import *
from .writer import *


class Struct(Configurable):

	# yapf: disable
	__schema = Schema({
		Optional(r'prologue', default='')    : Stripped(str),
		Optional(r'header', default='')    : Stripped(str),
		Optional(r'footer', default='')    : Stripped(str),
		Optional(r'epilogue', default='')    : Stripped(str),
		Optional(r'brief', default='')    : Stripped(str),
		Optional(r'allocator', default='')    : Stripped(str),
		Optional(r'static_variables', default=list) : [ object ],
		Optional(r'variables', default=list)  : [ object ],
		Optional(r'movable', default=True)    : bool,
		Optional(r'copyable', default=True)    : bool,
		Optional(r'default_constructible', default=True)    : bool,
		Optional(r'swappable', default=True)    : bool,

		Optional(r'column_providers', default=True) : bool,
		Optional(r'stripes', default=list)   : [ object ],
		Optional(r'rows', default=True)    : bool,
		Optional(r'spans', default=True)   : bool,
		Optional(r'strong_indices', default=False) : bool,
		Optional(r'growth_factor', default=1.5)  : And(Or(int, float), Use(float), lambda x: x >= 1.0 and math.isfinite(x)),

		# additional #includes to add to any header files this struct appears in
		Optional(r'internal_includes', default=list) : And(ValueOrArray(str, name=r'internal_includes'), Use(set), Use(list), Use(utils.sort)),
		Optional(r'external_includes', default=list) : And(ValueOrArray(str, name=r'external_includes'), Use(set), Use(list), Use(utils.sort)),

		# span headers
		Optional(r'span_header', default='')  : Stripped(str),
		Optional(r'mutable_span_header', default=''): Stripped(str),
		Optional(r'const_span_header', default='')  : Stripped(str),
		Optional(r'struct_and_span_header', default='')  : Stripped(str),
		Optional(r'struct_and_mutable_span_header', default='')  : Stripped(str),

		# span footers
		Optional(r'span_footer', default='')  : Stripped(str),
		Optional(r'mutable_span_footer', default=''): Stripped(str),
		Optional(r'const_span_footer', default='')  : Stripped(str),
		Optional(r'struct_and_span_footer', default='')  : Stripped(str),
		Optional(r'struct_and_mutable_span_footer', default='')  : Stripped(str),

		# row headers
		Optional(r'row_header', default='')     : Stripped(str),
		Optional(r'lvalue_row_header', default='')   : Stripped(str),
		Optional(r'mutable_lvalue_row_header', default='') : Stripped(str),
		Optional(r'const_lvalue_row_header', default='') : Stripped(str),
		Optional(r'rvalue_row_header', default='')   : Stripped(str),
		Optional(r'mutable_row_header', default='')   : Stripped(str),
		Optional(r'const_row_header', default='')   : Stripped(str),

		# row footers
		Optional(r'row_footer', default='')     : Stripped(str),
		Optional(r'lvalue_row_footer', default='')   : Stripped(str),
		Optional(r'mutable_lvalue_row_footer', default='') : Stripped(str),
		Optional(r'const_lvalue_row_footer', default='') : Stripped(str),
		Optional(r'rvalue_row_footer', default='')   : Stripped(str),
		Optional(r'mutable_row_footer', default='')   : Stripped(str),
		Optional(r'const_row_footer', default='')   : Stripped(str)
	})
	# yapf: enable

	def __init__(self, cfg, name, vals):
		super().__init__(cfg)

		vals = Struct.__schema.validate(vals)
		self.__dict__.update(vals)

		self.name = name
		self.type = name
		self.qualified_type = rf'{self.config.namespace}::{self.type}' if self.config.namespace else self.type
		self.index = -1  # set by the config
		self.meta = MetaVars()
		self.meta.push('name', self.name)
		self.meta.push('type', self.type)
		self.meta.push('struct::name', self.name)
		self.meta.push('struct::type', self.type)
		self.meta.push('struct::scope', '')

		if not self.allocator:
			self.allocator = self.config.allocator

		self.variables = [Variable(self, v) for v in self.variables]
		for v in self.variables:
			self.meta.push(rf'{v.name}::type', v.type)
			self.meta.push(rf'{v.name}::default', v.default if v.default else '{}')

		self.columns = []
		self.double_buffered = []
		index = 0
		for v in self.variables:
			v.index = index
			index += 1
			if v.double_buffered:
				self.double_buffered.append(v)
			self.columns += v.columns

		index = 0
		for c in self.columns:
			c.index = index
			index += 1
		self.variable_types = TypeList([v.type for v in self.variables])
		self.variable_lvalues = TypeList([rf'{v.type}&' for v in self.variables])
		self.column_types = TypeList([c.type for c in self.columns])
		self.const_column_types = TypeList(["const " + c.type for c in self.columns])
		self.distinct_types = list({v.type for v in self.variables})
		self.distinct_types.sort()
		self.distinct_types = TypeList(self.distinct_types)
		self.index_types = TypeList(['size_t', 'index_type']) if self.strong_indices else TypeList('index_type')

		self.prologue = rf'''
		{self.config.all_structs_and_spans.prologue}

		{self.config.all_structs_and_mutable_spans.prologue}

		{self.config.all_structs_and_const_spans.prologue}

		{self.config.all_structs.prologue}

		{self.prologue}
		'''.strip()

		self.header = rf'''
		{self.config.all_structs_and_spans.header}

		{self.config.all_structs_and_mutable_spans.header}

		{self.config.all_structs_and_const_spans.header}

		{self.config.all_structs.header}

		{self.header}

		{self.struct_and_span_header}

		{self.struct_and_mutable_span_header}
		'''.strip()

		self.footer = rf'''
		{self.config.all_structs_and_spans.footer}

		{self.config.all_structs_and_mutable_spans.footer}

		{self.config.all_structs_and_const_spans.footer}

		{self.config.all_structs.footer}

		{self.footer}

		{self.struct_and_span_footer}

		{self.struct_and_mutable_span_footer}
		'''.strip()

		self.epilogue = rf'''
		{self.config.all_structs_and_spans.epilogue}

		{self.config.all_structs_and_mutable_spans.epilogue}

		{self.config.all_structs_and_const_spans.epilogue}

		{self.config.all_structs.epilogue}

		{self.epilogue}
		'''.strip()

		if not self.brief:
			self.brief = name

		static_vars = [v for v in self.config.all_structs_and_spans.static_variables]
		static_vars += [v for v in self.config.all_structs_and_mutable_spans.static_variables]
		static_vars += [v for v in self.config.all_structs_and_const_spans.static_variables]
		static_vars += [v for v in self.config.all_structs.static_variables]
		static_vars += [StaticVariable(self.config, v) for v in self.static_variables]
		static_vars = [(v, v.access) for v in static_vars]
		self.static_variables = {'public': [], 'protected': [], 'private': []}
		for v, access in static_vars:
			self.static_variables[access].append(v)
		assert len(self.static_variables) == 3

		if self.column_providers:
			self.column_providers = [ColumnProvider(self), ColumnProvider(self, True)]
		else:
			self.column_providers = []

		self.stripes = [StripeSet(self, s) for s in self.stripes]

		if self.rows:
			self.rows = [Row(self, '&'), Row(self, '&&'), Row(self, '&', True)]
			self.meta.push('struct::lvalue_row_type', self.rows[0].type)
			self.meta.push('struct::rvalue_row_type', self.rows[1].type)
			self.meta.push('struct::const_row_type', self.rows[2].const_type)
		else:
			self.rows = []

		if self.spans:
			self.spans = [Span(self), Span(self, True)]
			self.meta.push('struct::mutable_span_type', self.spans[0].type)
			self.meta.push('struct::const_span_type', self.spans[1].const_type)
		else:
			self.spans = []

	def set_index(self, index):
		assert self.index == -1
		assert isinstance(index, int)
		assert index >= 0
		self.index = index
		self.meta.push('index', index)
		self.meta.push('struct::index', index)

	def write_class_forward_declaration(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			o(rf'class {self.type};')

	def write_class_definition(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta) as meta:

			if self.prologue:
				o(f'''
				{self.prologue}
				''')

			if o.doxygen and self.brief:
				o(rf'''/// \brief {self.brief}''')

			with ClassDefinition(o, f'class {self.name}'):

				with Private(o):
					o(
					rf'''
					template <typename ValueType,
							typename ParamType = soagen::param_type<ValueType>,
							size_t Align		 = alignof(ValueType)>
					using make_col = soagen::column_traits<ValueType, ParamType, soagen::max(Align, alignof(ValueType))>;'''
					)

				with Public(o):
					o(
						rf'''
					using size_type = std::size_t;
					using difference_type = std::ptrdiff_t;
					using allocator_type = {self.allocator};'''
					)

					max_length = 0
					for col in self.columns:
						max_length = max(len(col.name), max_length)
					with StringIO() as buf:
						buf.write('soagen::table_traits<\n')
						for i in range(len(self.columns)):
							if i:
								buf.write(f',\n')
							col = self.columns[i]
							buf.write(f'\t/* {col.name:>{max_length}} */ make_col<{col.type}')
							if col.param_type:
								buf.write(rf', {col.param_type}')
							if col.alignment > 0:
								if not col.param_type:
									buf.write(rf', soagen::param_type<{col.type}>')
								buf.write(rf', {col.alignment}')
							buf.write(rf'>')
						buf.write(rf'>')
						o(rf'''
						using table_traits = {buf.getvalue()};
						''')

					o(
						rf'''
					template <size_type I>
					using column_traits = typename table_traits::template column<I>;

					template <size_type I>
					using column_type = typename column_traits<I>::value_type;

					static constexpr size_type aligned_stride = table_traits::aligned_stride;
					'''
					)

					with ClassDefinition(o, r'struct column_indices'):
						for col in self.columns:
							o(rf'static constexpr size_type {col.name} = {col.index};')

					o(rf'''
					template <size_type> static constexpr const char column_name[] = "";''')
					for col in self.columns:
						o(rf'template <> static constexpr const char column_name<{col.index}>[] = "{col.name}";')
					o()

					o(rf'''
					{self.header}
					''')

				for access in (r'public', r'protected', r'private'):
					if self.static_variables[access]:
						o()
						o.cpp_access_level = access
						for var in self.static_variables[access]:
							var.write(o)

				with Private(o):
					o(
						rf'''
					using table_type = soagen::table<table_traits, allocator_type>;
					table_type table_;
					'''
					)

				with Public(o):
					ctor_attrs = 'SOAGEN_NODISCARD_CTOR'
					o(
						rf'''

					{ctor_attrs if self.default_constructible else ""} {self.name}() = {"default" if self.default_constructible else "delete"};

					{ctor_attrs if self.copyable else ""} {self.name}(const {self.name}&) = {"default" if self.copyable else "delete"};

					{ctor_attrs if self.movable else ""} {self.name}({self.name}&&) = {"default" if self.movable else "delete"};

					{self.name}& operator=(const {self.name}&) = {"default" if self.copyable else "delete"};

					{self.name}& operator=({self.name}&&) = {"default" if self.movable else "delete"};

					~{self.name}() = default;

					{ctor_attrs}
					constexpr explicit {self.name}(const allocator_type& alloc) noexcept //
						: table_{{ alloc }}
					{{}}

					SOAGEN_PURE_INLINE_GETTER
					constexpr size_type size() const noexcept
					{{
						return table_.size();
					}}

					SOAGEN_PURE_INLINE_GETTER
					constexpr size_type max_size() const noexcept
					{{
						return table_.max_size();
					}}

					SOAGEN_PURE_INLINE_GETTER
					constexpr bool empty() const noexcept
					{{
						return table_.empty();
					}}

					SOAGEN_PURE_INLINE_GETTER
					constexpr size_type capacity() const noexcept
					{{
						return table_.capacity();
					}}

					SOAGEN_ALWAYS_INLINE
					SOAGEN_CPP20_CONSTEXPR
					void clear() noexcept
					{{
						return table_.clear();
					}}

					SOAGEN_ALWAYS_INLINE
					SOAGEN_CPP20_CONSTEXPR
					void reserve(size_type new_cap) noexcept(noexcept(std::declval<table_type&>().reserve(size_type{{}})))
					{{
						table_.reserve(new_cap);
					}}

					SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = soagen::has_resize_member<table_type>)
					SOAGEN_ALWAYS_INLINE
					SOAGEN_CPP20_CONSTEXPR
					void resize(size_type new_size) noexcept(soagen::has_nothrow_resize_member<table_type>)
					{{
						table_.resize(new_size);
					}}

					SOAGEN_ALWAYS_INLINE
					SOAGEN_CPP20_CONSTEXPR
					void shrink_to_fit() noexcept(noexcept(std::declval<table_type&>().shrink_to_fit()))
					{{
						table_.shrink_to_fit();
					}}

					SOAGEN_ALWAYS_INLINE
					SOAGEN_CPP20_CONSTEXPR
					void pop_back(size_type num = 1) noexcept(noexcept(std::declval<table_type&>().pop_back(size_type{{}})))
					{{
						table_.pop_back(num);
					}}

					SOAGEN_INLINE_GETTER
					constexpr allocator_type get_allocator() const noexcept
					{{
						return table_.get_allocator();
					}}

					template <size_t I>
					SOAGEN_ALIGNED_COLUMN(I)
					column_type<I>* column() noexcept
					{{
						static_assert(I < table_traits::column_count, "column index out of range");

						return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, I>>(table_.template column<I>());
					}}

					template <size_t I>
					SOAGEN_ALIGNED_COLUMN(I)
					std::add_const_t<column_type<I>>* column() const noexcept
					{{
						static_assert(I < table_traits::column_count, "column index out of range");

						return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, I>>(table_.template column<I>());
					}}

					'''
					)

					for i in range(len(self.columns)):
						o(
							rf'''
							SOAGEN_ALIGNED_COLUMN({i})
							{self.columns[i].type}* {self.columns[i].name}() noexcept
							{{
								return column<{i}>();
							}}

							SOAGEN_ALIGNED_COLUMN({i})
							std::add_const_t<{self.columns[i].type}>* {self.columns[i].name}() const noexcept
							{{
								return column<{i}>();
							}}
							'''
						)

					if self.swappable:
						o(
							rf'''
						SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = soagen::has_swap_member<table_type>)
						SOAGEN_ALWAYS_INLINE
						constexpr void swap({self.name}& other) noexcept(soagen::has_nothrow_swap_member<table_type>)
						{{
							table_.swap(other.table_);
						}}
						'''
						)

					o(rf'''
					{self.footer}
					''')

			if self.swappable:
				o(
					rf'''
				SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = soagen::has_swap_member<{self.name}>)
				SOAGEN_ALWAYS_INLINE
				constexpr void swap({self.name}& lhs, {self.name}& rhs) //
					noexcept(soagen::has_nothrow_swap_member<{self.name}>)
				{{
					lhs.swap(rhs);
				}}
				'''
				)

			if self.epilogue:
				o(f'''
				{self.epilogue}
				''')

			if 1:
				o(
					f'''
				static_assert(std::is_move_constructible_v<{self.name}> == {self.name}::table_traits::all_move_constructible);
				static_assert(std::is_move_assignable_v<{self.name}> == {self.name}::table_traits::all_move_assignable);
				static_assert(std::is_copy_constructible_v<{self.name}> == {self.name}::table_traits::all_copy_constructible);
				static_assert(std::is_copy_assignable_v<{self.name}> == {self.name}::table_traits::all_copy_assignable);
				static_assert(std::is_nothrow_destructible_v<{self.name}>);
				'''
				)

	def write_outline_member_implementations(self, o: Writer):
		pass

	def write_soagen_specializations(self, o: Writer):
		o(rf'''
		template <>
		inline constexpr bool is_soa<{self.qualified_type}> = true;
		''')


	def detect_additional_includes(self) -> list[str]:
		out = list()

		out += detect_includes(self.prologue)
		out += detect_includes(self.header)
		out += detect_includes(self.footer)
		out += detect_includes(self.epilogue)
		out += detect_includes(self.allocator)
		for col in self.columns:
			out += col.detect_additional_includes()
		for _, vars in self.static_variables.items():
			for var in vars:
				out += var.detect_additional_includes()

		return out



__all__ = [r'Struct']
