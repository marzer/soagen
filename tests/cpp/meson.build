# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

assert(build_tests)

#-----------------------------------------------------------------------------------------------------------------------
# regenerate the committed test fixture headers (soa.hpp etc.) from their .toml on every build (write-if-changed,
# so unchanged headers don't bust downstream rebuilds)
#-----------------------------------------------------------------------------------------------------------------------

tests_stamp = custom_target(
	'soagen_tests',
	input: files('soa.toml'),
	output: 'soagen_tests.stamp',
	command: [ python, '-m', 'soagen', '@INPUT@', '--no-doxygen', '--stamp', '@OUTPUT@' ],
	env: { 'PYTHONPATH': meson.project_source_root() / 'src' },
	build_by_default: true,
	build_always_stale: true,
	console: true,
)
tests_regen_dep = declare_dependency(sources: tests_stamp)

#-----------------------------------------------------------------------------------------------------------------------
# the test suite
#-----------------------------------------------------------------------------------------------------------------------

test_file_names = [
	'tests',
	'settings',
	'lib_catch2',
	'soa',
	'traits',
	'table',
	'access',
	'rows',
	'spans',
	'iterators',
	'for_each',
	'utility',
	'allocator',
	'throwing',
	'source_offset',
]
test_cpp_files = []
test_extra_files = [ cpp_hint ]
foreach name : test_file_names
	foreach suffix : ['.hpp','.cpp','.toml','.natvis']
		if fs.is_file(name + suffix)
			if suffix == '.cpp'
				test_cpp_files += files(name + suffix)
			else
				test_extra_files += files(name + suffix)
			endif
		endif
	endforeach
endforeach

test_args = []
test_args += global_args
test_args += cpp.get_supported_arguments('-fno-var-tracking-assignments', '-fno-var-tracking')

test_link_args = []
test_link_args += global_link_args

if is_clang and is_coverage
	test_args += [ '-fprofile-instr-generate', '-fcoverage-mapping' ]
	test_link_args += [ '-fprofile-instr-generate' ]
endif

test_overrides = []
test_overrides += global_overrides

# the catch2 suite needs exceptions; under -Dcpp_eh=none only the standalone testers below are built
if has_exceptions
	test_dependencies = [ soagen_dep, soagen_single_regen_dep, tests_regen_dep ]
	test_dependencies += subproject('catch2', default_options: subproject_overrides).get_variable('catch2_with_main_dep')

	test_exe = executable(
		meson.project_name() + '_tests',
		test_cpp_files,
		cpp_args: test_args,
		link_args: test_link_args,
		dependencies: test_dependencies,
		override_options: test_overrides,
		extra_files: test_extra_files
	)

	test(meson.project_name() + '_tests', test_exe)
endif

#-----------------------------------------------------------------------------------------------------------------------
# single-header tester; just to enforce the use of the single-header version
#-----------------------------------------------------------------------------------------------------------------------

single_header_exe = executable(
	meson.project_name() + '_single_header',
	files('single_header.cpp'),
	cpp_args: test_args,
	link_args: test_link_args,
	override_options: test_overrides,
	dependencies: [ soagen_single_dep, tests_regen_dep ],
)

test(meson.project_name() + '_single_header', single_header_exe)

#-----------------------------------------------------------------------------------------------------------------------
# no-exceptions tester; compiles the rollback machinery with exceptions disabled (exercised by the -Dcpp_eh=none CI job)
#-----------------------------------------------------------------------------------------------------------------------

no_exceptions_exe = executable(
	meson.project_name() + '_no_exceptions',
	files('no_exceptions.cpp'),
	cpp_args: test_args,
	link_args: test_link_args,
	override_options: test_overrides,
	dependencies: [ soagen_dep, tests_regen_dep ],
)

test(meson.project_name() + '_no_exceptions', no_exceptions_exe)

#-----------------------------------------------------------------------------------------------------------------------
# coverage report (clang source-based; see root meson.build)
#-----------------------------------------------------------------------------------------------------------------------

if has_exceptions and is_coverage
	run_target('coverage-report',
		command: [ python, files('coverage.py'), test_exe, meson.current_build_dir() / 'coverage' ],
	)
endif
