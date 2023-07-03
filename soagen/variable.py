#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re

from . import log, utils
from .column import Column
from .configurable import Configurable
from .detect_includes import *
from .schemas import And, Optional, Or, Schema, Stripped, Use
from .writer import *



class Variable(Configurable):

	# yapf: disable
	__schema = Schema({
		r'name': And(Stripped(str, allow_empty=False, name=r'variable name'), Use(utils.make_snake_case)),
		r'type': Stripped(str, allow_empty=False, name=r'variable type'),
		Optional(r'double_buffered', default=False): bool,
		Optional(r'param_type', default=''): Stripped(str),
		Optional(r'brief', default=''): Stripped(str),
		Optional(r'default', default=''): Stripped(str),
		Optional(r'alignment', default=0)  : And(Or(int, str), Use(int), lambda x: x <= 0 or utils.is_pow2(x), error=r'alignment must be a power-of-two integer'),
	})
	# yapf: enable

	def __init__(self, struct, vals):
		super().__init__(struct)
		self.struct = struct
		self.index = -1  # set by the struct

		vals = Variable.__schema.validate(vals)
		self.__dict__.update(vals)

		self.columns = [Column(self)]
		if self.double_buffered:
			self.columns.append(Column(self, True))

	def detect_additional_includes(self) -> list[str]:
		return detect_includes(self.type) + detect_includes(self.param_type)



class StaticVariable(Configurable):

	__schema = Schema({
		'name': And(Stripped(str, allow_empty=False, name='static variable name'), Use(utils.make_snake_case)),
		'type': Stripped(str, allow_empty=False, name='static variable type'),
		Optional('value', default=None): Or(str, int, float, bool),
		Optional('const', default='constexpr'): Or('', 'const', 'constexpr', bool),
		Optional('access', default='public'): Or('public', 'protected', 'private'),
		Optional('brief', default=''): Stripped(str),
	})

	def __init__(self, cfg, vals):
		super().__init__(cfg)
		self.__dict__.update(StaticVariable.__schema.validate(vals))
		if isinstance(self.const, bool):
			self.const = 'constexpr' if self.const else ''

	def write(self, o: Writer):
		if self.brief:
			o(f'/// \brief {self.brief}')
		s = 'static '
		if self.const != 'constexpr' and self.value is not None:
			s += 'inline '
		if self.const:
			s += f'{self.const} '
		s += f'{self.type} {self.name}'
		if self.value is not None:
			s += ' = '
			if isinstance(self.value, bool):
				s += 'true' if self.value else 'false'
			elif isinstance(self.value, str):
				s += f"{self.value}"
			else:
				s += str(self.value)
		s += ';'
		o(s)

	def detect_additional_includes(self) -> list[str]:
		return detect_includes(self.type)



__all__ = [r'Variable', r'StaticVariable']
