#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re
from io import StringIO
from pathlib import Path

from . import log, utils

# -----------------------------------------------------------------------------------------------------------------------
# code file writer object
# -----------------------------------------------------------------------------------------------------------------------


class Writer(object):
    def __init__(self, path, meta=None, indent='\t', clang_format=False, doxygen=True, on_flush=None):
        assert isinstance(path, Path)
        assert isinstance(indent, str)
        self.__path = path.resolve()
        self.__buffer = StringIO()
        self.__indent = 0
        self.__indent_str = str(indent)
        self.__indent_width = len(utils.untabify(self.__indent_str))
        self.__meta = meta
        self.__clang_format = bool(clang_format)
        self.__doxygen = bool(doxygen)
        self.__cpp_access_level = None
        self.__on_flush = on_flush
        self.emit_access_levels = True

    def write(self, text=None, end='\n', reindent=True):
        if text is None:
            text = ''
        elif not isinstance(text, str):
            text = str(text)
        if end is None:
            end = ''
        if end or text:
            if text:
                text = utils.replace_metavar(r'current_indent', self.__indent_str * self.__indent, text)
            if text and reindent:
                text = utils.reindent(text, indent=self.__indent_str * self.__indent)
            text += end
            if self.__meta is not None:
                text = self.__meta(text)
            self.__buffer.write(text)
        return self

    def __call__(self, text=None, **kwargs):
        return self.write(text=text, **kwargs)

    @property
    def indent(self) -> int:
        return self.__indent

    @indent.setter
    def indent(self, value: int):
        self.__indent = int(value)
        assert self.__indent >= 0

    @property
    def indent_width(self) -> int:
        return self.__indent * self.__indent_width

    @property
    def indent_str(self) -> str:
        return self.__indent_str

    @property
    def clang_format(self) -> bool:
        return self.__clang_format

    @property
    def doxygen(self) -> bool:
        return self.__doxygen

    @property
    def cpp_access_level(self) -> str:
        return self.__cpp_access_level

    @cpp_access_level.setter
    def cpp_access_level(self, value: str):
        assert value is None or value in (r'public', r'protected', r'private')
        if self.__cpp_access_level == value:
            return
        if self.__cpp_access_level == r'private' and self.doxygen and self.emit_access_levels:
            self('/// @endcond')
        self.__cpp_access_level = value
        if self.__cpp_access_level is not None and self.emit_access_levels:
            self()
            self(rf'{self.__cpp_access_level}:')
            if self.__cpp_access_level == r'private' and self.doxygen:
                self('/// @cond')

    def flush(self):
        s = self.__buffer.getvalue().lstrip()
        # do any user modifications via the on-flush callback
        if self.__on_flush is not None:
            s = str(self.__on_flush(self, s))
        # minor cleanup
        s = re.sub(r'\n(?:[ \t]*\n[ \t]*)+\n', '\n\n', s)  # double blank lines
        s = re.sub(r'([^@][({,])\n\n', r'\1\n', s)  # blank lines following opening brackets or a comma
        s = re.sub(r'\n\n([ \t]*[})])', r'\n\1', s)  # blank lines preceeding closing brackets
        s = re.sub(
            r'([ \t\n](?:namespace|struct|class|union)[ \t]+[a-zA-z0-9_:]+[ \t]*\n)\n+([ \t]*[{])', r'\1\2', s
        )  # blank lines between a namespace/class name and opening bracket
        # clang-format
        if self.__clang_format:
            log.i(f"Running clang-format for {self.__path}")
            try:
                s = utils.clang_format(s, cwd=self.__path.parent)
            except Exception as ex:
                log.w(f"Clang-format failed: {ex}")
        # write to disk
        log.i(f"Writing {self.__path}")
        with open(self.__path, 'w', encoding='utf-8', newline='\n') as f:
            f.write(s)
        return self

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.flush()


# -----------------------------------------------------------------------------------------------------------------------
# logical blocks within a file
# -----------------------------------------------------------------------------------------------------------------------


class WriterBlockBase(object):
    def __init__(self, writer: Writer):
        assert isinstance(writer, Writer)
        self.__writer = writer

    @property
    def writer(self) -> Writer:
        return self.__writer


class Indent(WriterBlockBase):
    def __init__(self, writer: Writer, pre='', post=''):
        super().__init__(writer)
        self.__pre = pre
        self.__post = post

    def __enter__(self):
        if self.__pre:
            self.writer(self.__pre)
        self.writer.indent += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.writer.indent -= 1
        if self.__post:
            self.writer(self.__post)


class Namespace(WriterBlockBase):
    def __init__(self, writer: Writer, ns):
        super().__init__(writer)
        assert ns is None or isinstance(ns, str)
        self.__ok = False
        if ns is None:
            self.__ok = True
            self.__namespace = ''
        elif ns:
            self.__ok = True
            self.__namespace = ns

    def __enter__(self):
        if self.__ok:
            self.writer(f'namespace {self.__namespace}')
            self.writer('{')
            self.writer.indent += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if self.__ok:
            self.writer.indent -= 1
            self.writer('}')


