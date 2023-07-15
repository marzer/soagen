//# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT

//# note: this header is the exact reverse of header_start.hpp

#include "generated/preprocessor.hpp"

//# reset optimizations
#if SOAGEN_ALWAYS_OPTIMIZE
	#if SOAGEN_MSVC
		#pragma strict_gs_check(pop)
		#pragma runtime_checks("", restore)
		#pragma optimize("", on)
		#pragma inline_recursion(off)
	#elif SOAGEN_GCC
		#pragma GCC pop_options
	#endif
#endif

//# re-enable windows.h min/max macro crimes
#if SOAGEN_MSVC_LIKE
	#pragma pop_macro("min")
	#pragma pop_macro("max")
#endif

//# pop the current warning state
SOAGEN_POP_WARNINGS;
