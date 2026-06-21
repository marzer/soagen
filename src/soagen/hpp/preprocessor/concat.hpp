//# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
//# SPDX-License-Identifier: MIT
#pragma once

#ifndef SOAGEN_CONCAT
    #define SOAGEN_CONCAT_2(x, y) x##y
    #define SOAGEN_CONCAT_1(x, y) SOAGEN_CONCAT_2(x, y)
    #define SOAGEN_CONCAT(x, y)   SOAGEN_CONCAT_1(x, y)
#endif
