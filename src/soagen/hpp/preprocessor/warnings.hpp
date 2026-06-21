//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#include "warnings_base.hpp"
#include "warnings_compilers.hpp"

#ifndef SOAGEN_DISABLE_SWITCH_WARNINGS
    #define SOAGEN_DISABLE_SWITCH_WARNINGS                                                                             \
        /* clang */                                                                                                    \
        SOAGEN_DISABLE_CLANG_WARNING("-Wcovered-switch-default");                                                      \
        SOAGEN_DISABLE_CLANG_WARNING("-Wswitch-default");                                                              \
        SOAGEN_DISABLE_CLANG_WARNING("-Wswitch-enum");                                                                 \
        SOAGEN_DISABLE_CLANG_WARNING("-Wswitch");                                                                      \
        /* gcc */                                                                                                      \
        SOAGEN_DISABLE_GCC_WARNING("-Wswitch-default");                                                                \
        SOAGEN_DISABLE_GCC_WARNING("-Wswitch-enum");                                                                   \
        SOAGEN_DISABLE_GCC_WARNING("-Wswitch");                                                                        \
        /* msvc */                                                                                                     \
        SOAGEN_DISABLE_MSVC_WARNING(4061);                                                                             \
        SOAGEN_DISABLE_MSVC_WARNING(4062);                                                                             \
        SOAGEN_DISABLE_MSVC_WARNING(4063);                                                                             \
        SOAGEN_DISABLE_MSVC_WARNING(4468);  /* 'fallthrough': attribute must be followed by a case label */            \
        SOAGEN_DISABLE_MSVC_WARNING(5262);  /* implicit through */                                                     \
        SOAGEN_DISABLE_MSVC_WARNING(26819); /* cg: unannotated fallthrough */                                          \
        static_assert(true)

#endif

#ifndef SOAGEN_DISABLE_LIFETIME_WARNINGS
    #define SOAGEN_DISABLE_LIFETIME_WARNINGS                                                                           \
        /* clang */                                                                                                    \
        SOAGEN_DISABLE_CLANG_WARNING("-Wexit-time-destructors");                                                       \
        SOAGEN_DISABLE_CLANG_WARNING("-Wglobal-constructors");                                                         \
        SOAGEN_DISABLE_CLANG_WARNING("-Wmissing-field-initializers");                                                  \
        /* gcc */                                                                                                      \
        SOAGEN_DISABLE_GCC_WARNING("-Wclass-memaccess");                                                               \
        SOAGEN_DISABLE_GCC_WARNING("-Wdelete-non-virtual-dtor");                                                       \
        SOAGEN_DISABLE_GCC_WARNING("-Wnon-virtual-dtor");                                                              \
        SOAGEN_DISABLE_GCC_WARNING("-Wmaybe-uninitialized");                                                           \
        SOAGEN_DISABLE_GCC_WARNING("-Wmissing-field-initializers");                                                    \
        SOAGEN_DISABLE_GCC_WARNING("-Wuninitialized");                                                                 \
        static_assert(true)
#endif

