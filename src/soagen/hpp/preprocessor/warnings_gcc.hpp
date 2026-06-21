//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "concat.hpp"
#include "warnings_base.hpp"

#if SOAGEN_GCC >= 5
    #define SOAGEN_DISABLE_GCC_WARNING_GE_5(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_5(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 6
    #define SOAGEN_DISABLE_GCC_WARNING_GE_6(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_6(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 7
    #define SOAGEN_DISABLE_GCC_WARNING_GE_7(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_7(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 8
    #define SOAGEN_DISABLE_GCC_WARNING_GE_8(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_8(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 9
    #define SOAGEN_DISABLE_GCC_WARNING_GE_9(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_9(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 10
    #define SOAGEN_DISABLE_GCC_WARNING_GE_10(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_10(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 11
    #define SOAGEN_DISABLE_GCC_WARNING_GE_11(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_11(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 12
    #define SOAGEN_DISABLE_GCC_WARNING_GE_12(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_12(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 13
    #define SOAGEN_DISABLE_GCC_WARNING_GE_13(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_13(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 14
    #define SOAGEN_DISABLE_GCC_WARNING_GE_14(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_14(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 15
    #define SOAGEN_DISABLE_GCC_WARNING_GE_15(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_15(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 16
    #define SOAGEN_DISABLE_GCC_WARNING_GE_16(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_16(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 17
    #define SOAGEN_DISABLE_GCC_WARNING_GE_17(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_17(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 18
    #define SOAGEN_DISABLE_GCC_WARNING_GE_18(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_18(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 19
    #define SOAGEN_DISABLE_GCC_WARNING_GE_19(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_19(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 20
    #define SOAGEN_DISABLE_GCC_WARNING_GE_20(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_20(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 21
    #define SOAGEN_DISABLE_GCC_WARNING_GE_21(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_21(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 22
    #define SOAGEN_DISABLE_GCC_WARNING_GE_22(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_22(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 23
    #define SOAGEN_DISABLE_GCC_WARNING_GE_23(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_23(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 24
    #define SOAGEN_DISABLE_GCC_WARNING_GE_24(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_24(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 25
    #define SOAGEN_DISABLE_GCC_WARNING_GE_25(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_25(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 26
    #define SOAGEN_DISABLE_GCC_WARNING_GE_26(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_26(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 27
    #define SOAGEN_DISABLE_GCC_WARNING_GE_27(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_27(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 28
    #define SOAGEN_DISABLE_GCC_WARNING_GE_28(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_28(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 29
    #define SOAGEN_DISABLE_GCC_WARNING_GE_29(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_29(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 30
    #define SOAGEN_DISABLE_GCC_WARNING_GE_30(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_30(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 31
    #define SOAGEN_DISABLE_GCC_WARNING_GE_31(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_31(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 32
    #define SOAGEN_DISABLE_GCC_WARNING_GE_32(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_32(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 33
    #define SOAGEN_DISABLE_GCC_WARNING_GE_33(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_33(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 34
    #define SOAGEN_DISABLE_GCC_WARNING_GE_34(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_34(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 35
    #define SOAGEN_DISABLE_GCC_WARNING_GE_35(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_35(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 36
    #define SOAGEN_DISABLE_GCC_WARNING_GE_36(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_36(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 37
    #define SOAGEN_DISABLE_GCC_WARNING_GE_37(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_37(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 38
    #define SOAGEN_DISABLE_GCC_WARNING_GE_38(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_38(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 39
    #define SOAGEN_DISABLE_GCC_WARNING_GE_39(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_39(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 40
    #define SOAGEN_DISABLE_GCC_WARNING_GE_40(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_40(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 41
    #define SOAGEN_DISABLE_GCC_WARNING_GE_41(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_41(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 42
    #define SOAGEN_DISABLE_GCC_WARNING_GE_42(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_42(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 43
    #define SOAGEN_DISABLE_GCC_WARNING_GE_43(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_43(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 44
    #define SOAGEN_DISABLE_GCC_WARNING_GE_44(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_44(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 45
    #define SOAGEN_DISABLE_GCC_WARNING_GE_45(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_45(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 46
    #define SOAGEN_DISABLE_GCC_WARNING_GE_46(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_46(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 47
    #define SOAGEN_DISABLE_GCC_WARNING_GE_47(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_47(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 48
    #define SOAGEN_DISABLE_GCC_WARNING_GE_48(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_48(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 49
    #define SOAGEN_DISABLE_GCC_WARNING_GE_49(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_49(...) static_assert(true)
#endif

#if SOAGEN_GCC >= 50
    #define SOAGEN_DISABLE_GCC_WARNING_GE_50(...) SOAGEN_DISABLE_GCC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_GCC_WARNING_GE_50(...) static_assert(true)
#endif

#define SOAGEN_DISABLE_GCC_WARNING_GE(vers, ...) SOAGEN_CONCAT(SOAGEN_DISABLE_GCC_WARNING_GE_, vers)(__VA_ARGS__)
