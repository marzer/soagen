# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
In-tree PEP 517 build backend: stages the canonical root .clang-format next to the packaged headers
(src/soagen/hpp/) before delegating to setuptools, so the repository only tracks the single root copy
while the built distributions still bundle one for soagen's runtime formatting fallback.
"""

import shutil
from pathlib import Path

from setuptools import build_meta as _setuptools
from setuptools.build_meta import *  # noqa: F401, F403

_ROOT = Path(__file__).resolve().parent


def _stage_bundled_clang_format():
    src = _ROOT / r'.clang-format'
    if not src.is_file():
        return
    dst = _ROOT / r'src' / r'soagen' / r'hpp' / r'.clang-format'
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(src, dst)


def build_sdist(sdist_directory, config_settings=None):
    _stage_bundled_clang_format()
    return _setuptools.build_sdist(sdist_directory, config_settings)


def build_wheel(wheel_directory, config_settings=None, metadata_directory=None):
    _stage_bundled_clang_format()
    return _setuptools.build_wheel(wheel_directory, config_settings, metadata_directory)


def build_editable(wheel_directory, config_settings=None, metadata_directory=None):
    _stage_bundled_clang_format()
    return _setuptools.build_editable(wheel_directory, config_settings, metadata_directory)