#ifndef SOAGEN_DISABLE_ARITHMETIC_WARNINGS
    #define SOAGEN_DISABLE_ARITHMETIC_WARNINGS                                                                         \
        /* clang */                                                                                                    \
        SOAGEN_DISABLE_CLANG_WARNING_GE(10, "-Wimplicit-int-float-conversion");                                        \
        SOAGEN_DISABLE_CLANG_WARNING("-Wchar-subscripts");                                                             \
        SOAGEN_DISABLE_CLANG_WARNING("-Wdouble-promotion");                                                            \
        SOAGEN_DISABLE_CLANG_WARNING("-Wfloat-equal");                                                                 \
        SOAGEN_DISABLE_CLANG_WARNING("-Wimplicit-int-conversion");                                                     \
        SOAGEN_DISABLE_CLANG_WARNING("-Wshift-sign-overflow");                                                         \
        SOAGEN_DISABLE_CLANG_WARNING("-Wsign-compare");                                                                \
        SOAGEN_DISABLE_CLANG_WARNING("-Wsign-conversion");                                                             \
        /* gcc */                                                                                                      \
        SOAGEN_DISABLE_GCC_WARNING("-Wsign-promo");                                                                    \
        SOAGEN_DISABLE_GCC_WARNING_GE(10, "-Warith-conversion");                                                       \
        SOAGEN_DISABLE_GCC_WARNING("-Wchar-subscripts");                                                               \
        SOAGEN_DISABLE_GCC_WARNING("-Wdouble-promotion");                                                              \
        SOAGEN_DISABLE_GCC_WARNING("-Wfloat-conversion");                                                              \
        SOAGEN_DISABLE_GCC_WARNING("-Wfloat-equal");                                                                   \
        SOAGEN_DISABLE_GCC_WARNING("-Wlong-long");                                                                     \
        SOAGEN_DISABLE_GCC_WARNING("-Wpointer-arith");                                                                 \
        SOAGEN_DISABLE_GCC_WARNING("-Wshift-negative-value");                                                          \
        SOAGEN_DISABLE_GCC_WARNING("-Wsign-compare");                                                                  \
        SOAGEN_DISABLE_GCC_WARNING("-Wsign-conversion");                                                               \
        SOAGEN_DISABLE_GCC_WARNING("-Wconversion");                                                                    \
        /* msvc */                                                                                                     \
        SOAGEN_DISABLE_MSVC_WARNING(4244); /* int <-> x: possible loss of data */                                      \
        SOAGEN_DISABLE_MSVC_WARNING(4365); /* argument signed/unsigned mismatch */                                     \
        SOAGEN_DISABLE_MSVC_WARNING(4389); /* comparison signed/unsigned mismatch */                                   \
        SOAGEN_DISABLE_MSVC_WARNING(4738); /* storing 32-bit float result in memory */                                 \
        SOAGEN_DISABLE_MSVC_WARNING(5219); /* implicit conversion from integral to float */                            \
        static_assert(true)
#endif

#ifndef SOAGEN_DISABLE_SHADOW_WARNINGS
    #define SOAGEN_DISABLE_SHADOW_WARNINGS                                                                             \
        /* clang */                                                                                                    \
        SOAGEN_DISABLE_CLANG_WARNING("-Wshadow");                                                                      \
        SOAGEN_DISABLE_CLANG_WARNING("-Wshadow-field");                                                                \
        /* gcc */                                                                                                      \
        SOAGEN_DISABLE_GCC_WARNING("-Wshadow");                                                                        \
        /* msvc */                                                                                                     \
        SOAGEN_DISABLE_MSVC_WARNING(4458);                                                                             \
        SOAGEN_DISABLE_MSVC_WARNING(4459);                                                                             \
        static_assert(true)
#endif

