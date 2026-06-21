#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
Runs a clang-instrumented test executable, merges its raw profile and emits an llvm-cov report
scoped to the soagen library headers. Invoked by the meson 'coverage' run-target.

usage: coverage.py <test-exe> <output-dir>

Optional floor gate (opt-in, off by default so local runs stay report-only): set any of the
environment variables below to a percentage and the script exits non-zero if that metric falls
below it. CI sets these; a developer running the run-target locally does not.

    SOAGEN_COVERAGE_MIN_LINES
    SOAGEN_COVERAGE_MIN_FUNCTIONS
    SOAGEN_COVERAGE_MIN_REGIONS
    SOAGEN_COVERAGE_MIN_BRANCHES
"""

import json
import os
import subprocess
import sys
from pathlib import Path

# only the library headers are of interest; everything else compiled into the test TU is noise
IGNORE = r'(^/usr/|/subprojects/|/tests/|/examples/|/hpp/single/)'

FLOORS = {
    'lines': 'SOAGEN_COVERAGE_MIN_LINES',
    'functions': 'SOAGEN_COVERAGE_MIN_FUNCTIONS',
    'regions': 'SOAGEN_COVERAGE_MIN_REGIONS',
    'branches': 'SOAGEN_COVERAGE_MIN_BRANCHES',
}


def enforce_floors(common: list) -> int:
    floors = {metric: float(os.environ[var]) for metric, var in FLOORS.items() if os.environ.get(var)}
    if not floors:
        return 0

    summary = json.loads(subprocess.check_output(['llvm-cov', 'export', '-summary-only', *common]))
    totals = summary['data'][0]['totals']

    failed = False
    print('coverage floor gate:')
    for metric, minimum in floors.items():
        actual = totals[metric]['percent']
        ok = actual >= minimum
        failed = failed or not ok
        print(f'  {"PASS" if ok else "FAIL"}  {metric:<10} {actual:6.2f}% (floor {minimum:.2f}%)')
    return 1 if failed else 0


def main() -> int:
    exe = Path(sys.argv[1]).resolve()
    out_dir = Path(sys.argv[2]).resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    profraw = out_dir / 'coverage.profraw'
    profdata = out_dir / 'coverage.profdata'
    html_dir = out_dir / 'html'

    subprocess.run([str(exe)], check=True, env={**os.environ, 'LLVM_PROFILE_FILE': str(profraw)})
    subprocess.run(['llvm-profdata', 'merge', '-sparse', str(profraw), '-o', str(profdata)], check=True)

    common = [str(exe), f'-instr-profile={profdata}', '-ignore-filename-regex', IGNORE]
    subprocess.run(['llvm-cov', 'report', *common], check=True)
    subprocess.run(
        ['llvm-cov', 'show', *common, '-format=html', f'-output-dir={html_dir}', '-show-line-counts-or-regions'],
        check=True,
    )
    print(f'coverage report: {html_dir / "index.html"}')
    return enforce_floors(common)


if __name__ == '__main__':
    sys.exit(main())
