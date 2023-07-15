#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
Misc utility functions.
"""

import re
import shutil
import subprocess
import textwrap
from pathlib import Path

from misk import *

from . import paths


def to_snake_case(s: str) -> str:
    s = str(s)
    s = s.strip()
    s = re.sub('\s+', '_', s)
    s = re.sub('__+', '_', s)
    s = re.sub('([a-z])([A-Z])', lambda m: m[1] + '_' + m[2].lower(), s)
    s = s.lower()
    return s


def to_pascal_case(s: str) -> str:
    s = to_snake_case(s)
    s = re.sub('_([a-z])', lambda m: m[1].upper(), s)
    s = s.strip('_')
    if len(s):
        s = s[:1].upper() + s[1:]
    return s


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


def repeat_pattern(pattern: str, count: int) -> str:
    if len(pattern) == 1:
        return pattern * count
    text = ''
    for i in range(0, count):
        text = text + pattern[i % len(pattern)]
    return text


def make_divider(text: str, text_col=40, pattern='-', line_length=120) -> str:
    text = "//{}  {}  ".format(repeat_pattern(pattern, text_col - 2), text)
    if len(text) < line_length:
        return text + repeat_pattern(pattern, line_length - len(text))
    else:
        return text


def reflow_text(text: str, line_length=120, tab_size=4) -> str:
    text = text.replace('\n\n', '\b')
    text = text.replace('\n', ' ')
    text = text.replace('\b', '\n')
    text = text.split('\n')
    for i in range(len(text)):
        text[i] = '\n'.join(textwrap.wrap(text[i], width=int(line_length), tabsize=int(tab_size)))
    return '\n\n'.join(text)
