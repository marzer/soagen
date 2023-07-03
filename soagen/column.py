#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from .configurable import Configurable
from .metavars import *
from .writer import *



class Column(Configurable):

	def __init__(self, var, prev=False):
		super().__init__(var)
		self.variable = var
		self.struct = var.struct
		self.index = -1  # set by the struct
		self.name = 'previous_' + var.name if prev else var.name
		self.type = var.type
		self.param_type = var.param_type
		self.alignment = var.alignment

	def detect_additional_includes(self) -> list[str]:
		return self.variable.detect_additional_includes()



class ColumnProvider(Configurable):

	def __init__(self, struct, const=False):
		super().__init__(struct)
		self.struct = struct
		self.const = "const " if bool(const) else ""
		self.column_types = struct.const_column_types if self.const else struct.column_types

		self.meta = MetaVars()
		self.meta.push('name', rf'soa_columns')
		self.meta.push('type', rf'soa_columns<{self.const}{self.struct.type}, Derived>')
		self.meta.push('struct::scope', rf'{self.struct.type}::')

	@classmethod
	def write_template_forward_declaration(cls, o: Writer):
		o(r'''template <typename, typename>
		struct soa_columns;''')

	def write_class_forward_declaration(self, o: Writer):
		pass

	def write_class_definition(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			o()
			if self.config.documentation:
				o(rf'/// \brief CRTP {self.const}column provider for {self.struct.qualified_type}.')
			o(r'template <typename Derived>')
			with ClassDefinition(o, f'struct soa_columns<{self.const}{self.struct.type}, Derived>'):
				o(
					rf'''
				using column_indices = {self.struct.type}::column_indices;

				using column_types = type_list<{self.column_types}>;

				template <size_t I>
				using column_type = typename column_types::template select<I>;

				template <size_t I>
				static constexpr size_t column_alignment = {self.struct.type}::template column_alignment<I>;

				static constexpr size_t perfect_stride = {self.struct.type}::perfect_stride;'''
				)

				with DoxygenMemberGroup(o, 'Columns'), PreprocessorRegion(o):
					for col in self.struct.columns:
						for const in ('', 'const '):
							if self.const and not const:
								continue
							type = rf'column_type<{col.index}>'
							if len(type) > len(col.type):
								type = col.type
							o(
								rf'''
							SOAGEN_ALIGNED_COLUMN({col.index})
							{const}{col.type}* {col.name}() {const}noexcept
							{{
								return soagen::assume_aligned<column_alignment<{col.index}>>(
									static_cast<{const}Derived*>(this)->template column<{col.index}>()
								);
							}}

							SOAGEN_PURE_INLINE_GETTER
							span<{const}{col.type}> {col.name}_span() {const}noexcept
							{{
								return static_cast<{const}Derived*>(this)->template column_span<{col.index}>();
							}}'''
							)

					if not self.const:

						o(
							rf'''
						template <size_t I>
						SOAGEN_ALWAYS_INLINE
						void zerofill_column() noexcept
						{{
							static_assert(I < {len(self.struct.columns)}, "column index out of range");
							static_assert(is_safe_to_memset<column_type<I>>, "the target column must be memset-safe");

							std::memset(static_cast<Derived*>(this)->template column<I>(), 0, sizeof(column_type<I>) * static_cast<Derived*>(this)->size());
						}}
						'''
						)

						for col in self.struct.columns:
							o(
								rf'''
							SOAGEN_CONSTRAINED_COLUMN({col.index}, is_safe_to_memset<column_type<{col.index}>>)
							SOAGEN_ALWAYS_INLINE
							void zerofill_{col.name}() noexcept
							{{
								zerofill_column<{col.index}>();
							}}'''
							)

				if self.struct.stripes:
					o()
					with DoxygenMemberGroup(o, 'Stripes'), PreprocessorRegion(o):
						for stripes in self.struct.stripes:
							for stripe in stripes:
								if self.const and not stripe.const:
									continue
								o(
									rf'''
								SOAGEN_PURE_INLINE_GETTER
								{stripe.type} {stripe.name}({self.struct.type}::index_type idx = {{}}) {stripe.const}noexcept'''
								)
								with Block(o):
									with Block(o, pre=rf'return {stripe.type}', post=';'):
										for col in stripe.columns:
											o(
												rf'''static_cast<{stripe.const}Derived*>(this)->template column<column_indices::{col.name}>() + idx,'''
											)



__all__ = [r'Column', r'ColumnProvider']
