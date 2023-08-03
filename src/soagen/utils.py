#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
Misc utility functions.
"""

import shutil
import subprocess
from pathlib import Path

from misk import *

from . import paths


def clang_format(s, cwd=None):
    cwd = Path.cwd() if cwd is None else cwd
    # respect the user's .clang-format file if one exists, but
    # fallback to the internal one (so there's always a sane default)
    style_dir = cwd
    style_file = None
    while True:
        file = Path(style_dir, r'.clang-format')
        if file.is_file():
            style_file = file
            break
        dir = style_dir.parent
        if not dir or dir == style_dir or not dir.is_dir():
            break
        style_dir = dir
    if style_file is None:
        style_file = Path(paths.HPP, r'.clang-format')
        assert style_file.is_file()

    result = subprocess.run(
        [r'clang-format', rf'--style=file:{style_file}'],
        capture_output=True,
        cwd=str(cwd),
        encoding='utf-8',
        check=True,
        input=s,
    )
    return str(result.stdout)


def is_tool(name):
    return shutil.which(name) is not None


def make_divider(text: str, text_col=40, pattern='-', line_length=120) -> str:
    text = "//{}  {}  ".format(repeat_pattern(pattern, text_col - 2), text)
    if len(text) < line_length:
        return text + repeat_pattern(pattern, line_length - len(text))
    else:
        return text
