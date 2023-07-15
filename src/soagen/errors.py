#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT


class Error(Exception):

    """Base class for other exceptions."""

    def __init__(self, *message):
        self.__message = r' '.join([str(m) for m in message])
        super().__init__(self.__message)

    def __str__(self):
        return self.__message


__all__ = ['Error']