#ifndef SOAGEN_DISABLE_SPAM_WARNINGS
    #define SOAGEN_DISABLE_SPAM_WARNINGS                                                                               \
        /* clang */                                                                                                    \
        SOAGEN_DISABLE_CLANG_WARNING_GE(10, "-Wzero-as-null-pointer-constant");                                        \
        SOAGEN_DISABLE_CLANG_WARNING_GE(11, "-Wsuggest-destructor-override");                                          \
        SOAGEN_DISABLE_CLANG_WARNING_GE(13, "-Wc++20-compat");                                                         \
        SOAGEN_DISABLE_CLANG_WARNING_GE(16, "-Wunsafe-buffer-usage");                                                  \
        SOAGEN_DISABLE_CLANG_WARNING_GE(8, "-Wdefaulted-function-deleted");                                            \
        SOAGEN_DISABLE_CLANG_WARNING_GE(9, "-Wctad-maybe-unsupported");                                                \
        SOAGEN_DISABLE_CLANG_WARNING("-Wc++2a-compat");                                                                \
        SOAGEN_DISABLE_CLANG_WARNING("-Wc++98-compat-pedantic");                                                       \
        SOAGEN_DISABLE_CLANG_WARNING("-Wc++98-compat");                                                                \
        SOAGEN_DISABLE_CLANG_WARNING("-Wcovered-switch-default");                                                      \
        SOAGEN_DISABLE_CLANG_WARNING("-Wdisabled-macro-expansion");                                                    \
        SOAGEN_DISABLE_CLANG_WARNING("-Wdouble-promotion");                                                            \
        SOAGEN_DISABLE_CLANG_WARNING("-Wmissing-field-initializers");                                                  \
        SOAGEN_DISABLE_CLANG_WARNING("-Wpacked");                                                                      \
        SOAGEN_DISABLE_CLANG_WARNING("-Wpadded");                                                                      \
        SOAGEN_DISABLE_CLANG_WARNING("-Wtautological-pointer-compare");                                                \
        SOAGEN_DISABLE_CLANG_WARNING("-Wused-but-marked-unused");                                                      \
        SOAGEN_DISABLE_CLANG_WARNING("-Wweak-template-vtables");                                                       \
        SOAGEN_DISABLE_CLANG_WARNING("-Wweak-vtables");                                                                \
        /* gcc */                                                                                                      \
        SOAGEN_DISABLE_GCC_WARNING_GE(12, "-Wc++17-extensions");                                                       \
        SOAGEN_DISABLE_GCC_WARNING_GE(12, "-Winterference-size");                                                      \
        SOAGEN_DISABLE_GCC_WARNING("-Wabi-tag");                                                                       \
        SOAGEN_DISABLE_GCC_WARNING("-Wctad-maybe-unsupported");                                                        \
        SOAGEN_DISABLE_GCC_WARNING("-Wctor-dtor-privacy");                                                             \
        SOAGEN_DISABLE_GCC_WARNING("-Wnamespaces");                                                                    \
        SOAGEN_DISABLE_GCC_WARNING("-Wsubobject-linkage");                                                             \
        SOAGEN_DISABLE_GCC_WARNING("-Wtemplates");                                                                     \
        SOAGEN_DISABLE_GCC_WARNING("-Wuseless-cast");                                                                  \
        SOAGEN_DISABLE_GCC_WARNING("-Wabi");                                                                           \
        SOAGEN_DISABLE_GCC_WARNING("-Wcast-align");                                                                    \
        SOAGEN_DISABLE_GCC_WARNING("-Wcomment");                                                                       \
        SOAGEN_DISABLE_GCC_WARNING("-Wdisabled-optimization");                                                         \
        SOAGEN_DISABLE_GCC_WARNING("-Winline");                                                                        \
        SOAGEN_DISABLE_GCC_WARNING("-Wmaybe-uninitialized");                                                           \
        SOAGEN_DISABLE_GCC_WARNING("-Wmissing-field-initializers");                                                    \
        SOAGEN_DISABLE_GCC_WARNING("-Wpadded");                                                                        \
        SOAGEN_DISABLE_GCC_WARNING("-Wtype-limits");                                                                   \
        /* msvc */                                                                                                     \
        SOAGEN_DISABLE_MSVC_WARNING(4127); /* conditional expr is constant */                                          \
        SOAGEN_DISABLE_MSVC_WARNING(4324); /* structure was padded due to alignment specifier */                       \
        SOAGEN_DISABLE_MSVC_WARNING(4348);                                                                             \
        SOAGEN_DISABLE_MSVC_WARNING(4464);  /* relative include path contains '..' */                                  \
        SOAGEN_DISABLE_MSVC_WARNING(4505);  /* unreferenced local function removed */                                  \
        SOAGEN_DISABLE_MSVC_WARNING(4514);  /* unreferenced inline function has been removed */                        \
        SOAGEN_DISABLE_MSVC_WARNING(4582);  /* constructor is not implicitly called */                                 \
        SOAGEN_DISABLE_MSVC_WARNING(4619);  /* there is no warning number 'XXXX' */                                    \
        SOAGEN_DISABLE_MSVC_WARNING(4623);  /* default constructor was implicitly defined as deleted */                \
        SOAGEN_DISABLE_MSVC_WARNING(4625);  /* copy constructor was implicitly defined as deleted */                   \
        SOAGEN_DISABLE_MSVC_WARNING(4626);  /* assignment operator was implicitly defined as deleted */                \
        SOAGEN_DISABLE_MSVC_WARNING(4686);  /* possible change in behavior, change in UDT return callconv */           \
        SOAGEN_DISABLE_MSVC_WARNING(4710);  /* function not inlined */                                                 \
        SOAGEN_DISABLE_MSVC_WARNING(4711);  /* function selected for automatic expansion */                            \
        SOAGEN_DISABLE_MSVC_WARNING(4820);  /* N bytes padding added */                                                \
        SOAGEN_DISABLE_MSVC_WARNING(4866);  /* compiler may not enforce left-to-right evaluation order for call */     \
        SOAGEN_DISABLE_MSVC_WARNING(4946);  /* reinterpret_cast used between related classes */                        \
        SOAGEN_DISABLE_MSVC_WARNING(5026);  /* move constructor was implicitly defined as deleted */                   \
        SOAGEN_DISABLE_MSVC_WARNING(5027);  /* move assignment operator was implicitly defined as deleted */           \
        SOAGEN_DISABLE_MSVC_WARNING(5039);  /* potentially throwing function passed to 'extern "C"' function */        \
        SOAGEN_DISABLE_MSVC_WARNING(5045);  /* Compiler will insert Spectre mitigation */                              \
        SOAGEN_DISABLE_MSVC_WARNING(5246);  /* initialization of a subobject should be wrapped in braces */            \
        SOAGEN_DISABLE_MSVC_WARNING(5264);  /* const variable is not used (false-positive) */                          \
        SOAGEN_DISABLE_MSVC_WARNING(26490); /* cg: dont use reinterpret_cast */                                        \
        SOAGEN_DISABLE_MSVC_WARNING(26812); /* cg: Prefer 'enum class' over 'enum' */                                  \
        SOAGEN_DISABLE_MSVC_WARNING(4848);  /* msvc::no_unique_address in C++17 is a vendor extension */               \
        static_assert(true)

