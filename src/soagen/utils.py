#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
Misc utility functions.
"""

import os
import shutil
import subprocess
import tempfile
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
        if not style_file.is_file():
            style_file = Path(paths.REPOSITORY, r'.clang-format')
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


def write_file(path, text, *, logger=None) -> bool:
    path = Path(path)
    try:
        # universal-newline + BOM-tolerant read so a CRLF checkout (git autocrlf) or a stray BOM doesn't defeat
        # the write-if-changed check and trigger a needless rewrite every build
        with open(path, encoding='utf-8-sig') as f:
            if f.read() == text:
                return False
    except FileNotFoundError:
        pass
    path.parent.mkdir(parents=True, exist_ok=True)
    if logger is not None:
        logger(rf'Writing {path}')
    # write to a sibling temp file then atomically rename, so a concurrent reader (e.g. a compiler consuming a
    # generated header mid-build) never sees a half-written file
    fd, tmp = tempfile.mkstemp(dir=path.parent, prefix=f'.{path.name}.', suffix='.tmp')
    try:
        with os.fdopen(fd, 'w', encoding='utf-8', newline='\n') as f:
            f.write(text)
        try:
            os.chmod(tmp, os.stat(path).st_mode & 0o777)  # preserve the existing file's perms (mkstemp makes 0600)
        except FileNotFoundError:
            os.chmod(tmp, 0o644)
        os.replace(tmp, path)
    except BaseException:
        try:
            os.unlink(tmp)
        except OSError:
            pass
        raise
    return True


def make_divider(text: str, text_col=40, pattern='-', line_length=120) -> str:
    text = f"//{repeat_pattern(pattern, text_col - 2)}  {text}  "
    if len(text) < line_length:
        return text + repeat_pattern(pattern, line_length - len(text))
    else:
        return text
