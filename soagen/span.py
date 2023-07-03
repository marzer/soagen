#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from .configurable import Configurable
from .metavars import *
from .writer import *



class Span(Configurable):

	def __init__(self, struct, const=False):
		super().__init__(struct)
		self.struct = struct
		self.const = "const " if bool(const) else ""
		self.inv_const = "" if bool(const) else "const "
		self.column_types = struct.const_column_types if self.const else struct.column_types
		self.type = f'soa_span<{self.const}{self.struct.type}>'
		self.qualified_type = (
			rf'{self.config.namespace}::soa_span<{self.const}{self.struct.qualified_type}>'
			if self.config.namespace else self.type
		)
		self.inv_const_type = f'soa_span<{self.inv_const}{self.struct.type}>'
		self.const_type = f'const_soa_span<{self.struct.type}>'
		self.qualified_const_type = (
			rf'{self.config.namespace}::const_soa_span<{self.struct.qualified_type}>'
			if self.config.namespace else self.const_type
		)

		self.header = rf'''
		{self.config.all_structs_and_spans.header}

		{self.config.all_structs_and_mutable_spans.header if not const else ""}

		{self.config.all_structs_and_const_spans.header if const else ""}

		{self.config.all_spans.header}

		{self.config.all_mutable_spans.header if not const else ""}

		{self.config.all_const_spans.header if const else ""}

		{self.struct.span_header}

		{self.struct.mutable_span_header if not const else ""}

		{self.struct.const_span_header if const else ""}

		{self.struct.struct_and_span_header}

		{self.struct.struct_and_mutable_span_header if not const else ""}

		'''.strip()

		self.footer = rf'''
		{self.config.all_structs_and_spans.footer}

		{self.config.all_structs_and_mutable_spans.footer if not const else ""}

		{self.config.all_structs_and_const_spans.footer if const else ""}

		{self.config.all_spans.footer}

		{self.config.all_mutable_spans.footer if not const else ""}

		{self.config.all_const_spans.footer if const else ""}

		{self.struct.span_footer}

		{self.struct.mutable_span_footer if not const else ""}

		{self.struct.const_span_footer if const else ""}

		{self.struct.struct_and_span_footer}

		{self.struct.struct_and_mutable_span_footer if not const else ""}
		'''.strip()

		self.static_variables = [v for v in self.config.all_spans.static_variables]
		self.static_variables += [v for v in self.config.all_structs_and_spans.static_variables]
		if self.const:
			self.static_variables += [v for v in self.config.all_const_spans.static_variables]
			self.static_variables += [v for v in self.config.all_structs_and_const_spans.static_variables]
		else:
			self.static_variables += [v for v in self.config.all_mutable_spans.static_variables]
			self.static_variables += [v for v in self.config.all_structs_and_mutable_spans.static_variables]

		self.meta = MetaVars()
		self.meta.push('name', rf'soa_span')
		self.meta.push('type', rf'{self.type}')
		self.meta.push('struct::scope', rf'{self.struct.type}::')

	@classmethod
	def write_template_forward_declaration(cls, o: Writer):
		o(r'''template <typename>
			class soa_span;
			template <typename>
			struct soa_strong_span_index_tag;
			''')

	def write_class_forward_declaration(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			o(rf'template <> class {self.type};')

	def write_class_definition(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			o()
			if self.config.documentation:
				o(rf'/// \brief Array {self.const}subspan for {self.struct.qualified_type}.')
			o(r'template <>')
			with ClassDefinition(o, rf'class {self.type}'):
				with Private(o):
					o(
						rf'''
					friend class {self.inv_const_type};

					{self.const}{self.struct.type}* arr_ = {{}};
					size_t offset_ = {{}};
					size_t size_ = {{}};

					SOAGEN_NODISCARD_CTOR
					constexpr soa_span({self.const}{self.struct.type}* arr, size_t start, size_t count) noexcept //
						: arr_{{ arr }},
							offset_{{ start }},
							size_{{ count }}
					{{}}
					'''
					)

				with Public(o):
					o(f'''
					{self.header}
					''')

					if self.struct.strong_indices:
						o(rf'''using index_type = soa_strong_span_index<{self.struct.type}>;''')
					else:
						o(rf'''using index_type = size_t;''')

					if self.static_variables:
						o()
						for var in self.static_variables:
							var.write(o)

					o(
						rf'''
					using column_indices = {self.struct.type}::column_indices;

					using column_types = type_list<{self.column_types}>;

					template <size_t I>
					using column_type = typename column_types::template select<I>;

					SOAGEN_NODISCARD_CTOR
					constexpr soa_span() noexcept = default;

					SOAGEN_NODISCARD_CTOR
					soa_span({self.const}{self.struct.type}& arr, size_t start, size_t count) noexcept //
						: soa_span{{ &arr, start, count }}
					{{
						SOAGEN_ASSERT(offset_ < arr.size() || arr.empty());
						SOAGEN_ASSERT(offset_ + size_ <= arr.size());
					}}

					'''
					)

					if self.struct.strong_indices:
						o(
							rf'''
							SOAGEN_NODISCARD_CTOR
							soa_span({self.const}{self.struct.type}& arr, {self.struct.type}::index_type start, size_t count) noexcept //
								: soa_span{{ arr, static_cast<size_t>(start), count }}
							{{
							}}
						'''
						)

					o(
						rf'''

					SOAGEN_NODISCARD_CTOR
					/*implicit*/ soa_span({self.const}{self.struct.type}& arr) noexcept //
						: arr_{{ &arr }},
						  size_{{ arr.size() }}
					{{}}

					SOAGEN_PURE_INLINE_GETTER
					constexpr size_t size() const noexcept
					{{
						return size_;
					}}

					SOAGEN_PURE_INLINE_GETTER
					constexpr bool empty() const noexcept
					{{
						return !size_;
					}}

					SOAGEN_PURE_INLINE_GETTER
					explicit constexpr operator bool() const noexcept
					{{
						return arr_ && size_;
					}}

					SOAGEN_PURE_INLINE_GETTER
					{self.struct.type}::index_type to_absolute_index(index_type index) const noexcept
					{{
						return {self.struct.type}::index_type{{ static_cast<size_t>(index) + offset_ }};
					}}

					SOAGEN_PURE_INLINE_GETTER
					index_type to_relative_index({self.struct.type}::index_type index) const noexcept
					{{
						return index_type{{ static_cast<size_t>(index) - offset_ }};
					}}

					SOAGEN_PURE_INLINE_GETTER
					{self.const}{self.struct.type}& source() const noexcept
					{{
						assume(arr_);

						return *arr_;
					}}
					'''
					)

					if not self.const:
						o(
							rf'''
						void update(index_type index, {", ".join(self.struct.initializer_param_decls)}) noexcept({self.struct.type}::row_update_is_nothrow)
						{{
							SOAGEN_ASSERT(static_cast<size_t>(index) < size_);

							arr_->update(to_absolute_index(index), {", ".join(self.struct.initializer_passes)});
						}}

						void reset(index_type index, {", ".join(self.struct.initializer_param_decls)}) noexcept({self.struct.type}::row_update_is_nothrow)
						{{
							SOAGEN_ASSERT(static_cast<size_t>(index) < size_);

							arr_->reset(to_absolute_index(index), {", ".join(self.struct.initializer_passes)});
						}}

						SOAGEN_PURE_INLINE_GETTER
						/*implicit*/ constexpr operator soa_span<{self.inv_const}{self.struct.type}>() const noexcept;'''
						)

					with DoxygenMemberGroup(o, 'Columns'), PreprocessorRegion(o):
						o(
							rf'''
						template <size_t I>
						SOAGEN_COLUMN(I)
						column_type<I>* column() const noexcept
						{{'''
						)
						with Indent(o):
							o(
								rf'''
							static_assert(I < {len(self.struct.columns)}, "column index out of range");

							return arr_->template column<I>() + offset_;'''
							)
						o('}')

						o(
							rf'''
						template <size_t I>
						SOAGEN_PURE_INLINE_GETTER
						span<column_type<I>> column_span() const noexcept
						{{'''
						)
						with Indent(o):
							o(
								rf'''
							static_assert(I < {len(self.struct.columns)}, "column index out of range");

							return span<column_type<I>>{{ column<I>(), size_ }};'''
							)
						o('}')

						for col in self.struct.columns:
							o(
								rf'''
							SOAGEN_COLUMN({col.index})
							{self.const}{col.type}* {col.name}() const noexcept
							{{
								return column<{col.index}>();
							}}

							SOAGEN_PURE_INLINE_GETTER
							span<{self.const}{col.type}> {col.name}_span() const noexcept
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
							static_assert(I < {len(self.struct.columns)}, "column index out of range");
							static_assert(is_safe_to_memset<column_type<I>>, "the target column must be memset-safe");

							std::memset(column<I>(), 0, sizeof(column_type<I>) * size_);
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

						o(
							rf'''
						template <size_t Source, size_t Dest>
						SOAGEN_ALWAYS_INLINE
						void copy_column() noexcept
						{{
							static_assert(Source < {len(self.struct.columns)}, "source column index out of range");
							static_assert(Dest < {len(self.struct.columns)}, "destination column index out of range");
							static_assert(std::is_trivially_copyable_v<column_type<Source>>, "the source column must be trivially-copyable");
							static_assert(is_safe_to_memset<column_type<Dest>>, "the destination column must be memset-safe");
							static_assert(sizeof(column_type<Source>) == sizeof(column_type<Dest>), "source and dest types must be the same size");

							std::memcpy(column<Dest>(), column<Source>(), sizeof(column_type<Source>) * size_);
						}}
						'''
						)

					if self.struct.stripes:
						o()
						with DoxygenMemberGroup(o, 'Stripes'), PreprocessorRegion(o):
							for stripes in self.struct.stripes:
								for stripe in stripes:
									if not self.const or self.const == stripe.const:
										o(
											rf'''
										SOAGEN_PURE_INLINE_GETTER
										{stripe.type} {stripe.name}({self.struct.type}::index_type idx = {{}}) {stripe.const}noexcept'''
										)
										with Block(o):
											with Block(o, pre=rf'return {stripe.type}', post=';'):
												for col in stripe.columns:
													o(rf'column<column_indices::{col.name}>() + idx,')

					if self.struct.rows:
						with DoxygenMemberGroup(o, 'Rows'), PreprocessorRegion(o):
							for index_type in self.struct.index_types:
								o(
									rf'''
								SOAGEN_PURE_INLINE_GETTER
								soa_row<{self.const}{self.struct.type}&> operator[]({index_type} index) const noexcept;
								'''
								)
							o(
								rf'''
							SOAGEN_PURE_INLINE_GETTER
							soa_row<{self.const}{self.struct.type}&> front() const noexcept;

							SOAGEN_PURE_INLINE_GETTER
							soa_row<{self.const}{self.struct.type}&> back() const noexcept;
							'''
							)

					with DoxygenMemberGroup(o, 'Spans'), PreprocessorRegion(o):
						for index_type in self.struct.index_types:
							o(
								rf'''
							SOAGEN_PURE_INLINE_GETTER
							{self.type} subspan({index_type} start, size_t count = static_cast<size_t>(-1)) const noexcept;

							SOAGEN_PURE_INLINE_GETTER
							{self.const_type} const_subspan({index_type} start, size_t count = static_cast<size_t>(-1)) const noexcept;
							'''
							)

					o(rf'''
					{self.footer}
					''')

	def write_outline_member_implementations(self, o: Writer):
		with MetaScope(self.config.meta_stack, self.meta):
			if not self.const:
				o(
					rf'''
				SOAGEN_PURE_INLINE_GETTER
				constexpr {self.type}::operator soa_span<{self.inv_const}{self.struct.type}>() const noexcept'''
				)
				with Block(o):
					o(
						rf'''
					#if SOAGEN_USE_BIT_CAST
						return soagen::bit_cast<soa_span<{self.inv_const}{self.struct.type}>>(*this);
					#else
						return {{ arr_, offset_, size_ }};
					#endif'''
					)

			if self.struct.rows:
				for index_type in self.struct.index_types:
					o(
						rf'''
					SOAGEN_PURE_INLINE_GETTER
					soa_row<{self.const}{self.struct.type}&> {self.type}::operator[]({index_type} index) const noexcept
					{{
						return (*arr_)[offset_ + static_cast<size_t>(index)];
					}}
					'''
					)
				o(
					rf'''
				SOAGEN_PURE_INLINE_GETTER
				soa_row<{self.const}{self.struct.type}&> {self.type}::front() const noexcept
				{{
					return (*arr_)[offset_];
				}}

				SOAGEN_PURE_INLINE_GETTER
				soa_row<{self.const}{self.struct.type}&> {self.type}::back() const noexcept
				{{
					return (*arr_)[(offset_ + size_) - 1u];
				}}
				'''
				)

			for index_type in self.struct.index_types:
				o(
					rf'''
				SOAGEN_PURE_INLINE_GETTER
				{self.type} {self.type}::subspan({index_type} start, size_t count) const noexcept
				{{
					return {{ *arr_, offset_ + static_cast<size_t>(start), size_ < count ? size_ : count }};
				}}

				SOAGEN_PURE_INLINE_GETTER
				{self.const_type} {self.type}::const_subspan({index_type} start, size_t count) const noexcept
				{{
					return {{ *arr_, offset_ + static_cast<size_t>(start), size_ < count ? size_ : count }};
				}}
				'''
				)



__all__ = [r'Span']