#endif

#ifndef SOAGEN_DISABLE_DEPRECATION_WARNINGS
    #define SOAGEN_DISABLE_DEPRECATION_WARNINGS                                                                        \
        /* clang */                                                                                                    \
        SOAGEN_DISABLE_CLANG_WARNING("-Wdeprecated");                                                                  \
        /* gcc */                                                                                                      \
        SOAGEN_DISABLE_GCC_WARNING_GE(11, "-Wdeprecated-enum-enum-conversion");                                        \
        SOAGEN_DISABLE_GCC_WARNING_GE(11, "-Wdeprecated-enum-float-conversion");                                       \
        SOAGEN_DISABLE_GCC_WARNING_GE(9, "-Wdeprecated-copy-dtor");                                                    \
        SOAGEN_DISABLE_GCC_WARNING_GE(9, "-Wdeprecated-copy");                                                         \
        SOAGEN_DISABLE_GCC_WARNING("-Wdeprecated-declarations");                                                       \
        SOAGEN_DISABLE_GCC_WARNING("-Wdeprecated");                                                                    \
        /* msvc */                                                                                                     \
        SOAGEN_DISABLE_MSVC_WARNING(4996);                                                                             \
        static_assert(true)
#endif

#ifndef SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS
    #if SOAGEN_MSVC
        #if SOAGEN_HAS_INCLUDE(<CodeAnalysis / Warnings.h>)
            #pragma warning(push, 0)
            #include <CodeAnalysis/Warnings.h>
            #pragma warning(pop)
            #define SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS SOAGEN_DISABLE_MSVC_WARNING(ALL_CODE_ANALYSIS_WARNINGS)
        #else
            #define SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS static_assert(true)
        #endif
    #else
        #define SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS static_assert(true)
    #endif
#endif

