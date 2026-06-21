#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from __future__ import annotations

from pathlib import Path

from . import log, utils
from .configurable import ConfigBase
from .errors import *
from .header_file import HeaderFile
from .injectors import StructInjector
from .metavars import *
from .natvis_file import NatvisFile
from .schemas import *
from .struct import Struct

try:
    import pytomlpp as toml  # pyright: ignore[reportMissingImports]  # optional fast path, based on toml++ (C++)
except ImportError:
    try:
        import tomllib as toml  # PEP 680
    except ImportError:
        import tomli as toml  # pyright: ignore[reportMissingImports]


class Config(ConfigBase):
    hpp: list[HeaderFile]
    namespace: str
    structs: tuple[Struct, ...]
    allocator: str
    all_structs: StructInjector
    path: Path
    dir: Path
    out_dir: Path
    meta: MetaVars
    meta_stack: MetaStack
    natvis: NatvisFile
    all_outputs: list

    __schema = Schema(
        {
            Optional(r'hpp', default=lambda: {}): {object: object},
            Optional(r'namespace', default=''): And(
                str,
                Use(lambda x: x.strip(': \t\n\f\b\v').split('::')),
                Use(lambda x: [s.strip(': \t\n\f\b\v') for s in x]),
                Use(lambda x: '::'.join(x).replace('::::', '::')),
            ),
            Optional(r'structs', default=lambda: {}): {
                Stripped(str, allow_empty=False, name=r'struct name'): {object: object}
            },
            Optional(r'allocator', default=r'soagen::allocator'): Stripped(str),
            Optional(r'all_structs', default=lambda: {}): {object: object},
        }
    )

    def __init__(self, path, output_dir=None):
        assert isinstance(path, Path)
        self.path = path.resolve()
        self.dir = path.parent
        self.out_dir = output_dir.resolve() if output_dir is not None else self.path.parent

        self.meta_stack = MetaStack()
        self.meta = MetaVars()
        self.meta.push('tab', '\t')
        self.meta.push('space', ' ')
        self.meta_stack.push(self.meta)

        with SchemaContext(self.path.name):
            try:
                cfg = toml.loads(utils.read_all_text_from_file(self.path, logger=log.i))
                validated = Config.__schema.validate(cfg)
            except Exception as ex:
                raise SchemaError(str(ex), None) from ex
            self.__dict__.update(validated)

            # namespace
            if self.namespace:
                if self.namespace in (r'std', r'soagen', r'soagen::detail') or self.namespace.startswith(r'std::'):
                    raise SchemaError(rf"namespace: '{self.namespace}' is reserved", None)
                self.meta.push('namespace', self.namespace)
                self.meta.push('namespace::name', self.namespace)
                self.meta.push('namespace::start', f'namespace {self.namespace}\n{{')
                self.meta.push('namespace::end', r'}')
                self.meta.push('namespace::scope', rf'{self.namespace}::')
            else:
                log.w(rf'{current_schema_context()}namespace: not set! polluting the global namespace is bad practice')
                self.meta.push('namespace', '')
                self.meta.push('namespace::name', '')
                self.meta.push('namespace::start', '')
                self.meta.push('namespace::end', '')
                self.meta.push('namespace::scope', '')

            # injectors for the 'all_X' sections
            self.all_structs = StructInjector(self, validated['all_structs'])

            # structs
            raw_structs = list(validated['structs'].items())
            if not raw_structs:
                raise SchemaError(r"structs: at least one struct must be defined", None)
            structs = []
            for name, body in raw_structs:
                with SchemaContext(rf"struct '{name}'"):
                    structs.append(Struct(self, name, body))
            self.structs = tuple(sorted(structs, key=lambda s: s.name))
            self.meta.push('struct_names', ', '.join([s.name for s in self.structs]))
            self.meta.push('struct_types', ', '.join([s.type for s in self.structs]))
            self.meta.push('qualified_struct_names', ', '.join([s.qualified_type for s in self.structs]))
            self.meta.push('qualified_struct_types', ', '.join([s.qualified_type for s in self.structs]))
            index = 0
            for struct in self.structs:
                struct.set_index(index)
                index += 1

            # hpp files (needs a little bit of special handling because of the 'combined' option)
            self.hpp = [HeaderFile(self, self.structs, validated['hpp'])]
            self.hpp += [*self.hpp[0].additional_header_files]
            self.hpp[0].additional_header_files = []

            # all outputs
            self.natvis = NatvisFile(self, self.structs)
            self.all_outputs = [self.natvis, *self.hpp]
            log.d('\n  -> '.join([str(self.path)] + [str(o.path) for o in self.all_outputs]))
            for o in self.all_outputs:
                if o.path.is_dir():
                    raise Error(rf"invalid output '{o.path}': outputs cannot be existing directories")
                if o.path == self.path:
                    raise Error(rf"invalid output '{o.path}': outputs cannot overwrite input")
                if o.path.name.lower() == r'soagen.hpp':
                    raise Error(rf"invalid output '{o.path}': 'soagen.hpp' is a reserved name")


__all__ = [r'Config']
