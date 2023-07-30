#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re

from . import cpp, utils
from .column import Column
from .configurable import *
from .schemas import *
from .writer import *


class Variable(Configurable):
    __schema = Schema(
        {
            r'name': Stripped(str, allow_empty=False, name=r'variable name'),
            r'type': Stripped(str, allow_empty=False, name=r'variable type'),
            Optional(r'param_type', default=''): Stripped(str),
            Optional(r'default', default=None): And(
                Or(str, float, int), Use(lambda x: x.strip() if isinstance(x, str) else str(x))
            ),
            Optional(r'alignment', default=0): And(
                Or(int, str),
                Use(int),
                lambda x: x <= 0 or utils.is_pow2(x),
                error=r'alignment must be a power-of-two integer',
            ),
        }
    )

    def __init__(self, struct, vals):
        super().__init__(struct)
        self.struct = struct
        self.index = -1  # set by the struct

        vals = Variable.__schema.validate(vals)
        self.__dict__.update(vals)

        valid = cpp.is_valid_identifier(self.name)
        if not valid[0]:
            raise SchemaError(rf"name: '{self.name}': {valid[1]}", None)

        if self.default is None:
            self.default = ''

        self.pointer_type = rf'{self.type}*'
        if re.fullmatch(r'[a-zA-Z_][a-zA-Z_0-9:]*', self.type):
            self.const_pointer_type = rf'const {self.pointer_type}'
        else:
            self.const_pointer_type = rf'std::add_const_t<{self.type}>*'

        self.columns = [Column(self)]


class StaticVariable(Configurable):
    __schema = Schema(
        {
            r'name': Stripped(str, allow_empty=False, name=r'static variable name'),
            r'type': Stripped(str, allow_empty=False, name=r'static variable type'),
            r'value': And(Or(str, float, int), Use(lambda x: x.strip() if isinstance(x, str) else str(x))),
            Optional(r'const', default=None): Or('', r'const', r'constexpr', bool),
            Optional(r'access', default=r'public'): Or('public', r'protected', r'private'),
            Optional(r'brief', default=''): Stripped(str),
        }
    )

    def __init__(self, cfg, vals):
        super().__init__(cfg)
        self.__dict__.update(StaticVariable.__schema.validate(vals))

        valid = cpp.is_valid_identifier(self.name)
        if not valid[0]:
            raise SchemaError(rf"name: '{self.name}': {valid[1]}", None)

        if not self.value:
            raise SchemaError(rf"value: cannot be blank", None)

        if self.const is None:
            self.const = 'constexpr'
        elif isinstance(self.const, bool):
            self.const = 'const' if self.const else ''

    def write(self, o: Writer):
        if self.brief:
            o(f'/// @brief {self.brief}')
        s = 'static '
        if self.const != 'constexpr' and self.value is not None:
            s += 'inline '
        if self.const != 'const':
            s += f'{self.const} '
        s += f'{self.type} '
        if self.const == 'const':
            s += f'{self.const} '
        s += self.name
        if self.value is not None:
            s += rf' = {self.value}'

        s += ';'
        o(s)


__all__ = [r'Variable', r'StaticVariable']
