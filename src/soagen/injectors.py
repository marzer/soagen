#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from .configurable import Configurable
from .schemas import Optional, Schema, Stripped
from .variable import StaticVariable


class StructInjector(Configurable):
    __schema = Schema(
        {
            Optional(r'prologue', default=''): Stripped(str),
            Optional(r'header', default=''): Stripped(str),
            Optional(r'footer', default=''): Stripped(str),
            Optional(r'epilogue', default=''): Stripped(str),
            Optional(r'static_variables', default=list): [object],
        }
    )

    def __init__(self, config, vals):
        super().__init__(config)
        vals = StructInjector.__schema.validate(vals)
        self.__dict__.update(vals)

        self.static_variables = [StaticVariable(self.config, v) for v in self.static_variables]


__all__ = [r'StructInjector']
