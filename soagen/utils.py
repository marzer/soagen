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
from pathlib import Path

from misk import *

from . import paths



def make_snake_case(s):
	if not isinstance(s, str):
		s = str(s)
	s = s.strip()
	s = re.sub('\s+', '_', s)
	s = re.sub('__+', '_', s)
	s = re.sub('([a-z])([A-Z])', lambda m: m[1] + '_' + m[2].lower(), s)
	s = s.lower()
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
		style_file = Path(paths.DATA, r'.clang-format')
		assert style_file.is_file()

	result = subprocess.run([r'clang-format', rf'--style=file:{style_file}'],
		capture_output=True,
		cwd=str(cwd),
		encoding='utf-8',
		check=True,
		input=s)
	return str(result.stdout)



def lib_version() -> tuple[int, int, int]:
	if not hasattr(lib_version, "val"):
		with open(Path(paths.DATA, 'version.txt'), encoding='utf-8') as file:
			lib_version.val = [int(v.strip()) for v in file.read().strip().split('.')]
			assert len(lib_version.val) == 3
			lib_version.val = tuple(lib_version.val)
	return lib_version.val



def lib_version_string() -> str:
	return r'.'.join([str(v) for v in lib_version()])



def is_tool(name):
	return shutil.which(name) is not None



def sort(x):
	x.sort()
	return x



def repeat_pattern(pattern: str, count: int) -> str:
	if len(pattern) == 1:
		return pattern * count
	text = ''
	for i in range(0, count):
		text = text + pattern[i % len(pattern)]
	return text



def make_divider(text: str, text_col=40, pattern='-', line_length=120) -> str:
	text = "//{}  {}  ".format(repeat_pattern(pattern, text_col - 2), text)
	if (len(text) < line_length):
		return text + repeat_pattern(pattern, line_length - len(text))
	else:
		return text