#ifndef SOAGEN_DISABLE_SUGGESTION_WARNINGS
    #define SOAGEN_DISABLE_SUGGESTION_WARNINGS                                                                         \
        SOAGEN_DISABLE_GCC_WARNING("-Wsuggest-attribute=const");                                                       \
        SOAGEN_DISABLE_GCC_WARNING("-Wsuggest-attribute=pure");                                                        \
        SOAGEN_DISABLE_GCC_WARNING("-Wsuggest-final-methods");                                                         \
        SOAGEN_DISABLE_GCC_WARNING("-Wsuggest-final-types");                                                           \
        SOAGEN_DISABLE_GCC_WARNING("-Wsuggest-override");                                                              \
        static_assert(true)
#endif

#ifndef SOAGEN_DISABLE_EXCEPTION_WARNINGS
    #define SOAGEN_DISABLE_EXCEPTION_WARNINGS                                                                          \
        SOAGEN_DISABLE_CLANG_WARNING("-Wexceptions");                                                                  \
        SOAGEN_DISABLE_GCC_WARNING("-Wnoexcept-type");                                                                 \
        SOAGEN_DISABLE_GCC_WARNING("-Wnoexcept");                                                                      \
        SOAGEN_DISABLE_GCC_WARNING("-Wterminate");                                                                     \
        SOAGEN_DISABLE_MSVC_WARNING(4297); /* function assumed not to throw an exception but does */                   \
        SOAGEN_DISABLE_MSVC_WARNING(5039); /* potentially throwing function passed to 'extern "C"' function */         \
        static_assert(true)
#endif

