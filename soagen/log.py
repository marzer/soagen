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

_min_level = logging.INFO
_treat_warnings_as_errors = False
_on_warning = None
_on_error = None
_initialized = False



def init(min_level=logging.INFO, treat_warnings_as_errors=False, on_warning=None, on_error=None):
	global _initialized
	if _initialized:
		return
	_initialized = True

	global _min_level
	_min_level = int(min_level)

	global _treat_warnings_as_errors
	_treat_warnings_as_errors = bool(treat_warnings_as_errors)

	global _on_warning
	_on_warning = on_warning

	global _on_error
	_on_error = on_error



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
			rf'{Fore.RED}error:{Style.RESET_ALL}',
			*message,
			rf'{Fore.RED}(warning treated as error){Style.RESET_ALL}',
			file=sys.stderr
		)
		if _on_error is not None:
			_on_error()
	else:
		print(rf'{Fore.YELLOW}warning:{Style.RESET_ALL}', *message, file=sys.stderr)
		if _on_warning is not None:
			_on_warning()



def e(*message):
	if _min_level > logging.ERROR:
		return
	print(rf'{Fore.RED}error:{Style.RESET_ALL}', *message, file=sys.stderr)
	if _on_error is not None:
		_on_error()
