#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re
from pathlib import Path

from . import utils


def get_relative_path(path: Path, relative_to: Path) -> Path:
    relative_root = relative_to
    relative = None
    relative_prefix = ''
    while True:
        try:
            relative = path.relative_to(relative_root)
            break
        except:
            if not relative_root.is_dir() or relative_root.parent == relative_root:
                break
            relative_prefix = rf'../{relative_prefix}'
            relative_root = relative_root.parent
            pass
    if relative is not None:
        return Path(rf'{relative_prefix}{relative}')
    return path


class Preprocessor:

    """
    A simple C++ file 'preprocessor' for almalgamating source files.
    """

    __re_includes = re.compile(r'^\s*#\s*include\s+"(.+?)".*?$', re.I | re.M)
    __re_pragma_once = re.compile(r'^\s*#\s*pragma\s+once\s*$', re.M)
    __repeatable_headers = (r'header_start.hpp', r'header_end.hpp')

    def __init__(self, file):
        self.__processed_files = set()
        self.__once_only = set()
        self.__directory_stack = [Path.cwd()]
        self.__include_stack = []
        self.__entry_root = None
        self.__string = self.__preprocess(file)
        # do some cleanup of the preprocessed file
        while True:
            s = self.__string
            # trailing whitespace
            self.__string = re.sub('([^ \t])[ \t]+\n', r'\1\n', s)
            # magic comments
            blank_line = r'(?:[ \t]*\n)'
            magic_comment = r'(?:[ \t]*//[#!</][^\n]*)'
            s = re.sub(rf'\n{magic_comment}\n{blank_line}+{magic_comment}\n', '\n', s)
            s = re.sub(rf'([{{,])\s*\n(?:{magic_comment}\n|{blank_line})+', r'\1\n', s)
            s = re.sub(rf'\n?(?:{magic_comment}\n)+', '\n', s)
            # consecutive header separators
            header_separator = r'(?://\*\*\*\**[ \t]+[a-zA-Z0-9_/.-]+[ \t]+\*\*\*\*+\n)'
            s = re.sub(rf'(?:{header_separator}{blank_line}*)+({header_separator})', r'\1', s)
            # double blank lines
            s = re.sub('\n(?:[ \t]*\n[ \t]*)+\n', '\n\n', s)
            # weird spacing edge case around pp directives
            s = re.sub('\n[}]\n#', r'\n}\n\n#', s)
            # blank lines following opening brackets or a comma
            s = re.sub(r'([^@][({,])\n\n', r'\1\n', s)
            # blank lines preceeding closing brackets
            s = re.sub(r'\n\n([ \t]*[})])', r'\n\1', s)
            # empty #if blocks
            s = re.sub(r'#if(n?def)?[ \t]+[a-zA-Z0-9_]+\s*?\n+\s*?#endif', r'\n', s)
            # empty #else blocks
            s = re.sub(r'#else\s*?\n+\s*?#endif', r'#endif\n', s)
            # #if SOAGEN_DOXYGEN ... #else ... #endif
            s = re.sub(r'#if[ \t]+SOAGEN_DOXYGEN\s*?(?:\n+[^#]*?)?\n+\s*?#else\s*?\n+([^#]*?)\n+\s*?#endif', r'\1\n', s)
            # #if !SOAGEN_DOXYGEN ... #else ... #endif
            s = re.sub(
                r'#if[ \t]+![ \t]*SOAGEN_DOXYGEN\s*?\n+([^#]*?)\n+\s*?#else\s*?(?:\n+[^#]*?)?\n+\s*?#endif', r'\1\n', s
            )
            # #if SOAGEN_DOXYGEN ... #endif
            s = re.sub(r'#if[ \t]+SOAGEN_DOXYGEN\s*?(?:\n+[^#]*?)?\n+\s*?#endif', r'\n', s)
            # #if !SOAGEN_DOXYGEN ... #endif
            s = re.sub(r'#if[ \t]+![ \t]*SOAGEN_DOXYGEN\s*?\n+([^#]*?)\n+\s*?#endif', r'\1\n', s)
            if s == self.__string:
                break
            self.__string = s
        self.__string = self.__string.strip() + '\n'

    def __preprocess(self, incl):
        if not isinstance(incl, (Path, str)):  # a regex match object
            incl = incl.group(1).strip()
        if isinstance(incl, str):
            incl = Path(incl.strip().replace('\\', r'/'))
        if not incl.is_absolute():
            incl = Path(self.__directory_stack[-1], incl).resolve()
        if not incl.is_file():
            sibling = Path(incl.parent.parent, 'hpp', incl.name)
            if sibling.is_file():
                incl = sibling
        self.__processed_files.add(incl)
        if incl in self.__once_only:
            return ''
        self.__include_stack.append(incl)

        text = utils.read_all_text_from_file(incl, logger=True).strip() + '\n'
        text = text.replace('\r\n', '\n')  # convert windows newlines

        self.__directory_stack.append(incl.parent)
        if self.__re_pragma_once.search(text):
            self.__once_only.add(incl)
        if len(self.__include_stack) == 1 and self.__entry_root is None:
            self.__entry_root = incl.parent.resolve()
        if len(self.__include_stack) > 1:
            text = self.__re_pragma_once.sub('', text)

        text = self.__re_includes.sub(lambda m: self.__preprocess(m), text, 0)

        # explicit 'strip this' blocks
        text = re.sub(r'(?:\n[ \t]*)?//[#!][ \t]*[{][{].*?//[#!][ \t]*[}][}].*?\n', '\n', text, flags=re.S)

        incl_name = incl.name
        incl = get_relative_path(incl, self.__entry_root)
        incl = str(incl).replace('\\', r'/').strip('/')

        if len(self.__include_stack) > 1 and incl_name not in self.__repeatable_headers:
            header = utils.make_divider(str(incl), 10, pattern=r'*')

            footer = ''
            if len(self.__include_stack) > 2:
                footer = get_relative_path(self.__include_stack[-2], self.__entry_root)
                footer = str(footer).replace('\\', r'/').strip('/')
                footer = utils.make_divider(footer, 10, pattern=r'*')

            text = f'\n\n{header}\n\n{text}\n\n{footer}'.rstrip()

        self.__include_stack.pop()
        self.__directory_stack.pop()
        return '\n\n' + text + '\n\n'

    def __str__(self):
        return self.__string

    def processed_files(self):
        out = list(self.__processed_files)
        out.sort()
        return out


__all__ = [r'Preprocessor']
