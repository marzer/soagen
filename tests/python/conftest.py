#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import sys
from pathlib import Path

# make `import soagen` work when running from a source checkout that hasn't been pip-installed
sys.path.insert(0, str(Path(__file__).parent.parent.parent / 'src'))
