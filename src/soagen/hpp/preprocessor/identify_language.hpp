//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#ifndef SOAGEN_CPP
    #ifdef _MSVC_LANG
        #if _MSVC_LANG > __cplusplus
            #define SOAGEN_CPP _MSVC_LANG
        #endif
    #endif
    #ifndef SOAGEN_CPP
        #define SOAGEN_CPP __cplusplus
    #endif
    #if SOAGEN_CPP >= 202900L
        #undef SOAGEN_CPP
        #define SOAGEN_CPP 29
    #elif SOAGEN_CPP >= 202600L
        #undef SOAGEN_CPP
        #define SOAGEN_CPP 26
    #elif SOAGEN_CPP >= 202302L
        #undef SOAGEN_CPP
        #define SOAGEN_CPP 23
    #elif SOAGEN_CPP >= 202002L
        #undef SOAGEN_CPP
        #define SOAGEN_CPP 20
    #elif SOAGEN_CPP >= 201703L
        #undef SOAGEN_CPP
        #define SOAGEN_CPP 17
    #else
        #undef SOAGEN_CPP
        #define SOAGEN_CPP 0
    #endif
#endif
#if !SOAGEN_CPP
    #error soagen requires C++17 or higher.
#endif
