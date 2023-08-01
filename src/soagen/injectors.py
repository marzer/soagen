#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from . import utils
from .configurable import Configurable
from .schemas import *
from .variable import StaticVariable


class StructInjector(Configurable):
    __schema = Schema(
        {
            Optional(r'annotations', default=[]): And(
                ValueOrArray(str, name=r'annotations'),
                Use(lambda x: utils.remove_duplicates([s.strip() for s in x if s.strip()])),
            ),
            Optional(r'attributes', default=[]): And(
                ValueOrArray(str, name=r'attributes'),
                Use(lambda x: utils.remove_duplicates([s.strip() for s in x if s.strip()])),
            ),
            Optional(r'epilogue', default=''): Stripped(str),
            Optional(r'footer', default=''): Stripped(str),
            Optional(r'header', default=''): Stripped(str),
            Optional(r'prologue', default=''): Stripped(str),
            Optional(r'static_variables', default=list): [object],
        }
    )

    def __init__(self, config, vals):
        super().__init__(config)
        vals = StructInjector.__schema.validate(vals)
        self.__dict__.update(vals)

        self.static_variables = [StaticVariable(self.config, v) for v in self.static_variables]


__all__ = [r'StructInjector']
