//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#ifndef SOAGEN_UNIX
    #ifdef __unix__
        #define SOAGEN_UNIX 1
    #else
        #define SOAGEN_UNIX 0
    #endif
#endif
/// @def SOAGEN_UNIX
/// @brief `1` when building for a GNU/Unix variant, otherwise `0`.

#ifndef SOAGEN_LINUX
    #ifdef __linux__
        #define SOAGEN_LINUX 1
    #else
        #define SOAGEN_LINUX 0
    #endif
#endif
/// @def SOAGEN_LINUX
/// @brief `1` when building for a Linux distro, otherwise `0`.

#ifndef SOAGEN_WINDOWS
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        #define SOAGEN_WINDOWS 1
    #else
        #define SOAGEN_WINDOWS 0
    #endif
#endif
/// @def SOAGEN_WINDOWS
/// @brief `1` when building for the Windows operating system, otherwise `0`.

// todo: apple mac os? is unix sufficient?
