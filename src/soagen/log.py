#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
Logging functions.
"""

import logging
import sys

from colorama import Fore, Style
from colorama import init as colorama_init

_min_level = logging.INFO
_treat_warnings_as_errors = False
_on_warning = None
_on_error = None
_initialized = False
_styles_allowed = True

STYLE_RED = rf'{Fore.RED}{Style.BRIGHT}'
STYLE_GREEN = rf'{Fore.GREEN}{Style.BRIGHT}'
STYLE_BLUE = rf'{Fore.BLUE}{Style.BRIGHT}'
STYLE_CYAN = rf'{Fore.CYAN}{Style.BRIGHT}'
STYLE_YELLOW = rf'{Fore.YELLOW}{Style.BRIGHT}'
STYLE_MAGENTA = rf'{Fore.MAGENTA}{Style.BRIGHT}'

STYLE_RED_DIM = rf'{Fore.RED}{Style.DIM}'
STYLE_GREEN_DIM = rf'{Fore.GREEN}{Style.DIM}'
STYLE_BLUE_DIM = rf'{Fore.BLUE}{Style.DIM}'
STYLE_CYAN_DIM = rf'{Fore.CYAN}{Style.DIM}'
STYLE_YELLOW_DIM = rf'{Fore.YELLOW}{Style.DIM}'
STYLE_MAGENTA_DIM = rf'{Fore.MAGENTA}{Style.DIM}'

STYLE_RESET = rf'{Style.RESET_ALL}'


def _init(min_level: int, treat_warnings_as_errors: bool, on_warning, on_error, allow_styles=True, allow_reinit=False):
    global _initialized
    if _initialized and not bool(allow_reinit):
        return

    global _min_level
    _min_level = int(min_level)

    global _treat_warnings_as_errors
    _treat_warnings_as_errors = bool(treat_warnings_as_errors)

    global _on_warning
    _on_warning = on_warning

    global _on_error
    _on_error = on_error

    if not _initialized:
        global _styles_allowed
        _styles_allowed = bool(allow_styles)
        colorama_init(strip=not _styles_allowed)

    _initialized = True


def init(min_level=logging.INFO, treat_warnings_as_errors=False, on_warning=None, on_error=None, allow_styles=True):
    _init(
        min_level=min_level,
        treat_warnings_as_errors=treat_warnings_as_errors,
        on_warning=on_warning,
        on_error=on_error,
        allow_styles=allow_styles,
        allow_reinit=False,
    )


def reinit(min_level=logging.INFO, treat_warnings_as_errors=False, on_warning=None, on_error=None):
    _init(
        min_level=min_level,
        treat_warnings_as_errors=treat_warnings_as_errors,
        on_warning=on_warning,
        on_error=on_error,
        allow_reinit=True,
    )


def styles_allowed():
    global _styles_allowed
    return _styles_allowed


def clear_hooks():
    global _on_warning
    _on_warning = None

    global _on_error
    _on_error = None


def d(*message):
    if _min_level > logging.DEBUG:
        return
    print(*message, file=sys.stdout)


def i(*message):
    if _min_level > logging.INFO:
        return
    print(*message, file=sys.stdout)


def w(*message):
    if _min_level > logging.WARNING:
        return
    if _treat_warnings_as_errors:
        print(
            rf'{STYLE_RED}error:{STYLE_RESET}',
            *message,
            rf'{STYLE_YELLOW}(warning treated as error){STYLE_RESET}',
            file=sys.stderr,
        )
        if _on_error is not None:
            _on_error()
    else:
        print(rf'{STYLE_YELLOW}warning:{STYLE_RESET}', *message, file=sys.stderr)
        if _on_warning is not None:
            _on_warning()


def e(*message):
    if _min_level > logging.ERROR:
        return
    print(rf'{STYLE_RED}error:{STYLE_RESET}', *message, file=sys.stderr)
    if _on_error is not None:
        _on_error()


__all__ = [
    'init',
    'reinit',
    'clear_hooks',
    'styles_allowed',
    'd',
    'i',
    'w',
    'e',
    r'STYLE_RED',
    r'STYLE_GREEN',
    r'STYLE_BLUE',
    r'STYLE_CYAN',
    r'STYLE_YELLOW',
    r'STYLE_MAGENTA',
    r'STYLE_RED_DIM',
    r'STYLE_GREEN_DIM',
    r'STYLE_BLUE_DIM',
    r'STYLE_CYAN_DIM',
    r'STYLE_YELLOW_DIM',
    r'STYLE_MAGENTA_DIM',
    r'STYLE_RESET',
]
