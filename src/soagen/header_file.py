#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re

from . import cpp, utils
from .column import *
from .configurable import Configurable
from .metavars import *
from .schemas import And, Optional, Schema, Stripped, Use, ValueOrArray
from .version import *
from .writer import *


class HeaderFile(Configurable):
    __schema = Schema(
        {
            Optional(r'banner', default=''): Stripped(str),
            Optional(r'prologue', default=''): Stripped(str),
            Optional(r'header', default=''): Stripped(str),
            Optional(r'footer', default=''): Stripped(str),
            Optional(r'internal_includes', default=list): And(
                ValueOrArray(str, name=r'internal_includes'),
                Use(lambda x: cpp.remove_implicit_includes(sorted(set([s.strip() for s in x if s])))),
            ),
            Optional(r'external_includes', default=list): And(
                ValueOrArray(str, name=r'external_includes'),
                Use(lambda x: cpp.remove_implicit_includes(sorted(set([s.strip() for s in x if s])))),
            ),
            Optional(r'brief', default=''): Stripped(str),
        }
    )

    def __init__(self, config, structs, vals, path=''):
        super().__init__(config)
        self.path = path
        if not self.path:
            self.path = self.config.path.with_suffix('.hpp')
        self.__dict__.update(HeaderFile.__schema.validate(vals))

        self.structs = structs if utils.is_collection(structs) else [structs]

        # add any includes requested by member structs
        self.internal_includes = set(self.internal_includes)
        self.external_includes = set(self.external_includes)
        for struct in self.structs:
            for h in struct.internal_includes:
                self.internal_includes.add(h)
            for h in struct.external_includes:
                self.external_includes.add(h)
        self.internal_includes = sorted(self.internal_includes)
        self.external_includes = sorted(self.external_includes)

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
        if self.internal_includes:
            o()
            for inc in self.internal_includes:
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
        o('\n\n// #### SOAGEN_EXTERNAL_HEADERS #### \n\n')

        # misc preprocessor boilerplate
        o(
            rf'''
        SOAGEN_PUSH_WARNINGS;
        SOAGEN_DISABLE_SPAM_WARNINGS;
        #if SOAGEN_CLANG >= 16
            #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
        #endif
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

            #ifndef SOAGEN_DOXYGEN
                #define SOAGEN_DOXYGEN 1
            #endif
            #ifndef SOAGEN_MAKE_NAME
                #define SOAGEN_MAKE_NAME(...) static_assert(true)
            #endif
            #ifndef SOAGEN_MAKE_COL
                #define SOAGEN_MAKE_COL(...) static_assert(true)
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
            #ifndef SOAGEN_ALIGNED_COLUMN
                #define SOAGEN_ALIGNED_COLUMN(...)
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
        with HiddenFromDoxygen(o):
            with Namespace(o, self.config.namespace):
                for struct in self.structs:
                    with MetaScope(self.config.meta_stack, struct.meta):
                        struct.write_forward_declarations(o)
            o()
            with Namespace(o, 'soagen'):
                for struct in self.structs:
                    with MetaScope(self.config.meta_stack, struct.meta):
                        struct.write_soagen_specializations(o)
            o()
            with Namespace(o, 'soagen::detail'):
                names = set()
                for struct in self.structs:
                    for col in struct.columns:
                        names.add(col.name)
                names = sorted(list(names))
                with ClangFormatOff(o):
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
                for struct in self.structs:
                    with MetaScope(self.config.meta_stack, struct.meta):
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
            with MetaScope(self.config.meta_stack, struct.meta):
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
