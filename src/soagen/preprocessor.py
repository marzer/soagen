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


RX_INCLUDES = re.compile(r'^\s*#\s*include\s+"(.+?)".*?$', flags=re.I | re.M)
RX_PRAGMA_ONCE = re.compile(r'^\s*#\s*pragma\s+once\s*$', flags=re.M)

# explicit 'strip this' blocks
RX_STRIP_BLOCKS = re.compile(r'(?:\n[ \t]*)?//[#!][ \t]*[{][{].*?//[#!][ \t]*[}][}].*?\n', flags=re.S)

header_separator = r'(?://[ \t]*\*\*\*\**[ \t]*[a-zA-Z0-9_/.-]+[ \t]*\*\*\*\*+\n)'

# consecutive header end/header start
RX_HEADER_END_START_OUTER = re.compile(
    r'//[ \t]*__SOAGEN_HEADER_END\{\{.+?\}\}__SOAGEN_HEADER_START[ \t]*\n', flags=re.DOTALL
)
RX_HEADER_END_START_INNER = re.compile(
    rf'//[ \t]*\}}\}}__SOAGEN_HEADER_END\n+({header_separator})\n+//[ \t]*__SOAGEN_HEADER_START\{{\{{[ \t]*\n',
    flags=re.DOTALL,
)
RX_HEADER_START_END = re.compile(r'//[ \t]*(?:\}\}[ \t]*)?__SOAGEN_HEADER_(?:START|END)[ \t]*(?:\{\{[ \t]*)?\n')

# misc cleanup
RX_CLEANUP = (
    # trailing whitespace
    (re.compile(r'([^ \t])[ \t]+\n'), r'\1\n'),
    # double blank lines
    (re.compile('\n\n\n+'), '\n\n'),
    # magic comments
    (re.compile(rf'(?://(?:[#!</]|[ \t]?(?:\^\^\^|vvv))).*?($)', flags=re.MULTILINE), r'\1'),
    # consecutive header separators
    (re.compile(rf'(?:{header_separator}(?:[ \t]*\n)*)+({header_separator})'), r'\1'),
    # weird spacing edge case around pp directives
    (re.compile('\n[}]\n#'), r'\n}\n\n#'),
    # blank lines following a comma, opening bracket, or colon
    (re.compile(r'([{(\[:,])\n\n'), r'\1\n'),
    # blank lines preceeding closing brackets
    (re.compile(r'\n\n([ \t]*[\]})])'), r'\n\1'),
    # empty #if blocks
    (re.compile(r'#if(n?def)?[ \t]+[a-zA-Z0-9_]+\s*?\n+\s*?#endif'), r'\n'),
    # empty #else blocks
    (re.compile(r'#else\s*?\n+\s*?#endif'), r'#endif\n'),
    # #if SOAGEN_DOXYGEN ... #else ... #endif
    (re.compile(r'#if[ \t]+SOAGEN_DOXYGEN\s*?(?:\n+[^#]*?)?\n+\s*?#else\s*?\n+([^#]*?)\n+\s*?#endif'), r'\1\n'),
    # #if !SOAGEN_DOXYGEN ... #else ... #endif
    (re.compile(r'#if[ \t]+![ \t]*SOAGEN_DOXYGEN\s*?\n+([^#]*?)\n+\s*?#else\s*?(?:\n+[^#]*?)?\n+\s*?#endif'), r'\1\n'),
    # #if SOAGEN_DOXYGEN ... #endif
    (re.compile(r'#if[ \t]+SOAGEN_DOXYGEN\s*?(?:\n+[^#]*?)?\n+\s*?#endif'), r'\n'),
    # #if !SOAGEN_DOXYGEN ... #endif
    (re.compile(r'#if[ \t]+![ \t]*SOAGEN_DOXYGEN\s*?\n+([^#]*?)\n+\s*?#endif'), r'\1\n'),
)


class Preprocessor:

    """
    A simple C++ file 'preprocessor' for almalgamating source files.
    """

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
            # misc cleanup
            for rx, repl in RX_CLEANUP:
                s = rx.sub(repl, s)
            # remove bookended header_end / header_start
            pos = 0
            m = RX_HEADER_END_START_OUTER.search(s)
            while m:
                m2 = RX_HEADER_END_START_INNER.search(m[0])
                if m2:
                    new_s = s[: m.start()] + m2[1]
                    pos = len(new_s)
                    new_s += s[m.end() :]
                    s = new_s
                else:
                    pos = m.end()
                m = RX_HEADER_END_START_OUTER.search(s, pos=pos)
            s = RX_HEADER_START_END.sub('\n', s)
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
        if RX_PRAGMA_ONCE.search(text):
            self.__once_only.add(incl)
        if len(self.__include_stack) == 1 and self.__entry_root is None:
            self.__entry_root = incl.parent.resolve()
        if len(self.__include_stack) > 1:
            text = RX_PRAGMA_ONCE.sub('', text)

        text = RX_INCLUDES.sub(lambda m: self.__preprocess(m), text, 0)
        text = RX_STRIP_BLOCKS.sub('\n', text)

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
