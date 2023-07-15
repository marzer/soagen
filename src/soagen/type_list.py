#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT


class TypeList(object):
    def __init__(self, *types):
        self.__types = []
        if types:
            for t in types:
                self.__flattening_insert(t)

    def __flattening_insert(self, typ):
        if isinstance(typ, TypeList):
            self.__flattening_insert(typ.__types)
        elif isinstance(typ, (list, tuple, set)):
            for t in typ:
                self.__flattening_insert(t)
        else:
            assert isinstance(typ, str)
            self.__types.append(typ)

    def __str__(self):
        return r', '.join(self.__types)

    def __repr__(self):
        return repr(self.__types)

    def __add__(lhs, rhs):
        assert isinstance(lhs, TypeList)
        assert isinstance(rhs, (TypeList, list, tuple, set, str))
        return TypeList(lhs, rhs)

    def __iadd__(lhs, rhs):
        assert isinstance(lhs, TypeList)
        assert isinstance(rhs, (TypeList, list, tuple, set, str))
        lhs.__flattening_insert(rhs)
        return lhs

    def __len__(self):
        return len(self.__types)

    def __bool__(self):
        return len(self.__types) > 0

    def __iter__(self):
        for typ in self.__types:
            yield typ

    def __getitem__(self, key):
        return self.__types[key]

    def __contains__(self, val):
        assert isinstance(val, str)
        return val in self.__types

    def add_prefix(self, prefix):
        assert isinstance(prefix, str)
        if prefix:
            for i in range(len(self.__types)):
                self.__types[i] = prefix + self.__types[i]
        return self

    def add_suffix(self, suffix):
        assert isinstance(suffix, str)
        if suffix:
            for i in range(len(self.__types)):
                self.__types[i] = self.__types[i] + suffix
        return self

    def sort(self):
        self.__types.sort()
        return self
