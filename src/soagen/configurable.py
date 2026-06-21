#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from typing import TYPE_CHECKING, cast

if TYPE_CHECKING:
    from .config import Config


class ConfigBase:
    pass


class Configurable:
    def __init__(self, cfg):
        assert cfg is not None

        if isinstance(cfg, ConfigBase):
            self.__cfg = cfg
            return

        getter = getattr(cfg, 'config', None)
        c = getter() if callable(getter) else getter
        self.__cfg = c if isinstance(c, ConfigBase) else None

    @property
    def config(self) -> 'Config':
        return cast('Config', self.__cfg)
