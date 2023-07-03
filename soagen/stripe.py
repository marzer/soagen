#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from . import utils
from .configurable import Configurable
from .errors import Error
from .schemas import And, Optional, Schema, Stripped, Use
from .writer import *



class Stripe(Configurable):

	def __init__(self, struct, name, type, cols, const=False):
		super().__init__(struct)
		self.struct = struct
		self.name = name
		self.columns = cols
		self.const = 'const ' if const else ''
		self.type = rf'soa_stripe<{self.const}{type}>'



class StripeSet(Configurable):

	__schema = Schema({
		r'name': And(Stripped(str, allow_empty=False, name=r'name'), Use(utils.make_snake_case)),
		Optional(r'type', default=''): Stripped(str, allow_empty=False, name=r'type'),
		r'variables': [And(Stripped(str, allow_empty=False, name=r'variable'), Use(utils.make_snake_case))]
	})

	__scalar_to_vector_mapping = {
		'half': 'vec{}h',
		'float': 'vec{}',
		'double': 'vec{}d',
		'size_t': 'vec{}sz',
		'int32_t': 'vec{}i',
		'uint32_t': 'vec{}u'
	}

	def __init__(self, struct, vals):
		super().__init__(struct)
		self.struct = struct

		vals = StripeSet.__schema.validate(vals)
		self.__dict__.update(vals)

		self.all_same_type = False
		self.double_buffered = False
		for i in range(len(self.variables)):
			var = next((v for v in self.struct.variables if v.name == self.variables[i]), None)
			if var is None:
				raise Error(
					rf"{self.struct.name}.stripes.{self.name}: no variable named '{self.variables[i]}' in {self.struct.name}"
				)
			self.variables[i] = var
			self.double_buffered = self.double_buffered or var.double_buffered
			self.all_same_type = i == 0 or self.variables[i].type == self.variables[0].type

		if self.type == '':
			if not self.all_same_type:
				raise Error(
					rf"{self.struct.name}.stripes.{self.name}: stripe type is required when member variables are of different types"
				)
			if 2 <= len(self.variables) <= 4 and self.variables[0].type in StripeSet.__scalar_to_vector_mapping:
				self.type = StripeSet.__scalar_to_vector_mapping[self.variables[0].type].format(len(self.variables))
			else:
				self.type = rf'vector<{self.variables[0].type}, {len(self.variables)}>'

		self.__stripes = []
		for prev in (False, True):
			cols = [v.columns[1 if (prev and v.double_buffered) else 0] for v in self.variables]
			for const in (False, True):
				self.__stripes.append(
					Stripe(struct, rf'previous_{self.name}' if prev else self.name, self.type, cols, const)
				)
			if not self.double_buffered:
				break

	def __len__(self):
		return len(self.__stripes)

	def __bool__(self):
		return len(self.__stripes) > 0

	def __iter__(self):
		for s in self.__stripes:
			yield s

	def __getitem__(self, key):
		return self.__stripes[key]

	@classmethod
	def write_template_forward_declaration(cls, o: Writer):
		o(r'''template <typename>
			struct soa_stripe;''')



__all__ = [r'Stripe', r'StripeSet']
