# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

example_names = [
	'shapes'
]

example_args = []
example_args += global_args

example_link_args = []
example_link_args += global_link_args

example_overrides = []
example_overrides += global_overrides

example_executables = []
foreach name : example_names

	example_cpp_files = []
	example_extra_files = [ soagen_hpp, cpp_hint ]
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
			dependencies: soagen_dep,
			cpp_args: example_args,
			link_args: example_link_args,
			override_options: example_overrides,
			extra_files: example_extra_files
		)
	]]

endforeach