#ifndef SOAGEN_DISABLE_WARNINGS
    #if SOAGEN_CLANG
        #define SOAGEN_DISABLE_WARNINGS                                                                                \
            SOAGEN_PUSH_WARNINGS;                                                                                      \
            SOAGEN_DISABLE_CLANG_WARNING("-Weverything");                                                              \
            static_assert(true)
    #elif SOAGEN_MSVC
        #define SOAGEN_DISABLE_WARNINGS                                                                                \
            __pragma(warning(push, 0))                                                                                 \
            SOAGEN_DISABLE_MSVC_WARNING(4348);                                                                         \
            SOAGEN_DISABLE_MSVC_WARNING(4668);                                                                         \
            SOAGEN_DISABLE_MSVC_WARNING(5105);                                                                         \
            SOAGEN_DISABLE_ARITHMETIC_WARNINGS;                                                                        \
            SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS;                                                                     \
            SOAGEN_DISABLE_DEPRECATION_WARNINGS;                                                                       \
            SOAGEN_DISABLE_EXCEPTION_WARNINGS;                                                                         \
            SOAGEN_DISABLE_SHADOW_WARNINGS;                                                                            \
            SOAGEN_DISABLE_SPAM_WARNINGS;                                                                              \
            SOAGEN_DISABLE_SWITCH_WARNINGS;                                                                            \
            static_assert(true)
    #elif SOAGEN_GCC
        // https://hackmd.io/@VZ/gcc-warnings
        #define SOAGEN_DISABLE_WARNINGS                                                                                \
            SOAGEN_PUSH_WARNINGS;                                                                                      \
            SOAGEN_DISABLE_ARITHMETIC_WARNINGS;                                                                        \
            SOAGEN_DISABLE_CODE_ANALYSIS_WARNINGS;                                                                     \
            SOAGEN_DISABLE_DEPRECATION_WARNINGS;                                                                       \
            SOAGEN_DISABLE_EXCEPTION_WARNINGS;                                                                         \
            SOAGEN_DISABLE_LIFETIME_WARNINGS;                                                                          \
            SOAGEN_DISABLE_SHADOW_WARNINGS;                                                                            \
            SOAGEN_DISABLE_SPAM_WARNINGS;                                                                              \
            SOAGEN_DISABLE_SUGGESTION_WARNINGS;                                                                        \
            SOAGEN_DISABLE_SWITCH_WARNINGS;                                                                            \
            SOAGEN_DISABLE_GCC_WARNING_GE(10, "-Wcomma-subscript");                                                    \
            SOAGEN_DISABLE_GCC_WARNING_GE(10, "-Wmismatched-tags");                                                    \
            SOAGEN_DISABLE_GCC_WARNING_GE(10, "-Wredundant-tags");                                                     \
            SOAGEN_DISABLE_GCC_WARNING_GE(10, "-Wstring-compare");                                                     \
            SOAGEN_DISABLE_GCC_WARNING_GE(10, "-Wvolatile");                                                           \
            SOAGEN_DISABLE_GCC_WARNING_GE(11, "-Winvalid-imported-macros");                                            \
            SOAGEN_DISABLE_GCC_WARNING_GE(11, "-Wstringop-overread");                                                  \
            SOAGEN_DISABLE_GCC_WARNING("-Wabi-tag");                                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Wconditionally-supported");                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Wextra-semi");                                                                \
            SOAGEN_DISABLE_GCC_WARNING("-Wliteral-suffix");                                                            \
            SOAGEN_DISABLE_GCC_WARNING("-Wmultiple-inheritance");                                                      \
            SOAGEN_DISABLE_GCC_WARNING("-Wnamespaces");                                                                \
            SOAGEN_DISABLE_GCC_WARNING("-Wnoexcept-type");                                                             \
            SOAGEN_DISABLE_GCC_WARNING("-Wnoexcept");                                                                  \
            SOAGEN_DISABLE_GCC_WARNING("-Wold-style-cast");                                                            \
            SOAGEN_DISABLE_GCC_WARNING("-Woverloaded-virtual");                                                        \
            SOAGEN_DISABLE_GCC_WARNING("-Wpessimizing-move");                                                          \
            SOAGEN_DISABLE_GCC_WARNING("-Wregister");                                                                  \
            SOAGEN_DISABLE_GCC_WARNING("-Wreorder");                                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Wstrict-null-sentinel");                                                      \
            SOAGEN_DISABLE_GCC_WARNING("-Wsynth");                                                                     \
            SOAGEN_DISABLE_GCC_WARNING("-Wtemplates");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wvirtual-inheritance");                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Wzero-as-null-pointer-constant");                                             \
            SOAGEN_DISABLE_GCC_WARNING("-Wabi");                                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Waddress");                                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Waggregate-return");                                                          \
            SOAGEN_DISABLE_GCC_WARNING("-Wall");                                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Walloc-zero");                                                                \
            SOAGEN_DISABLE_GCC_WARNING("-Walloca");                                                                    \
            SOAGEN_DISABLE_GCC_WARNING("-Warray-bounds");                                                              \
            SOAGEN_DISABLE_GCC_WARNING("-Wbuiltin-macro-redefined");                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Wcast-qual");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wchkp");                                                                      \
            SOAGEN_DISABLE_GCC_WARNING("-Wclobbered");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wcomment");                                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Wconversion");                                                                \
            SOAGEN_DISABLE_GCC_WARNING("-Wdate-time");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wdisabled-optimization");                                                     \
            SOAGEN_DISABLE_GCC_WARNING("-Wduplicated-branches");                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Wduplicated-cond");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wempty-body");                                                                \
            SOAGEN_DISABLE_GCC_WARNING("-Wendif-labels");                                                              \
            SOAGEN_DISABLE_GCC_WARNING("-Wenum-compare");                                                              \
            SOAGEN_DISABLE_GCC_WARNING("-Wextra");                                                                     \
            SOAGEN_DISABLE_GCC_WARNING("-Wformat-contains-nul");                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Wformat-extra-args");                                                         \
            SOAGEN_DISABLE_GCC_WARNING("-Wformat-nonliteral");                                                         \
            SOAGEN_DISABLE_GCC_WARNING("-Wformat-security");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wformat-signedness");                                                         \
            SOAGEN_DISABLE_GCC_WARNING("-Wformat-zero-length");                                                        \
            SOAGEN_DISABLE_GCC_WARNING("-Wformat");                                                                    \
            SOAGEN_DISABLE_GCC_WARNING("-Wignored-qualifiers");                                                        \
            SOAGEN_DISABLE_GCC_WARNING("-Winit-self");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Winline");                                                                    \
            SOAGEN_DISABLE_GCC_WARNING("-Winvalid-pch");                                                               \
            SOAGEN_DISABLE_GCC_WARNING("-Wlogical-op");                                                                \
            SOAGEN_DISABLE_GCC_WARNING("-Wmain");                                                                      \
            SOAGEN_DISABLE_GCC_WARNING("-Wmaybe-uninitialized");                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Wmissing-braces");                                                            \
            SOAGEN_DISABLE_GCC_WARNING("-Wmissing-declarations");                                                      \
            SOAGEN_DISABLE_GCC_WARNING("-Wmissing-field-initializers");                                                \
            SOAGEN_DISABLE_GCC_WARNING("-Wmissing-format-attribute");                                                  \
            SOAGEN_DISABLE_GCC_WARNING("-Wmissing-include-dirs");                                                      \
            SOAGEN_DISABLE_GCC_WARNING("-Wmissing-noreturn");                                                          \
            SOAGEN_DISABLE_GCC_WARNING("-Wmultichar");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wnarrowing");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wnonnull");                                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Wnull-dereference");                                                          \
            SOAGEN_DISABLE_GCC_WARNING("-Woverlength-strings");                                                        \
            SOAGEN_DISABLE_GCC_WARNING("-Wpacked-bitfield-compat");                                                    \
            SOAGEN_DISABLE_GCC_WARNING("-Wpacked");                                                                    \
            SOAGEN_DISABLE_GCC_WARNING("-Wparentheses");                                                               \
            SOAGEN_DISABLE_GCC_WARNING("-Wpedantic");                                                                  \
            SOAGEN_DISABLE_GCC_WARNING("-Wpointer-arith");                                                             \
            SOAGEN_DISABLE_GCC_WARNING("-Wredundant-decls");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wrestrict");                                                                  \
            SOAGEN_DISABLE_GCC_WARNING("-Wreturn-type");                                                               \
            SOAGEN_DISABLE_GCC_WARNING("-Wsequence-point");                                                            \
            SOAGEN_DISABLE_GCC_WARNING("-Wshadow");                                                                    \
            SOAGEN_DISABLE_GCC_WARNING("-Wstack-protector");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wstrict-aliasing");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wstrict-overflow");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wstringop-truncation");                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Wsystem-headers");                                                            \
            SOAGEN_DISABLE_GCC_WARNING("-Wtrampolines");                                                               \
            SOAGEN_DISABLE_GCC_WARNING("-Wtrigraphs");                                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wtype-limits");                                                               \
            SOAGEN_DISABLE_GCC_WARNING("-Wundef");                                                                     \
            SOAGEN_DISABLE_GCC_WARNING("-Wuninitialized");                                                             \
            SOAGEN_DISABLE_GCC_WARNING("-Wunknown-pragmas");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wunreachable-code");                                                          \
            SOAGEN_DISABLE_GCC_WARNING("-Wunsafe-loop-optimizations");                                                 \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-but-set-parameter");                                                  \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-but-set-variable");                                                   \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-function");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-label");                                                              \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-local-typedefs");                                                     \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-macros");                                                             \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-parameter");                                                          \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-result");                                                             \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-value");                                                              \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused-variable");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wunused");                                                                    \
            SOAGEN_DISABLE_GCC_WARNING("-Wvariadic-macros");                                                           \
            SOAGEN_DISABLE_GCC_WARNING("-Wvector-operation-performance");                                              \
            SOAGEN_DISABLE_GCC_WARNING("-Wvla");                                                                       \
            SOAGEN_DISABLE_GCC_WARNING("-Wvolatile-register-var");                                                     \
            SOAGEN_DISABLE_GCC_WARNING("-Wwrite-strings");                                                             \
            static_assert(true)
    #else
        #define SOAGEN_DISABLE_WARNINGS static_assert(true)
    #endif
#endif

#ifndef SOAGEN_ENABLE_WARNINGS
    #if SOAGEN_CLANG || SOAGEN_MSVC || SOAGEN_GCC
        #define SOAGEN_ENABLE_WARNINGS SOAGEN_POP_WARNINGS
    #else
        #define SOAGEN_ENABLE_WARNINGS static_assert(true)
    #endif
#endif
