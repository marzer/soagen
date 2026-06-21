#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re
from pathlib import Path

from . import utils
from .errors import Error


def get_relative_path(path: Path, relative_to: Path) -> Path:
    relative_root = relative_to
    relative = None
    relative_prefix = ''
    while True:
        try:
            relative = path.relative_to(relative_root)
            break
        except ValueError:
            if not relative_root.is_dir() or relative_root.parent == relative_root:
                break
            relative_prefix = rf'../{relative_prefix}'
            relative_root = relative_root.parent
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
    # || SOAGEN_DOXYGEN
    (re.compile(r'[|][|] SOAGEN_DOXYGEN'), r''),
    # hidden implementation details
    (re.compile(r'POXY_IMPLEMENTATION_DETAIL\((.+?)\);', flags=re.DOTALL), r'\1;'),
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
        self.__string = re.sub(
            r'#\s*define\s+POXY_IMPLEMENTATION_DETAIL\s*\(', r'#define POXY_IMPLEMENTATION_DETAIL_____(', self.__string
        )
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
        self.__string = re.sub(
            r'#\s*define\s+POXY_IMPLEMENTATION_DETAIL_____\s*\(', r'#define POXY_IMPLEMENTATION_DETAIL(', self.__string
        )

    def __preprocess(self, incl):
        incl_spelling = None
        if not isinstance(incl, (Path, str)):  # a regex match object
            incl_spelling = incl.group(1).strip()
            incl = incl_spelling
        if isinstance(incl, str):
            incl = Path(incl.strip().replace('\\', r'/'))
        if not incl.is_absolute():
            incl = Path(self.__directory_stack[-1], incl).resolve()
        if not incl.is_file():
            sibling = Path(incl.parent.parent, 'hpp', incl.name)
            if sibling.is_file():
                incl = sibling
            else:
                raise Error(self.__include_error(incl_spelling, [incl, sibling]))
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
        assert self.__entry_root is not None
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

    def __render_path(self, path):
        return str(get_relative_path(Path(path), Path.cwd())).replace('\\', r'/')

    def __find_include_line(self, includer, spelling):
        if not spelling:
            return None
        try:
            text = utils.read_all_text_from_file(includer).replace('\r\n', '\n')
        except Exception:
            return None
        target = spelling.replace('\\', r'/').strip()
        for number, line in enumerate(text.split('\n'), start=1):
            m = RX_INCLUDES.search(line)
            if m and m.group(1).strip().replace('\\', r'/') == target:
                return number
        return None

    def __include_error(self, spelling, candidates):
        if spelling:
            lines = [rf'could not resolve #include "{spelling}"']
        else:
            lines = [rf'could not read source file: {self.__render_path(candidates[0])}']

        includer = self.__include_stack[-1] if self.__include_stack else None
        if includer is not None:
            location = self.__render_path(includer)
            line_number = self.__find_include_line(includer, spelling)
            if line_number is not None:
                location = rf'{location}:{line_number}'
            lines.append(rf'    included from: {location}')

        looked_for = []
        for candidate in candidates:
            rendered = self.__render_path(candidate)
            if rendered not in looked_for:
                looked_for.append(rendered)
        lines.append(r'    looked for:')
        lines.extend(rf'        {p}' for p in looked_for)

        if len(self.__include_stack) > 1:
            lines.append(r'    include chain:')
            lines.extend(rf'        {self.__render_path(f)}' for f in self.__include_stack)

        return '\n' + '\n'.join(lines)

    def __str__(self):
        return self.__string

    def processed_files(self):
        out = list(self.__processed_files)
        out.sort()
        return out


RX_DEFINE_NAME = re.compile(r'^[ \t]*#[ \t]*define[ \t]+([A-Za-z_][A-Za-z0-9_]*)', flags=re.M)
RX_DEFINE_LINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+([A-Za-z_][A-Za-z0-9_]*)(.*)$', flags=re.M)
RX_SOAGEN_TOKEN = re.compile(r'\b[Ss][Oo][Aa][Gg][Ee][Nn]_[A-Za-z0-9_]+')


def defined_macros(text) -> 'set[str]':
    return set(RX_DEFINE_NAME.findall(text))


def referenced_soagen_macros(text) -> 'set[str]':
    return set(RX_SOAGEN_TOKEN.findall(text))


def macro_dependencies(text) -> 'dict[str, set[str]]':
    joined = re.sub(r'\\\r?\n', ' ', text)
    deps = {}
    for m in RX_DEFINE_LINE.finditer(joined):
        name = m.group(1)
        # a macro may be defined more than once (per-compiler branches); union every branch's references
        deps.setdefault(name, set()).update(referenced_soagen_macros(m.group(2)) - {name})
    return deps


def internal_macros(text, keep) -> 'list[str]':
    deps = macro_dependencies(text)
    defined = list(deps.keys())
    # a kept macro implicitly keeps every macro its expansion transitively references
    reachable = set(keep)
    stack = list(reachable)
    while stack:
        current = stack.pop()
        for token in deps.get(current, ()):
            if token not in reachable:
                reachable.add(token)
                stack.append(token)
        # a reachable token ending in '_' is a token-paste prefix (e.g. SOAGEN_FOO_ ## n);
        # the pasted targets can't be seen statically, so keep the whole prefixed family
        if current.endswith('_'):
            for name in defined:
                if name.startswith(current) and name not in reachable:
                    reachable.add(name)
                    stack.append(name)
    return sorted(
        name for name in defined if (name.startswith('SOAGEN_') or name.startswith('soagen_')) and name not in reachable
    )


__all__ = [r'Preprocessor', r'defined_macros', r'referenced_soagen_macros', r'macro_dependencies', r'internal_macros']
