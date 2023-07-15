//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT

//# note: any preprocessor/compiler state modified here must be undone in header_end.hpp

#include "generated/preprocessor.hpp"

//# push the current warning state
SOAGEN_PUSH_WARNINGS;
SOAGEN_DISABLE_SPAM_WARNINGS;

//# disable windows.h min/max macro crimes
#if SOAGEN_MSVC_LIKE
	#pragma push_macro("min")
	#pragma push_macro("max")
	#undef min
	#undef max
#endif

//# set optimizations
#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma inline_recursion(on)
		#pragma optimize("gt", on)
		#pragma runtime_checks("", off)
		#pragma strict_gs_check(push, off)
	#elif SOAGEN_GCC
		#pragma GCC push_options
		#pragma GCC optimize("O2")
	#endif
#endif
