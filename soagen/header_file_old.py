#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from . import utils
from .column import *
from .configurable import Configurable
from .metavars import *
from .row import Row
from .schemas import And, Optional, Schema, Stripped, Use, ValueOrArray
from .span import Span
from .stripe import StripeSet
from .writer import *



class HeaderFile_Old(Configurable):

	# yapf: disable
	__schema = Schema({
		Optional(r'header', default='')     : Stripped(str),
		Optional(r'footer', default='')     : Stripped(str),
		Optional(r'clang_format', default=True)   : bool,
		Optional(r'internal_includes', default=list) : And(ValueOrArray(str, name=r'internal_includes'), Use(set), Use(list), Use(utils.sort)),
		Optional(r'external_includes', default=list) : And(ValueOrArray(str, name=r'external_includes'), Use(set), Use(list), Use(utils.sort)),
		Optional(r'brief', default='')     : Stripped(str)
	})
	# yapf: enable

	def __init__(self, config, structs, vals, path=''):
		super().__init__(config)
		self.path = path
		if not self.path:
			self.path = self.config.path.with_suffix('.hpp')
		self.__dict__.update(HeaderFile_Old.__schema.validate(vals))

		self.structs = structs if utils.is_collection(structs) else [structs]

		# add any includes requested by member structs
		self.internal_includes = set(self.internal_includes)
		self.external_includes = set(self.external_includes)
		for struct in self.structs:
			for h in struct.internal_includes:
				self.internal_includes.add(h)
			for h in struct.external_includes:
				self.external_includes.add(h)
		self.internal_includes = list(self.internal_includes)
		self.internal_includes.sort()
		self.external_includes = list(self.external_includes)
		self.external_includes.sort()

		# includes we require for the library machinery to work
		self.library_includes = ['soagen.hpp']
		self.library_includes.sort()
		for h in self.library_includes:
			if h in self.internal_includes:
				self.internal_includes.remove(h)
			if h in self.external_includes:
				self.external_includes.remove(h)

		# documentation
		if not self.brief and self.structs:
			self.brief = ', '.join([n.qualified_type for n in self.structs])
			self.brief = rf'Contains the definition{"s" if len(self.structs) > 1 else ""} of {self.brief}.'

	def write(self, o: Writer):
		o(
			rf'''
		//{"-"*(120 - o.indent_width - 2)}
		// This file was generated by soagen - do not modify it directly
		//{"-"*(120 - o.indent_width - 2)}
		#pragma once
		'''
		)

		if self.config.documentation:
			o(rf'''
			/// \file
			/// \brief {self.brief}
			''')

		# library includes
		if self.library_includes:
			o()
			for inc in self.library_includes:
				o(rf'#include <{inc}>')

		# internal includes (#include "blah.h")
		if self.internal_includes:
			o()
			for inc in self.internal_includes:
				o(rf'#include "{inc}"')

		# external/system includes (#include <blah.h>)
		if self.external_includes:
			o('\nSOAGEN_DISABLE_WARNINGS;')
			for inc in self.external_includes:
				o(rf'#include <{inc}>')
			o(r'SOAGEN_ENABLE_WARNINGS;')

		# misc preprocessor boilerplate
		o(
			rf'''
		SOAGEN_PUSH_WARNINGS;
		SOAGEN_DISABLE_SHADOW_WARNINGS;
		SOAGEN_DISABLE_SPAM_WARNINGS;
		#if SOAGEN_MSVC
			#pragma inline_recursion(on)
		#endif
		#if SOAGEN_MSVC_LIKE
			#pragma push_macro("min")
			#pragma push_macro("max")
			#undef min
			#undef max
		#endif
		'''
		)

		# forward declarations
		with HiddenFromDoxygen(o), Namespace(o, self.config.namespace):
			if self.config.has_column_providers:
				ColumnProvider.write_template_forward_declaration(o)
			if self.config.has_stripes:
				StripeSet.write_template_forward_declaration(o)
			if self.config.has_rows:
				Row.write_template_forward_declaration(o)
			if self.config.has_spans:
				Span.write_template_forward_declaration(o)

			o(
				rf'''
			template <typename T>
			using soa_strong_index = muu::strong_typedef<size_t,
															T,
															muu::strong_typedef_traits::comparable,
															muu::strong_typedef_traits::incrementable,
															muu::strong_typedef_traits::decrementable,
															muu::strong_typedef_traits::pointer_arithmetic>;

			template <typename T>
			using soa_strong_span_index = muu::strong_typedef<size_t,
																soa_strong_span_index_tag<T>,
																muu::strong_typedef_traits::comparable,
																muu::strong_typedef_traits::incrementable,
																muu::strong_typedef_traits::decrementable,
																muu::strong_typedef_traits::pointer_arithmetic>;
			'''
			)

			if self.config.has_spans:
				o(rf'''
				template <typename T>
				using const_soa_span = soa_span<muu::add_const<T>>;
				''')

			if self.config.has_rows:
				o(
					rf'''
				template <typename T>
				using const_soa_row = soa_row<std::conditional_t<  //
					std::is_reference_v<T>,						   //
					muu::add_const<T>,							   //
					muu::add_const<std::add_lvalue_reference_t<T>> //
					>>;
				'''
				)

			for struct in self.structs:
				with MetaScope(self.config.meta_stack, struct.meta):
					o()
					struct.write_class_forward_declaration(o)

					for provider in struct.column_providers:
						provider.write_class_forward_declaration(o)

					for row in struct.rows:
						row.write_class_forward_declaration(o)

					for span in struct.spans:
						span.write_class_forward_declaration(o)

		# header
		if self.header:
			o(
				rf'''
			//{"-"*(120 - o.indent_width - 2)}
			// header
			//{"-"*(120 - o.indent_width - 2)}

			{self.header}
			'''
			)

		# structs (class definitions, then outline member implementations)
		for struct in self.structs:
			with MetaScope(self.config.meta_stack, struct.meta):
				o(
					rf'''
				//{"-"*(120 - o.indent_width - 2)}
				// {struct.name}
				//{"-"*(120 - o.indent_width - 2)}
				'''
				)
				with Namespace(o, self.config.namespace), DoxygenGroup(o, r'soa', r'Struct-of-Arrays'):
					with DoxygenGroup(o, rf'soa_{struct.name}', struct.name):
						struct.write_class_definition(o)
						o()
						for provider in struct.column_providers:
							provider.write_class_definition(o)
						for row in struct.rows:
							row.write_class_definition(o)
						for span in struct.spans:
							span.write_class_definition(o)
						o()
						struct.write_index_typedef_aliases(o)
						with HiddenFromDoxygen(o):
							struct.write_outline_member_implementations(o)

		# std specializations for soa_rows
		if self.config.has_rows:
			o(
				rf'''
			//{"-"*(120 - o.indent_width - 2)}
			// std specializations
			//{"-"*(120 - o.indent_width - 2)}
			'''
			)
			with HiddenFromDoxygen(o), Namespace(o, 'std'):
				for struct in self.structs:
					with MetaScope(self.config.meta_stack, struct.meta):
						for row in struct.rows:
							row.write_std_specializations(o)

		# footer
		if self.footer:
			o(
				rf'''
			//{"-"*(120 - o.indent_width - 2)}
			// footer
			//{"-"*(120 - o.indent_width - 2)}

			{self.footer}
			'''
			)

		o(
			rf'''
		#if SOAGEN_MSVC_LIKE
			#pragma pop_macro("min")
			#pragma pop_macro("max")
		#endif
		#if SOAGEN_MSVC
			#pragma inline_recursion(off)
		#endif
		SOAGEN_POP_WARNINGS;'''
		)
