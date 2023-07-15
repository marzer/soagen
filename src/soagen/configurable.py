#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT


class ConfigBase(object):
    pass


class Configurable(object):
    def __init__(self, cfg):
        assert cfg is not None

        if isinstance(cfg, ConfigBase):
            self.__cfg = cfg
            return

        try:
            c = cfg.config()
            if isinstance(c, ConfigBase):
                self.__cfg = c
                return
        except:
            pass

        try:
            c = cfg.config
            if isinstance(c, ConfigBase):
                self.__cfg = c
                return
        except:
            pass

        self.__cfg = None

    @property
    def config(self) -> ConfigBase:
        return self.__cfg
