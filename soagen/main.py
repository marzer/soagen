#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import argparse
import logging
import re
import sys
from io import StringIO
from pathlib import Path

from colorama import Fore, Style
from colorama import init as colorama_init

from . import log, paths, utils
from .config import Config
from .errors import Error, SchemaError
from .preprocessor import Preprocessor
from .writer import Writer



def bug_report(args: argparse.Namespace):

	bug_report_directory = (Path.cwd() / r'soagen_bug_report').absolute()
	bug_report_zip = (Path.cwd() / r'soagen_bug_report.zip').absolute()
	# todo



def update_hpp(preprocess=True):
	# regenerate all the source headers from the templates
	for template in utils.enumerate_files(paths.HPP_TEMPLATES, all='*.hpp.in', recursive=True):
		output = Path(paths.HPP, template.stem)
		utils.run_python_script(
			paths.MAKE_SINGLE, str(template), r'--output', output, r'--namespaces', r'soagen', r'detail', r'--macros',
			r'SOAGEN'
		)
		text = utils.read_all_text_from_file(output, logger=log.i)
		try:
			text = utils.clang_format(text, cwd=output.parent)
		except:
			pass
		log.i(rf'Writing {output}')
		with open(output, 'w', encoding='utf-8', newline='\n') as f:
			f.write(text)
	# read soagen.hpp + preprocess into single header
	if preprocess:
		soagen_hpp = Path(paths.HPP, 'soagen.hpp')
		text = str(Preprocessor(soagen_hpp))
		# clang-format
		try:
			text = utils.clang_format(text, cwd=soagen_hpp.parent)
		except:
			pass
		log.i(rf'Writing {soagen_hpp}')
		with open(output, 'w', encoding='utf-8', newline='\n') as f:
			f.write(text)



class NonExitingArgParser(argparse.ArgumentParser):

	def error(self, message):
		self.print_usage(sys.stderr)
		raise Error(message)



