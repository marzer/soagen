#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import datetime

from schema import And, Optional, Or, Schema, SchemaError, Use

_py2toml = {
	str: r'string',
	list: r'array',
	dict: r'table',
	int: r'integer',
	float: r'float',
	bool: r'boolean',
	datetime.date: r'date',
	datetime.time: r'time',
	datetime.datetime: r'date-time'
}



def FixedArrayOf(typ, length, name=''):
	global _py2toml
	return And([typ],
		lambda v: len(v) == length,
		error=rf'{name + ": " if name else ""}expected array of {length} {_py2toml[typ]}{"s" if length != 1 else ""}')



def ValueOrArray(typ, name='', length=None):
	global _py2toml
	inner = None
	if length is None:
		inner = Or(
			typ, [typ], error=rf'{name + ": " if name else ""}expected {_py2toml[typ]} or array of {_py2toml[typ]}s'
		)
	else:
		err = rf'{name + ": " if name else ""}expected {_py2toml[typ]} or array of {length} {_py2toml[typ]}{"s" if length != 1 else ""}'
		inner = And(Or(typ, [typ], error=err), lambda v: not isinstance(v, list) or len(v) == length, error=err)
	return And(inner, Use(lambda x: x if isinstance(x, list) else [x]))



def Stripped(typ, allow_empty=True, name=''):
	if not name:
		name = 'value'
	return And(
		And(typ, Use(lambda x: x.strip())), (lambda x: True) if allow_empty else (lambda x: len(x) > 0),
		error=rf'{name} cannot be blank'
	)
