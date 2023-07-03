#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import copy
import re
from pathlib import Path

from . import log, utils
from .configurable import ConfigBase
from .errors import Error
from .header_file import HeaderFile
from .injectors import StructInjector
from .metavars import *
from .natvis_file import NatvisFile
from .schemas import And, Optional, Schema, Stripped, Use
from .struct import Struct
from .type_list import *

try:
	import pytomlpp as toml  # fast; based on toml++ (C++)
except ImportError:
	try:
		import tomllib as toml  # PEP 680
	except ImportError:
		import tomli as toml



class Config(ConfigBase):

	# yapf: disable
	__schema = Schema({
		Optional(r'hpp', default=dict)    : { object : object },
		Optional(r'cpp', default=dict)    : { object : object },
		Optional(r'namespace', default='')   : And(
		str,
		Use(lambda x: '::'.join([part.strip(': \t') for part in x.strip(': \t').split('::')])),
		Stripped(str)
		),
		Optional(r'structs', default=dict)   : {
		And(Stripped(str, allow_empty=False, name=r'struct name'), Use(utils.make_snake_case)) : { object : object }
		},
		Optional(r'doxygen', default=True) : bool,
		Optional(r'clang_format', default=True) : bool,
		Optional(r'allocator', default=r'soagen::allocator')    : Stripped(str),
		Optional(r'all_structs', default=dict)      : { object : object },
		Optional(r'all_structs_and_spans', default=dict)   : { object : object },
		Optional(r'all_structs_and_mutable_spans', default=dict) : { object : object },
		Optional(r'all_structs_and_const_spans', default=dict)  : { object : object },
		Optional(r'all_spans', default=dict)      : { object : object },
		Optional(r'all_mutable_spans', default=dict)    : { object : object },
		Optional(r'all_const_spans', default=dict)     : { object : object },
		Optional(r'all_rows', default=dict)       : { object : object },
		Optional(r'all_lvalue_rows', default=dict)     : { object : object },
		Optional(r'all_mutable_lvalue_rows', default=dict)   : { object : object },
		Optional(r'all_const_lvalue_rows', default=dict)   : { object : object },
		Optional(r'all_rvalue_rows', default=dict)     : { object : object },
		Optional(r'all_mutable_rows', default=dict)     : { object : object },
		Optional(r'all_const_rows', default=dict)     : { object : object }
	})
	# yapf: enable

	def __init__(self, path):
		assert isinstance(path, Path)
		self.path = path.resolve()
		self.dir = path.parent
		self.meta_stack = MetaStack()
		self.meta = MetaVars()
		self.meta_stack.push(self.meta)

		text = None
		try:
			text = toml.loads(utils.read_all_text_from_file(self.path, logger=log.i))
		except Exception as err:
			log.e(err)
			return
		self.__dict__.update(Config.__schema.validate(text))

		# namespace
		if self.namespace:
			self.meta.push('namespace', self.namespace)
			self.meta.push('namespace::name', self.namespace)
			self.meta.push('namespace::start', f'namespace {self.namespace}\n{{')
			self.meta.push('namespace::end', r'}')
			self.meta.push('namespace::scope', rf'{self.namespace}::')
		else:
			self.meta.push('namespace', '')
			self.meta.push('namespace::name', '')
			self.meta.push('namespace::start', '')
			self.meta.push('namespace::end', '')
			self.meta.push('namespace::scope', '')
		self.namespace_macro_alias = re.sub(r'__+', '_', self.namespace.upper().replace('::', '_'))

		# injectors for the 'all_X' sections
		self.all_structs = StructInjector(self, self.all_structs)
		self.all_structs_and_spans = StructInjector(self, self.all_structs_and_spans)
		self.all_structs_and_mutable_spans = StructInjector(self, self.all_structs_and_mutable_spans)
		self.all_structs_and_const_spans = StructInjector(self, self.all_structs_and_const_spans)
		self.all_spans = StructInjector(self, self.all_spans)
		self.all_mutable_spans = StructInjector(self, self.all_mutable_spans)
		self.all_const_spans = StructInjector(self, self.all_const_spans)
		self.all_rows = StructInjector(self, self.all_rows)
		self.all_lvalue_rows = StructInjector(self, self.all_lvalue_rows)
		self.all_mutable_lvalue_rows = StructInjector(self, self.all_mutable_lvalue_rows)
		self.all_const_lvalue_rows = StructInjector(self, self.all_const_lvalue_rows)
		self.all_rvalue_rows = StructInjector(self, self.all_rvalue_rows)
		self.all_mutable_rows = StructInjector(self, self.all_mutable_rows)
		self.all_const_rows = StructInjector(self, self.all_const_rows)

		# structs
		self.structs = [Struct(self, k, v) for k, v in self.structs.items()]
		self.structs.sort(key=lambda s: s.name)
		self.struct_types = TypeList([s.type for s in self.structs])
		self.meta.push('struct_names', ', '.join([s.name for s in self.structs]))
		self.meta.push('struct_types', ', '.join([s.type for s in self.structs]))
		self.meta.push('qualified_struct_types', ', '.join([s.qualified_type for s in self.structs]))
		self.has_column_providers = False
		self.has_stripes = False
		self.has_rows = False
		self.has_spans = False
		self.has_strong_indices = False
		index = 0
		for struct in self.structs:
			struct.set_index(index)
			self.has_column_providers = self.has_column_providers or struct.column_providers
			self.has_stripes = self.has_stripes or struct.stripes
			self.has_rows = self.has_rows or struct.rows
			self.has_spans = self.has_spans or struct.spans
			self.has_strong_indices = self.has_strong_indices or struct.strong_indices
			index += 1

		# output file configs
		self.hpp = [HeaderFile(self, self.structs, self.hpp)]
		self.natvis = NatvisFile(self, self.structs)
		self.all_outputs = [self.natvis, *self.hpp]
		log.d('\n  -> '.join([str(self.path)] + [str(o.path) for o in self.all_outputs]))
		for o in self.all_outputs:
			if o.path.is_dir():
				raise Error(rf"invalid output '{o.path}': outputs cannot be existing directories")
			if o.path == self.path:
				raise Error(rf"invalid output '{o.path}': outputs cannot overwrite input")
			if o.path.name.lower() == r'soagen.hpp':
				raise Error(rf"invalid output '{o.path}': soagen.hpp is a reserved name")

		# includes
		for i in range(len(self.hpp)):
			log.d(rf'{self.hpp[i].path.name}.internal_includes:', self.hpp[i].internal_includes)
			log.d(rf'{self.hpp[i].path.name}.external_includes:', self.hpp[i].external_includes)



__all__ = [r'Config']
