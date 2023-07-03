# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

example_args = []
example_args += global_args

examples = [
	'particles'
]

example_executables = []
foreach example : examples
	example_executables += [[
		example,
		executable(
			example,
			[ example + '.cpp' ],
			dependencies: soagen_dep,
			cpp_args: example_args,
			link_args: global_link_args,
			override_options: global_overrides
		)
	]]
endforeach
