//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT

//# note: this header is the exact reverse of header_start.hpp

#include "preprocessor.hpp"

// __SOAGEN_HEADER_END{{

// reset optimizations
#if SOAGEN_ALWAYS_OPTIMIZE
    #if SOAGEN_MSVC_LIKE
        #pragma float_control(pop)
        #pragma strict_gs_check(pop)
        #pragma runtime_checks("", restore)
        #if !SOAGEN_CLANG
            #pragma optimize("", on)
        #endif
        #pragma inline_recursion(off)
    #endif
#endif

// pop current states
#if SOAGEN_MSVC_LIKE || SOAGEN_GCC_LIKE
    #pragma pop_macro("min")
    #pragma pop_macro("max")
    #pragma pop_macro("finite")
#endif
#if SOAGEN_GCC
    #pragma GCC pop_options
#endif
SOAGEN_POP_WARNINGS;

// }}__SOAGEN_HEADER_END
