# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

subdir('single')

soagen_dep = declare_dependency(
	sources: [ files('soagen.hpp') ],
	include_directories: include_directories('.'),
)
meson.override_dependency(meson.project_name(), soagen_dep)
