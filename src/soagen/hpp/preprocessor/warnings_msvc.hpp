//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "concat.hpp"
#include "warnings_base.hpp"

#if SOAGEN_MSVC >= 1900
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1900(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1900(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1901
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1901(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1901(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1902
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1902(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1902(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1903
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1903(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1903(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1904
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1904(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1904(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1905
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1905(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1905(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1906
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1906(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1906(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1907
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1907(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1907(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1908
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1908(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1908(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1909
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1909(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1909(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1910
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1910(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1910(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1911
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1911(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1911(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1912
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1912(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1912(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1913
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1913(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1913(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1914
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1914(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1914(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1915
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1915(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1915(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1916
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1916(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1916(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1917
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1917(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1917(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1918
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1918(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1918(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1919
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1919(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1919(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1920
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1920(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1920(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1921
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1921(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1921(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1922
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1922(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1922(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1923
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1923(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1923(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1924
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1924(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1924(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1925
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1925(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1925(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1926
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1926(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1926(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1927
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1927(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1927(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1928
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1928(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1928(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1929
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1929(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1929(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1930
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1930(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1930(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1931
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1931(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1931(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1932
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1932(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1932(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1933
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1933(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1933(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1934
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1934(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1934(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1935
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1935(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1935(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1936
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1936(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1936(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1937
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1937(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1937(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1938
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1938(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1938(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1939
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1939(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1939(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1940
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1940(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1940(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1941
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1941(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1941(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1942
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1942(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1942(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1943
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1943(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1943(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1944
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1944(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1944(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1945
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1945(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1945(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1946
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1946(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1946(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1947
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1947(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1947(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1948
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1948(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1948(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1949
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1949(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1949(...) static_assert(true)
#endif

#if SOAGEN_MSVC >= 1950
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1950(...) SOAGEN_DISABLE_MSVC_WARNING(__VA_ARGS__)
#else
    #define SOAGEN_DISABLE_MSVC_WARNING_GE_1950(...) static_assert(true)
#endif

#define SOAGEN_DISABLE_MSVC_WARNING_GE(vers, ...) SOAGEN_CONCAT(SOAGEN_DISABLE_MSVC_WARNING_GE_, vers)(__VA_ARGS__)
