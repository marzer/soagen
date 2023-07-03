#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from .configurable import Configurable
from .metavars import *
from .writer import *



class Row(Configurable):

	def __init__(self, struct, ref, const=False):
		super().__init__(struct)
		self.struct = struct
		self.ref = ref
		self.const = "const " if bool(const) else ""
		self.type = rf'soa_row<{self.const}{self.struct.type}{self.ref}>'
		self.qualified_type = rf'{self.config.namespace}::soa_row<{self.const}{self.struct.qualified_type}{self.ref}>' if self.config.namespace else self.type
		self.const_type = rf'const_soa_row<{self.struct.type}{self.ref if self.ref == "&&" else ""}>'
		self.conversions = []
		if not self.const:
			if self.ref == '&&':
				self.conversions.append(('/*implicit*/', '', '&'))
			elif self.ref == '&':
				self.conversions.append(('explicit', '', '&&'))
			self.conversions.append(('/*implicit*/', 'const ', '&'))

		assert self.ref in ('&', '&&')
		assert self.const in ('', 'const ')
		assert not self.const or self.ref == '&'  # we don't bother specializing const&& because it's stupid

		self.header = rf'''
		{self.config.all_rows.header}

		{self.config.all_lvalue_rows.header if self.ref == '&' else ''}

		{self.config.all_mutable_lvalue_rows.header if self.ref == '&' and not self.const else ''}

		{self.config.all_const_lvalue_rows.header if self.ref == '&' and self.const else ''}

		{self.config.all_rvalue_rows.header if self.ref == '&&' else ''}

		{self.config.all_mutable_rows.header if not self.const else ''}

		{self.config.all_const_rows.header if self.const else ''}

		{self.struct.row_header}

		{self.struct.lvalue_row_header if self.ref == '&' else ''}

		{self.struct.mutable_lvalue_row_header if self.ref == '&' and not self.const else ''}

		{self.struct.const_lvalue_row_header if self.ref == '&' and self.const else ''}

		{self.struct.rvalue_row_header if self.ref == '&&' else ''}

		{self.struct.mutable_row_header if not self.const else ''}

		{self.struct.const_row_header if self.const else ''}
		'''.strip()

		self.footer = rf'''
		{self.config.all_rows.footer}

		{self.config.all_lvalue_rows.footer if self.ref == '&' else ''}

		{self.config.all_mutable_lvalue_rows.footer if self.ref == '&' and not self.const else ''}

		{self.config.all_const_lvalue_rows.footer if self.ref == '&' and self.const else ''}

		{self.config.all_rvalue_rows.footer if self.ref == '&&' else ''}

		{self.config.all_mutable_rows.footer if not self.const else ''}

		{self.config.all_const_rows.footer if self.const else ''}

		{self.struct.row_footer}

		{self.struct.lvalue_row_footer if self.ref == '&' else ''}

		{self.struct.mutable_lvalue_row_footer if self.ref == '&' and not self.const else ''}

		{self.struct.const_lvalue_row_footer if self.ref == '&' and self.const else ''}

		{self.struct.rvalue_row_footer if self.ref == '&&' else ''}

		{self.struct.mutable_row_footer if not self.const else ''}

		{self.struct.const_row_footer if self.const else ''}
		'''.strip()

		self.static_variables = [v for v in self.config.all_rows.static_variables]
		if self.ref == '&':
			self.static_variables += [v for v in self.config.all_lvalue_rows.static_variables]
			if self.const:
				self.static_variables += [v for v in self.config.all_const_lvalue_rows.static_variables]
			else:
				self.static_variables += [v for v in self.config.all_mutable_lvalue_rows.static_variables]
		if self.ref == '&&':
			self.static_variables += [v for v in self.config.all_rvalue_rows.static_variables]
			if self.const:
				self.static_variables += [v for v in self.config.all_const_rows.static_variables]
			else:
				self.static_variables += [v for v in self.config.all_mutable_rows.static_variables]

		self.meta = MetaVars()
		self.meta.push('name', rf'soa_row')
		self.meta.push('type', rf'{self.type}')
		self.meta.push('struct::scope', rf'{self.struct.type}::')

	@classmethod
	def write_template_forward_declaration(cls, o: Writer):
		o(r'''template <typename>
			struct soa_row;''')

	def write_class_forward_declaration(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			o(rf'template <> struct {self.type};')

	def write_class_definition(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			o()
			if self.config.documentation:
				o(
					rf'/// \brief Row reference for {self.struct.qualified_type}{f" (`{self.const}{self.ref}` specialization)" if (self.const or self.ref=="&&") else ""}.'
				)
			o(rf'''template <>''')
			with ClassDefinition(o, rf'struct {self.type}'):
				qualify = lambda x: rf'{self.const}{x}{self.ref}'

				# header
				o(rf'''
				{self.header}
				''')

				# static vars
				if self.static_variables:
					o()
					for var in self.static_variables:
						var.write(o)

				# members
				for col in self.struct.columns:
					o(rf'{qualify(col.type)} {col.name};')

				# get<>()
				for const in ('', 'const '):
					if self.const and const:
						continue
					func_const = 'const ' if self.const else const
					o(
						rf'''
					template <size_t I>
					SOAGEN_PURE_INLINE_GETTER
					{const}{qualify("auto")} get() {func_const}noexcept'''
					)
					with Block(o):
						o(rf'''static_assert(I < {len(self.struct.columns)}, "element index out of range");
						''')
						for col in self.struct.columns:
							o(rf'if constexpr (I == {col.index})')
							with Indent(o):
								o(
									rf'return {f"static_cast<{const}{col.type}{self.ref}>({col.name})" if self.ref == "&&" else col.name};'
								)

				# conversions
				for explicit, const, ref in self.conversions:
					o(
						rf'''
					SOAGEN_PURE_INLINE_GETTER
					{explicit} constexpr operator soa_row<{const}{self.struct.type}{ref}>() const noexcept;'''
					)

				# footer
				o(rf'''
				{self.footer}
				''')

	def write_outline_member_implementations(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			for _, const, ref in self.conversions:
				o(
					rf'''
				SOAGEN_PURE_INLINE_GETTER
				constexpr {self.type}::operator soa_row<{const}{self.struct.type}{ref}>() const noexcept'''
				)
				with Block(o):
					o(
						rf'''
					#if SOAGEN_USE_BIT_CAST
						return soagen::bit_cast<soa_row<{const}{self.struct.type}{ref}>>(*this);
					#else'''
					)
					with Block(o, 'return ', ';'):
						for col in self.struct.columns:
							o(
								rf'''{rf"static_cast<{const}{col.type}{ref}>({col.name})" if ref == "&&" else col.name},'''
							)
					o('#endif')

	def write_std_specializations(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			o(
				rf'''
			template <>
			struct tuple_size<{self.qualified_type}> //
				: std::integral_constant<size_t, {len(self.struct.columns)}>
			{{}};

			template <size_t I>
			struct tuple_element<I, {self.qualified_type}>
			{{
				using type = {self.const} typename {self.struct.qualified_type}::template column_type<I> {self.ref};
			}};
			'''
			)



__all__ = [r'Row']
