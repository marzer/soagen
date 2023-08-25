#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re
from pathlib import Path

from . import utils
from .column import *
from .configurable import Configurable
from .includes import *
from .metavars import *
from .schemas import *
from .version import *
from .writer import *


class HeaderFile(Configurable):
    __schema = Schema(
        {
            Optional(r'banner', default=''): Stripped(str),
            Optional(r'brief', default=''): Stripped(str),
            Optional(r'combined', default=True): bool,
            Optional(r'footer', default=''): Stripped(str),
            Optional(r'header', default=''): Stripped(str),
            Optional(r'includes', default=dict): {object: object},
            Optional(r'prologue', default=''): Stripped(str),
        }
    )

    def __init__(self, config, structs, vals):
        super().__init__(config)
        self.__dict__.update(HeaderFile.__schema.validate(vals))

        self.path = self.config.path.with_suffix('.hpp')

        self.structs = tuple(utils.coerce_collection(structs))
        assert len(self.structs) >= 1

        self.additional_header_files = []
        if not self.combined:
            self.additional_header_files = [HeaderFile(config, struct, vals) for struct in self.structs[1:]]
            self.structs = tuple(self.structs[:1])
            self.path = Path(self.config.path.parent, rf'{self.structs[0].name}.hpp')

        self.meta = MetaVars()
        for prefix in (r'file_', r'file::'):
            self.meta.push(rf'{prefix}name', self.path.name)
            self.meta.push(rf'{prefix}path', str(self.path))

        # includes
        with SchemaContext('includes'):
            self.includes = Includes(config, self.includes)

            # add any includes requested by member structs
            self.includes.internal = set(self.includes.internal)
            self.includes.external = set(self.includes.external)
            for struct in self.structs:
                for h in struct.includes.internal:
                    self.includes.internal.add(h)
                for h in struct.includes.external:
                    self.includes.external.add(h)
            self.includes.internal = sorted(self.includes.internal)
            self.includes.external = sorted(self.includes.external)

        # documentation
        if not self.brief and self.structs:
            self.brief = ', '.join([n.qualified_type for n in self.structs])
            self.brief = rf'Contains the definition{"s" if len(self.structs) > 1 else ""} of {self.brief}.'

        # banner
        if self.banner:
            self.banner = [s.rstrip() for s in self.banner.split('\n')]
            self.banner = [re.sub(r'(^|\n)// ?', r'\1', s) for s in self.banner]
            self.banner = [rf'// {s}'.rstrip() for s in self.banner]
            self.banner = '\n'.join(self.banner)

    def write(self, o: Writer):
        with MetaScope(self):
            if self.banner:
                o(rf'//{"-"*(120 - o.indent_width - 2)}')
                o(self.banner)

            o(rf'//{"-"*(120 - o.indent_width - 2)}')
            o(rf'// This file was generated by soagen v{VERSION_STRING} - do not modify it directly')
            o(rf'// https://marzer.github.io/soagen')
            o(rf'//{"-"*(120 - o.indent_width - 2)}')
            o(rf'#pragma once')
            o()

            if o.doxygen:
                o(
                    rf'''
                /// @file
                /// @brief {self.brief}
                ///
                /// @note The code and documentation in this file were generated by soagen - https://marzer.github.io/soagen
                '''
                )

            # internal includes (#include "blah.h")
            if self.includes.internal:
                o()
                for inc in self.includes.internal:
                    o(rf'#include "{inc}"')

            # stick the prologue here so users can use it to override the various
            # SOAGEN_ macros etc before including soagen.hpp
            o(
                rf'''
            {self.prologue}
            '''
            )

            # soagen.hpp
            o(
                rf'''
            #include <soagen.hpp>
            #if SOAGEN_VERSION_MAJOR != {VERSION[0]} || SOAGEN_VERSION_MINOR < {VERSION[1]}
                #error soagen version mismatch - expected v{VERSION[0]}.{VERSION[1]}.X
            #endif
            '''
            )

            # external/system includes (#include <blah.h>)
            # (deferred until later so we can take advantage of detect_includes() for the whole file)
            o('\n\n// __SOAGEN_EXTERNAL_HEADERS\n\n')

            # misc preprocessor boilerplate
            o(
                rf'''
            SOAGEN_PUSH_WARNINGS;
            SOAGEN_DISABLE_SPAM_WARNINGS;
            #if SOAGEN_MSVC
                #pragma inline_recursion(on)
            #endif
            #if SOAGEN_MSVC_LIKE
                #pragma push_macro("min")
                #pragma push_macro("max")
                #undef min
                #undef max
            #endif
            '''
            )

            # doxygen crap
            if o.doxygen:
                o(
                    rf'''
                //{"-"*(120 - o.indent_width - 2)}
                // doxygen safeguards
                //{"-"*(120 - o.indent_width - 2)}

                #if defined(DOXYGEN) || defined(__DOXYGEN) || defined(__DOXYGEN__) \
                    || defined(__doxygen__) || defined(__POXY__) || defined(__poxy__)

                #ifndef SOAGEN_CONSTRAINED_TEMPLATE
                    #define SOAGEN_CONSTRAINED_TEMPLATE(cond, ...)  template <__VA_ARGS__>
                #endif
                #ifndef SOAGEN_DOXYGEN
                    #define SOAGEN_DOXYGEN 1
                #endif
                #ifndef SOAGEN_MAKE_NAME
                    #define SOAGEN_MAKE_NAME(...) static_assert(true)
                #endif
                #ifndef SOAGEN_MAKE_NAMED_COLUMN
                    #define SOAGEN_MAKE_NAMED_COLUMN(...) static_assert(true)
                #endif
                #ifndef SOAGEN_NODISCARD
                    #define SOAGEN_NODISCARD
                #endif
                #ifndef SOAGEN_NODISCARD_CTOR
                    #define SOAGEN_NODISCARD_CTOR
                #endif
                #ifndef SOAGEN_INLINE_GETTER
                    #define SOAGEN_INLINE_GETTER inline
                #endif
                #ifndef SOAGEN_PURE_GETTER
                    #define SOAGEN_PURE_GETTER
                #endif
                #ifndef SOAGEN_PURE_INLINE_GETTER
                    #define SOAGEN_PURE_INLINE_GETTER inline
                #endif
                #ifndef SOAGEN_ALWAYS_INLINE
                    #define SOAGEN_ALWAYS_INLINE inline
                #endif
                #ifndef SOAGEN_CPP20_CONSTEXPR
                    #define SOAGEN_CPP20_CONSTEXPR constexpr
                #endif
                #ifndef SOAGEN_HIDDEN
                    #define SOAGEN_HIDDEN(...)
                #endif
                #ifndef SOAGEN_HIDDEN_BASE
                    #define SOAGEN_HIDDEN_BASE(...)
                #endif
                #ifndef SOAGEN_HIDDEN_CONSTRAINT
                    #define SOAGEN_HIDDEN_CONSTRAINT(...)
                #endif
                #ifndef SOAGEN_HIDDEN_PARAM
                    #define SOAGEN_HIDDEN_PARAM(...)
                #endif
                #ifndef SOAGEN_ENABLE_IF_T
                    #define SOAGEN_ENABLE_IF_T(T, ...) T
                #endif
                #ifndef SOAGEN_ENABLE_IF
                    #define SOAGEN_ENABLE_IF(...)
                #endif
                #ifndef SOAGEN_REQUIRES
                    #define SOAGEN_REQUIRES(...)
                #endif
                #ifndef SOAGEN_EMPTY_BASES
                    #define SOAGEN_EMPTY_BASES
                #endif
                #ifndef SOAGEN_COLUMN
                    #define SOAGEN_COLUMN(...)
                #endif
                #if !defined(POXY_IMPLEMENTATION_DETAIL) && !(defined(__POXY__) || defined(__poxy__))
                    #define POXY_IMPLEMENTATION_DETAIL(...) __VA_ARGS__
                #endif

                #endif // doxygen

                '''
                )

            # forward declarations
            o(
                rf'''
            //{"-"*(120 - o.indent_width - 2)}
            // forward declarations + soagen internal boilerplate
            //{"-"*(120 - o.indent_width - 2)}
            '''
            )
            with HiddenFromDoxygen(o), ClangFormatOff(o):
                with Namespace(o, self.config.namespace):
                    for struct in self.structs:
                        struct.write_forward_declarations(o)
                o()
                with Namespace(o, 'soagen::detail'):
                    names = set()
                    for struct in self.structs:
                        for col in struct.columns:
                            if col.name not in (
                                r'first',
                                r'second',
                                r'third',
                                r'fourth',
                                r'fifth',
                                r'sixth',
                                r'seventh',
                                r'eighth',
                                r'ninth',
                                r'tenth',
                                r'eleventh',
                                r'twelfth',
                                r'thirteenth',
                                r'fourteenth',
                                r'fifteenth',
                                r'sixteenth',
                            ):
                                names.add(col.name)
                    names = sorted(list(names))
                    for name in names:
                        sanitized_name = name.replace('::', '_')
                        pp_define = rf'SOAGEN_NAME_{sanitized_name}'
                        o(
                            rf'''
                        #ifndef {pp_define}
                            #define {pp_define}
                            SOAGEN_MAKE_NAME({name});
                        #endif
                        '''
                        )
                o()
                for struct in self.structs:
                    struct.write_impl_namespace(o)
                o()
                with Namespace(o, 'soagen::detail'):
                    for struct in self.structs:
                        struct.write_soagen_detail_specializations(o)

            # header
            if self.header:
                o(
                    rf'''
                //{"-"*(120 - o.indent_width - 2)}
                // header
                //{"-"*(120 - o.indent_width - 2)}

                '''
                )
                o(self.header)

            # structs (class definitions, then outline member implementations)
            for struct in self.structs:
                o(
                    rf'''
                //{"-"*(120 - o.indent_width - 2)}
                // {struct.name}
                //{"-"*(120 - o.indent_width - 2)}
                '''
                )
                with Namespace(o, self.config.namespace):
                    struct.write_class_definition(o)
                    with HiddenFromDoxygen(o):
                        struct.write_outline_member_implementations(o)

            # footer
            if self.footer:
                o(
                    rf'''
                //{"-"*(120 - o.indent_width - 2)}
                // footer
                //{"-"*(120 - o.indent_width - 2)}

                {self.footer}
                '''
                )

            # clean up preprocessor boilerplate
            o(
                rf'''
            #if SOAGEN_MSVC_LIKE
                #pragma pop_macro("min")
                #pragma pop_macro("max")
            #endif
            #if SOAGEN_MSVC
                #pragma inline_recursion(off)
            #endif
            SOAGEN_POP_WARNINGS;'''
            )


__all__ = [r'HeaderFile']
