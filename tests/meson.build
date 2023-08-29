# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

assert(build_tests)

test_file_names = [
	'tests',
	'soa',
	'settings',
	'lib_catch2',
	'employees',
	'source_offset'
]
test_cpp_files = []
test_extra_files = [ soagen_hpp, cpp_hint ]
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

test_dependencies = [ soagen_dep ]
test_dependencies += subproject('catch2', default_options: subproject_overrides).get_variable('catch2_with_main_dep')

test_args = []
test_args += global_args
test_args += cpp.get_supported_arguments('-fno-var-tracking-assignments', '-fno-var-tracking')

test_link_args = []
test_link_args += global_link_args

test_overrides = []
test_overrides += global_overrides

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
