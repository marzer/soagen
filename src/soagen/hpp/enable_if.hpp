#pragma once

#include "core.hpp"
SOAGEN_DISABLE_WARNINGS;
#include <type_traits>
SOAGEN_ENABLE_WARNINGS;
#include "header_start.hpp"

#ifndef SOAGEN_ENABLE_IF_T
    #if !SOAGEN_DOXYGEN
        #define SOAGEN_ENABLE_IF_T(T, ...) std::enable_if_t<!!(__VA_ARGS__), T>
    #else
        #define SOAGEN_ENABLE_IF_T(T, ...) T
    #endif
#endif

#ifndef SOAGEN_CONSTRAINED_TEMPLATE
    #if !SOAGEN_DOXYGEN
        #define SOAGEN_CONSTRAINED_TEMPLATE_IMPL(condition, T, ...)                                                    \
            template <__VA_ARGS__, std::enable_if_t<!!(condition), T> = 0>                                             \
            SOAGEN_REQUIRES(condition)
        #define SOAGEN_CONSTRAINED_TEMPLATE(condition, ...)                                                            \
            SOAGEN_CONSTRAINED_TEMPLATE_IMPL(condition, bool, __VA_ARGS__)
        #define SOAGEN_CONSTRAINED_TEMPLATE_2(condition, ...)                                                          \
            SOAGEN_CONSTRAINED_TEMPLATE_IMPL(condition, signed char, __VA_ARGS__)
        #define SOAGEN_CONSTRAINED_TEMPLATE_3(condition, ...)                                                          \
            SOAGEN_CONSTRAINED_TEMPLATE_IMPL(condition, signed short, __VA_ARGS__)
        #define SOAGEN_CONSTRAINED_TEMPLATE_4(condition, ...)                                                          \
            SOAGEN_CONSTRAINED_TEMPLATE_IMPL(condition, signed int, __VA_ARGS__)
        #define SOAGEN_CONSTRAINED_TEMPLATE_5(condition, ...)                                                          \
            SOAGEN_CONSTRAINED_TEMPLATE_IMPL(condition, signed long, __VA_ARGS__)
        #define SOAGEN_CONSTRAINED_TEMPLATE_6(condition, ...)                                                          \
            SOAGEN_CONSTRAINED_TEMPLATE_IMPL(condition, signed long long, __VA_ARGS__)
    #else
        #define SOAGEN_CONSTRAINED_TEMPLATE(condition, ...)   template <__VA_ARGS__>
        #define SOAGEN_CONSTRAINED_TEMPLATE_2(condition, ...) template <__VA_ARGS__>
        #define SOAGEN_CONSTRAINED_TEMPLATE_3(condition, ...) template <__VA_ARGS__>
        #define SOAGEN_CONSTRAINED_TEMPLATE_4(condition, ...) template <__VA_ARGS__>
        #define SOAGEN_CONSTRAINED_TEMPLATE_5(condition, ...) template <__VA_ARGS__>
        #define SOAGEN_CONSTRAINED_TEMPLATE_6(condition, ...) template <__VA_ARGS__>
    #endif
#endif

#ifndef SOAGEN_HIDDEN_CONSTRAINT
    #if !SOAGEN_DOXYGEN
        #define SOAGEN_HIDDEN_CONSTRAINT(condition, ...)   SOAGEN_CONSTRAINED_TEMPLATE(condition, __VA_ARGS__)
        #define SOAGEN_HIDDEN_CONSTRAINT_2(condition, ...) SOAGEN_CONSTRAINED_TEMPLATE_2(condition, __VA_ARGS__)
        #define SOAGEN_HIDDEN_CONSTRAINT_3(condition, ...) SOAGEN_CONSTRAINED_TEMPLATE_3(condition, __VA_ARGS__)
        #define SOAGEN_HIDDEN_CONSTRAINT_4(condition, ...) SOAGEN_CONSTRAINED_TEMPLATE_4(condition, __VA_ARGS__)
        #define SOAGEN_HIDDEN_CONSTRAINT_5(condition, ...) SOAGEN_CONSTRAINED_TEMPLATE_5(condition, __VA_ARGS__)
        #define SOAGEN_HIDDEN_CONSTRAINT_6(condition, ...) SOAGEN_CONSTRAINED_TEMPLATE_6(condition, __VA_ARGS__)
    #else
        #define SOAGEN_HIDDEN_CONSTRAINT(condition, ...)
        #define SOAGEN_HIDDEN_CONSTRAINT_2(condition, ...)
        #define SOAGEN_HIDDEN_CONSTRAINT_3(condition, ...)
        #define SOAGEN_HIDDEN_CONSTRAINT_4(condition, ...)
        #define SOAGEN_HIDDEN_CONSTRAINT_5(condition, ...)
        #define SOAGEN_HIDDEN_CONSTRAINT_6(condition, ...)
    #endif
#endif

#include "header_end.hpp"
