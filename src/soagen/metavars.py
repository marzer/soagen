#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
Utility classes to build stacks of 'meta variables' for text substitution.
"""

from . import utils


class MetaVars(object):

    """
    A collection of meta-variables localized to one context (e.g. a class scope).
    """

    def __init__(self):
        self.__vars = dict()

    def push(self, name: str, val):
        assert val is not None
        assert name is not None
        assert not isinstance(name, MetaVars)
        assert not isinstance(val, MetaVars)
        if not isinstance(name, str):
            name = str(name)
        name = name.strip()
        assert name
        if not isinstance(val, str):
            val = str(val)
        if name not in self.__vars:
            self.__vars[name] = []
        self.__vars[name].append(val)
        return self

    def pop(self, name: str):
        assert name is not None
        assert not isinstance(name, MetaVars)
        if not isinstance(name, str):
            name = str(name)
        name = name.strip()
        assert name
        self.__vars[name].pop()
        return self

    def __call__(self, text, skip_names=None) -> str:
        for name, stack in self.__vars.items():
            if not stack or (skip_names and name in skip_names):
                continue
            text = utils.replace_metavar(name, stack[-1], text)
            if skip_names is not None:
                skip_names.add(name)
        return text


class MetaStack(object):

    """
    A stack of MetaVars objects.
    """

    def __init__(self):
        self.__vars = []

    def push(self, vars: MetaVars):
        assert vars is not None
        assert isinstance(vars, MetaVars)
        self.__vars.append(vars)

    def pop(self):
        if self.__vars:
            self.__vars.pop()

    def __call__(self, text: str) -> str:
        if not self.__vars:
            return text
        if len(self.__vars) == 1:
            return self.__vars[0](text)

        skip_names = set()
        for i in range(len(self.__vars) - 1, -1, -1):
            text = self.__vars[i](text, skip_names)
        return text


class MetaScope(object):

    """
    A scope that pushes a MetaVars object onto a MetaStack upon entry, and pops it upon exit.
    """

    def __init__(self, stack, vars):
        assert stack is not None
        assert isinstance(stack, MetaStack)
        assert vars is not None
        assert isinstance(vars, MetaVars)
        self.__stack = stack
        self.__stack.push(vars)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.__stack.pop()

    def __call__(self, text) -> str:
        return self.__stack(text)


__all__ = [r'MetaVars', r'MetaStack', r'MetaScope']