class Block(WriterBlockBase):
    def __init__(self, writer: Writer, pre='', post=''):
        super().__init__(writer)
        self.__pre = pre
        self.__post = post

    def __enter__(self):
        self.writer(rf'{self.__pre}{{')
        self.writer.indent += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.writer.indent -= 1
        self.writer(rf'}}{self.__post}')


class ClassDefinition(WriterBlockBase):
    def __init__(self, writer: Writer, name: str, base_classes=None, hidden_base_classes=None):
        super().__init__(writer)
        self.__name = name
        self.__prev_cpp_access_level = None
        self.__bases = []
        if base_classes is not None:
            self.__bases += [b for b in utils.coerce_collection(base_classes)]
        self.__hidden_bases = []
        if hidden_base_classes is not None:
            self.__hidden_bases += [b for b in utils.coerce_collection(hidden_base_classes)]

    def __enter__(self):
        self.__prev_cpp_access_level = self.writer.cpp_access_level
        self.writer.cpp_access_level = None
        self.writer(self.__name + (' //' if self.__bases or self.__hidden_bases else ''))
        if self.__bases and self.__hidden_bases:
            self.writer(
                ': '
                + f',\n{self.writer.indent_str}'.join(self.__bases)
                + f'\n{self.writer.indent_str}SOAGEN_HIDDEN(, '
                + f',\n{self.writer.indent_str}'.join(self.__hidden_bases)
                + ')'
            )
        elif self.__bases:
            self.writer(': ' + f',\n{self.writer.indent_str}'.join(self.__bases))
        elif self.__hidden_bases:
            self.writer('SOAGEN_HIDDEN_BASE(' + f',\n{self.writer.indent_str}'.join(self.__hidden_bases) + ')')
        self.writer('{')
        self.writer.indent += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.writer.indent -= 1
        self.writer('};')
        self.writer.emit_access_levels = False
        self.writer.cpp_access_level = self.__prev_cpp_access_level
        self.writer.emit_access_levels = True


class Private(WriterBlockBase):
    def __enter__(self):
        self.writer.cpp_access_level = r'private'
        self.writer.indent += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.writer.indent -= 1
        self.writer()


class Protected(WriterBlockBase):
    def __enter__(self):
        self.writer.cpp_access_level = r'protected'
        self.writer.indent += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.writer.indent -= 1
        self.writer()


class Public(WriterBlockBase):
    def __enter__(self):
        self.writer.cpp_access_level = r'public'
        self.writer.indent += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.writer.indent -= 1
        self.writer()


class DoxygenGroup(WriterBlockBase):
    def __init__(self, writer: Writer, key, name):
        super().__init__(writer)
        self.__key = key
        self.__name = name

    def __enter__(self):
        if self.writer.doxygen:
            self.writer(
                rf'''
            /// @addtogroup {self.__key} {self.__name}
            /// @{{
            '''
            )
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if self.writer.doxygen:
            self.writer(
                r'''
            /// @}
            '''
            )


class DoxygenMemberGroup(WriterBlockBase):
    def __init__(self, writer: Writer, name: str, brief='', details='', availability=''):
        super().__init__(writer)
        self.__name = str(name)
        self.__brief = str(brief)
        self.__details = str(details)
        self.__availability = str(availability)

    def __enter__(self):
        if self.writer.doxygen:
            self.writer()
            self.writer(rf'/// @name {self.__name}')
            if self.__brief:
                self.writer(rf'/// @brief {self.__brief}')
            if self.__details:
                self.writer(rf'/// @details {self.__details}')
            if self.__availability:
                self.writer(rf'/// @availability {self.__availability}')
            self.writer(r'/// @{')
            self.writer()
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if self.writer.doxygen:
            self.writer(
                r'''
            /// @}
            '''
            )


class HiddenFromDoxygen(WriterBlockBase):
    def __enter__(self):
        if self.writer.doxygen:
            self.writer(
                rf'''
            /// @cond
            '''
            )
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if self.writer.doxygen:
            self.writer(
                r'''
            /// @endcond
            '''
            )


class PreprocessorRegion(WriterBlockBase):
    def __init__(self, writer: Writer, condition='1'):
        super().__init__(writer)
        self.__condition = str(condition)

    def __enter__(self):
        self.writer(
            rf'''#if {self.__condition}
        '''
        )
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.writer(
            r'''
        #endif'''
        )


class ClangFormatOff(WriterBlockBase):
    def __enter__(self):
        if self.writer.doxygen:
            self.writer('// clang-format off')
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if self.writer.doxygen:
            self.writer('// clang-format on')


__all__ = [
    r'WriterBlockBase',
    r'Writer',
    r'Indent',
    r'Namespace',
    r'Block',
    r'ClassDefinition',
    r'Private',
    r'Protected',
    r'Public',
    r'DoxygenGroup',
    r'DoxygenMemberGroup',
    r'HiddenFromDoxygen',
    r'PreprocessorRegion',
    r'ClangFormatOff',
]
