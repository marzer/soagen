#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from .configurable import Configurable
from .metavars import *
from .writer import *


class Column(Configurable):
    def __init__(self, var):
        super().__init__(var)
        self.variable = var
        self.struct = var.struct
        self.index = -1  # set by the struct
        self.name = var.name
        self.alignment = var.alignment
        self.type = var.type
        self.param_type = var.param_type
        self.pointer_type = var.pointer_type
        self.const_pointer_type = var.const_pointer_type
        self.default = var.default


__all__ = [r'Column']
