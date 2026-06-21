# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

example_names = [
	'employees',
	'entities',
	'shapes'
]

# regenerate the committed example headers from their .toml on every build (write-if-changed, so unchanged
# headers don't bust downstream rebuilds), then build each example against them.
example_tomls = []
foreach name : example_names
	if fs.is_file(name + '.toml')
		example_tomls += files(name + '.toml')
	endif
endforeach

examples_stamp = custom_target(
	'soagen_examples',
	input: example_tomls,
	output: 'soagen_examples.stamp',
	command: [ python, '-m', 'soagen', '@INPUT@', '--doxygen', '--stamp', '@OUTPUT@' ],
	env: { 'PYTHONPATH': meson.project_source_root() / 'src' },
	build_by_default: true,
	build_always_stale: true,
	console: true,
)
examples_regen_dep = declare_dependency(sources: examples_stamp)

example_args = []
example_args += global_args

example_link_args = []
example_link_args += global_link_args

example_overrides = []
example_overrides += global_overrides

example_executables = []
foreach name : example_names

	example_cpp_files = []
	example_extra_files = [ cpp_hint ]
	foreach suffix : ['.hpp','.cpp','.toml','.natvis']
		if fs.is_file(name + suffix)
			if suffix == '.cpp'
				example_cpp_files += files(name + suffix)
			else
				example_extra_files += files(name + suffix)
			endif
		endif
	endforeach

	example_executables += [[
		name,
		executable(
			name,
			example_cpp_files,
			dependencies: [ soagen_dep, soagen_single_regen_dep, examples_regen_dep ],
			cpp_args: example_args,
			link_args: example_link_args,
			override_options: example_overrides,
			extra_files: example_extra_files
		)
	]]

endforeach
