#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import math

from . import utils
from .column import *
from .configurable import Configurable
from .metavars import *
from .row import *
from .schemas import And, Optional, Or, Schema, Stripped, Use, ValueOrArray
from .span import *
from .stripe import *
from .type_list import *
from .variable import *



class Struct_Old(Configurable):

	# yapf: disable
	__schema = Schema({
		Optional(r'pre', default='')    : Stripped(str),
		Optional(r'header', default='')    : Stripped(str),
		Optional(r'footer', default='')    : Stripped(str),
		Optional(r'post', default='')    : Stripped(str),
		Optional(r'brief', default='')    : Stripped(str),
		Optional(r'column_providers', default=True) : bool,
		Optional(r'stripes', default=list)   : [ object ],
		Optional(r'rows', default=True)    : bool,
		Optional(r'spans', default=True)   : bool,
		Optional(r'strong_indices', default=False) : bool,
		Optional(r'growth_factor', default=1.5)  : And(Or(int, float), Use(float), lambda x: x >= 1.0 and math.isfinite(x)),
		Optional(r'variables', default=list)  : [ object ],
		Optional(r'static_variables', default=list) : [ object ],

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

		vals = Struct_Old.__schema.validate(vals)
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

		self.variables = [Variable(self, v) for v in self.variables]
		for v in self.variables:
			self.meta.push(rf'{v.name}::type', v.type)
			self.meta.push(rf'{v.name}::default', v.default if v.default else '{}')

		self.columns = []
		self.initializer_param_decls = []
		self.initializer_params = []
		self.initializer_passes = []
		self.row_update_nothrow = set()
		self.double_buffered = []
		index = 0
		for v in self.variables:
			v.index = index
			index += 1
			if v.double_buffered:
				self.double_buffered.append(v)
			self.columns += v.columns
			self.initializer_param_decls.append(v.initializer_param_decl)
			self.initializer_params.append(v.initializer_param)
			self.initializer_passes.append(v.initializer_pass)
			if isinstance(v.assign_nothrow, bool):
				if not v.assign_nothrow:
					self.row_update_nothrow.add('false')
			else:
				self.row_update_nothrow.add(v.assign_nothrow)
		if len(self.row_update_nothrow):
			self.row_update_nothrow = list(self.row_update_nothrow)
			self.row_update_nothrow.sort()
			self.row_update_nothrow = ' // \n&& '.join(self.row_update_nothrow)
		else:
			self.row_update_nothrow = 'true'
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

		{self.config.all_structs_and_const_spans.header}

		{self.config.all_structs.footer}

		{self.footer}

		{self.struct_and_span_footer}

		{self.struct_and_mutable_span_footer}
		'''.strip()

		if not self.brief:
			self.brief = name

		this_static_vars = [StaticVariable(self.config, v) for v in self.static_variables]
		self.static_variables = [v for v in self.config.all_structs_and_spans.static_variables]
		self.static_variables += [v for v in self.config.all_structs_and_mutable_spans.static_variables]
		self.static_variables += [v for v in self.config.all_structs_and_const_spans.static_variables]
		self.static_variables += [v for v in self.config.all_structs.static_variables]
		self.static_variables += this_static_vars

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

			if self.pre:
				o(f'''
				{self.pre}
				''')

			if self.config.documentation and self.brief:
				o(rf'''/// \brief {self.brief}''')
			newline = "\n"
			with ClassDefinition(o, f'class {self.type}'):
				with Private(o):
					o(
						rf'''
					struct impl
					{{
						memory_pool* pool;
						std::byte* allocation;
						void* columns[{len(self.columns)}];
						size_t row_count;
						size_t row_capacity;
					}} impl_;
					'''
					)

				with Public(o):

					o(f'''
					{self.header}
					''')

					if self.strong_indices:
						o(rf'''using index_type = soa_strong_index<{self.type}>;''')
					else:
						o(rf'''using index_type = size_t;''')

					if self.static_variables:
						o()
						for var in self.static_variables:
							var.write(o)

					o(rf'''
					static constexpr size_t columns = {len(self.columns)};
					''')

					with ClassDefinition(o, r'struct column_indices'):
						for col in self.columns:
							o(rf'static constexpr size_t {col.name}	= {col.index};')

					o(
						rf'''
					using column_types				= type_list<{self.column_types}>;
					using distinct_types			= type_list<{self.distinct_types}>;

					template <size_t I>
					using column_type = typename column_types::template select<I>;

					template <size_t I>
					static constexpr size_t column_alignment = std::max(cache_line_width, alignof(column_type<I>));

					static constexpr size_t perfect_stride = soagen::lcm('''
					)
					with Indent(o):
						strides = [
							rf'soagen::lcm(column_alignment<{i}>, sizeof(column_type<{i}>)) / sizeof(column_type<{i}>)'
							for i in range(len(self.columns))
						]
						o(rf',{newline}					'.join(strides) + ');')
					o('')

					if self.spans:
						o(
							rf'''using span_type		  = soa_span<{self.type}>;
						using const_span_type = const_soa_span<{self.type}>;'''
						)

					o(
						f'''
					SOAGEN_NODISCARD_CTOR
					explicit {self.type}(memory_pool&) noexcept;

					~{self.type}() noexcept;

					{self.type}({self.type}&&) = delete;
					{self.type}& operator=({self.type}&&) = delete;
					{self.type}(const {self.type}&)		  = delete;
					{self.type}& operator=(const {self.type}&) = delete;
					'''
					)

					with DoxygenMemberGroup(o, 'Array operations'), PreprocessorRegion(o):
						if self.double_buffered:
							o('void swap_buffers() noexcept;')
						o(
							rf'''
						void reserve(size_t);
						void resize(size_t);
						void pop_back() noexcept;
						void move(index_type from, index_type to) noexcept;
						void swap(index_type, index_type) noexcept;
						void erase(index_type) noexcept;
						std::optional<index_type> unordered_erase(index_type) noexcept;
						void clear() noexcept;
						void shrink_to_fit();

						void push_back({", ".join(self.initializer_param_decls)});

						static constexpr bool row_update_is_nothrow = {self.row_update_nothrow};

						void update(index_type index, {", ".join(self.initializer_param_decls)}) noexcept(row_update_is_nothrow);

						void reset(index_type index, {", ".join(self.initializer_param_decls)}) noexcept(row_update_is_nothrow);

						SOAGEN_PURE_INLINE_GETTER
						size_t size() const noexcept
						{{
							return impl_.row_count;
						}}

						SOAGEN_CONST_GETTER
						static size_t max_size() noexcept;

						SOAGEN_CONST_GETTER
						static size_t capacity_per_mb() noexcept;

						SOAGEN_PURE_INLINE_GETTER
						size_t capacity() const noexcept
						{{
							return impl_.row_capacity;
						}}

						SOAGEN_PURE_INLINE_GETTER
						bool empty() const noexcept
						{{
							return !impl_.row_count;
						}}
						'''
						)

					o()
					with DoxygenMemberGroup(o, 'Columns'), PreprocessorRegion(o):
						for const in ('', 'const '):
							o(
								rf'''
							template <size_t I>
							SOAGEN_ALIGNED_COLUMN(I)
							{const}column_type<I>* column() {const}noexcept
							{{'''
							)
							with Indent(o):
								o(
									rf'''
								static_assert(I < {len(self.columns)}, "column index out of range");

								return soagen::assume_aligned<column_alignment<I>>(static_cast<{const}column_type<I>*>(impl_.columns[I]));'''
								)
							o('}')

							o(
								rf'''
							template <size_t I>
							SOAGEN_PURE_INLINE_GETTER
							span<{const}column_type<I>> column_span() {const}noexcept
							{{'''
							)
							with Indent(o):
								o(
									rf'''
								static_assert(I < {len(self.columns)}, "column index out of range");

								return span<{const}column_type<I>>{{ column<I>(), impl_.row_count }};'''
								)
							o('}')

						for col in self.columns:
							for const in ('', 'const '):
								type = rf'column_type<{col.index}>'
								if len(type) > len(col.type):
									type = col.type
								o(
									rf'''
								SOAGEN_ALIGNED_COLUMN({col.index})
								{const}{col.type}* {col.name}() {const}noexcept
								{{
									return column<{col.index}>();
								}}

								SOAGEN_PURE_INLINE_GETTER
								span<{const}{col.type}> {col.name}_span() {const}noexcept
								{{
									return column_span<{col.index}>();
								}}'''
								)

						o(
							rf'''
						template <size_t I>
						SOAGEN_ALWAYS_INLINE
						void zerofill_column() noexcept
						{{
							static_assert(I < {len(self.columns)}, "column index out of range");
							static_assert(is_safe_to_memset<column_type<I>>, "the target column must be memset-safe");

							std::memset(column<I>(), 0, sizeof(column_type<I>) * impl_.row_count);
						}}
						'''
						)

						for col in self.columns:
							o(
								rf'''
							SOAGEN_CONSTRAINED_COLUMN({col.index}, is_safe_to_memset<column_type<{col.index}>>)
							SOAGEN_ALWAYS_INLINE
							void zerofill_{col.name}() noexcept
							{{
								zerofill_column<{col.index}>();
							}}'''
							)

						o(
							rf'''
						template <size_t Source, size_t Dest>
						SOAGEN_ALWAYS_INLINE
						void copy_column() noexcept
						{{
							static_assert(Source < {len(self.columns)}, "source column index out of range");
							static_assert(Dest < {len(self.columns)}, "destination column index out of range");
							static_assert(std::is_trivially_copyable_v<column_type<Source>>, "the source column must be trivially-copyable");
							static_assert(is_safe_to_memset<column_type<Dest>>, "the destination column must be memset-safe");
							static_assert(sizeof(column_type<Source>) == sizeof(column_type<Dest>), "source and dest types must be the same size");

							std::memcpy(column<Dest>(), column<Source>(), sizeof(column_type<Source>) * impl_.row_count);
						}}
						'''
						)

					if self.stripes:
						o()
						with DoxygenMemberGroup(o, 'Stripes'), PreprocessorRegion(o):
							for stripes in self.stripes:
								for stripe in stripes:
									o(
										rf'''
									SOAGEN_PURE_INLINE_GETTER
									{stripe.type} {stripe.name}(index_type idx = {{}}) {stripe.const}noexcept'''
									)
									with Block(o):
										with Block(o, pre=rf'return {stripe.type}', post=';'):
											for col in stripe.columns:
												o(rf'column<column_indices::{col.name}>() + idx,')

					if self.rows:
						o()
						with DoxygenMemberGroup(o, 'Rows'), PreprocessorRegion(o):
							for index_type in self.index_types:
								for row in self.rows:
									o(
										rf'''
									SOAGEN_PURE_GETTER
									{row.type} operator[]({index_type}) {row.const} {row.ref} noexcept;
									'''
									)
							for const, ref in (('', '&'), ('', '&&'), ('const ', '&')):
								o(
									rf'''
								SOAGEN_PURE_INLINE_GETTER
								soa_row<{const}{self.type}{ref}> front() {const}{ref} noexcept;

								SOAGEN_PURE_INLINE_GETTER
								soa_row<{const}{self.type}{ref}> back() {const}{ref} noexcept;
								'''
								)

					if self.spans:
						o()
						with DoxygenMemberGroup(o, 'Spans'), PreprocessorRegion(o):
							for index_type in self.index_types:
								for span in self.spans:
									o(
										rf'''
									SOAGEN_PURE_INLINE_GETTER
									{span.type} subspan({index_type} start, size_t count = static_cast<size_t>(-1)) {span.const}noexcept;
									'''
									)
									if not span.const:
										o(
											rf'''
										SOAGEN_PURE_INLINE_GETTER
										{span.inv_const_type} const_subspan({index_type} start, size_t count = static_cast<size_t>(-1)) const noexcept;
										'''
										)

					if self.footer:
						o(rf'''
						{self.footer}''')

			if self.post:
				o(f'''
				{self.post}
				''')

	def write_index_typedef_aliases(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta) as meta:

			o()
			if self.config.documentation:
				o(rf'/// \brief Alias for {self.type}::index_type.')
			o(rf'using {self.name}_index = {self.type}::index_type;')

			if self.spans:
				o()
				if self.config.documentation:
					o(rf'/// \brief Alias for soa_span<{self.type}>::index_type.')
				o(rf'using {self.name}_span_index = soa_span<{self.type}>::index_type;')

	def write_outline_member_implementations(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			if self.columns:
				pass

			if self.rows:
				for const, ref in (('', '&'), ('', '&&'), ('const ', '&')):
					o(
						rf'''
					SOAGEN_PURE_INLINE_GETTER
					soa_row<{const}{self.type}{ref}> {self.type}::front() {const}{ref} noexcept'''
					)
					with Block(o):
						o(f'return static_cast<{const}{self.type}{ref}>(*this)[index_type{{}}];')

					o(
						rf'''
					SOAGEN_PURE_INLINE_GETTER
					soa_row<{const}{self.type}{ref}> {self.type}::back() {const}{ref} noexcept'''
					)
					with Block(o):
						o(f'return static_cast<{const}{self.type}{ref}>(*this)[index_type{{ impl_.row_count - 1u }}];')
				for row in self.rows:
					row.write_outline_member_implementations(o)

			for index_type in self.index_types:
				for span in self.spans:
					o(
						rf'''
					SOAGEN_PURE_INLINE_GETTER
					{span.type} {self.type}::subspan({index_type} start, size_t count) {span.const}noexcept
					{{
						return {{ *this, index_type{{ start }}, impl_.row_count < count ? impl_.row_count : count }};
					}}
					'''
					)
					if not span.const:
						o(
							rf'''
						SOAGEN_PURE_INLINE_GETTER
						{span.inv_const_type} {self.type}::const_subspan({index_type} start, size_t count) const noexcept
						{{
							return {{ *this, index_type{{ start }}, impl_.row_count < count ? impl_.row_count : count }};
						}}
						'''
						)
			for span in self.spans:
				span.write_outline_member_implementations(o)

	def __set_source_alias(self):
		assert self.index >= 0
		self.source_alias = rf'soa_class_{self.index}'

	def write_source_alias(self, o: Writer):
		self.__set_source_alias()
		with MetaScope(self.config.meta_stack, self.meta):
			o(rf'''
			using {self.source_alias} = {self.qualified_type};
			''')

	def write_source_traits(self, o: Writer):
		self.__set_source_alias()
		with MetaScope(self.config.meta_stack, self.meta):
			local_meta = MetaVars()
			local_meta.push('struct::scope', rf'{self.source_alias}::')
			with MetaScope(self.config.meta_stack, local_meta):
				array_joiner = ", //\n"
				o(
					rf'''
				#define {self.source_alias}_distinct_types {self.distinct_types}

				template <>
				struct soa_traits<{self.source_alias}>
				{{
					// "variable" == specified in the toml config ignoring pseudo-variables (e.g. double-buffered members)
					using variable_types = type_list<{self.variable_types}>;
					using variable_lvalues = type_list<{self.variable_lvalues}>;
					template <size_t Index>
					using variable_type							   = typename variable_types::template select<Index>;
					static constexpr auto variable_indices = std::index_sequence<{", ".join( [str(i) for i in range(len(self.variables))] ) }>{{}};
					static constexpr size_t variable_count = {len(self.variables)};
					static constexpr bool variable_double_buffered[] = {{ {", ".join([   (str(v.double_buffered).lower()) for v in self.variables])} }};
					static constexpr bool has_double_buffered_variables = {'true' if self.double_buffered else 'false'};

					// "column" == the actual columns in the data after any pseudo-members are resolved
					template <size_t Index>
					using column_type = typename {self.source_alias}::template column_type<Index>;
					using column_offsets = std::array<size_t, {len(self.columns)+1}>;
					static constexpr auto column_indices = std::index_sequence<{", ".join( [str(i) for i in range(len(self.columns))] ) }>{{}};
					static constexpr size_t column_count = {len(self.columns)};
					static constexpr size_t column_sizes[] = {{
						{array_joiner.join( [rf"sizeof(column_type<{i}>)" for i in range(len(self.columns))] ) }
					}};
					static constexpr size_t column_alignments[] = {{
						{array_joiner.join( [rf"{self.source_alias}::column_alignment<{i}>" for i in range(len(self.columns))] ) }
					}};

					static constexpr size_t variable_to_column_mapping[] = {{ {", ".join([str(v.columns[0].index) for v in self.variables])} }};

					static constexpr bool all_noncvref = ::all_noncvref<{self.source_alias}_distinct_types>;
					static constexpr bool all_default_constructible = ::all_default_constructible<{self.source_alias}_distinct_types>;
					static constexpr bool all_nothrow_default_constructible = ::all_nothrow_default_constructible<{self.source_alias}_distinct_types>;
					static constexpr bool all_trivially_default_constructible = ::all_trivially_default_constructible<{self.source_alias}_distinct_types>;
					static constexpr bool all_nothrow_movable = ::all_nothrow_movable<{self.source_alias}_distinct_types>;
					static constexpr bool all_nothrow_destructible = ::all_nothrow_destructible<{self.source_alias}_distinct_types>;
					static constexpr bool all_trivially_destructible = ::all_trivially_destructible<{self.source_alias}_distinct_types>;

					template <typename... Args>
					static constexpr bool row_constructible_from = zip_and<
						std::is_constructible,
						variable_types,
						type_list<Args...>
					>::value;

					template <typename... Args>
					static constexpr bool row_nothrow_constructible_from = zip_and<
						std::is_nothrow_constructible,
						variable_types,
						type_list<Args...>
					>::value;

					template <typename... Args>
					static constexpr bool row_assignable_from = zip_and<
						std::is_assignable,
						variable_lvalues,
						type_list<Args...>
					>::value;

					template <typename... Args>
					static constexpr bool row_nothrow_assignable_from = zip_and<
						std::is_nothrow_assignable,
						variable_lvalues,
						type_list<Args...>
					>::value;

					static constexpr long double growth_factor = {self.growth_factor};

					static_assert(all_noncvref, "SoA members may not be const, volatile, or references.");
					static_assert(all_default_constructible, "SoA members must be default-constructible.");
					static_assert(all_nothrow_movable, "SoA members must be nothrow-movable.");
					static_assert(all_nothrow_destructible, "SoA members must be nothrow-destructible.");
					static_assert(growth_factor > 1.0L, "SoA growth factors must be greater than 1.0.");
				}};
				'''
				)

	def write_source_member_implementations(self, o: Writer):
		self.__set_source_alias()
		with MetaScope(self.config.meta_stack, self.meta):
			local_meta = MetaVars()
			local_meta.push('struct::scope', rf'{self.source_alias}::')
			with MetaScope(self.config.meta_stack, local_meta):
				o(
					rf'''
				{self.source_alias}::{self.type}(memory_pool& pool) noexcept //
					: impl_{{ &pool }}
				{{}}

				{self.source_alias}::~{self.type}() noexcept
				{{
					::soa_clear<{self.source_alias}>(impl_);
					::soa_free(impl_);
				}}

				void {self.source_alias}::reserve(size_t new_capacity)
				{{
					::soa_reserve<{self.source_alias}>(impl_, new_capacity);
				}}

				void {self.source_alias}::resize(size_t new_size)
				{{
					::soa_resize<{self.source_alias}>(impl_, new_size);
				}}

				void {self.source_alias}::push_back({", ".join(self.initializer_params)})
				{{
					::soa_push_back<{self.source_alias}>(impl_, {", ".join(self.initializer_passes)});
				}}

				void {self.source_alias}::update(index_type index, {", ".join(self.initializer_params)}) noexcept({self.source_alias}::row_update_is_nothrow)
				{{
					::soa_update<{self.source_alias}>(impl_, static_cast<size_t>(index), {", ".join(self.initializer_passes)});
				}}

				void {self.source_alias}::reset(index_type index, {", ".join(self.initializer_params)}) noexcept({self.source_alias}::row_update_is_nothrow)
				{{
					::soa_reset<{self.source_alias}>(impl_, static_cast<size_t>(index), {", ".join(self.initializer_passes)});
				}}

				void {self.source_alias}::pop_back() noexcept
				{{
					::soa_pop_back<{self.source_alias}>(impl_);
				}}

				void {self.source_alias}::move(index_type from, index_type to) noexcept
				{{
					if SOAGEN_LIKELY(from != to)
						::soa_move_row<{self.source_alias}>(impl_, static_cast<size_t>(from), static_cast<size_t>(to));
				}}

				void {self.source_alias}::swap(index_type index_a, index_type index_b) noexcept
				{{
					if SOAGEN_LIKELY(index_a != index_b)
						::soa_swap_rows<{self.source_alias}>(impl_, static_cast<size_t>(index_a), static_cast<size_t>(index_b));
				}}

				void {self.source_alias}::erase(index_type index) noexcept
				{{
					::soa_erase<{self.source_alias}>(impl_, static_cast<size_t>(index));
				}}

				std::optional<{self.source_alias}::index_type> {self.source_alias}::unordered_erase(index_type index) noexcept
				{{
					if (size_t moved_index; ::soa_unordered_erase<{self.source_alias}>(impl_, static_cast<size_t>(index), moved_index))
						return index_type{{ moved_index }};
					return {{}};
				}}

				void {self.source_alias}::clear() noexcept
				{{
					::soa_clear<{self.source_alias}>(impl_);
				}}

				void {self.source_alias}::shrink_to_fit()
				{{
					::soa_shrink_to_fit<{self.source_alias}>(impl_);
				}}

				SOAGEN_CONST_GETTER
				size_t {self.source_alias}::max_size() noexcept
				{{
					static constexpr size_t val = ::soa_rows_per_allocation<{self.source_alias}>(static_cast<size_t>(-1));
					return val;
				}}

				SOAGEN_CONST_GETTER
				size_t {self.source_alias}::capacity_per_mb() noexcept
				{{
					static constexpr size_t val = ::soa_rows_per_allocation<{self.source_alias}>(1_mb);
					return val;
				}}
				'''
				)

				if self.rows:
					for index_type in self.index_types:
						o(
							rf'''
						SOAGEN_PURE_GETTER
						soa_row<const {self.source_alias}&> {self.source_alias}::operator[]({index_type} index) const& noexcept
						{{
							return ::soa_row_view<const {self.source_alias}&>(impl_, static_cast<size_t>(index));
						}}

						SOAGEN_PURE_GETTER
						soa_row<{self.source_alias}&&> {self.source_alias}::operator[]({index_type} index) && noexcept
						{{
							return ::soa_row_view<{self.source_alias}&&>(impl_, static_cast<size_t>(index));
						}}

						SOAGEN_PURE_GETTER
						soa_row<{self.source_alias}&> {self.source_alias}::operator[]({index_type} index) & noexcept
						{{
							return ::soa_row_view<{self.source_alias}&>(impl_, static_cast<size_t>(index));
						}}
						'''
						)

				if self.double_buffered:
					o(
						rf'''
					void {self.source_alias}::swap_buffers() noexcept
					{{
						for (auto i : {{ { ', '.join([rf"{v.columns[0].index}_sz" for v in self.double_buffered]) } }})
						{{
							auto temp			 = impl_.columns[i];
							impl_.columns[i]	 = impl_.columns[i + 1];
							impl_.columns[i + 1] = temp;
						}}
					}}
					'''
					)
