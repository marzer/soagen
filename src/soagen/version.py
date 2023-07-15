#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE.txt for the full license text.
# SPDX-License-Identifier: MIT

from pathlib import Path
from . import paths

VERSION = ()

with open(paths.VERSION_TXT, encoding='utf-8') as file:
    VERSION = tuple([int(v.strip()) for v in file.read().strip().split('.')])
    assert len(VERSION) == 3

VERSION_STRING = r'.'.join([str(v) for v in VERSION])

__all__ = [r'VERSION', r'VERSION_STRING']
