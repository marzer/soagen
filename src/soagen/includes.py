#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from __future__ import annotations

from . import cpp
from .configurable import *
from .schemas import *


class Includes(Configurable):
    internal: list[str]
    external: list[str]

    __schema = Schema(
        {
            Optional(r'internal', default=lambda: []): And(
                ValueOrArray(str, name=r'internal'),
                Use(lambda x: cpp.remove_implicit_includes(sorted(set([s.strip() for s in x if s])))),
            ),
            Optional(r'external', default=lambda: []): And(
                ValueOrArray(str, name=r'external'),
                Use(lambda x: cpp.remove_implicit_includes(sorted(set([s.strip() for s in x if s])))),
            ),
        }
    )

    def __init__(self, cfg, vals):
        super().__init__(cfg)

        vals = Includes.__schema.validate(vals)
        self.__dict__.update(vals)


__all__ = [r'Includes']
