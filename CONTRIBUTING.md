# Contributing to Soagen

Firstly: thanks! Any help is greatly appreciated.

For most situations the easiest way for you to contribute is to simply let me know what's going on:

-   Reporting issues or requesting features: [Issues]

If you'd like to contribute more directly via a pull request, see below.

<br/>

## Pull Requests

### Getting started

A prerequisite of working on Soagen with the intention of making a pull request is to have it installed
as 'editable' from a clone of the repository:

```sh
git clone <your soagen fork>
cd soagen
pip install -e '.[dev]'
pre-commit install
```

The `[dev]` extra pulls in `pytest`, `pre-commit`, `ruff` and `pyright` (matching the versions CI uses).

`pre-commit` is the formatting/lint gate for both languages: [ruff] for Python and `clang-format` for C++, both
pinned in `.pre-commit-config.yaml` and enforced in CI. With the hooks installed they run automatically on commit;
you do not need a local `clang-format` (pre-commit fetches the pinned version). To run them by hand:

```sh
pre-commit run --all-files
```

<br/>

### Python contributions

#### Code style

It's Python. I'm primarily a C++ programmer. I really don't care that much. The codebase is linted and formatted
with [ruff] (config in `pyproject.toml`), run via the pre-commit hooks above.

#### Source files

Python source files can be found in `src/soagen`.

#### Versioning and releases

The version lives in a single place: the `VERSION` file at the repo root, a bare `major.minor.patch` triplet.
setuptools reads it at build time, and at runtime soagen reads it back from the same file (falling back to the
installed package metadata when running from a wheel rather than a checkout).

Releases are automated and maintainer-only: bumping `VERSION` on `main` is what cuts a release. CI tags
`v<version>`, and that tag triggers the publish workflow that uploads to PyPI. So for a PR, **don't touch
`VERSION`** - I bump it myself when cutting a release.

<br/>

### C++ contributions

#### Code style

C++ is formatted with `clang-format` (config in `.clang-format`), run via the pre-commit hooks. You don't need
`clang-format` installed locally - pre-commit fetches the pinned version.

#### Source files

C++ source files can be found in `src/soagen/hpp`.

#### Modifying `soagen.hpp`

There are two versions of `soagen.hpp` in the codebase:

-   `src/soagen/hpp/single/soagen.hpp`:<br>
    This is the one used when a user invokes `--install`. It is preprocessed from other headers into one,
    to simplify the end-user experience. **Do not edit it directly - your changes will be lost!**

-   `src/soagen/hpp/soagen.hpp`:<br>
    This one is the entry-point for the preprocessor that generates the single-header version, and doesn't contain
    any code. It's just a bunch of `#include` directives. There's nothing to edit! 😄&#xFE0F;

The _actual_ way to contribute changes to `soagen.hpp` is:

1. Near the code you wish to change in `soagen.hpp` will be a banner indicating the `.hpp` file responsible for that
   part of the file. Find it in `src/soagen/hpp` and edit it directly.
2. Regenerate the single-header version of `soagen.hpp` using the 'hidden' developer option `soagen --update-hpp`
   (working directory does not matter, this should work from anywhere).
    - **Alternatively**: building the examples or tests in the repo itself will automatically regenerate this file.
3. _Somewhat-optional step:_ There's some tests set up in `tests/`. They use Catch2. Any pull-requests that also update
   the tests are going to move much more quickly!

[issues]: https://github.com/marzer/soagen/issues
[ruff]: https://docs.astral.sh/ruff/
