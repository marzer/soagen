# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

soagen_single_stamp = custom_target(
	'soagen_single_stamp',
	output: 'soagen_single.stamp',
	command: [ python, '-m', 'soagen', '--update-hpp', '--stamp', '@OUTPUT@', '--no-natvis' ],
	env: { 'PYTHONPATH': meson.project_source_root() / 'src' },
	build_by_default: true,
	build_always_stale: true,
	console: true,
)

soagen_single_regen_dep = declare_dependency(
	sources: [ soagen_single_stamp ]
)

soagen_single_dep = declare_dependency(
	sources: [ soagen_single_stamp ],
	include_directories: include_directories('.'),
)