def main_impl():
	# yapf: disable
	args = NonExitingArgParser(
		description=
		rf'{Fore.CYAN}'
		r'''  ___  ___   __ _  __ _  ___ _ __  ''' '\n'
		r''' / __|/ _ \ / _` |/ _` |/ _ \ '_ \ ''' '\n'
		r''' \__ \ (_) | (_| | (_| |  __/ | | |''' '\n'
		r''' |___/\___/ \__,_|\__, |\___|_| |_|''' '\n'
		r'''                   __/ |           ''' '\n'
		r'''                  |___/  '''  rf'{Style.RESET_ALL}'
		rf' v{utils.lib_version_string()} - github.com/marzer/soagen'
		'\n\n'
		r'Struct-of-Arrays generator for C++ projects.',
		formatter_class=argparse.RawTextHelpFormatter,
		exit_on_error=False
	)
	# yapf: enable

	#--------------------------------------------------------------
	# public (user-facing, documented) arguments
	#--------------------------------------------------------------

	args.add_argument(r'files', type=str, nargs=r'*')
	args.add_argument(
		r'-v',  #
		r'--verbose',
		action=r'store_true',
		help=r"enable very noisy diagnostic output"
	)
	args.add_argument(
		r'--version',  #
		action=r'store_true',
		help=r"print the version and exit",
		dest=r'print_version'
	)
	args.add_argument(
		r'--werror', action=argparse.BooleanOptionalAction, default=False, help=r'treat warnings as errors'
	)
	args.add_argument(
		r'--clang-format',
		action=argparse.BooleanOptionalAction,
		default=True,
		help=r'run clang-format on generated code if it is available'
	)
	args.add_argument(
		r'--doxygen',
		action=argparse.BooleanOptionalAction,
		default=True,
		help=r'include doxygen markup in the generated code'
	)
	args.add_argument(
		r'--bug-report',  #
		action=r'store_true',
		help=r"captures all inputs and output in a zip file for easier bug reporting."
	)

	#--------------------------------------------------------------
	# hidden/developer-only/deprecated/diagnostic arguments
	#--------------------------------------------------------------

	args.add_argument(
		r'--update',  #
		action=r'store_true',
		help=argparse.SUPPRESS
	)

	args.add_argument(
		r'--preprocess',  #
		action=argparse.BooleanOptionalAction,
		default=True,
		help=argparse.SUPPRESS
	)

	args.add_argument(
		r'--dev',  #
		action=r'store_true',
		help=argparse.SUPPRESS
	)

	args = args.parse_args()
	if args.dev:
		args.update = True
		args.preprocess = False
		args.werror = True
		args.verbose = True

	log.init(
		min_level=logging.DEBUG if args.verbose else logging.INFO,
		treat_warnings_as_errors=args.werror,
		on_warning=(lambda: sys.exit(1)) if args.werror else None,
		on_error=lambda: sys.exit(1)
	)

	#--------------------------------------------------------------
	# --version
	#--------------------------------------------------------------

	if args.print_version:
		print(utils.lib_version_string())
		return

	#--------------------------------------------------------------
	# bug report invocation
	#--------------------------------------------------------------

	if args.bug_report:
		bug_report(args)
		return

	#--------------------------------------------------------------
	# regular invocation
	#--------------------------------------------------------------

	log.i(rf'{Fore.CYAN}soagen{Style.RESET_ALL} v{utils.lib_version_string()}')

	done_work = False

	if args.update:
		done_work = True
		update_hpp(preprocess=args.preprocess)
		utils.copy_file(Path(paths.REPOSITORY, r'.clang-format'), Path(paths.DATA, r'.clang-format'), logger=log.i)

	configs = []
	for p in args.files:
		if p.find('*') != -1:
			configs = configs + [f for f in Path('.').glob(p) if f.is_file()]
		else:
			p = Path(p)
			if not p.exists() or not p.is_file():
				log.e(rf"'{p}' did not exist or was not a file")
			configs.append(p)
	configs.sort()
	if not configs:
		if not done_work:
			log.i('No work to do.')
		return 0
	configs = [Config(f) for f in configs]

	def get_cascading_pool_property(src, name: str, default: bool) -> bool:
		nonlocal args
		try:
			return bool(getattr(args, name))
		except:
			pass
		try:
			return bool(getattr(src.config, name))
		except:
			pass
		try:
			return bool(getattr(src, name))
		except:
			pass
		return bool(default)

	def should_clang_format(src):
		if not get_cascading_pool_property(src, 'clang_format', True):
			return False
		if getattr(should_clang_format, 'ok', None) is None:
			setattr(should_clang_format, 'ok', utils.is_tool(r'clang-format'))
			if not should_clang_format.ok:
				log.w(r'clang-format not found on system PATH')
		if not should_clang_format.ok:
			return False
		return True

	def should_doxygen(src):
		return get_cascading_pool_property(src, 'doxygen', True)

	for config in configs:

		for src in config.hpp:
			with Writer(
				src.path, meta=config.meta_stack, clang_format=should_clang_format(src), doxygen=should_doxygen(src)
			) as o:
				src.write(o)

		#src = config.cpp
		#with Writer(src.path, meta=config.meta_stack, clang_format=should_clang_format(src), doxygen=should_doxygen(src)) as o:
		#	src.write(o)

		#src = config.natvis
		#with Writer(src.path, meta=config.meta_stack, clang_format=should_clang_format(src), doxygen=should_doxygen(src)) as o:
		#	src.write(o)



def main():
	colorama_init()
	try:
		try:
			main_impl()
		except BaseException as err:
			log.clear_hooks()
			raise err from None
	except SystemExit as exit:
		raise exit from None
	except (Error, SchemaError) as err:
		log.e(err)
		sys.exit(1)
	except BaseException as err:
		with StringIO() as buf:
			buf.write(
				'You appear to have triggered an internal bug!'
				'\nPlease re-run soagen with --bug-report and file an issue at github.com/marzer/soagen/issues'
				'\nMany thanks!'
				'\n\n*************\n\n'
			)
			utils.print_exception(err, include_type=True, include_traceback=True, skip_frames=1, logger=buf)
			buf.write('\n*************\n')
			log.e(buf.getvalue())
		sys.exit(1)
	sys.exit(0)



if __name__ == '__main__':
	main()
