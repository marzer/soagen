#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import argparse
import logging
import os
import re
import subprocess
import sys
import zipfile
from io import StringIO
from pathlib import Path

from . import cpp, log, paths, utils
from .config import Config
from .errors import Error
from .preprocessor import Preprocessor
from .schemas import SchemaError, current_schema_context
from .version import *
from .writer import Writer


def launch_worker(*args, cwd=None):
    if cwd is None:
        cwd = Path.cwd()
    cwd = utils.coerce_path(cwd)
    utils.run_python_script(
        Path(__file__).parent.parent / r'__main__.py',
        *args,
        r'--worker',
        r'--color' if log.styles_allowed() else r'--no-color',
        cwd=cwd,
    )


def bug_report():
    BUG_REPORT_STRIP_ARGS = (
        r'--bug-report',
        r'--bug-report-internal',
        r'-v',
        r'--verbose',
        r'--color',
        r'--no-color',
        r'--colour',
        r'--no-colour',
    )

    bug_report_args = [arg for arg in sys.argv[1:]]
    bug_report_args_stripped = [arg for arg in bug_report_args if arg not in BUG_REPORT_STRIP_ARGS]
    bug_report_zip = (Path.cwd() / r'soagen_bug_report.zip').resolve()

    log.i(rf'{log.STYLE_CYAN}Preparing a bug report!{log.STYLE_RESET}')
    log.i(r'Preparing output paths')
    utils.delete_directory(paths.BUG_REPORT_DIR, logger=log.d)
    utils.delete_file(bug_report_zip, logger=log.d)
    os.makedirs(str(paths.BUG_REPORT_DIR), exist_ok=True)

    log.i(r'Invoking soagen')
    result = subprocess.run(
        args=[r'soagen', *bug_report_args_stripped, r'--bug-report-internal', r'--verbose', r'--no-color'],
        cwd=str(Path.cwd()),
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        encoding=r'utf-8',
    )

    if result.stdout is not None:
        log.i(r'Writing stdout')
        with open(paths.BUG_REPORT_DIR / r'stdout.txt', r'w', newline='\n', encoding=r'utf-8') as f:
            f.write(result.stdout)

    if result.stderr is not None:
        log.i(r'Writing stderr')
        with open(paths.BUG_REPORT_DIR / r'stderr.txt', r'w', newline='\n', encoding=r'utf-8') as f:
            f.write(result.stderr)

    log.i(r'Writing metadata')
    with open(paths.BUG_REPORT_DIR / r'metadata.txt', r'w', newline='\n', encoding=r'utf-8') as f:
        f.write(f'version: {VERSION_STRING}\n')
        f.write(f'args: {bug_report_args_stripped}\n')
        f.write(f'returncode: {result.returncode}\n')

    # zip file
    log.i(r'Zipping files')
    with zipfile.ZipFile(str(bug_report_zip), 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as zip:
        file_prefix_len = len(str(paths.BUG_REPORT_DIR))
        for file in utils.get_all_files(paths.BUG_REPORT_DIR, recursive=True):
            if file.suffix is not None and file.suffix.lower() in (r'.pyc',):
                continue
            relative_file = str(file)[file_prefix_len:].replace('\\', '/').strip('/')
            zip.write(file, arcname=rf'soagen_bug_report/{relative_file}')

    log.i(r'Cleaning up')
    utils.delete_directory(paths.BUG_REPORT_DIR)

    log.i(
        f'{log.STYLE_CYAN}Zip generated: {bug_report_zip}\n'
        f'Please attach this file when you make a report at github.com/marzer/soagen/issues, thanks!{log.STYLE_RESET}'
    )


def update(
    new_version: tuple = None,
    update_templated_hpps=True,
    update_soagen_hpp=True,
    update_examples=True,
    update_tests=True,
    update_documentation=True,
):
    # set the version number
    if new_version is not None:
        assert isinstance(new_version, tuple)
        assert len(new_version) == 3
        for val in new_version:
            assert isinstance(val, int) and val >= 0
        global VERSION
        global VERSION_STRING
        if VERSION != new_version:
            new_version_str = rf'{new_version[0]}.{new_version[1]}.{new_version[2]}'
            log.i(rf'Changing version number: v{VERSION_STRING} -> v{new_version_str}')
            matcher = re.compile(rf'\b({VERSION[0]})[.]({VERSION[1]})[.]({VERSION[2]})\b')
            matcher_v = re.compile(rf'\b([vV])({VERSION[0]})[.]({VERSION[1]})[.]({VERSION[2]})\b')

            # update version
            VERSION = new_version
            VERSION_STRING = new_version_str
            assert VERSION == new_version
            assert VERSION_STRING == new_version_str
            log.i(rf'Writing {paths.VERSION_TXT}')
            with open(paths.VERSION_TXT, 'w', encoding='utf-8', newline='\n') as f:
                print(new_version_str, file=f)

            # regenerate version.hpp
            version_hpp = paths.HPP_GENERATED / r'version.hpp'
            log.i(rf'Writing {version_hpp}')
            with open(version_hpp, 'w', encoding='utf-8', newline='\n') as f:
                f.write(
                    rf'''
//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#define SOAGEN_VERSION_MAJOR {VERSION[0]}
#define SOAGEN_VERSION_MINOR {VERSION[1]}
#define SOAGEN_VERSION_PATCH {VERSION[2]}
#define SOAGEN_VERSION_STRING "{VERSION_STRING}"
'''.lstrip()
                )

            # update version in all the other files
            DIRS = (
                (paths.REPOSITORY, False),
                (paths.DOCS, True),
                (paths.EXAMPLES, True),
                (paths.SRC, True),
                (paths.TESTS, True),
            )
            for dir, recursive in DIRS:
                for file in utils.enumerate_files(
                    dir,
                    any=(
                        r'*.html',
                        r'*.py',
                        r'*.txt',
                        r'*.md',
                        r'*.hpp',
                        r'*.cpp',
                        r'*.dox',
                        r'*.toml',
                        r'*.natvis',
                        r'meson.build',
                    ),
                    recursive=recursive,
                ):
                    if str(file).find('.egg-info') != -1 or file.name.lower.find('changelog') != -1:
                        continue
                    text = utils.read_all_text_from_file(file, logger=log.d)
                    new_text = matcher.sub(VERSION_STRING, text)
                    new_text = matcher_v.sub(lambda m: m[1] + VERSION_STRING, new_text)
                    if text != new_text:
                        log.i(rf'Writing updated version numbers in {file}')
                        with open(file, 'w', encoding='utf-8', newline='\n') as f:
                            f.write(new_text)

    # copy .clang-format
    if update_templated_hpps or update_soagen_hpp or update_tests or update_examples:
        utils.copy_file(Path(paths.REPOSITORY, r'.clang-format'), paths.HPP, logger=log.i)

    # regenerate the generated headers from their templates
    if update_templated_hpps:
        if paths.MAKE_SINGLE.exists():
            for template in utils.enumerate_files(paths.HPP_TEMPLATES, all='*.hpp.in', recursive=True):
                output = Path(paths.HPP_GENERATED, template.stem)
                os.makedirs(str(paths.HPP_GENERATED), exist_ok=True)
                utils.run_python_script(
                    *(
                        [
                            paths.MAKE_SINGLE,
                            str(template),
                            r'--output',
                            output,
                            r'--namespaces',
                            r'soagen',
                            r'detail',
                            r'--macros',
                            r'SOAGEN',
                        ]
                        + ([r'--no-strip-hidden-bases'] if template.name not in (r'compressed_pair.hpp.in',) else [])
                        + (
                            [r'--no-strip-doxygen-from-snippets']
                            if template.name not in (r'compressed_pair.hpp.in', r'preprocessor.hpp.in')
                            else []
                        )
                    )
                )
                text = utils.read_all_text_from_file(output, logger=log.i)
                try:
                    text = utils.clang_format(text, cwd=output.parent)
                except:
                    pass
                log.i(rf'Writing {output}')
                with open(output, 'w', encoding='utf-8', newline='\n') as f:
                    f.write(text)
        else:
            log.w(rf'could not regenerate headers using muu: {paths.MAKE_SINGLE.name} did not exist or was not a file')

    # read soagen.hpp + preprocess into single header
    if update_soagen_hpp:
        soagen_hpp_in = Path(paths.HPP, 'soagen.hpp')
        soagen_hpp_out = Path(paths.HPP_SINGLE, 'soagen.hpp')
        text = str(Preprocessor(soagen_hpp_in))
        text = utils.replace_metavar(r'version', VERSION_STRING, text)
        license = utils.read_all_text_from_file(paths.REPOSITORY / r'LICENSE.txt', log.i).replace('\r\n', '\n').strip()
        license = '\n// '.join(utils.reflow_text(license, line_length=117).split('\n'))
        text = utils.replace_metavar(r'license', license, text)
        try:
            text = utils.clang_format(text, cwd=soagen_hpp_in.parent)
        except:
            pass
        log.i(rf'Writing {soagen_hpp_out}')
        os.makedirs(str(paths.HPP_SINGLE), exist_ok=True)
        with open(soagen_hpp_out, 'w', encoding='utf-8', newline='\n') as f:
            f.write(text)

    # re-run soagen itself on the examples and tests
    if update_examples and paths.EXAMPLES.exists():
        launch_worker(r'*.toml', r'--doxygen', cwd=paths.EXAMPLES)
    if update_tests and paths.TESTS.exists():
        launch_worker(r'*.toml', r'--no-doxygen', cwd=paths.TESTS)

    # invoke poxy on the documentation
    if update_documentation and utils.is_tool(r'poxy') and paths.DOCS.is_dir():
        subprocess.run(args=[r'poxy'], cwd=str(paths.DOCS), encoding=r'utf-8')


class NonExitingArgParser(argparse.ArgumentParser):
    def error(self, message):
        self.print_usage(sys.stderr)
        raise Error(message)


def main_impl():
    # yapf: disable
    args = NonExitingArgParser(
        description=
        rf'{log.STYLE_CYAN}'
        r'''  ___  ___   __ _  __ _  ___ _ __  ''' '\n'
        r''' / __|/ _ \ / _` |/ _` |/ _ \ '_ \ ''' '\n'
        r''' \__ \ (_) | (_| | (_| |  __/ | | |''' '\n'
        r''' |___/\___/ \__,_|\__, |\___|_| |_|''' '\n'
        r'''                   __/ |           ''' '\n'
        r'''                  |___/  '''  rf'{log.STYLE_RESET}'
        rf' v{VERSION_STRING} - github.com/marzer/soagen'
        '\n\n'
        r'Struct-of-Arrays generator for C++ projects.',
        formatter_class=argparse.RawTextHelpFormatter,
        exit_on_error=False
    )
    # yapf: enable

    # --------------------------------------------------------------
    # public (user-facing, documented) arguments
    # --------------------------------------------------------------

    args.add_argument(
        r'configs',
        type=str,
        nargs=r'*',
        help="zero or more .toml files describing your structures-of-arrays\n(wildcards are accepted, e.g. soa/*.toml)",
    )
    args.add_argument(r'-v', r'--verbose', action=r'store_true', help=r"enable very noisy diagnostic output")
    args.add_argument(r'--version', action=r'store_true', help=r"print the version and exit", dest=r'print_version')
    args.add_argument(
        r'--install',  #
        type=Path,
        default=None,
        metavar=r'<dir>',
        help=rf"install {log.STYLE_CYAN}soagen.hpp{log.STYLE_RESET} into a directory",
    )
    args.add_argument(
        r'--werror',  #
        action=argparse.BooleanOptionalAction,
        default=False,
        help=f'treat {log.STYLE_YELLOW}warnings{log.STYLE_RESET} as {log.STYLE_RED}errors{log.STYLE_RESET} (default: %(default)s)',
    )
    args.add_argument(
        r'--color',  #
        action=argparse.BooleanOptionalAction,
        default=None,
        help=f'use {log.STYLE_RED}c{log.STYLE_YELLOW}o{log.STYLE_GREEN}l{log.STYLE_BLUE}o'
        + f'{log.STYLE_MAGENTA}r{log.STYLE_CYAN}s{log.STYLE_RESET} in terminal output (default: True)\n'
        r'(the British spelling "colour" is also accepted)',
    )
    args.add_argument(
        r'--clang-format',  #
        action=argparse.BooleanOptionalAction,
        default=True,
        help=f'attempt to run {log.STYLE_CYAN}clang-format{log.STYLE_RESET} on generated code (default: %(default)s)',
    )
    args.add_argument(
        r'--doxygen',  #
        action=argparse.BooleanOptionalAction,
        default=False,
        help=f'include {log.STYLE_CYAN}doxygen{log.STYLE_RESET} markup in the generated code (default: %(default)s)',
    )
    args.add_argument(
        r'--natvis',  #
        action=argparse.BooleanOptionalAction,
        default=True,
        help=f'generate {log.STYLE_CYAN}.natvis{log.STYLE_RESET} files for Visual Studio (default: %(default)s)',
    )
    args.add_argument(
        r'--bug-report', action=r'store_true', help=r"capture all inputs and outputs in a bug-report zip file"
    )  #

    # --------------------------------------------------------------
    # hidden/developer-only/deprecated/diagnostic arguments
    # --------------------------------------------------------------
    args.add_argument(r'--where', action=r'store_true', help=argparse.SUPPRESS)
    args.add_argument(r'--update', nargs='?', const=True, default=False, help=argparse.SUPPRESS)
    args.add_argument(r'--update-hpp', action=r'store_true', help=argparse.SUPPRESS)
    args.add_argument(r'--update-docs', action=r'store_true', help=argparse.SUPPRESS)
    args.add_argument(r'--bug-report-internal', action=r'store_true', help=argparse.SUPPRESS)
    args.add_argument(
        r'--colour', action=argparse.BooleanOptionalAction, default=None, dest='color', help=argparse.SUPPRESS
    )
    args.add_argument(r'--worker', action=r'store_true', help=argparse.SUPPRESS)

    usage_str = args.format_usage()
    args = args.parse_args()

    log.reinit(
        min_level=logging.DEBUG if args.verbose else logging.INFO,
        treat_warnings_as_errors=args.werror,
        on_warning=(lambda: sys.exit(1)) if args.werror else None,
        on_error=lambda: sys.exit(1),
    )

    # --------------------------------------------------------------
    # --version
    # --------------------------------------------------------------

    if args.print_version:
        print(VERSION_STRING)
        return

    # --------------------------------------------------------------
    # --where
    # --------------------------------------------------------------

    if args.where:
        print(paths.PACKAGE)
        return

    if not args.worker:
        log.i(rf'{log.STYLE_CYAN}soagen{log.STYLE_RESET} v{VERSION_STRING}')

    # --------------------------------------------------------------
    # bug report invocation
    # --------------------------------------------------------------

    if args.bug_report:
        bug_report()
        return

    # --------------------------------------------------------------
    # regular invocation
    # --------------------------------------------------------------

    done_work = False

    if args.bug_report_internal:
        os.makedirs(str(paths.BUG_REPORT_OUTPUTS), exist_ok=True)

        # bug reports should always copy the submitter's version of
        # soagen.hpp in case the report is related to soagen.hpp
        utils.copy_file(paths.HPP / r'soagen.hpp', paths.BUG_REPORT_DIR, logger=log.i)

    if args.update:
        new_version = None
        if isinstance(args.update, str):
            args.update = args.update.strip()
            if args.update:
                m = re.fullmatch(r'\s*[vV]?\s*([0-9]+)\s*[.]+\s*([0-9]+)\s*[.]+\s*([0-9]+)\s*', args.update)
                if m:
                    new_version = (int(m[1]), int(m[2]), int(m[3]))
        done_work = True
        update(new_version)
    elif args.update_hpp or args.update_docs:
        done_work = True
        update(
            update_templated_hpps=False,
            update_soagen_hpp=args.update_hpp,
            update_examples=False,
            update_tests=False,
            update_documentation=args.update_docs,
        )

    if args.install is not None:
        done_work = True
        args.install: Path
        if not args.install.exists() or not args.install.is_dir():
            log.e(rf"--install: path '{args.install}' did not exist or was not a directory")
        log.i(rf"Copying soagen.hpp to {args.install.resolve()}")
        utils.copy_file(paths.HPP_SINGLE / r'soagen.hpp', args.install)

    configs = []
    for p in args.configs:
        if p.find('*') != -1:
            configs = configs + [f for f in Path('.').glob(p) if f.is_file()]
        else:
            p = Path(p)
            if not p.exists() or not p.is_file():
                log.e(rf"configs: '{p}' did not exist or was not a file")
            configs.append(p)
    configs.sort()
    if args.bug_report_internal:
        os.makedirs(str(paths.BUG_REPORT_INPUTS), exist_ok=True)
        for f in configs:
            utils.copy_file(f, paths.BUG_REPORT_INPUTS, log.i)
    configs = [Config(f) for f in configs]
    done_work = done_work or bool(configs)

    if configs:
        if args.clang_format and not utils.is_tool(r'clang-format'):
            log.w(r'clang-format not found on system PATH')
            args.clang_format = False

        for config in configs:
            for src in (config.hpp,):

                def on_flush(o: Writer, s: str) -> str:
                    nonlocal src
                    # sub in external headers
                    includes = sorted(set(src.includes.external + cpp.detect_includes(s)))
                    includes = cpp.remove_implicit_includes(includes)
                    PATTERN = r'\n[ \t]*//[ \t]*####[ \t]+SOAGEN_EXTERNAL_HEADERS[ \t]+####[ \t]*\n'
                    rep = '\nSOAGEN_DISABLE_WARNINGS;'
                    for inc in includes:
                        rep += f'\n#include <{inc}>'
                    rep += '\n#if SOAGEN_HAS_EXCEPTIONS\n\t#include <stdexcept>\n#endif'
                    rep += '\nSOAGEN_ENABLE_WARNINGS;\n'
                    s = re.sub(PATTERN, rep, s)
                    # strip doxygen stuff if we have that disabled
                    if not o.doxygen:
                        s = re.sub(
                            r'\n[ \t]*#[ \t]*if[ \t]*SOAGEN_DOXYGEN[ \t]*\n.+?\n[ \t]*#[ \t]*endif[ \t]*\n',
                            '\n',
                            s,
                            flags=re.DOTALL,
                        )
                        s = re.sub(
                            r'SOAGEN_ENABLE_IF_T\s*\(\s*(.+?)\s*,\s*sfinae\s*\)',
                            r'std::enable_if_t<sfinae, \1>',
                            s,
                            flags=re.DOTALL,
                        )
                        s = re.sub(
                            r'SOAGEN_ENABLE_IF\s*\(\s*\(\s*(.+?)\s*\)\s*\)',  # two sets of brackets
                            r', std::enable_if_t<(\1), int> = 0',
                            s,
                            flags=re.DOTALL,
                        )
                        s = re.sub(  # one set of brackets
                            r'SOAGEN_ENABLE_IF\s*\(\s*(.+?)\s*\)',
                            r', std::enable_if_t<\1, int> = 0',
                            s,
                            flags=re.DOTALL,
                        )
                        s = re.sub(
                            r'SOAGEN_HIDDEN_PARAM\s*\(\s*bool\s*sfinae\s*=\s*\(\s*(.+?)\s*\)\s*\)',  # two sets of brackets
                            r', bool sfinae = (\1)',
                            s,
                            flags=re.DOTALL,
                        )
                        s = re.sub(r'SOAGEN_HIDDEN_BASE\s*\(\s*(.+?)\s*\)', r': \1', s, flags=re.DOTALL)
                        s = re.sub(
                            r'SOAGEN_HIDDEN\s*\(\s*template\s*<(.+?)\s*>\s*\)', r'template <\1>', s, flags=re.DOTALL
                        )
                    return s

                with Writer(
                    src.path,
                    meta=config.meta_stack,
                    clang_format=args.clang_format,
                    doxygen=args.doxygen,
                    on_flush=on_flush,
                ) as o:
                    src.write(o)

                if args.bug_report_internal:
                    utils.copy_file(src.path, paths.BUG_REPORT_OUTPUTS, logger=log.i)

            if args.natvis:
                for src in (config.natvis,):
                    with Writer(src.path, meta=config.meta_stack, clang_format=False, doxygen=False) as o:
                        src.write(o)

                if args.bug_report_internal:
                    utils.copy_file(src.path, paths.BUG_REPORT_OUTPUTS, logger=log.i)

    if not args.worker:
        if done_work:
            log.i(r'All done!')
        else:
            log.i(r'No work to do.')
            if len(sys.argv) == 1:
                log.i(usage_str)


def main():
    allow_styles = True
    for arg in (r'--no-color', r'--no-colour'):
        if arg in sys.argv[1:]:
            allow_styles = False
            break
    log.init(allow_styles=allow_styles)
    try:
        try:
            main_impl()
        except BaseException as err:
            log.clear_hooks()
            raise err from None
    except SystemExit as exit:
        raise exit from None
    except SchemaError as err:
        log.e(rf'{current_schema_context()}{err}')
        sys.exit(1)
    except (Error, SchemaError, argparse.ArgumentError) as err:
        log.e(err)
        sys.exit(1)
    except BaseException as err:
        with StringIO() as buf:
            buf.write(
                f'\n{log.STYLE_RED_DIM}*************{log.STYLE_RESET}\n\n'
                'You appear to have triggered an internal bug!'
                f'\n{log.STYLE_CYAN}Please re-run soagen with --bug-report and file an issue at github.com/marzer/soagen/issues{log.STYLE_RESET}'
                '\nMany thanks!'
                f'\n\n{log.STYLE_RED_DIM}*************{log.STYLE_RESET}\n\n'
            )
            utils.print_exception(err, include_type=True, include_traceback=True, skip_frames=1, logger=buf)
            buf.write(f'{log.STYLE_RED_DIM}*************{log.STYLE_RESET}\n')
            print(buf.getvalue(), file=sys.stderr)
        sys.exit(1)
    sys.exit(0)


if __name__ == '__main__':
    main()
